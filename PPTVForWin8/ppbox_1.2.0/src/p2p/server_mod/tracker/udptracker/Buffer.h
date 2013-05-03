#pragma once

typedef u_char byte;

#ifndef ppassert
#define ppassert assert
#endif

struct Buffer
{
	boost::shared_array<byte> data_;
	size_t length_;

	const boost::uint8_t * Data()
	{
		return this->data_.get();
	}

	size_t Length()
	{
		return this->length_;
	}

	Buffer() : length_(0) {}

	Buffer(const Buffer& buffer)
	{
		if (this != &buffer)
		{
			data_ = buffer.data_;
			length_ = buffer.length_;
		}
	}

	Buffer& operator = (const Buffer& buffer)
	{
		if (this != &buffer)
		{
			data_ = buffer.data_;
			length_ = buffer.length_;
		}
		return *this;
	}

	explicit Buffer(size_t length) : length_(length), data_(new byte[length]) {}

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

	void Clear(byte padding = 0)
	{
		if (length_ > 0) {
			memset(data_.get(), padding, length_);
		}
	}

	bool operator ! () const
	{
		return length_ == 0;
	}

	operator bool () const
	{
		return length_ != 0;
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
		ppassert( buffer );
		ppassert( data >= buffer.data_.get());
		ppassert( data < buffer.data_.get() + buffer.length_);
	}

	TempBuffer(const Buffer& buffer, size_t offset, size_t length)
		: buffer_(buffer), data_(buffer_.data_.get() + offset), length_(length)
	{
		ppassert( buffer );
		ppassert( offset >= 0);
		ppassert( offset < buffer_.length_);
	}

	TempBuffer(const Buffer& buffer, byte* data)
		: buffer_(buffer), data_(data)
	{
		ppassert( buffer );
		ppassert( data >= buffer.data_.get());
		ppassert( data < buffer.data_.get() + buffer.length_);
		length_ = buffer.length_ - (data - buffer.data_.get());
	}

	TempBuffer(const Buffer& buffer, size_t offset)
		: buffer_(buffer), data_(buffer_.data_.get() + offset), length_(buffer_.length_-offset)
	{
		ppassert( buffer );
		ppassert( offset >= 0);
		ppassert( offset < buffer_.length_);
	}

	explicit TempBuffer()
		: data_(NULL), length_(0)
	{}

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

