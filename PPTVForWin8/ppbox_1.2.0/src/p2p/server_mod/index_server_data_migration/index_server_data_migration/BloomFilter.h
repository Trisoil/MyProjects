#pragma once
#include <stdlib.h>
#include <vector>

namespace index_migration
{

	//////////////////////////////////////////////////////////////////////////
	//
	inline UINT32 HashA(const char *arKey)
	{ 
		unsigned long h = 0, g; 

		while (*arKey != 0) {
			h = (h << 4) + *arKey++; 
			if ((g = (h & 0xF0000000))) { 
				h = h ^ (g >> 24); 
				h = h ^ g; 
			} 
		} 
		return h; 
	}
	inline UINT32 HashB(const char *str)
	{ 
		int i,l; 
		unsigned long ret=0; 
		unsigned short *s; 

		if (str == NULL) return(0); 
		l=(strlen(str)+1)/2; 
		s=(unsigned short *)str; 
		for (i=0; i<l;i++)
			ret^=(s[i]<<(i&0x0f)); 
		return(ret); 
	}
	inline UINT32 HashC(const char *key)
	{ 
		register UINT32 nr=1, nr2=4; 
		while (*key) 
		{ 
			nr^= (((nr & 63)+nr2)*((UINT32) (unsigned char) *key++))+ (nr << 8); 
			nr2+=3; 
		} 
		return((UINT32) nr); 
	} 
	inline UINT32 HashD(const char *str) 
	{ 
		register unsigned int h; 
		register unsigned char *p; 

		for(h=0, p = (unsigned char *)str; *p ; p++) 
			h = 31 * h + *p; 

		return h; 
	}
	inline UINT32 HashE(const char* c)
	{
		unsigned long ret=0;
		long n;
		unsigned long v;
		int r;

		if ((c == NULL) || (*c == '\0'))
			return(ret);

		n=0x100;
		while (*c)
		{
			v=n|(*c);
			n+=0x100;
			r= (int)((v>>2)^v)&0x0f;
			ret=(ret^(32-r));
			ret&=0xFFFFFFFFL;
			ret^=v*v;
			c++;
		}
		return((ret>>16)^ret);
	}
	typedef UINT32 (*HashFunc)(const char* str);

	static HashFunc funcs[] = {
		HashA, HashB, HashC, //HashD, //HashE
	};

	static UINT32 func_num = sizeof(funcs)/sizeof(funcs[0]);

	class BloomFilter
	{
	public:

		const static UINT32 BLOCK_SIZE = sizeof(UINT32);

		UINT32* data;
		UINT32 SIZE;
		UINT64 MASK;
		UINT32 MAXN;
	   
		BloomFilter(UINT32 size)
			: SIZE(size), MAXN(1U<<size), MASK((1LL<<(size+5))-1)
		{
			data = new UINT32[MAXN];
			memset(data, 0, MAXN * BLOCK_SIZE);
		}

		~BloomFilter()
		{
			delete[] data; 
		}

		void Clear()
		{
			memset(data, 0, MAXN * BLOCK_SIZE);
		}
		void SetBit(UINT32 pos)
		{
			pos &= MASK;

			UINT32 index = (pos >> 5);
			UINT32& block = data[index];
			UINT32 offset = (pos & 0x1Fu);
			block |= (1U << offset);
		}
		bool HasBit(UINT32 pos)
		{
			pos &= MASK;
			UINT32 index = (pos >> 5);
			UINT32 block = data[index];
			UINT32 offset = (pos & 0x1Fu);
			return (block & (1U<<offset)) != 0;
		}

		void AddString(const char* str)
		{
			/*
			for (int i = 0; i < func_num; i++)
			{
				UINT32 pos = (*(funcs[i]))(str);
				SetBit(pos);
			}
			*/
			SetBit(HashA(str));
			SetBit(HashB(str));
			SetBit(HashC(str));
		}
		bool HasString(const char* str)
		{
			/*
			for (int i = 0; i < func_num; i++)
			{
				UINT32 pos = (*(funcs[i]))(str);
				if (HasBit(pos) == false)
					return false;
			}
			return true;
			*/
			if (!HasBit(HashA(str))) return false;
			if (!HasBit(HashB(str))) return false;
			if (!HasBit(HashC(str))) return false;
			return true;
		}

	};

}