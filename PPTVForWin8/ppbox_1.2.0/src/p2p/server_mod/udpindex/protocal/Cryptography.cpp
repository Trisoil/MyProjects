#include "stdafx.h"
#include "protocal/Cryptography.h"

//#include <boost/crc.hpp>

namespace protocal
{
	bool Cryptography::Encrypt(Buffer buffer, int peer_version)
	{
        if (peer_version > 0x0104)
        {
            CheckSum2(buffer, true);
        }
        else
        {
		    CheckSum(buffer, true);
        }
#ifdef ENCRYPT
		TEncrypt(buffer);
#endif
		return true;
	}

	bool Cryptography::Decrypt(Buffer buffer)
	{
#ifdef ENCRYPT
		TDecrypt(buffer);
#endif
		return CheckSum(buffer, false) || CheckSum2(buffer, false);
	}

	inline bool Cryptography::CheckSum(Buffer buffer, bool is_override = false)
	{
		u_int crc;
#ifdef CRC
		boost::crc_32_type result;
		result.process_bytes(buffer.data_.get() + CHECKSUM_LENGTH, buffer.length_ - CHECKSUM_LENGTH);
		crc = result.checksum();
#elif 1
		crc = 0x15926326184LL;
		const byte * const data = buffer.data_.get();
		size_t length = buffer.length_;
		size_t i;
		// 8 bytes 跳, 取2个4bytes, 减少循环次数
		for (i = CHECKSUM_LENGTH; i + BLOCK_SIZE < length; i += BLOCK_SIZE)
		{
			crc ^= ((*(u_int*)(data + i)) ^ (crc << 15) ^ (*(u_int*)(data + i + BLOCK_SIZE_HALF)) ^ (crc >> 7));
		}
		while (i < length) 
		{
			crc ^= ((crc >> 13) ^ (data[i++]&0xFF) ^ (crc << 7));
		}
#else
		unsigned long cksum=0;
		size_t size = buffer.length_ - CHECKSUM_LENGTH;
		byte const* buf = buffer.data_.get() + CHECKSUM_LENGTH;
		while (size > 1) 
		{
			cksum += *buf++;
			size -= sizeof(unsigned short);
		}
		if (size) 
		{
			cksum += *(byte*)buf;
		}
		cksum = (cksum >> 16) + (cksum & 0xffff);
		cksum += (cksum >>16);
		crc = (u_int)(~cksum);
#endif
		// get value
		u_int* p = (u_int*) buffer.data_.get();
		if (is_override)
		{
			*p = crc;
			return true;
		}
		else
		{
			return *p == crc;
		}
	}

    inline bool Cryptography::CheckSum2(Buffer buffer, bool is_override = false)
    {
        u_int crc = 0x20110312312LL;
        const byte * const data = buffer.data_.get();
        size_t length = buffer.length_;
        size_t i;
        // 8 bytes 跳, 取2个4bytes, 减少循环次数
        for (i = CHECKSUM_LENGTH; i + BLOCK_SIZE < length; i += BLOCK_SIZE)
        {
            crc ^= ((*(u_int*)(data + i)) ^ (crc << 14) ^ (*(u_int*)(data + i + BLOCK_SIZE_HALF)) ^ (crc >> 6));
        }
        while (i < length)
        {
            crc ^= ((crc >> 13) ^ (data[i++]&0xFF) ^ (crc << 7));
        }

        // get value
        u_int* p = (u_int*) buffer.data_.get();
        if (is_override)
        {
            *p = crc;
            return true;
        }
        else
        {
            return *p == crc;
        }
    }

	inline void Cryptography::TGetKey(Buffer buffer, u_int& k0, u_int& k1, u_int& k2, u_int& k3)
	{
		assert( buffer.length_ >= BLOCK_SIZE ); // 4 bytes check sum header

		k0 = *(u_int*)(buffer.data_.get());
		k1 = ((k0 << 24)|(k0 >> 8));
		k2 = ((k1 << 24)|(k1 >> 8));
		k3 = ((k2 << 24)|(k2 >> 8));
	}

	inline void Cryptography::TEncrypt(Buffer buffer)
	{
		u_int k0, k1, k2, k3;

		TGetKey(buffer, k0, k1, k2, k3);

		for (int i = BLOCK_SIZE; i + BLOCK_SIZE_TWICE < buffer.length_; i += BLOCK_SIZE_TWICE)
		{
			u_int* v = (u_int*) (buffer.data_.get() + i);
			u_int v0 = v[0], v1 = v[1], sum = 0;
			for (int j = 0; j < ENCRYPT_ROUNDS; j++)
			{
				sum += DELTA;
				v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
				v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3); 
			}
			v[0] = v0; v[1] = v1;
		}
	}

	inline void Cryptography::TDecrypt(Buffer buffer)
	{
		u_int k0, k1, k2, k3;

		TGetKey(buffer, k0, k1, k2, k3);

		for (int i = BLOCK_SIZE; i + BLOCK_SIZE_TWICE < buffer.length_; i += BLOCK_SIZE_TWICE)
		{
			u_int* v = (u_int*) (buffer.data_.get() + i);
			u_int v0 = v[0], v1 = v[1], sum = FINAL_SUM;
			for (int j = 0; j < ENCRYPT_ROUNDS; j++)
			{
				v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
				v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
				sum -= DELTA;
			}
			v[0] = v0; v[1] = v1;
		}
	}
}
