#include "stdafx.h"
#include "io.h"
#include "framework\util\random.h"
#include "CfgFile.h"
namespace storage
{
	bool SecFile::SecCreate(const TCHAR* path)
	{
		crash_b_ = false;
		only_read = false;	
		sec_content_count_ = 0;
		CHmacMD5::Reset();
		if(!StdFile::Open(path,TEXT("wb"),_SH_DENYWR))
		{
			crash_b_ = true;
			return false;
		}

		head_.verison_ = version2;
		u_int64 st = framework::util::Random::GetGlobal().Next();
		st = st<<32;
		st += framework::util::Random::GetGlobal().Next();
		//if(!GetFileTime( _fileno(m_handle),(FILETIME *)&st,NULL,NULL))
		//{
		//	crash_b_ = true;
		//	return false;
		//}

		head_.time_stamp_ = st;

		if(!SecWrite((u_char*)&head_,sizeof(head_)))
		{
			crash_b_ = true;
			return false;
		}

		return true;
	}

	bool SecFile::SecOpen(const TCHAR* path)
	{
		crash_b_ = true;
		CHmacMD5::Reset();
		sec_content_count_ = 0;
		if(!StdFile::Open(path,TEXT("rb"),_SH_DENYWR))
		{
			return false;
		}
		DWORD low_size = _filelength( _fileno(m_handle));
		if(low_size==INVALID_FILE_SIZE)
		{
			return false;
		}
		if(low_size>max_sec_file_size)
		{
			return false;
		}
		
		crash_b_ = false;
		if(!DoVerify())
		{
			crash_b_ = true;
			Close();
			return false;
		}
		only_read = true;
		sec_content_count_ = low_size;
		crash_b_ = false;
		return true;
	}

	bool SecFile::DoVerify()
	{
		assert(!(crash_b_));
		char content[1024*64];

		if(!IsOpen())
		{
			return false;
		}
		size_t readnum = Read(&head_,sizeof(SecFileHead));


		if(readnum!=sizeof(SecFileHead))
		{
			return false;
		}

		if(head_.verison_!=version_ && head_.verison_!=version2)
		{
			return false;
		}

		u_int64 st = 0;
		//if(!GetFileTime( _fileno(m_handle),(FILETIME*)&st,NULL,NULL))
		//{
		//	return false;
		//}
		//if(head_.time_stamp_!=st)
		//{
		//	return false;
		//}
		MD5 file_hash = head_.sec_md5_;
		head_.sec_md5_.Clear();
		CHmacMD5::Add((char*)&head_,sizeof(head_));

		while(readnum = Read(content,sizeof(content)))
		{
			CHmacMD5::Add(content,readnum);
		}

		CHmacMD5::Finish();
		SMD5 hash;
		CHmacMD5::GetHash(&hash);

		if(memcmp((char*)&hash,(char*)&(file_hash),sizeof(hash)))
			return false;
		Seek(sizeof(head_),SEEK_SET);
		return true;
	}

	bool  SecFile::DoSign()
	{
		assert(!only_read);
		assert(!crash_b_);
		assert(head_.sec_md5_.IsEmpty());

		Finish();
		SMD5 hash;
		GetHash(&hash);

		memcpy((char*)&(head_.sec_md5_.Data1),(char*)&hash,sizeof(head_.sec_md5_));

		Seek(SecFileHead::md5_offset(),SEEK_SET);

		size_t wlen = Write((char*)&hash,sizeof(MD5));

		if(wlen!=sizeof(MD5))
		{
			return false;
		}
		return true;
	}

	void  SecFile::SecClose()
	{
		if(!IsOpen())
			return;

		StdFile::Flush();
		if(!crash_b_)
		{
			DWORD low_size = _filelength(_fileno(m_handle));
			assert( low_size == sec_content_count_);
			if(!only_read)
			{
				if(!DoSign())
				{
					STORAGE_ERR_LOG("SecFile::SecClose error: DoSign()");
				}
			}
		}
		Close();
		return ;
	}

	bool SecFile::SecWrite(const u_char *buf, size_t buflen)
	{
		if(crash_b_)
		{
			return false;
		}
		assert(!only_read);
		if(sec_content_count_+buflen>=max_sec_file_size)
		{
			return false;
		}
		size_t w_len = Write(buf,buflen);
		{
			if(w_len!=buflen)
			{
				crash_b_ = true;
				return false;
			}
		}
		CHmacMD5::Add(buf,buflen);
		sec_content_count_+=buflen;
		return true;
	}

	size_t  SecFile::SecRead(u_char *buf, size_t buflen)
	{
		if(crash_b_)
		{
			return 0;
		}
		assert(only_read);

		return Read(buf,buflen);
	}

