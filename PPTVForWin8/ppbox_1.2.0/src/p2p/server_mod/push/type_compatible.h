/* ======================================================================
 *	type_compatible.h
 *	Copyright (c) 2012 Synacast. All rights reserved.
 *
 *	push serverƽ̨�������Ͷ���
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2012-02-03     youngky      ����
 */

#ifndef __TYPE_COMPATIBLE_H__
#define __TYPE_COMPATIBLE_H__

#ifdef WIN32

#else
typedef unsigned long DWORD;
typedef long long __int64;
typedef __int64 __time64_t;

#endif

#define MIN(a,b)            (((a) < (b)) ? (a) : (b))

#endif