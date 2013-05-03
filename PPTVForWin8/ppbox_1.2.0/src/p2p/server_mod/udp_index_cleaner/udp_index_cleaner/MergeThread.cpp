#include "stdafx.h"
#include "MergeThread.h"
#include "framework/MainThread.h"

namespace udplog
{
	boost::shared_ptr<MergeThread> MergeThread::inst_ = 
		boost::shared_ptr<MergeThread>(new MergeThread());

	void MergeThread::Start()
	{
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			thread_ = ::CreateThread(NULL, 0, MergeThread::Run, NULL, NULL, 0);
		}
	}

	void MergeThread::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;
			::WaitForSingleObject(thread_, INFINITE);
		}
	}
}
