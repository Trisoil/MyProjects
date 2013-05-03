#pragma once

#include <string>

namespace framework 
{ 
namespace network
{
    class UrlCodec
    {
    public:
        /**
         * @param {url} Url to be encoded.
         * 
         */
        static string Encode(const string& url);
        /**
         * @param {encoded_url} Encoded Url.
         */
        static string Decode(const string& encoded_url);
    };
}
}
