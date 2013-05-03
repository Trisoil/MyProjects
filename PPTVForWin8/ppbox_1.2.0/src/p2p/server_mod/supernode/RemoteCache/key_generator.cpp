/* ======================================================================
 *	key_generator.cpp
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	从nginx移植过来的生成url key的代码
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 */

#include "Common.h"
#include "key_generator.h"

#include <ctime>
#include <cstdlib>

using namespace std;

string KeyGenerator::GenKey()
{
	unsigned char bytes[16] = {0};
	string keypwd = "qqqqqww";
	char key[16] = {0};
	memcpy(key, keypwd.c_str(), keypwd.length());

	unsigned char result[33];

	unsigned int time_now = static_cast<unsigned int>(time(0));
	time_now -= 600;

	Time2Str(time_now, bytes, 16);

	string result_tmp = "";
	for (int i = 0; i < 16; i++)
	{
		result_tmp += (char)(bytes[i]);
		if (bytes[i]==0)
		{
			bytes[i] = (unsigned char)(rand() % 256);
		}
	}

	TEncrypt(bytes, 16, key, 16);

	Str2Hex(bytes, 16, result, 33);

	string result_str((const char*)result);

	return result_str;
// 
// 	string result_str = "";
// 	for (int i = 0; i < 33; i++)
// 	{
// 		result_str += (char)(result[i]);
// 	}
// 	return result_str;
}

void KeyGenerator::TEncrypt(unsigned char *buffer, unsigned int buf_size, 
							char* key, size_t len)
/*加密buffer大小不能小于16字节,32位时间戳后补任意字符凑齐后加密*/
{
	size_t i;
	unsigned int k0 = GetkeyFromstr(key, len), k1, k2, k3;
	TGetKey(&k0, &k1, &k2, &k3);
	for (i = 0; i + BLOCK_SIZE_TWICE <= buf_size; i += BLOCK_SIZE_TWICE)
	{
		unsigned int *v = (unsigned int*) (buffer + i);
		unsigned int v0 = v[0], v1 = v[1], sum = 0;
		size_t j;
		for (j = 0; j < ENCRYPT_ROUNDS; j++)
		{
			sum += DELTA;
			v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
			v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
		}
		v[0] = v0; v[1] = v1;
	}
}

unsigned int KeyGenerator::GetkeyFromstr(char* str, size_t len)
{
	size_t i=0;
	union tagkey
	{
		char ch[4];
		unsigned int key;
	}tmp_key;
	memset(&tmp_key,0,sizeof(tmp_key));

	for(i=0;i<len;i++)
	{
		tmp_key.ch[i%4] ^= str[i];
	}
	return tmp_key.key;
}

void KeyGenerator::TGetKey(const unsigned int *k0, unsigned int *k1, 
						   unsigned int *k2, unsigned int *k3 )
{
	*k1 = *k0<<8|*k0>>24;
	*k2 = *k0<<16|*k0>>16;
	*k3 = *k0<<24|*k0>>8;
}

void KeyGenerator::Time2Str(unsigned int timet , unsigned char str[],
							unsigned int len)
{
	for(unsigned int i=0; i<len && i < 8;i++)
	{
		str[i] = (unsigned char)((timet >> (28 - i % 8 * 4)) & 0xF);
		str[i] += (unsigned char)(str[i] > 9 ? 'a'-(unsigned char)10 : '0');
	}
}

int KeyGenerator::Str2Hex(unsigned char buffer[], unsigned int buf_size, 
						  unsigned char hexstr[], unsigned int hs_size)
{
	if (hs_size < 2*buf_size+1) return 0;

	for (unsigned int i = 0; i < buf_size; i++)
	{
		hexstr[2 * i] = (unsigned char)(buffer[i] & 0xF);
		hexstr[2 * i + 1] = (unsigned char)((buffer[i] >> 4) & 0xF);
	}
	for (unsigned int i = 0; i < 2 * buf_size; i++)
	{
		hexstr[i] += (unsigned char)(hexstr[i] > 9 ? 'a' - (unsigned char)10 : '0');
	}
	hexstr[2*buf_size] = (unsigned char)0;
	return 1;
}