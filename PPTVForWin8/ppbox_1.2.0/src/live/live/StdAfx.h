// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__31AD2729_2475_4BE0_B087_D5E9BE183D76__INCLUDED_)
#define AFX_STDAFX_H__31AD2729_2475_4BE0_B087_D5E9BE183D76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef LIVE_ENABLE_ASSERT
#  define LIVE_ASSERT(x) assert(x)
#else
#  define LIVE_ASSERT(x) (void)0
#endif


#define _PPL_USE_ASIO

#include "common/stdbase.h"

#ifdef BOOST_WINDOWS_API
#include <ppl/mswin/windows.h>
#endif

#include <ppl/util/time_counter.h>
using ppl::util::time_counter;



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__31AD2729_2475_4BE0_B087_D5E9BE183D76__INCLUDED_)
