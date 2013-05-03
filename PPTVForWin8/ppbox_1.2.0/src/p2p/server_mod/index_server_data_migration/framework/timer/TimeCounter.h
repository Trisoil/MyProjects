
#pragma once


/**
* @file
* @brief ���������࣭��ʱ�����
*/

namespace framework
{
	namespace timer
	{
		class TickCounter64
		{
		public:
			TickCounter64()
			{
				m_count32 = ::GetTickCount();
				m_count64 = m_count32;
			}

			/// ��ȡ��ǰ��ʱ�����
			UINT64 GetCount()
			{
				DWORD newCount32 = ::GetTickCount();
				DWORD diff = newCount32 - m_count32;
				m_count32 = newCount32;
				m_count64 += diff;
				return m_count64;
			}

		private:
			UINT64 m_count64;
			DWORD m_count32;
		};

		//#define _PPL_USES_TICK_COUNTER64


		/// ����GetTickCount�ļ�ʱ��(��λ������)
		class TimeCounter
		{
		public:
#if defined(_PPL_USES_TICK_COUNTER64)
			typedef UINT64 count_value_type;
#else
			typedef DWORD count_value_type;
#endif

			TimeCounter()
			{
				m_count = GetTimeCount();
			}
			explicit TimeCounter(count_value_type count) : m_count(count)
			{
			}

			/// ͬ����ʱ
			void Sync()
			{
				m_count = GetTimeCount();
			}
			/// ��ȡ���ϴμ�ʱ��ʼ��ʱ��
			count_value_type GetElapsed() const
			{
				return GetTimeCount() - m_count;
			}

			/// ��ȡ����¼��ʱ��
			count_value_type GetCount() const
			{
				return m_count;
			}
			/// �Զ�ת����DWORD
			operator count_value_type() const
			{
				return GetCount();
			}


#if defined(_PPL_USES_TICK_COUNTER64)
#pragma message("------ use 64-bit GetTickCount")
			/// ��ȡ��ǰ��ʱ��
			static UINT64 GetTimeCount()
			{
				static TickCounter64 counter64;
				return counter64.GetCount();
			}
#else
			static DWORD GetTimeCount()
			{
				return ::GetTickCount();
			}
#endif

		private:
			/// ����¼��ʱ��
			count_value_type m_count;
		};
	}
}









