#pragma once

#include <boost/shared_array.hpp>

namespace statistic
{
	// Be careful
	template <typename StructWithTailEmptySizeArray, typename TailElement>
	class VarSizeStruct // 基类不能有大小为0的数组
	{
	public:

		typedef VarSizeStruct<StructWithTailEmptySizeArray, TailElement> Type;

	public:

		VarSizeStruct(u_int tail_element_count = 0)
			: tail_element_count_(tail_element_count)
			, data_(new byte[ sizeof(StructWithTailEmptySizeArray) + tail_element_count * sizeof(TailElement) ])
		{
            Clear();
		}
/*
		VarSizeStruct(const Type& value)
		{
			tail_element_count_ = value.tail_element_count_;
			data_ = boost::shared_array<byte>(new byte[ value.Size() ]);
			memcpy(data_.get(), value.data_.get(), value.Size());
		}

		Type& operator = (const Type& value)
		{
			tail_element_count_ = value.tail_element_count_;
			data_ = boost::shared_array<byte>(new byte[ value.Size() ]);
			memcpy(data_.get(), value.data_.get(), value.Size());
		}
*/
		StructWithTailEmptySizeArray& operater()
		{
			return Ref();
		}

		StructWithTailEmptySizeArray& Ref()
		{
			return *(StructWithTailEmptySizeArray*)data_.get();
		}

		void Clear()
		{
			memset(data_.get(), 0, Size());
		}

		u_int Size() const
		{
			return sizeof(StructWithTailEmptySizeArray) + tail_element_count_ * sizeof(TailElement);
		}

		u_int MaxTailElementCount() const
		{
			return tail_element_count_;
		}

	private:

		boost::shared_array<byte> data_;

		u_int tail_element_count_;
	};
}
