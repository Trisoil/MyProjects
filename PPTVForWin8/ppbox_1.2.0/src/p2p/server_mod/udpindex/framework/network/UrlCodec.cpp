#include "stdafx.h"
#include "framework/network/UrlCodec.h"

namespace framework
{
namespace network
{
    inline int char2hex(char c)
    {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return c;
    }
    string UrlCodec::Decode(const string& encoded_url)
    {
        string url;
        for (int i = 0; i < encoded_url.length(); ++i)
        {
            if (encoded_url[i] == '+') 
            {
                url.push_back(' ');
            }
            else if (encoded_url[i] == '%' && i + 2 < encoded_url.length() 
                && isxdigit(encoded_url[i+1]) && isxdigit(encoded_url[i+2]))
            {
                int value = (char2hex(encoded_url[i+1]) << 4) + char2hex(encoded_url[i+2]);
                url.push_back((char)value);
                i += 2;
            }
            else
            {
                url.push_back(encoded_url[i]);
            }
        }
        return url;
    }
}
}
