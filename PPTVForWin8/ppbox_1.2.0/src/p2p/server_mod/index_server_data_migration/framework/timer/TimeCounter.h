
#pragma once


/**
* @file
* @brief 基础工具类－－时间计数
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

			/// 获取当前的时间计数
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


		/// 基于GetTickCount的计时器(单位：毫秒)
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

			/// 同步计时
			void Sync()
			{
				m_count = GetTimeCount();
			}
			/// 获取从上次计时开始的时间
			count_value_type GetElapsed() const
			{
				return GetTimeCount() - m_count;
			}

			/// 获取所记录的时间
			count_value_type GetCount() const
			{
				return m_count;
			}
			/// 自动转换成DWORD
			operator count_value_type() const
			{
				return GetCount();
			}


#if defined(_PPL_USES_TICK_COUNTER64)
#pragma message("------ use 64-bit GetTickCount")
			/// 获取当前的时间
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
			/// 所记录的时间
			count_value_type m_count;
		};
	}
}









