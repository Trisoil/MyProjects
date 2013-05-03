#pragma once

// 用于从数据库读取数据的线程池

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

namespace udpindex
{
	class DataBaseThreadPoolAdd
		: public boost::noncopyable
	{
	public:
		const static int MAX_THREAD_COUNT = 1000;			// 线程池的最大个数
	public:
		void Start();
		void Stop();

	public:
		static DataBaseThreadPoolAdd& Inst() { return *inst_; }
		static boost::asio::io_service& IOS() { return Inst().ios_; }

	private:
		DataBaseThreadPoolAdd() : work_(NULL), thread_count_(25){}
		static DWORD WINAPI Run(LPVOID pParam)
		{
			IOS().run();
			return 0;
		}

	private:
		static boost::shared_ptr<DataBaseThreadPoolAdd> inst_;
	private:
		boost::asio::io_service ios_;
		boost::asio::io_service::work* work_;
		HANDLE thread_array_[MAX_THREAD_COUNT];
		int thread_count_;						// 线程池的个数	
	};
}