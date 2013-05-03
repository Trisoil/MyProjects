//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// SubPieceBuffer.h

#ifndef _STORAGE_BUFFER_H_
#define _STORAGE_BUFFER_H_

#include "SubPieceContent.h"
#ifdef DUMP_OBJECT
#include "../../../peer/count_object_allocate.h"
#endif

namespace protocol
{
    template <typename ContentType>
    struct SubPieceBufferImp
#ifdef DUMP_OBJECT
        : public count_object_allocate<SubPieceBufferImp<typename ContentType> >
#endif
    {
    private:
        typedef typename ContentType::pointer ContentTypePtr;
        ContentTypePtr data_;
        boost::uint32_t length_;
        boost::uint32_t offset_;

    public:
        SubPieceBufferImp()
            : length_(0)
            , offset_(0)
        {
        }

        SubPieceBufferImp(
            SubPieceBufferImp<ContentType> const & buffer)
        {
            if (this != &buffer)
            {
                data_ = buffer.data_;
                length_ = buffer.length_;
                offset_ = buffer.offset_;
            }
        }

        SubPieceBufferImp & operator = (
            SubPieceBufferImp<ContentType> const & buffer)
        {
            if (this != &buffer)
            {
                data_ = buffer.data_;
                length_ = buffer.length_;
                offset_ = buffer.offset_;
            }
            return *this;
        }

        SubPieceBufferImp(
            ContentType * data,
            boost::uint32_t length = ContentType::sub_piece_size)
            : data_(data)
            , length_(length)
            , offset_(0)
        {
        }

        SubPieceBufferImp(
            ContentTypePtr const & data,
            boost::uint32_t length = ContentType::sub_piece_size)
            : data_(data)
            , length_(length)
            , offset_(0)
        {
        }

        ContentTypePtr GetSubPieceBuffer() const
        {
            return data_;
        }

        boost::uint8_t * Data()
        {
            return *data_;
        }
/*
        boost::uint8_t * Data(
            boost::uint32_t offset)
        {
            return (boost::uint8_t *)*data_ + offset;
        }
*/
        boost::uint8_t const * Data() const
        {
            return data_ ? (boost::uint8_t*)(*data_) : NULL;
        }
/*
        boost::uint8_t const * Data(
            boost::uint32_t offset) const
        {
            return (boost::uint8_t const *)*data_ + offset;
        }
*/
        boost::uint32_t Length() const
        {
            return length_;
        }

        boost::uint32_t Offset() const
        {
            return offset_;
        }

        void Data(
            ContentTypePtr const & data)
        {
            data_ = data;
        }

        void Length(
            boost::uint32_t length)
        {
            length_ = length;
        }

        bool IsValid(boost::uint32_t subpiece_max_length) const
        {
            return Data() && length_ > 0 && length_ <= subpiece_max_length;
        }
/*
        void Offset(
            boost::uint32_t offset)
        {
            offset_ = offset;
        }

        SubPieceBuffer SubBuffer(
            boost::uint32_t offset) const
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
            boost::uint32_t offset,
            boost::uint32_t length) const
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

        bool operator == (const SubPieceBufferImp<ContentType>& buffer) const
        {
            return data_.get() == buffer.data_.get() && length_ == buffer.length_ && offset_ == buffer.offset_;
        }

        friend bool operator <(
            const SubPieceBufferImp<ContentType>& b1,
            const SubPieceBufferImp<ContentType>& b2)
        {
            return b1.data_.get() < b2.data_.get();
        }
    };

    typedef SubPieceBufferImp<SubPieceContent> SubPieceBuffer;
    typedef SubPieceBufferImp<LiveSubPieceContent> LiveSubPieceBuffer;
}
#endif  // _STORAGE_BUFFER_H_
