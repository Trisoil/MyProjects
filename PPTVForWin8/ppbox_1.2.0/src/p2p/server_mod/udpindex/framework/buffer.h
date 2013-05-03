#pragma once

typedef u_char byte;

namespace framework
{
	struct Buffer
	{
		boost::shared_array<byte> data_;
		size_t length_;

		Buffer() : length_(0) {}

		Buffer(size_t length) : length_(length), data_(new byte[length]) {}

		Buffer(const char* data, size_t length) : length_(length), data_(new byte[length])
		{
			memcpy(data_.get(), data, length);
		}

		Buffer(const byte* data, size_t length) : length_(length), data_(new byte[length])
		{
			memcpy(data_.get(), data, length);
		}

		Buffer(boost::shared_array<byte> data, size_t length) : length_(length), data_(new byte[length])
		{
			memcpy(data_.get(), data.get(), length);
		}

		void Malloc(size_t length)
		{
			length_ = length;
			data_ = boost::shared_array<byte>(new byte[length_]);
		}

        Buffer Clone() const
        {
            return Buffer(data_, length_);
        }

		bool operator ! () const
		{
			return length_ == 0;
		}

		inline bool operator ==(const Buffer& buffer) const
		{
			return data_.get() == buffer.data_.get() && length_ == buffer.length_; 
		}
	};

	inline bool operator < (const Buffer& b1,const Buffer& b2 )
	{
		if( b1.data_.get() != b2.data_.get() )
			return b1.data_.get() < b2.data_.get();
		else
			return b1.length_ < b2.length_;		
	}

	struct TempBuffer
	{
		Buffer buffer_;
		byte* data_;
		size_t length_;

		TempBuffer(const Buffer& buffer, byte* data, size_t length)
			: buffer_(buffer), data_(data), length_(length)
		{
			assert( buffer );
			assert( data >= buffer.data_.get());
			assert( data < buffer.data_.get() + buffer.length_);
		}

		TempBuffer(const Buffer& buffer, size_t offset, size_t length)
			: buffer_(buffer), data_(buffer_.data_.get() + offset), length_(length)
		{
			assert( buffer );
			assert( offset >= 0);
			assert( offset < buffer_.length_);
		}

		TempBuffer(const Buffer& buffer, byte* data)
			: buffer_(buffer), data_(data)
		{
			assert( buffer );
			assert( data >= buffer.data_.get());
			assert( data < buffer.data_.get() + buffer.length_);
			length_ = buffer.length_ - (data - buffer.data_.get());
		}

		TempBuffer(const Buffer& buffer, size_t offset)
			: buffer_(buffer), data_(buffer_.data_.get() + offset), length_(buffer_.length_-offset)
		{
			assert( buffer );
			assert( offset >= 0);
			assert( offset < buffer_.length_);
		}

		inline byte* Get() const { return data_; }
		inline size_t GetLength() const { return length_; }

		inline bool operator ==(const TempBuffer& buffer)
		{
			return buffer_ == buffer.buffer_ && Get() == buffer.Get() && GetLength() == buffer.GetLength(); 
		}

		inline Buffer ToBuffer()
		{
			return Buffer( Get(), GetLength() );
		}
	};

	inline bool operator < (const TempBuffer& b1,const TempBuffer& b2 )
	{
        if (b1.buffer_ == b2.buffer_)
        {
		    if( b1.Get() != b2.Get() )
			    return b1.Get() < b2.Get();
		    else
			    return b1.GetLength() < b2.GetLength();
        }
        else
        {
            return b1.buffer_ < b2.buffer_;
        }
	}

}
