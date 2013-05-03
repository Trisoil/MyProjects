#include "Common.h"
#include "MainThread.h"

MainThread::p MainThread::main_thread_ (new MainThread());

MainThread::MainThread() 
: work_(NULL)
#ifdef WIN32
, hwnd_(NULL)
#endif
, thread_(0)
, ios_(NULL)
{
}

void MainThread::Start()
{
    if( NULL == work_ )
    {
        ios_  = new boost::asio::io_service();
        work_ = new boost::asio::io_service::work(*ios_);
        thread_ = new boost::thread( &Run );
    }
}

void MainThread::Stop()
{
    if( NULL != work_ )
    {
        delete work_; 
        work_ = NULL;
        try {
            this->ios_->stop();
        }
        catch (const std::exception& e) {
        }

        this->thread_->join();

        // delete ios_;
        ios_ = NULL;
    }
}

size_t MainThread::Run()
{
    Instance().ios_->run();
    return 0;
}

#ifdef WIN32
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
#endif

void MainThread::Post(boost::function<void ()> handler)
{
    if (main_thread_->work_ != NULL && main_thread_->ios_ != NULL)
    {
        main_thread_->ios_->post(handler);
    }
}

void MainThread::Dispatch(boost::function<void ()> handler)
{
    if (main_thread_->work_ != NULL && main_thread_->ios_ != NULL)
    {
        main_thread_->ios_->dispatch(handler);
    }
}
