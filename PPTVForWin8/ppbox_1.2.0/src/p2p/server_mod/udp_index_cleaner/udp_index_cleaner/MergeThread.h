#pragma once

// ���ڶ�ȡTrackerList�����ļ� �� �߳�

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace udplog
{
	class MergeThread
		: public boost::noncopyable
		, public boost::enable_shared_from_this<MergeThread>
	{
	public:
		void Start();
		void Stop();

	public:
		static MergeThread& Inst() { return *inst_; }
		static boost::asio::io_service& IOS() { return Inst().ios_; }

	private:
		MergeThread() : work_(NULL), thread_(NULL) {}
		static DWORD WINAPI Run(LPVOID pParam)
		{
			IOS().run();
			return 0;
		}

	private:
		static boost::shared_ptr<MergeThread> inst_;
	private:
		boost::asio::io_service ios_;
		boost::asio::io_service::work* work_;
		HANDLE thread_;
	};
}
