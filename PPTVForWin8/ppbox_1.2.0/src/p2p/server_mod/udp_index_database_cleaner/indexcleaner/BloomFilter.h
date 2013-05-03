#pragma once

#include "base/guid.h"
#include <stdlib.h>
#include <hash_map>

namespace udpstatistic
{
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

	inline UINT32 HashGuidA(const Guid& g)
	{
		return g.Data1^((g.Data2<<0x10)|g.Data3)^((g.Data4[0]<<0x18)|(g.Data4[1]<<0x10)|(g.Data4[2]<<0x8)|g.Data4[3])^((g.Data4[4]<<0x18)|(g.Data4[5]<<0x10)|(g.Data4[6]<<0x8)|g.Data4[7]);
	}

	inline UINT32 HashGuidB(const Guid& g)
	{
		return g.Data1^((g.Data2<<0x10)|g.Data3)^((g.Data4[2]<<0x18)|(g.Data4[0]<<0x10)|(g.Data4[1]<<0x8)|g.Data4[3])^((g.Data4[5]<<0x18)|(g.Data4[6]<<0x10)|(g.Data4[4]<<0x8)|g.Data4[7]);
	}

	inline UINT32 HashGuidC(const Guid& g)
	{
		return g.Data1^((g.Data2<<0x10)|g.Data3)^((g.Data4[3]<<0x18)|(g.Data4[2]<<0x10)|(g.Data4[0]<<0x8)|g.Data4[1])^((g.Data4[6]<<0x18)|(g.Data4[5]<<0x10)|(g.Data4[4]<<0x8)|g.Data4[7]);
	}

	class BloomFilter
	{
	public:

		const static UINT32 BLOCK_SIZE = sizeof(UINT32);
		UINT32* data;
		UINT32 SIZE;
		UINT64 MASK;
		UINT32 MAXN;
		UINT32 count;

		explicit BloomFilter(UINT32 size)
			: SIZE(size), MAXN(1U<<size), MASK((1ULL<<(size+5))-1)
		{
			data = new UINT32[MAXN];
			memset(data, 0, MAXN * BLOCK_SIZE);
			count = 0;
		}

		~BloomFilter()
		{
			delete[] data; 
		}

		void Clear()
		{
			memset(data, 0, MAXN * BLOCK_SIZE);
			count = 0;
		}

		UINT32 Count() { return count; }

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

//////////////////////////////////////////////////////////////////////////
//  just for string
		void AddString(const char* str)
		{
			SetBit(HashA(str));
			SetBit(HashB(str));
			SetBit(HashC(str));
			++count;
		}
		bool HasString(const char* str)
		{
			if (!HasBit(HashA(str))) return false;
			if (!HasBit(HashB(str))) return false;
			if (!HasBit(HashC(str))) return false;
			return true;
		}

//////////////////////////////////////////////////////////////////////////
//  just for Guid
		void AddGuid(const Guid& g)
		{
			SetBit(HashGuidA(g));
			SetBit(HashGuidB(g));
			SetBit(HashGuidC(g));
			++count;
		}

		bool HasGuid(const Guid& g)
		{
			if (!HasBit(HashGuidA(g))) return false;
			if (!HasBit(HashGuidB(g))) return false;
			if (!HasBit(HashGuidC(g))) return false;
			return true;
		}

	};
}