#include "stdafx.h"
#include "DelThread.h"
#include "framework/MainThread.h"
#include "protocal/IndexPacket.h"

namespace indexcleaner
{
	boost::shared_ptr<DelThread> DelThread::inst_ = 
		boost::shared_ptr<DelThread>(new DelThread());

	void DelThread::Start()
	{
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			thread_ = ::CreateThread(NULL, 0, DelThread::Run, NULL, NULL, 0);
		}
	}

	void DelThread::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;
			::WaitForSingleObject(thread_, INFINITE);
		}
	}
}
