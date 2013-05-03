/******************************************************************************
 *
 * Copyright (c) 2011 PPLive Inc.  All rights reserved
 * 
 * pplivethreadpool.h
 * 
 * Description: Ïß³Ì³Ø
 *             
 * 
 * --------------------
 * 2011-09-23, kelvinchen create
 * --------------------
 ******************************************************************************/
#ifndef PPLIVE_THREADPOOL_H_CK_20110923
#define PPLIVE_THREADPOOL_H_CK_20110923
#include <boost/thread/thread.hpp> 


class PPliveThreadPool
{
	//const static int STACK_SIZE = (256 * 1024);

public:
	PPliveThreadPool(int pool_size);
	virtual ~PPliveThreadPool();

	bool StartThreads();
	void WaitThreadsTermination();

	inline int GetPoolSize()
	{
		return pool_size_;
	}

	//inline int get_active_thread_count()
	//{
	//	pthread_mutex_lock(&m_count_lock);
	//	int active_count = m_active_count;
	//	pthread_mutex_unlock(&m_count_lock);
	//	return active_count;
	//}

	//inline int get_dead_thread_count()
	//{
	//	pthread_mutex_lock(&m_count_lock);
	//	int dead_count = m_dead_count;
	//	pthread_mutex_unlock(&m_count_lock);
	//	return dead_count;
	//}
protected:
	virtual void DoIt() = 0;

	//inline void setActive()
	//{
	//	pthread_mutex_lock(&m_count_lock);
	//	m_active_count++;
	//	pthread_mutex_unlock(&m_count_lock);
	//}

	//inline void setInactive()
	//{
	//	pthread_mutex_lock(&m_count_lock);
	//	m_active_count--;
	//	pthread_mutex_unlock(&m_count_lock);
	//}

	//inline void setthreaddie()
	//{
	//	pthread_mutex_lock(&m_count_lock);
	//	++m_dead_count;
	//	pthread_mutex_unlock(&m_count_lock);
	//}
private:
	//DECL_LOGGER(logger);

	static void* ThreadProc(void* para);

	int pool_size_;
	//pthread_t *m_thread_ids;
	//pthread_mutex_t m_count_lock;
	std::vector<boost::thread*> thread_ids_;

	int active_count_;
	int dead_count_;
};
#endif