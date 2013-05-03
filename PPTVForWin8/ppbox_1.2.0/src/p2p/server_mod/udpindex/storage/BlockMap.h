#pragma once
//#include "bitfield.h"
#include <boost\dynamic_bitset.hpp>

namespace p2sp
{
	class BlockMap
		:boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<BlockMap> p;
	public:
		static BlockMap::p ParseFormBinary(Buffer inbuf)
		{
			BlockMap::p pointer;
			if( ! Check(inbuf) )
				return pointer;

			int bitmap_size = *(int*)inbuf.data_.get();

			pointer = BlockMap::p(new BlockMap(inbuf.data_.get()+4,inbuf.length_-4,bitmap_size));
			return pointer;	
		};
		static BlockMap::p Create(u_int bitmap_size)
		{
			BlockMap::p pointer;
			pointer = BlockMap::p(new BlockMap(NULL,0,bitmap_size));
			return pointer;	
		};
 
		inline bool HasBlock(u_int index)
		{
			return bitset_.test(index);
		};

		u_int GetCount() const
		{
			return bitset_.count();
		}

		Buffer MakeToBinary() const 
		{
			//分配Buffer
			Buffer outbuf(bitset_.num_blocks()+4);
			*(int*)outbuf.data_.get() = bitset_.size();
			for(int i = 0; i<outbuf.length_-4;i++)
			{
				outbuf.data_[i+4] = bitset_.m_bits[i];
			}
			//拷贝位图
			return outbuf;
		};
		void Resize(u_int num_bits, bool value = false)
		{
			bitset_.resize(num_bits,value);
		}

		void SetAll(bool value)
		{
			if(value)
				bitset_.set();
			else
				bitset_.reset();
		}
		void Set(int index)
		{
			assert(index<bitset_.size());
			bitset_.set(index);
		}

	private:
		static bool Check(Buffer inbuf)
		{
			return inbuf.length_>=4&&inbuf.length_<u_int(-1);
		}

		BlockMap(byte* inbuf, u_int bytes_num,u_int bits_num)
		{
			if(inbuf==NULL)
			{
				bitset_.resize(bits_num,false);
			}
			for(int i = 0;i<bytes_num;i++)
			{
				bitset_.append(inbuf[i]);
			}
			if(bits_num>0)
				bitset_.resize(bits_num);
		};

		/// 原始的位数据(以压缩形式存储于字节数组中)
		boost::dynamic_bitset<u_char> bitset_;
	};
};
