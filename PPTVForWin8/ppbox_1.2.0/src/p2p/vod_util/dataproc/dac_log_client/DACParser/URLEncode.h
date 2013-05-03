// URLEncode.h

#ifndef _URLENCODE_H_
#define _URLENCODE_H_

namespace Util
{

    class URLEncode
    {
    public:
        static std::string EncodeURL(std::string const &URL);
        static std::string DecodeURL(std::string const &URL);

    private:

        static char dec2hexChar(short int n);
        static short int hexChar2dec(char c);

    };

}

#endif
