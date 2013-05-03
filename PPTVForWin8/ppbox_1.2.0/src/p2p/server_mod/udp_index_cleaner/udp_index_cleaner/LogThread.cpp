#include "stdafx.h"
#include "LogThread.h"
#include "framework/MainThread.h"

namespace udplog
{
	boost::shared_ptr<LogThread> LogThread::inst_ = 
		boost::shared_ptr<LogThread>(new LogThread());

	void LogThread::Start()
	{
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			thread_ = ::CreateThread(NULL, 0, LogThread::Run, NULL, NULL, 0);
		}
	}

	void LogThread::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;
			::WaitForSingleObject(thread_, INFINITE);
		}
	}
}
