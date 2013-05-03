/* ======================================================================
 *	read_thread.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	读取推送任务列表线程
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-03     cnhbdu      创建
 */

#ifndef __PUSH_SERVER_READ_THREAD_H__
#define __PUSH_SERVER_READ_THREAD_H__

#include <boost/thread.hpp>

class AsioThread
	: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<AsioThread> p;

    static AsioThread::p create() 
	{
		return AsioThread::p(new AsioThread);
	}

public:
	AsioThread();

	~AsioThread();

	void Start();

	void Stop();

	boost::asio::io_service& Ios() { assert(m_p_ios); return *m_p_ios; }

	template <class Handler>
	void Post(Handler handler)
	{
		if (m_p_ios)
		{
			m_p_ios->post(handler);
		}
	}

protected:
//	static unsigned long WINAPI Run(void* pParam);
    static unsigned long Run(void* pParam);

private:
	boost::asio::io_service* m_p_ios;
	boost::asio::io_service::work* m_p_work;
//	void* m_p_thread;
    boost::shared_ptr<boost::thread> m_p_thread;
};

#endif // __PUSH_SERVER_READ_THREAD_H__