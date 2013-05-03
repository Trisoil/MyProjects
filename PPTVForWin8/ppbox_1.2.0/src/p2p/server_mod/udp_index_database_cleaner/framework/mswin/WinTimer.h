/******************************************************************
* Name: ��ʱ����
* Function: ��������winXP,win2000ϵͳ��ִ�ж�ʱ�����ܣ��첽��ʱ��
* ��ʱ���ص���������Ϊ void Handler(const boost::asio::error&)
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

			  // �趨��ʱ��ʱ��(���ʱ��)����λΪ����,�趨�ص��������첽�ȴ�����
			  // ������һ�Σ��Ƿ���Ҫ����ѭ����ʱ��ѡ��?
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

			  // ȡ����ʱ��
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

			  // �ж϶�ʱ���Ƿ�ֹͣ
			  bool IsStop() const
			  {
				  return m_bIsStop;
			  }

			  // ��ȡio_service
			  boost::asio::io_service& GetIoService() const
			  {
				  return m_IOService;
			  }

			  boost::function1<void,const boost::asio::error&> GetTimeoutHandler() const
			  {
				  return m_TimeoutHandler;
			  }

		private:
			// io_service����
			boost::asio::io_service&                            m_IOService;
			// ��ʱ����������
			CWinTimerQueue&                                     m_TimerQueue;
			// ��ʱ�����
			HANDLE                                              m_hTimer;
			// ��ʱ��ֹͣ��־
			bool                                                m_bIsStop;
			// �ص���������
			boost::function1<void,const boost::asio::error&>    m_TimeoutHandler;
		};

	}
}


extern "C" {
	// ��ʱ���ص��������ú�������TimerQueue���߳���ִ��
	VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		if (NULL != lpParam)
		{
			CWinTimer* pTimer = (CWinTimer*)lpParam;

			if(true == pTimer->IsStop())
			{// ��ʱ����ȡ��
				boost::asio::error e(boost::asio::error::operation_aborted);
				pTimer->GetIoService().post(boost::asio::detail::bind_handler(pTimer->GetTimeoutHandler(), e));
			}
			else
			{// ��ʱ����ʱ����
				boost::asio::error e(0);
				pTimer->GetIoService().post(boost::asio::detail::bind_handler(pTimer->GetTimeoutHandler(), e));
			}
		}

		return;
	}
}
