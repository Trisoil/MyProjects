#pragma  once

using namespace framework::io;
namespace p2sp
{

	class MappingFile
		:public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<MappingFile> p;
		MappingFile::p Create(tstring filename,size_t file_size);
		MappingFile::p Open(tstring filename,size_t file_size);
	private:
		MappingFile(HANDLE file_handle,size_t file_size,int resource_type)

		virtual void FlushBlcok(int Blocksn) = NULL;

		virtual void FlushFile() = NULL;

		virtual void Close() = NULL;

		virtual bool Write(size_t offset,Buffer buf) = NULL;

		virtual Buffer  Read(size_t offset,u_int len) = NULL;


	private:

		static bool Check();

		static HANDLE TryOpenFile(IN OUT tstring filename,size_t file_size);
		static HANDLE TryCreateFile(IN OUT tstring &filename,size_t file_size);
		static bool GetDiskFreeSpace(tstring path,size_t &free_space_size);


	public:
		string file_name_;
		size_t file_size;
	private:
		HANDLE file_h_;
		HANDLE mappingfile_h_;
		LPVOID *mapping_buf_;
	};
}
