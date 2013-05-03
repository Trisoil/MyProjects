#pragma once

#include <string>

#define _ATL_NO_EXCEPTIONS
#define _CONVERSION_DONT_USE_THREAD_LOCALE
#include <atlexcept.h>
#include <atlconv.h>

#pragma comment(lib, "atlsd.lib")

namespace std {
	inline ostream& operator<<(ostream& os, wstring& s)
	{
		using namespace ATL;

		os << CW2A(s.c_str());
		return os;
	}
}

namespace framework
{
	/// 字符串工具类
	class strings
	{
	public:
		/// 不分大小写的比较std::string
		static bool equals_ignore_case(const string& s1, const string& s2)
		{
			return iequals(s1, s2);
		}
		/// 不分大小写的比较std::string
		static bool iequals(const string& s1, const string& s2)
		{
			return (s1.size() == s2.size()) && (_strnicmp(s1.c_str(), s2.c_str(), s1.size()) == 0);
		}

		/// 大写
		template <typename CharT, typename TraitsT, typename AllocatorT>
		static void make_upper(std::basic_string<CharT, TraitsT, AllocatorT>& s)
		{
			std::transform(s.begin(), s.end(), s.begin(), ::toupper);
		}

		/// 小写
		template <typename CharT, typename TraitsT, typename AllocatorT>
		static void make_lower(std::basic_string<CharT, TraitsT, AllocatorT>& s)
		{
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		}

		/// 大写
		template <typename CharT, typename TraitsT, typename AllocatorT>
		static std::basic_string<CharT, TraitsT, AllocatorT> upper(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
		{
			std::basic_string<CharT, TraitsT, AllocatorT> result = s;
			make_upper(result);
			return result;
		}

		/// 小写
		template <typename CharT, typename TraitsT, typename AllocatorT>
		static std::basic_string<CharT, TraitsT, AllocatorT> lower(const std::basic_string<CharT, TraitsT, AllocatorT>& s)
		{
			std::basic_string<CharT, TraitsT, AllocatorT> result = s;
			make_lower(result);
			return result;
		}

		template <typename CharT, typename TraitsT, typename AllocatorT, typename OutputIteratorT>
		static void split(OutputIteratorT output, const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter)
		{
			typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
			size_t startPos = 0;
			size_t pos;
			string_type str;
			for (;;)
			{
				pos = src.find(delimiter, startPos);
				str = src.substr(startPos, pos - startPos);
				*output++ = str;
				if (pos == string_type::npos)
					break;
				startPos = pos + 1;
			}
		}

		template <typename CharT, typename TraitsT, typename AllocatorT>
		static pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
			split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, CharT delimiter)
		{
			typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
			size_t pos = src.find(delimiter, 0);
			if (pos == string_type::npos)
				return make_pair(src, string_type());
			assert(pos + 1 <= src.size());
			return make_pair(src.substr(0, pos), src.substr(pos + 1));
		}

		template <typename CharT, typename TraitsT, typename AllocatorT>
		static pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
			split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const std::basic_string<CharT, TraitsT, AllocatorT>& delimiter)
		{
			typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
			size_t pos = src.find(delimiter, 0);
			if (pos == string_type::npos)
				return make_pair(src, string_type());
			assert(pos + delimiter.size() <= src.size());
			return make_pair(src.substr(0, pos), src.substr(pos + delimiter.size()));
		}

		template <typename CharT, typename TraitsT, typename AllocatorT>
		static pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
			split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter)
		{
			return split_pair(src, delimiter, TraitsT::length(delimiter));
		}

		template <typename CharT, typename TraitsT, typename AllocatorT>
		static pair<std::basic_string<CharT, TraitsT, AllocatorT>, std::basic_string<CharT, TraitsT, AllocatorT> > 
			split_pair(const std::basic_string<CharT, TraitsT, AllocatorT>& src, const CharT* delimiter, size_t delimiterSize)
		{
			typedef std::basic_string<CharT, TraitsT, AllocatorT> string_type;
			size_t pos = src.find(delimiter, 0, delimiterSize);
			if (pos == string_type::npos)
				return make_pair(src, string_type());
			assert(pos + delimiterSize <= src.size());
			return make_pair(src.substr(0, pos), src.substr(pos + delimiterSize));
		}

		template <typename CharT, typename TraitsT, typename AllocatorT, typename InputIteratorT>
		static std::basic_string<CharT, TraitsT, AllocatorT> join(InputIteratorT begin, InputIteratorT end, char delimiter)
		{
			assert(!"not implemented");
			return "";
		}

		template <typename CharT, typename TraitsT, typename AllocatorT>
		static std::basic_string<CharT, TraitsT, AllocatorT> trim(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
		{
			const CharT* delimiter = TEXT(" ");
			std::basic_string<CharT, TraitsT, AllocatorT> s = str;
			assert(_tcslen(delimiter) > 0);
			s.erase(0, s.find_first_not_of(delimiter));
			s.erase(s.find_last_not_of(delimiter) + 1);
			return s;
		}


		static tstring format(const TCHAR* format, ...)
		{
			va_list args;
			va_start(args, format);
			tstring str = vformat(format, args);
			va_end(args);
			return str;
		}
		static tstring vformat(const TCHAR* format, va_list argList)
		{
			const size_t max_size = 1024;
			TCHAR str[max_size + 1];
			str[max_size] = 0;
			int count = _vsntprintf(str, max_size, format, argList);
			if (count < 0)
			{
				assert(false);
				return tstring();
			}
			assert(count < max_size);
			return tstring(str, count);
		}


		template <typename CharT, typename TraitsT, typename AllocatorT>
		static bool is_upper(const std::basic_string<CharT, TraitsT, AllocatorT>& str)
		{
			for (size_t i = 0; i < str.size(); ++i)
			{
				if (!::isupper(str[i]))
					return false;
			}
			return true;
		}

	};
}