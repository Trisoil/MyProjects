
#pragma once

#include <tchar.h>


namespace framework
{
	namespace diag
	{
		class Tracer
		{
		public:
			static void Output(const TCHAR* str)
			{
				assert(str != NULL);
				::OutputDebugString(str);
			}

			static void Trace(const TCHAR* formatString, ...)
			{
				va_list args;
				va_start(args, formatString);

				const size_t max_size = 4096;
				int nBuf;
				TCHAR szBuffer[max_size + 1] = { 0 };

				nBuf = _vsntprintf_s(szBuffer, max_size, formatString, args);

				// was there an error? was the expanded string too long?
				assert(nBuf >= 0);

				Output(szBuffer);

				va_end(args);
			}
		};


#ifndef TRACE
#	ifdef _DEBUG
#		define TRACE		::framework::diag::Tracer::Trace
#	else
#		define TRACE		
#	endif
#endif

#define TRACEOUT	::framework::diag::Tracer::Trace



		class ScopedTracer
		{
		public:
			explicit ScopedTracer(const tstring& str) : m_str(str)
			{
				Tracer::Trace(TEXT("%s:  Begin\n"), m_str.c_str());
			}
			~ScopedTracer()
			{
				Tracer::Trace(TEXT("%s:  End\n"), m_str.c_str());
			}
		private:
			tstring	m_str;
		};


#define TRACE_SCOPE(str)	ScopedTracer _scopedTracer(str)
	}
}








