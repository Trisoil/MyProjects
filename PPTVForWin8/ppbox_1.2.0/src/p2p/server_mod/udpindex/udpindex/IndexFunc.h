#include "stdafx.h"
#include "framework/log.h"
#include "storage/MD5.h"
#include "base/guid.h"
#ifndef INDEX_FUNC
#define INDEX_FUNC

namespace udpindex
{
	#define INDEX_INFO(message) LOG(__INFO, "Index", message)
	#define INDEX_EVENT(message) LOG(__EVENT, "Index", message)
	#define INDEX_DEBUG(message) LOG(__DEBUG, "Index", message)
	#define INDEX_WARN(message) LOG(__WARN, "Index", message)
	#define INDEX_ERROR(message) LOG(__ERROR, "Index", message)

	string Url2Mini(string url, string refer);

    MD5 Url2MD5(string mini_url);
    MD5 Ctt2MD5(MD5 ctt, size_t ctt_bytes, size_t file_length);
}

#endif