/* ======================================================================
 *    BHeader.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    Bootstrap工程相关定义的文件头
 * ======================================================================
 *    Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-23     cnhbdu      创建
 */

#ifndef __B_HEADER_H__
#define __B_HEADER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#ifdef WIN32
#pragma warning(disable:4819)
#define _CRT_SECURE_NO_WARNINGS
#endif // WIN32



#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include "framework/logger/Logger.h"
#include "framework/logger/Logger.h"

#define BOOTSTRAP_NAMESPACE_BEGIN namespace bootstrap {
#define BOOTSTRAP_NAMESPACE_END }

template<typename To_T, typename From_T>
inline To_T bootstrap_lexical_cast(const From_T& from, boost::uint8_t& err)
{
    To_T to;
    try
    {
        to = boost::lexical_cast<To_T>(from);
    }
    catch (boost::bad_lexical_cast&)
    {
        err = 1;
    }
    err = 0;
    return to;
}

#endif // __B_HEADER_H__
