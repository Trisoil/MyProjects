
#pragma once

#include "framework/timer/TimeCounter.h"
#include "base/guid.h"
#include <sstream>
#include <iostream>
#include <string>

#include "VarSizeStruct.h"

using namespace std;
using namespace base;

#define STATISTIC_TYPE "statistic"

#define STAT_INFO(message) LOG(__INFO, STATISTIC_TYPE, message)
#define STAT_EVENT(message) LOG(__EVENT, STATISTIC_TYPE, message)
#define STAT_ERROR(message) LOG(__ERROR, STATISTIC_TYPE, message)
#define STAT_WARN(message) LOG(__WARN, STATISTIC_TYPE, message)
#define STAT_DEBUG(message) LOG(__DEBUG, STATISTIC_TYPE, message)

namespace statistic
{
	inline u_int GetCurrentProcessID()
	{
		return ::GetCurrentProcessId();
	}

	inline string CreateStatisticModuleSharedMemoryName(u_int process_id)
	{
		stringstream is;
		is << "PPVIDEO_" << process_id;
		return is.str();
	}

	inline string CreateP2PDownloaderModuleSharedMemoryName(u_int process_id, const RID& rid)
	{
		stringstream is;
		is << "P2PDOWNLOADER_" << process_id << "_" << rid;
		return is.str();
	}

	inline string CreateDownloadDriverModuleSharedMemoryName(u_int process_id, u_int download_driver_id)
	{
		stringstream is;
		is << "DOWNLOADDRIVER_" << process_id << "_" << download_driver_id;
		return is.str();
	}

	inline u_int GetTickCountInMilliSecond()
	{
		return ::GetTickCount();
	}

	inline u_int GetTickCountInSecond()
	{
		return GetTickCountInMilliSecond() / 1000;
	}
}
