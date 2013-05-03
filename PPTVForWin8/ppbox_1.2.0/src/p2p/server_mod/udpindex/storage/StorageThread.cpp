#include "stdafx.h"
#include "framework/MainThread.h"
#include "storage/StorageThread.h"

namespace storage
{
	StorageThread::p StorageThread::main_thread_(new StorageThread());

	StorageThread::StorageThread() 
		: work_(NULL)
		, t_(NULL)
	{
	}

	void StorageThread::Start()
	{
		if( NULL == work_ )
		{
			work_ = new boost::asio::io_service::work(ios_);
			t_ = ::CreateThread( NULL, 0, StorageThread::Run, NULL, 0, NULL );
		}
	}

	void StorageThread::Stop()
	{
		if( NULL != work_ )
		{
			delete work_; 
			work_ = NULL;
			::WaitForSingleObject(t_, 5*1000);
		}
	}

	DWORD StorageThread::Run(LPVOID	pParam)
	{
		Inst().ios_.run();
		return 0;
	}

}