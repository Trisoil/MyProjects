#pragma once
#include <boost/utility.hpp>
namespace storage
{
	class StorageThread : public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<StorageThread> p;
		boost::asio::io_service ios_; 
	private:
		boost::asio::io_service::work* work_;
		HANDLE t_;

	public:
		StorageThread();
		void Start();
		void Stop();

	private:
		static StorageThread::p main_thread_;
	public:
		static StorageThread& Inst() { return *main_thread_; };
		static boost::asio::io_service& IOS() { return main_thread_->ios_; };
	protected:
		static DWORD WINAPI Run(LPVOID	pParam);
	};
}