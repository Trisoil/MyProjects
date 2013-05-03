#include "stdafx.h"
#include "ClearDBCacheThread.h"
#include "framework/MainThread.h"
#include "protocal/IndexPacket.h"

namespace udpindex
{
	boost::shared_ptr<ClearDBCacheThread> ClearDBCacheThread::inst_ = 
		boost::shared_ptr<ClearDBCacheThread>(new ClearDBCacheThread());

	void ClearDBCacheThread::Start()
	{
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			thread_ = ::CreateThread(NULL, 0, ClearDBCacheThread::Run, NULL, NULL, 0);
		}
	}

	void ClearDBCacheThread::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;
			::WaitForSingleObject(thread_, INFINITE);
		}
	}
}
