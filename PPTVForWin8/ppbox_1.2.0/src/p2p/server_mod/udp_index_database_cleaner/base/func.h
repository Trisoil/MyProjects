#pragma once

#include <string.h>

#include "base/guid.h"

namespace base
{
	template<typename T>
	inline T UpperDiv(const T& a, const T& b) 
	{
		return (a + b - 1) / b;
	}

	inline u_int GuidMod(const Guid& guid, u_int mod)
	{
        boost::uint64_t buf[2];
		memcpy(&buf, &guid, sizeof(guid));
		return buf[1] % mod;
	}

	template<typename T>
	inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
	{
		std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, ";"));
		return os;
	}

	inline string UrlTrunc(const string& url, size_t buffer_size)
	{
		if (buffer_size <= 4)
			return url.substr(0, buffer_size - 1);
		if (url.length() + 1 <= buffer_size)
			return url;
		size_t trunc_length = (buffer_size - 4) / 2;
		return url.substr(0, trunc_length) + "..." + url.substr(url.length() - trunc_length);
	}

	template <typename ByteType>
	inline void SetTruncUrl(const string& url, ByteType* buffer, u_int buffer_size)
	{
		BOOST_STATIC_ASSERT(sizeof(ByteType) == 1);
		string url_trunc = UrlTrunc(url, buffer_size);
		strcpy((char*) buffer, url_trunc.c_str());
	}

	template <typename ByteType, int Size>
	inline void SetTruncUrl(const string& url, ByteType (&buffer)[Size])
	{
		BOOST_STATIC_ASSERT(sizeof(ByteType) == 1);
		SetTruncUrl(url, buffer, Size);
	}

}
