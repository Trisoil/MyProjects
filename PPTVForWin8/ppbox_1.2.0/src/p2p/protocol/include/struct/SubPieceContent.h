//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// SubPieceContent.h

#ifndef _STORAGE_SUB_PIECE_BUFFER_H_
#define _STORAGE_SUB_PIECE_BUFFER_H_

#ifdef USE_MEMORY_POOL
    #include <framework/memory/MemoryPoolObject.h>
#endif

#include <boost/intrusive_ptr.hpp>

namespace protocol
{
    template <boost::uint32_t size>
    class SubPieceContentImp
#ifdef USE_MEMORY_POOL
        : public framework::memory::MemoryPoolObjectNoThrow<SubPieceContentImp<size> >
#endif
    {
    public:
        typedef boost::intrusive_ptr<SubPieceContentImp> pointer;

        SubPieceContentImp()
            : nref_(0)
        {
        }

        ~SubPieceContentImp()
        {
            assert(nref_ == 0);
        }

    public:
#ifdef USE_MEMORY_POOL
        static boost::uint32_t get_left_capacity()
        {
            return framework::memory::MemoryPoolObjectNoThrow<SubPieceContentImp<size> >::get_pool().left_object();
        }

        static boost::uint32_t get_num_object()
        {
            return framework::memory::MemoryPoolObjectNoThrow<SubPieceContentImp<size> >::get_pool().num_object();
        }

        static size_t get_consumption()
        {
            return framework::memory::MemoryPoolObjectNoThrow<SubPieceContentImp<size> >::get_pool().consumption();
        }

        static boost::uint32_t get_max_object()
        {
            return framework::memory::MemoryPoolObjectNoThrow<SubPieceContentImp<size> >::get_pool().max_object();
        }
#endif

    public:
        operator boost::uint8_t *()
        {
            return buffer_;
        }

        operator boost::uint8_t const *() const
        {
            return buffer_;
        }

        template <typename T>
        friend T buffer_static_cast(
            SubPieceContentImp<size> & buf)
        {
            return static_cast<T>(buf.buffer_);
        }

        template <typename T>
        friend T buffer_static_cast(
            SubPieceContentImp<size> const & buf)
        {
            return static_cast<T>(static_cast<boost::uint8_t const *>(buf.buffer_));
        }

        boost::uint8_t *get_buffer()
        {
            return buffer_;
        }

    private:
        // non copyable
        SubPieceContentImp(
            SubPieceContentImp<size> const &);

        SubPieceContentImp & operator = (
            SubPieceContentImp<size> const &);

    private:
        friend void intrusive_ptr_add_ref(
            SubPieceContentImp<size> * p)
        {
            ++p->nref_;
        }

        friend void intrusive_ptr_release(
            SubPieceContentImp<size> * p)
        {
            if (--p->nref_ == 0) {
                delete p;
            }
        }

    public:
        static const boost::uint32_t sub_piece_size = size;

    private:
        boost::uint8_t nref_;
        boost::uint8_t resv_[2];
        boost::uint8_t buffer_[size];
        boost::uint8_t resv2_;
    };

    typedef SubPieceContentImp<SUB_PIECE_SIZE> SubPieceContent;
    typedef SubPieceContentImp<LIVE_SUB_PIECE_SIZE> LiveSubPieceContent;
}

#endif  // _STORAGE_SUB_PIECE_BUFFER_H_
