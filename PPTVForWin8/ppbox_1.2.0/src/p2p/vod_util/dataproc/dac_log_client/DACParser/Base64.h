// Base64.h

#ifndef _BASE64_H_
#define _BASE64_H_

namespace Util {

    class Base64
    {
    public:

        typedef std::string::size_type size_t;
        typedef unsigned char byte;

        static std::string Encode(std::string const & str);
        static std::string Decode(std::string const & str);

        static std::string & GetDelimiter();
        static char const * GetEnBase64Table();
        static char const * GetDeBase64Table();

    public:
        // Encode
        enum { _MAX_LINE_LENGTH = 72 };
        static size_t DoEncode(char const * pSrc, size_t nSrcLen, char * pDst);
        static size_t DoDecode(char const * pSrc, size_t nSrcLen, char * pDst);
    private:
        static size_t calcEnoughEncodedLength(size_t length);
        static size_t calcEnoughDecodedLength(size_t length);
    private:
        static byte getFirstSixBits(byte val);
        static byte getLastSixBits(byte val);
    };

}
#endif
