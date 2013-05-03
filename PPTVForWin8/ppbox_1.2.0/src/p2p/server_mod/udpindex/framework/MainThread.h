#pragma once
//#include <boost/utility.hpp>
//#include <boost/shared_ptr.hpp>

namespace framework
{
	class MainThread 
        : public boost::noncopyable
	{
    public:
        typedef boost::shared_ptr<MainThread> p;
	public:
		boost::asio::io_service* ios_; 
	private:
		boost::asio::io_service::work* work_;
		HANDLE t_;

		HWND hwnd_;

	public:
		MainThread();
		void Start();
		void Stop();
		void PostWindowsMessage(UINT msg, WPARAM wParam = (WPARAM)0, LPARAM lParam = (WPARAM)0);
		void SendWindowsMessage(UINT msg, WPARAM wParam = (WPARAM)0, LPARAM lParam = (WPARAM)0);
		void SetHWND(HWND hwnd) { hwnd_ = hwnd; }
		HWND GetHWND() const { return hwnd_; }

	private:
        static MainThread::p main_thread_;
	public:
		static MainThread& Inst() { return *main_thread_; };
		static boost::asio::io_service& IOS() { return *main_thread_->ios_; }
        static bool IsClear() { return main_thread_ == NULL || main_thread_->ios_ == NULL; }
        
        static void Post(boost::function<void ()> handler)
        {
            if (main_thread_->ios_ != NULL)
            {
                main_thread_->ios_->post(handler);
            }
        }
        static void Dispatch(boost::function<void ()> handler)
        {
            if (main_thread_->ios_ != NULL)
            {
                main_thread_->ios_->dispatch(handler);
            }
        }
	protected:
		static DWORD WINAPI Run(LPVOID	pParam);
	};
}