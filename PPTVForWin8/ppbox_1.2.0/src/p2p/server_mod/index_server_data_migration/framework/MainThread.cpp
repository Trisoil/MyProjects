#include "stdafx.h"
#include "framework/MainThread.h"

namespace framework
{
    MainThread::p MainThread::main_thread_ (new MainThread());

	MainThread::MainThread() 
		: work_(NULL)
		, t_(NULL)
		, hwnd_(NULL)
		, ios_(NULL)
	{
	}

	void MainThread::Start()
	{
		if( NULL == work_ )
		{
			ios_  = new boost::asio::io_service;
			work_ = new boost::asio::io_service::work(*ios_);
			t_ = ::CreateThread( NULL, 0, MainThread::Run, NULL, 0, NULL );
		}
	}

	void MainThread::Stop()
	{
		if( NULL != work_ )
		{
			delete work_; 
			work_ = NULL;
			//this->ios_.reset();
			this->ios_->stop();
			::WaitForSingleObject(t_, 5*1000);
			::TerminateThread(t_, 0);
			delete ios_;
			ios_ = NULL;
		}
	}

	DWORD MainThread::Run(LPVOID	pParam)
	{
		Inst().ios_->run();
		return 0;
	}
	
	void MainThread::SendWindowsMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if( hwnd_ )
		{
			::SendMessage(hwnd_, msg, wParam, lParam);
		}
	}

	void MainThread::PostWindowsMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if( hwnd_ )
		{
            ::PostMessage(hwnd_, msg, wParam, lParam);
		}
	}
}
