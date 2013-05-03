// URLEncode.cpp
#include "stdafx.h"
#include "URLEncode.h"

#ifdef BOOST_WINDOWS_API
#else
#include <iconv.h>
#endif  // BOOST_WINDOWS_API

namespace Util 
{

    char URLEncode::dec2hexChar(short int n) {
        if ( 0 <= n && n <= 9 ) {
            return char( short('0') + n );
        } else if ( 10 <= n && n <= 15 ) {
            return char( short('A') + n - 10 );
        } else {
            return char(0);
        }
    } 

    short int URLEncode::hexChar2dec(char c) {
        if ( '0'<=c && c<='9' ) {
            return short(c-'0');
        } else if ( 'a'<=c && c<='f' ) {
            return ( short(c-'a') + 10 );
        } else if ( 'A'<=c && c<='F' ) {
            return ( short(c-'A') + 10 );
        } else {
            return -1;
        }
    } 

    std::string URLEncode::EncodeURL(std::string const &URL)
    {
        std::string result = "";
        for ( unsigned int i=0; i<URL.size(); i++ ) {
            char c = URL[i];
            if ( 
                ( '0'<=c && c<='9' ) ||
                ( 'a'<=c && c<='z' ) ||
                ( 'A'<=c && c<='Z' ) ||
                c=='/' || c=='.' 
                ) {
                    result += c;
            } else {
                int j = (short int)c;
                if ( j < 0 ) {
                    j += 256;
                }
                int i1, i0;
                i1 = j / 16;
                i0 = j - i1*16;
                result += '%';
                result += dec2hexChar(i1);
                result += dec2hexChar(i0); 
            }
        }
        return result;
    } 

    std::string URLEncode::DecodeURL(std::string const &URL) {
        std::string result = "";
        for ( unsigned int i=0; i<URL.size(); i++ ) {
            char c = URL[i];
            if ( c != '%' ) {
                result += c;
            } else if(i + 2 < URL.size()){
                char c1 = URL[++i];
                char c0 = URL[++i];
                int num = 0;
                num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
                result += char(num);
            }
            else
                break;
        }
        return result;
    }

    std::string UnicodeUTF8::UnicodeToUtf8 (const wchar_t * pWideChar, int nCharCounts)
    {
        std::string strResult;

        const unsigned short*   pwc = (const unsigned short*)pWideChar;
        const unsigned short*   pwce = (const unsigned short*)pWideChar + nCharCounts;

        while ( pwc < pwce ) {
            unsigned short  wc = *pwc++;

            if ( wc < 0x00000080 ) {
                //*pmb++ = (char)wc;
                strResult.push_back((char)wc);
            } else
                if ( wc < 0x00000800 ) {
                    //*pmb++ = (char)(0xC0 | ((wc >>  6) & 0x1F));
                    strResult.push_back((char)(0xC0 | ((wc >>  6) & 0x1F)));
                    //*pmb++ = (char)(0x80 |  (wc        & 0x3F));
                    strResult.push_back((char)(0x80 |  (wc        & 0x3F)));
                } else
                    if ( wc < 0x00010000 ) {
                        //*pmb++ = (char)(0xE0 | ((wc >> 12) & 0x0F));
                        strResult.push_back((char)(0xE0 | ((wc >> 12) & 0x0F)));
                        //*pmb++ = (char)(0x80 | ((wc >>  6) & 0x3F));
                        strResult.push_back((char)(0x80 | ((wc >>  6) & 0x3F)));
                        //*pmb++ = (char)(0x80 |  (wc        & 0x3F));
                        strResult.push_back((char)(0x80 |  (wc        & 0x3F)));
                    }
        }

        return strResult;
    }

    /* Convert UTF-8 coded string to UNICODE
    Return number of characters converted */
    std::wstring UnicodeUTF8::Utf8ToUnicode ( const char* pMultiByte, int nBytes)
    {
        const unsigned char*    pmb = (const unsigned char  *)pMultiByte;
        const unsigned char*    pmbe = (const unsigned char  *)(pMultiByte+nBytes);

        std::wstring strResult;

        size_t  cwChars = 0;

        while ( pmb < pmbe ) {
            char            mb = *pmb++;
            unsigned int    cc = 0;
            unsigned int    wc;

            while ( (cc < 7) && (mb & (1 << (7 - cc)))) {
                cc++;
            }

            if ( cc == 1 || cc > 6 )                    // illegal character combination for UTF-8
                continue;

            if ( cc == 0 ) {
                wc = mb;
            } else {
                wc = (mb & ((1 << (7 - cc)) - 1)) << ((cc - 1) * 6);
                while ( --cc > 0 ) {
                    if ( pmb == pmbe )                  // reached end of the buffer
                        return strResult;
                    mb = *pmb++;
                    if ( ((mb >> 6) & 0x03) != 2 )      // not part of multibyte character
                        return strResult;
                    wc |= (mb & 0x3F) << ((cc - 1) * 6);
                }
            }

            if ( wc & 0xFFFF0000 )
                wc = L'?';
            //*pwc++ = wc;
            strResult.push_back(wc);
        }

        return strResult;
    }

#ifdef BOOST_WINDOWS_API
    int UnicodeUTF8::API_Gbk2Utf8(const char* szSource, std::string &strDest)
    {
        int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)szSource, -1, NULL,0);
        unsigned short * wszUtf8 = new unsigned short[len+1];
        memset(wszUtf8, 0, len * 2 + 2);
        MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)szSource, -1, (LPWSTR)wszUtf8, len);
        len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
        char *szUtf8=new char[len + 1];
        memset(szUtf8, 0, len + 1);
        WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, (LPSTR)szUtf8, len, NULL,NULL);
        strDest = szUtf8;
        delete []wszUtf8;
        delete []szUtf8;

        return 0;
    }
#else
    int UnicodeUTF8::API_Gbk2Utf8(const char *szSource, std::string &strDest)
    {

        iconv_t cd = iconv_open("UTF-8//IGNORE","GB18030");
        if (cd == (iconv_t)-1)
        {
            iconv_close(cd);
            return -2;
        }
        int iInLen  = strlen(szSource);
        int iOutLen = 2*iInLen+1;
        char *sUTF= new char[iOutLen];
        if(!sUTF){
            iconv_close(cd);
            return -3;
        }        
        char * sPos = sUTF;
        int    iTemp = iOutLen;
        iconv(cd, (char**)&szSource,(size_t*)&iInLen, (char**)&sUTF, (size_t*)&iOutLen );

        sPos[iTemp-iOutLen] = '\0';      //success 
        strDest = sPos;
        delete[] sPos;  
        iconv_close(cd);
        return 0;
    }
#endif
 
}
