/******************************************************************
* Name: 定时器队列类
* Function: 仅适用于winXP,win2000系统，执行定时器队列功能
* Author: Ivan
* Created: 2007/10/05
* Revised:
******************************************************************/
#pragma once

#include <Windows.h>
#include "framework/mswin/WinTimer.h"

namespace framework
{
	namespace mswin
	{
		class CWinTimerQueue
		{
		public:
			CWinTimerQueue():m_hTimerQueue(NULL)
			{
				m_hTimerQueue = CreateTimerQueue();
			}

			~CWinTimerQueue()
			{
				DeleteTimerQueueEx(m_hTimerQueue,INVALID_HANDLE_VALUE);
			}

		private:
			// 定时器队列句柄
			HANDLE                            m_hTimerQueue;
			// CWinTimer为友元
			friend class                      CWinTimer;
		};
	}
}
