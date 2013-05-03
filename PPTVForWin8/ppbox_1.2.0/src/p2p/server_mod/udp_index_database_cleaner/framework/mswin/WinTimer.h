/******************************************************************
* Name: 定时器类
* Function: 仅适用于winXP,win2000系统，执行定时器功能，异步定时器
* 定时器回调函数类型为 void Handler(const boost::asio::error&)
* Author: Ivan
* Created: 2007/10/05
* Revised:
******************************************************************/
#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include <boost/function.hpp>
#include <Windows.h>
#include "WinTimerQueue.h"

extern "C" {
	VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
}

namespace framework
{
	namespace mswin
	{
		class CWinTimer
		{
		public:
			CWinTimer(boost::asio::io_service& IOService,CWinTimerQueue& TimerQueue):
			  m_IOService(IOService),
				  m_TimerQueue(TimerQueue),
				  m_hTimer(NULL),
				  m_bIsStop(false)
			  {}

			  ~CWinTimer()
			  {
				  if(NULL != m_hTimer)
				  {
					  Cancel();
				  }
			  }

			  // 设定定时器时间(相对时间)，单位为毫秒,设定回调函数并异步等待触发
			  // 仅触发一次，是否需要增加循环定时器选项?
			  void ExpiresAt(UINT32 uDueTime,const boost::function1<void,const boost::asio::error&>& TimeoutHandler)
			  {
				  if(true == m_bIsStop)
				  {
					  return;
				  }

				  if(NULL != m_hTimer)
				  {
					  DeleteTimerQueueTimer(m_TimerQueue.m_hTimerQueue,m_hTimer,INVALID_HANDLE_VALUE);
					  m_hTimer = NULL;
				  }

				  m_TimeoutHandler = TimeoutHandler;

				  (void)CreateTimerQueueTimer(&m_hTimer, m_TimerQueue.m_hTimerQueue, TimerRoutine, this , uDueTime, 0, WT_EXECUTEINTIMERTHREAD);

				  return;
			  }

			  // 取消定时器
			  void Cancel()
			  {
				  if(NULL != m_hTimer)
				  {
					  m_bIsStop = true;
					  DeleteTimerQueueTimer(m_TimerQueue.m_hTimerQueue,m_hTimer,INVALID_HANDLE_VALUE);
					  m_hTimer = NULL;
				  }

				  return;
			  }

			  // 判断定时器是否停止
			  bool IsStop() const
			  {
				  return m_bIsStop;
			  }

			  // 获取io_service
			  boost::asio::io_service& GetIoService() const
			  {
				  return m_IOService;
			  }

			  boost::function1<void,const boost::asio::error&> GetTimeoutHandler() const
			  {
				  return m_TimeoutHandler;
			  }

		private:
			// io_service引用
			boost::asio::io_service&                            m_IOService;
			// 定时器队列引用
			CWinTimerQueue&                                     m_TimerQueue;
			// 定时器句柄
			HANDLE                                              m_hTimer;
			// 定时器停止标志
			bool                                                m_bIsStop;
			// 回调函数对象
			boost::function1<void,const boost::asio::error&>    m_TimeoutHandler;
		};

	}
}


extern "C" {
	// 定时器回调函数，该函数将在TimerQueue的线程中执行
	VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		if (NULL != lpParam)
		{
			CWinTimer* pTimer = (CWinTimer*)lpParam;

			if(true == pTimer->IsStop())
			{// 定时器已取消
				boost::asio::error e(boost::asio::error::operation_aborted);
				pTimer->GetIoService().post(boost::asio::detail::bind_handler(pTimer->GetTimeoutHandler(), e));
			}
			else
			{// 定时器到时触发
				boost::asio::error e(0);
				pTimer->GetIoService().post(boost::asio::detail::bind_handler(pTimer->GetTimeoutHandler(), e));
			}
		}

		return;
	}
}
