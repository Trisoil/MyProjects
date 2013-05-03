#pragma once
//#include "bitfield.h"
#include <boost\dynamic_bitset.hpp>

namespace base
{
	class BlockMap
		:boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<BlockMap> p;
	private:
		BlockMap(byte* inbuf, size_t bytes_num,size_t bits_num)
		{
			if(inbuf==NULL)
			{
				bitset_.resize(bits_num,false);
			}
			for(size_t i = 0;i<bytes_num;i++)
			{
				bitset_.append(inbuf[i]);
			}
			if(bits_num>0)
				bitset_.resize(bits_num);
		};
		BlockMap()
		{
		};
	public:
		static BlockMap::p ParseFromBinary(const Buffer& inbuf)
		{
			BlockMap::p pointer;
			if( ! Check(inbuf) )
				return pointer;

			int bitmap_size = *(int*)inbuf.data_.get();

			pointer = BlockMap::p(new BlockMap(inbuf.data_.get()+4,inbuf.length_-4,bitmap_size));
			return pointer;	
		};

		static BlockMap::p ParseFromBinary(const TempBuffer& inbuf)
		{
			BlockMap::p pointer;
			if( ! Check(inbuf) )
				return pointer;

			int bitmap_size = *(int*)inbuf.data_;

			pointer = BlockMap::p(new BlockMap(inbuf.data_+4, inbuf.length_-4, bitmap_size));
			return pointer;	
		};

		static BlockMap::p Create(u_int bitmap_size)
		{
			BlockMap::p pointer;
			pointer = BlockMap::p(new BlockMap(NULL,0,bitmap_size));
			return pointer;	
		};
 
		BlockMap::p Clone()
		{
			BlockMap::p new_block_map =  BlockMap::p(new BlockMap());
			new_block_map->bitset_ = bitset_;
			return new_block_map;
		};

		inline bool HasBlock(u_int index)
		{
			if(index>=bitset_.size())
			{
				return false;
			}
			return bitset_.test(index);
		};

		size_t GetCount() const
		{
			return bitset_.count();
		}

		size_t GetSize() const
		{
			return bitset_.size();
		}

		Buffer MakeToBinary() const 
		{
			//分配Buffer
			Buffer outbuf(bitset_.num_blocks()+4);
			*(UINT32*)outbuf.data_.get() = bitset_.size();
			for(size_t i = 0; i<outbuf.length_-4;i++)
			{
				outbuf.data_[i+4] = bitset_.test(i);
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
		void Set(UINT32 index)
		{
			assert(index<bitset_.size());
			bitset_.set(index);
		}
		void Reset(UINT32 index)
		{
			assert(index<bitset_.size());
			bitset_.reset(index);
		}
		bool IsFull()
		{
			return bitset_.count()==bitset_.size();
		}

		friend   ostream& operator<<(ostream& os, const BlockMap& bm)
		{
			return os << bm.bitset_;
		}


	protected:
		/// 原始的位数据(以压缩形式存储于字节数组中)
		boost::dynamic_bitset<u_char> bitset_;
	private:
		static bool Check(const Buffer& inbuf)
		{
			return inbuf.length_>=4&&inbuf.length_<u_int(-1);
		}

		static bool Check(const TempBuffer& inbuf)
		{
			return inbuf.length_>=4&&inbuf.length_<u_int(-1);
		}
	};
};
