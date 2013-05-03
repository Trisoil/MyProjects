#pragma once

// 用于读取TrackerList配置文件 的 线程

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace udplog
{
	class LogThread
		: public boost::noncopyable
		, public boost::enable_shared_from_this<LogThread>
	{
	public:
		void Start();
		void Stop();

	public:
		static LogThread& Inst() { return *inst_; }
		static boost::asio::io_service& IOS() { return Inst().ios_; }

	private:
		LogThread() : work_(NULL), thread_(NULL) {}
		static DWORD WINAPI Run(LPVOID pParam)
		{
			IOS().run();
			return 0;
		}

	private:
		static boost::shared_ptr<LogThread> inst_;
	private:
		boost::asio::io_service ios_;
		boost::asio::io_service::work* work_;
		HANDLE thread_;
	};
}
