#include "PPliveThreadPool.h"
#include <boost/bind.hpp>


using namespace std;

PPliveThreadPool::PPliveThreadPool(int pool_size) : pool_size_(pool_size)
{
	//thread_ids_ = NULL;
	active_count_ = 0;
	dead_count_ = 0;//
	//pthread_mutex_init(&m_count_lock, NULL);
}

PPliveThreadPool::~PPliveThreadPool()
{
	if(thread_ids_.size())
	{
		for(int i=0;i<thread_ids_.size();++i)
		{
			delete thread_ids_[i];
			thread_ids_[i] = NULL;
		}
	}
	thread_ids_.clear();
	//pthread_mutex_destroy(&m_count_lock);
}

bool PPliveThreadPool::StartThreads()
{
	
	if(pool_size_ > 0)
	{		
		//pthread_attr_t attr;
		//pthread_attr_init(&attr);
		//pthread_attr_setstacksize(&attr, STACK_SIZE);

		for(int i = 0; i < pool_size_; i++)
		{
			boost::thread* pThread = new boost::thread(boost::bind(&PPliveThreadPool::ThreadProc,this));
			
			if(NULL == pThread)
			{
				return false;
			}
			else
			{
				thread_ids_.push_back(pThread);
			}
		}

		//pthread_attr_destroy(&attr);	
	}
	return 0 != thread_ids_.size();


}

void PPliveThreadPool::WaitThreadsTermination()
{
	if(thread_ids_.size())
	{
		for(int i = 0; i < pool_size_; i++)
		{
			//pthread_join(m_thread_ids[i], NULL);
			thread_ids_[i]->join();
		}

		//delete[] thread_ids_;
		//thread_ids_ = NULL;
	}
}

void* PPliveThreadPool::ThreadProc(void* para)
{
	PPliveThreadPool* me = (PPliveThreadPool*) para;
	//LOG4CPLUS_ERROR(logger, "*******************Thread start,m_pool_size=" << me->m_pool_size);
	try
	{
		me->DoIt();
		//DoIt();
	}
	catch (std::exception& e) 
	{
		//LOG4CPLUS_ERROR(logger, "*******************Thread Exception: "<<e.what());
		throw;		
	}
	catch(...)
	{
		//LOG4CPLUS_ERROR(logger, "*******************Thread occur unknown error");
		throw;		
	}
	//me->setthreaddie();
	return NULL;
}

