/* ======================================================================
 *	read_thread.cpp
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	任务分配和下载线程
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-03     cnhbdu      创建
 */

#include "stdafx.h"
#include "asio_thread.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

AsioThread::AsioThread() : m_p_work(NULL), m_p_ios(NULL)
{
}

AsioThread::~AsioThread()
{
}

void AsioThread::Start()
{
	if( NULL == m_p_work )
	{
		m_p_ios  = new boost::asio::io_service;
		m_p_work = new boost::asio::io_service::work(*m_p_ios);

        boost::function<void()> func = boost::bind(AsioThread::Run, this);
        m_p_thread.reset(new boost::thread(func));
	}
}

void AsioThread::Stop()
{
	if (NULL == m_p_work) return;
	delete m_p_work;
	m_p_work = NULL;

	m_p_ios->stop();

    m_p_thread->sleep(boost::get_system_time()+boost::posix_time::milliseconds(3 * 1000));

	delete m_p_ios;
	m_p_ios = NULL;
}

unsigned long AsioThread::Run(void* pParam)
{
    AsioThread * this_object = (AsioThread *)pParam;
	this_object->m_p_ios->run();
	return 0;
}