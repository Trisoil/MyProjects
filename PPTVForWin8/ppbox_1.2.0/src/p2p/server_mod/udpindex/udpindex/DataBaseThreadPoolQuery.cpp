#include "stdafx.h"
#include "udpindex/DataBaseThreadPoolQuery.h"

namespace udpindex
{
	boost::shared_ptr<DataBaseThreadPoolQuery> DataBaseThreadPoolQuery::inst_ = boost::shared_ptr<DataBaseThreadPoolQuery>(new DataBaseThreadPoolQuery());

	void DataBaseThreadPoolQuery::Start()
	{		
		if (NULL == work_)
		{
			work_ = new boost::asio::io_service::work(ios_);
			assert(thread_count_ <= MAX_THREAD_COUNT);
			LIMIT_MIN_MAX(thread_count_, 0, MAX_THREAD_COUNT);
			for(int i = 0; i < thread_count_; i ++)
			{
				thread_array_[i] = ::CreateThread(NULL, 0, DataBaseThreadPoolQuery::Run, NULL, NULL, 0);
			}
		}
	}
	void DataBaseThreadPoolQuery::Stop()
	{
		if (NULL != work_)
		{
			delete work_;
			work_ = NULL;

			assert(thread_count_ <= MAX_THREAD_COUNT);
			LIMIT_MIN_MAX(thread_count_, 0, MAX_THREAD_COUNT);
			// 等待所有的线程退出
			::WaitForMultipleObjects(thread_count_, thread_array_, TRUE, INFINITE);
		}
	}
}
