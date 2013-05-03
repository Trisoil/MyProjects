#include "stdafx.h"
#include "framework/ConsoleThread.h"
#include "message.h"

namespace framework
{
	ConsoleThread* ConsoleThread::console_thread_ = new ConsoleThread();

	ConsoleThread::ConsoleThread() 
		: work_(NULL)
		, t_(NULL)
	{
	}

	void ConsoleThread::Start()
	{
		if( NULL == work_ )
		{
			work_ = new boost::asio::io_service::work(ios_);
			t_ = ::CreateThread( NULL, 0, ConsoleThread::Run, NULL, 0, NULL );
		}
	}

	void ConsoleThread::Stop()
	{
		if( NULL != work_ )
		{
			delete work_; 
			work_ = NULL;
			::WaitForSingleObject(t_, 5*1000);
		}
	}

	DWORD ConsoleThread::Run(LPVOID	pParam)
	{
		Inst().ios_.run();
		return 0;
	}
}