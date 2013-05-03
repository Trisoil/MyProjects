// URLEncode.h

#ifndef _URLENCODE_H_
#define _URLENCODE_H_

#ifndef  USG_TYPE
# define USG_TYPE
typedef unsigned char  USGC;
typedef unsigned short USGS;
typedef unsigned int   USGI;
typedef unsigned long  USGL;
#endif

#ifndef  YES
# define YES             1
# define NO              0
#endif

#ifndef  TRUE
# define TRUE            1
# define FALSE           0
#endif

#ifndef  ON
# define ON              1
# define OFF             0
#endif

#ifndef  NUL
# define NUL             '\0'
#endif

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

    class UnicodeUTF8
    {
    public:
        static bool IsUtf8Str(const char* str, int length);
        static std::string UnicodeToUtf8 (const wchar_t * pWideChar, int nCharCounts);
        static std::wstring Utf8ToUnicode ( const char* pMultiByte, int nBytes);
        static USGI Word_Unicode2GBcode(USGI Unicode);
        static int  cvt_Unicode2GB(USGC *strfrom, USGC *strto);
        static int API_Gbk2Utf8(const char *szSource, std::string &strDest);
    };
}

#endif
