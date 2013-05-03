/******************************************************************
* Name: ��ʱ��������
* Function: ��������winXP,win2000ϵͳ��ִ�ж�ʱ�����й���
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
			// ��ʱ�����о��
			HANDLE                            m_hTimerQueue;
			// CWinTimerΪ��Ԫ
			friend class                      CWinTimer;
		};
	}
}
