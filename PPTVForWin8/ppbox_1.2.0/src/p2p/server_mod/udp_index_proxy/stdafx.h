// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0500	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifdef _DEBUG

#define NEED_LOG
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

#else

//#define NEED_LOG

#endif

#include <stdio.h>
#include <tchar.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
// Windows ͷ�ļ�:
//#include <windows.h>
#include <assert.h>
#include <WinSock2.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <hash_map>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace framework {}
namespace base{}
namespace protocal {}
namespace storage {}
namespace statistic {}

using namespace std;
using namespace framework;
using namespace base;
using namespace protocal;
using namespace storage;
using namespace statistic;

//#include <vld.h>

#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

#define interface struct

#include "framework/buffer.h"
#include "framework/mswin/strings.h"
#include "framework/util/macro.h"
#include "framework/log.h"
#include "base/ini.h"
#include "framework/network/uri.h"
//#include "base/base.h"

namespace framework
{
	string w2b(const wstring& _src);
	wstring b2w(const string& _src);
}

#pragma warning(disable:4200)
#pragma warning(disable:4267)


