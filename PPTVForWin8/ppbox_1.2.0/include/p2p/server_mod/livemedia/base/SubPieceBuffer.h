// SubPieceBuffer.h

#ifndef _STORAGE_BUFFER_H_
#define _STORAGE_BUFFER_H_

#include "SubPieceContent.h"

namespace base
{
	
    struct SubPieceBuffer
    {
    private:
        SubPieceContent::pointer data_;
        size_t length_;
        size_t offset_;

    public:
        SubPieceBuffer()
            : length_(0)
            , offset_(0)
        {
        }

        SubPieceBuffer(
            SubPieceBuffer const & buffer)
        {
            if (this != &buffer)
            {
                data_ = buffer.data_;
                length_ = buffer.length_;
                offset_ = buffer.offset_;
            }
        }

        SubPieceBuffer & operator =(
            SubPieceBuffer const & buffer)
        {
            if (this != &buffer)
            {
                data_ = buffer.data_;
                length_ = buffer.length_;
                offset_ = buffer.offset_;
            }
            return *this;
        }

        SubPieceBuffer(
            SubPieceContent * data, 
            size_t length = SubPieceContent::sub_piece_size)
            : data_(data)
            , length_(length)
            , offset_(0)
        {
        }

        SubPieceBuffer(
            SubPieceContent::pointer const & data, 
            size_t length = SubPieceContent::sub_piece_size)
            : data_(data)
            , length_(length)
            , offset_(0)
        {
        }

        SubPieceContent::pointer GetSubPieceBuffer() const
        {
            return data_;
        }

        boost::uint8_t * Data()
        {
            return *data_;
        }
/*
        boost::uint8_t * Data(
            size_t offset)
        {
            return (boost::uint8_t *)*data_ + offset;
        }
*/
        boost::uint8_t const * Data() const
        {
            return *data_;
        }
/*
        boost::uint8_t const * Data(
            size_t offset) const
        {
            return (boost::uint8_t const *)*data_ + offset;
        }
*/
        size_t Length() const
        {
            return length_;
        }

		size_t Offset() const
        {
            return offset_;
        }

		void Data(
            SubPieceContent::pointer const & data)
        {
            data_ = data;
        }

        void Length(
            size_t length)
        {
            length_ = length;
        }
/*
        void Offset(
            size_t offset)
        {
            offset_ = offset;
        }

        SubPieceBuffer SubBuffer(
            size_t offset) const
        {
            SubPieceBuffer buffer;
            if (offset < length_)
            {
                buffer.offset_ = offset_ + offset;
                buffer.length_ = length_ - offset;
                buffer.data_ = data_;
            }
            return buffer;
        }

        SubPieceBuffer SubBuffer(
            size_t offset, 
            size_t length) const
        {
            SubPieceBuffer buffer;
            if (offset + length <= length_)
            {
                buffer.offset_ = offset_ + offset;
                buffer.length_ = length;
                buffer.data_ = data_;
            }
            return buffer;
        }

        void Clear(boost::uint8_t padding = 0)
        {
            if (length_ > 0)
            {
                memset(Data(), padding, length_);
            }
        }
*/
        bool operator !() const
        {
            return !data_;
        }

        operator bool() const
        {
            return data_;
        }

        bool operator ==(const SubPieceBuffer& buffer) const
        {
            return data_.get() == buffer.data_.get() && length_ == buffer.length_ && offset_ == buffer.offset_;
        }

        friend bool operator <(
            const SubPieceBuffer& b1, 
            const SubPieceBuffer& b2)
        {
            return b1.data_.get() < b2.data_.get();
        }
    };

}
#endif // _STORAGE_BUFFER_H_
