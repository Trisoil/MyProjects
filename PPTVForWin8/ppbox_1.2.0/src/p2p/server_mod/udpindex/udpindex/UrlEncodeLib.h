typedef unsigned char byte;

#include <string>
#include <vector>
#include <algorithm>
using	namespace	std;

inline unsigned char ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55: x + 48; 
}

inline unsigned char FromHex(unsigned char x) 
{ 
	unsigned char y;
	if( x >= 'A' && x <= 'Z' ) y = x - 'A' + 10;
	else if( x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if( x >= '0' && x <= '9') y = x - '0';
	else assert( 0 );
	return y;
}

inline string UrlEncoder( string str )
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( (str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )
			strTemp += str[i];
		else if( str[i] == ' ' ) strTemp += "%20";
		else if( str[i] == '.') strTemp += '.';
		else if( str[i] == '/') strTemp += '/';
		else if( str[i] == '-') strTemp += '-';
		else if( str[i] == ':') strTemp += ':';
		else
		{
			strTemp += '%';
			strTemp += ToHex( (unsigned char)str[i] >> 4 );
			strTemp += ToHex( (unsigned char)str[i] % 16 );
		}
	}
	return strTemp;
}

inline string UrlDecoder( string str )
{
	string strTemp = "";
	size_t length = str.length();
	for( size_t i = 0; i < length; i ++ )
	{
		if( str[i] == '+') strTemp += ' ';
		else if( str[i] == '%')
		{
			assert( i + 2 < length );
			unsigned char hign = FromHex( (unsigned char)str[++i] );
			unsigned char low = FromHex( (unsigned char)str[++i] );
			strTemp += hign*16 + low;
		} else strTemp += str[i];
	}
	return strTemp;
}



class Base64Encoding
{
public:
	static string Encode(const string& str);
	static string Decode(const string& str);

	static string& GetDelimiter()
	{
		static string delimiter = "\r\n";
		return delimiter;
	}
	static const char* GetEnBase64Table()
	{
		static const char EnBase64Table[] = 
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		return EnBase64Table;
	}
	static const char* GetDeBase64Table()
	{
		static const char DeBase64Table[] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			62,        // '+'
			0, 0, 0,
			63,        // '/'
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
			0, 0, 0, 0, 0, 0, 0,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
			0, 0, 0, 0, 0, 0,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
		};
		return DeBase64Table;
	}


