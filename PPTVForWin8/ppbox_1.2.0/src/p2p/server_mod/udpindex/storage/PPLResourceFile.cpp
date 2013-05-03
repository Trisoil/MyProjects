#include "stdafx.h"
#include "PPLResourceFile.h"


namespace p2sp
{
	bool PPLResourceFile::Write(size_t offset,Buffer write_buf)
	{
		if(!Check(offset,write_buf))
			return false;
		memcpy(((char*)view_map_buffer_)+offset,write_buf.data_.get(),write_buf.length_);
		return true;
	}

	Buffer PPLResourceFile::Read(size_t offset,u_int buflen)
	{
		if(!Check(offset,buflen))
			return false;
		Buffer read_buf(buflen);
		memcpy(read_buf.data_.get(),((char*)view_map_buffer_)+offset,read_buf.length_);
		return read_buf;
	}
	bool PPLResourceFile::Check(size_t offset,Buffer buf)
	{
		if(view_map_buffer_==NULL)
		{
			STORAGE_ERR_LOG("view_map_buffer_==NULL");
			return false;
		}
		if(offset+buf.length_>file_size_)
		{
			STORAGE_ERR_LOG("out of range! offset+write_buflen:"<<offset+buf.length_<<"	file_size_:"<<file_size_);
			return false;
		}
		return true;
	}
	void PPLResourceFile::Flush()
	{
		FlushViewOfFile(view_map_buffer_,file_size_);
	}

	PPLResourceFile::PPLResourceFile(string filename,int file_size,int resource_type)
		:ResourceFile(filename,file_size,resource_type)
	{
		
	}

	//PPLResourceFile::p PPLResourceFile::Create(string filename,size_t file_size)
	//{
	//	MappingFile::p  = MappingFile::Create(filename,file_size);

	//}


}
