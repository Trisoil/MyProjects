#pragma once
#include <boost/utility.hpp>

namespace framework
{
	class ConsoleThread : public boost::noncopyable
	{
	public:
		boost::asio::io_service ios_; 
	private:
		boost::asio::io_service::work* work_;
		HANDLE t_;
	public:
		ConsoleThread();
		void Start();
		void Stop();
	private:
		static ConsoleThread* console_thread_;
	public:
		static ConsoleThread& Inst() { return *console_thread_; };
		static boost::asio::io_service& IOS() { return console_thread_->ios_; };
	protected:
		static DWORD WINAPI Run(LPVOID	pParam);
	};
}