	size_t SecFile::GetContentSize()
	{
		return sec_content_count_- sizeof(SecFileHead);
	}

// -------------------------------------------------------------------------
	// class CfgFile

	bool CfgFile::SecOpen(const tstring resource_file_path)
	{
		tstring cfg_file_path;
		cfg_file_path=resource_file_path;

		cfg_file_path.append(cfg_extname);

		if(!SecFile::SecOpen(cfg_file_path.c_str()))
		{
			return false;
		}
		size_t readnum = SecRead((u_char*)&resource_file_size_,sizeof(resource_file_size_));
		 cfg_head_len_+=sizeof(resource_file_size_);
		if(readnum!=sizeof(resource_file_size_))
		{
			SecClose();
			return false;
		}
		int file_name_len = 0;
		 readnum = SecRead((u_char*)&file_name_len,sizeof(file_name_len));
		 cfg_head_len_+=sizeof(file_name_len);
		if(readnum!=sizeof(file_name_len))
		{
			SecClose();
			return false;
		}
		assert(file_name_len<=256*8);
		TCHAR *file_name = new TCHAR[file_name_len/2];
		 readnum = SecRead((u_char*)file_name,file_name_len);
		if(readnum!=file_name_len)
		{
			SecClose();
			return false;
		}
		resource_file_name_.assign(file_name,file_name_len/2);
		if(resource_file_name_!=resource_file_path)
		{
			SecClose();
			return false;
		}
		cfg_head_len_+=file_name_len;
		return true;
	}

	bool CfgFile::SecCreate(const tstring resource_file_path,size_t resource_file_size)
	{
		tstring cfg_file_path = resource_file_path;
		cfg_file_path.append(cfg_extname);
		if(!SecFile::SecCreate(cfg_file_path.c_str()))
		{
			return false;
		}
		assert(cfg_file_path.size()<=1024);

		if(!SecWrite((u_char*)&resource_file_size,sizeof(resource_file_size)))
		{
			SecClose();
			return false;
		}

		size_t file_name_len = resource_file_path.size()*2;
		if(!SecWrite((u_char*)&file_name_len,sizeof(file_name_len)))
		{
			SecClose();
			return false;
		}
		if(!SecWrite((u_char*)resource_file_path.c_str(),file_name_len))
		{
			SecClose();
			return false;
		}
		resource_file_size_ = resource_file_size;
		resource_file_name_ = resource_file_path;
		return true;
	}

	void CfgFile::SecClose()
	{
		resource_file_size_ = 0;
		resource_file_name_.clear();
		cfg_head_len_ = 0;
		SecFile::SecClose();
		return ;
	}	

	bool CfgFile::AddContent(Buffer buf)
	{
		if(!SecFile::SecWrite((u_char*)&buf.length_,sizeof(buf.length_)))
		{
			return false;
		}

		if(!SecWrite(buf.data_.get(),buf.length_))
		{
			return false;
		}
		return true;
	}

	Buffer CfgFile::GetContent()
	{
		if(IsCrash())
		{
			Buffer buf(0);
			return buf;
		}
		size_t content_len = GetContentSize()-cfg_head_len_;
		size_t readbuflen = 0;
		size_t readnum = SecRead((u_char*)(&readbuflen),sizeof(readbuflen));

		assert(readnum==sizeof(readbuflen));

		assert(readbuflen==content_len-sizeof(readbuflen));

		Buffer content_buf(readbuflen);
		readnum = SecRead((u_char*)(content_buf.data_.get()),content_buf.length_);
		assert(readnum==content_buf.length_);
		return content_buf;
	}

// ------------------------------------------------------------------------------
	// class ResourceInfoListFile

	bool ResourceInfoListFile::AddResourceInfo(const FileResourceInfo &r_info)
	{
		Buffer buf = r_info.ToBuffer();		
		return SecWrite(buf.data_.get(),buf.length_);
	}

	bool ResourceInfoListFile::GetResourceInfo(FileResourceInfo &r_info)
	{
		static size_t item_max_len = 10*1024; 
		u_int item_len  = 0;
		size_t r_len = SecRead((u_char*)&item_len,4);
		if(r_len!=4)
		{
			return false;
		}
		Buffer in_buf(item_len);
		*(int*)(in_buf.data_.get()) = item_len;
		r_len = SecRead(in_buf.data_.get()+4,item_len-4);
		if(r_len!=item_len-4)
		{
			return false;
		}
		if (HeadVersion() == Version2())
		{
			return r_info.Parse(in_buf, true);
		}
		return r_info.Parse(in_buf);
	}
}
