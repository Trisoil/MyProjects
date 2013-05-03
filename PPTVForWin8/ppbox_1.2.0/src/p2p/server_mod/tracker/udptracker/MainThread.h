#pragma once
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/thread/thread.hpp>

class MainThread 
	: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<MainThread> p;
public:
	boost::asio::io_service* ios_; 
private:
	boost::asio::io_service::work* work_;

#ifdef WIN32
	HWND hwnd_;
#endif

	boost::thread * thread_;

public:
	MainThread();
	void Start();
	void Stop();
#ifdef WIN32
	void PostWindowsMessage(UINT msg, WPARAM wParam = (WPARAM)0, LPARAM lParam = (WPARAM)0);
	void SendWindowsMessage(UINT msg, WPARAM wParam = (WPARAM)0, LPARAM lParam = (WPARAM)0);
	void SetHWND(HWND hwnd) { hwnd_ = hwnd; }
	HWND GetHWND() const { return hwnd_; }
#endif

private:
	static MainThread::p main_thread_;
public:
	static MainThread& Inst() { return *main_thread_; };
	static boost::asio::io_service& IOS() { return *main_thread_->ios_; }
	static bool IsClear() { return 0 == main_thread_ || NULL == main_thread_->ios_; }

	static void Post(boost::function<void ()> handler);
	static void Dispatch(boost::function<void ()> handler);

	static size_t Run();
protected:
};