public:
// Encode
	enum { _MAX_LINE_LENGTH = 72 };
	static size_t DoEncode(const char* pSrc, size_t nSrcLen, char* pDst)
	{
		// 将来也可以考虑扩充到unicode版
		const size_t nMaxLineLen = _MAX_LINE_LENGTH;
		byte c1, c2, c3;    // 输入缓冲区读出个字节
		size_t nDstLen = 0;             // 输出的字符计数
		size_t nLineLen = 0;            // 输出的行长度计数
		size_t nDiv = nSrcLen / 3;      // 输入数据长度除以得到的倍数
		size_t nMod = nSrcLen % 3;      // 输入数据长度除以得到的余数
		const char* EnBase64Tab = GetEnBase64Table();
		// 每次取个字节，编码成个字符
		for (size_t i = 0; i < nDiv; i ++)
		{
			// 取个字节
			c1 = *pSrc++;
			c2 = *pSrc++;
			c3 = *pSrc++;
			// 编码成个字符
			*pDst++ = EnBase64Tab[c1 >> 2];
			*pDst++ = EnBase64Tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
			*pDst++ = EnBase64Tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
			*pDst++ = EnBase64Tab[c3 & 0x3f];
			nLineLen += 4;
			nDstLen += 4;
			// 输出换行？
			if (nLineLen > nMaxLineLen - 4)
			{
				const string& delimiter = GetDelimiter();
				std::copy(delimiter.begin(), delimiter.end(), pDst);
				nLineLen = 0;
				nDstLen += delimiter.size();
			}
		}
		// 编码余下的字节
		if (nMod == 1)
		{
			c1 = *pSrc++;
			*pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
			*pDst++ = EnBase64Tab[((c1 & 0x03) << 4)];
			*pDst++ = '=';
			*pDst++ = '=';
			nLineLen += 4;
			nDstLen += 4;
		}
		else if (nMod == 2)
		{
			c1 = *pSrc++;
			c2 = *pSrc++;
			*pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
			*pDst++ = EnBase64Tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
			*pDst++ = EnBase64Tab[((c2 & 0x0f) << 2)];
			*pDst++ = '=';
			nDstLen += 4;
		}
		return nDstLen;
	}

	static size_t DoDecode(const char* pSrc, size_t nSrcLen, char* pDst)
	{
		size_t nValue;             // 解码用到的长整数
		size_t nDstLen = 0;
		size_t i = 0;
		// 取个字符，解码到一个长整数，再经过移位得到个字节
		const char* DeBase64Tab = GetDeBase64Table();
		while (i < nSrcLen)
		{
			if (*pSrc != '\r' && *pSrc!='\n')
			{
				nValue = DeBase64Tab[*pSrc++] << 18;
				nValue += DeBase64Tab[*pSrc++] << 12;
				*pDst++ = (nValue & 0x00ff0000) >> 16;
				nDstLen++;
				if (*pSrc != '=')
				{
					nValue += DeBase64Tab[*pSrc++] << 6;
					*pDst++ = (nValue & 0x0000ff00) >> 8;
					nDstLen++;
					if (*pSrc != '=')
					{
						nValue += DeBase64Tab[*pSrc++];
						*pDst++ =nValue & 0x000000ff;
						nDstLen++;
					}
				}
				i += 4;
			}
			else        // 回车换行，跳过
			{
				pSrc++;
				i++;
			}
		}
		return nDstLen;
	}

private:
	static size_t calcEnoughEncodedLength(size_t length)
	{
		size_t len = length / 3;
		if (length % 3 > 0)
			++len;
		len *= 4;
		size_t line = len / _MAX_LINE_LENGTH;
		if (len % _MAX_LINE_LENGTH > 0)
			++line;
		len += line * 2;
		return len;
	}

	static size_t calcEnoughDecodedLength(size_t length)
	{
		return length;
	}
private:
};




inline string Base64Encoding::Encode(const string& str)
{
	string result;
	size_t size = calcEnoughEncodedLength(str.size());
	if (size > 0)
	{
		result.resize(size);
		size_t len = DoEncode(str.data(), str.size(), &result[0]);
		result.resize(len);
	}
	return result;
}

inline string Base64Encoding::Decode(const string& str)
{
	string result;
	size_t size = calcEnoughDecodedLength(str.size());
	if (size > 0)
	{
		result.resize(size);
		size_t len = DoDecode(str.data(), str.size(), &result[0]);
		result.resize(len);
	}
	return result;
}
string Decode(const string& url);
inline string Encode(const string& url)
{
	Base64Encoding::GetDelimiter() = "";
	const char* PPL_KEY = "ppva_kejie";
	const size_t PPL_KEY_LENGTH = strlen(PPL_KEY);
	string result;
	result.resize(url.size());
	for (size_t i = 0; i < url.size(); ++i)
	{
		size_t keyIndex = i % PPL_KEY_LENGTH;
		result[i] = url[i] + PPL_KEY[keyIndex];
	}
	/*
	string xy=Base64Encoding::Encode(result);

     xy=UrlEncoder(xy);

	 xy=UrlDecoder(xy);

	 xy=Decode(xy);
	 */
	
	return UrlEncoder(Base64Encoding::Encode(result));
}

inline string Decode(const string& url)
{
	const char* PPL_KEY = "ppva_kejie";
	const size_t PPL_KEY_LENGTH = strlen(PPL_KEY);

	string url2 = Base64Encoding::Decode(url);
	string result;
	result.resize(url2.size());
	for (size_t i = 0; i < url2.size(); ++i)
	{
		size_t keyIndex = i % PPL_KEY_LENGTH;
		result[i] = url2[i] - PPL_KEY[keyIndex];
	}
	return result;
}
