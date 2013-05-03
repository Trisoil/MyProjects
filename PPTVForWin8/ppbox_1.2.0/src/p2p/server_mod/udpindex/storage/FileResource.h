#pragma once

namespace storage
{

	class FileResource
		:public Resource
	{
	public:
	
		typedef boost::shared_ptr<FileResource> p;
	public:
		static p CreateResource(size_t file_length,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t init_size);
		static p ParseResource(ResourceDescriptor::p resource_desc_p,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t actual_size);
	protected:	
		FileResource(size_t file_length,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t init_size);
		FileResource(ResourceDescriptor::p resource_desc_p,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t actual_size);
		virtual size_t  GetLocalFileSize();

	protected:
		//¥Ê¥¢œﬂ≥Ã∑√Œ 
		virtual void	CloseResource() ;
//		virtual void	RemoveResource();
		virtual size_t  FreeDiskSpace() ;

		virtual Buffer ReadBuffer(const size_t startpos, const size_t length);
		virtual void   WriteBuffer(const size_t startpos, const Buffer & buffer);
		virtual void   Erase(const size_t startpos, const size_t length);
		virtual bool   TryRenameToNormalFile();
		virtual bool   TryRenameToTppFile();
		virtual void   Rename(const tstring& newname);
		virtual void   FlushStore();
	private:
		HANDLE file_handle_;

	};
}
