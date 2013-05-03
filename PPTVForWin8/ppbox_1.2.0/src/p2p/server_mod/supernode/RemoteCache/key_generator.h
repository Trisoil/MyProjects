/* ======================================================================
 *	key_generator.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	从nginx移植过来的生成url key的代码
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 */

#ifndef __KEY_GENERATOR_H__
#define __KEY_GENERATOR_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>

using std::string;

typedef unsigned int u_int;

static const size_t ENCRYPT_ROUNDS = 32; // at least 32
static const u_int DELTA = 0x9E3779B9;
static const u_int FINAL_SUM = 0xC6EF3720;
static const size_t BLOCK_SIZE = (sizeof(unsigned int) << 1);
static const size_t BLOCK_SIZE_TWICE = ((sizeof(unsigned int) << 1) << 1);
static const size_t BLOCK_SIZE_HALF = ((sizeof(unsigned int) << 1) >> 1);

class KeyGenerator
{
public:
	static string GenKey();

private:
	static void TEncrypt(unsigned char *buffer, 
		unsigned int buf_size, char* key, size_t len);

	static unsigned int GetkeyFromstr(char* str, size_t len);

	static void TGetKey(const unsigned int *k0, unsigned int *k1, 
		unsigned int *k2, unsigned int *k3 );

	static void Time2Str(unsigned int timet , unsigned char str[], 
		unsigned int len);

	static int Str2Hex(unsigned char buffer[], unsigned int buf_size,
		unsigned char hexstr[], unsigned int hs_size);
};

#endif // __KEY_GENERATOR_H__