// SubPieceContent.h

#ifndef _STORAGE_SUB_PIECE_BUFFER_H_
#define _STORAGE_SUB_PIECE_BUFFER_H_

#include <framework/memory/MemoryPoolObject.h>

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

const size_t SUB_PIECE_SIZE = 1024;
const size_t SUB_PIECE_COUNT_PER_PIECE = 128;
const size_t PIECE_SIZE = SUB_PIECE_SIZE * SUB_PIECE_COUNT_PER_PIECE;
const size_t BLOCK_MIN_SIZE = 2*1024*1024;
const size_t BLOCK_MAX_COUNT = 50;

namespace base
{

    class SubPieceContent
        // use QuickMemoryPoolT<SubPieceContent, PrivateMemory>, framework::thread::NullLock
//        : public framework::memory::MemoryPoolObjectNoThrow<SubPieceContent>
    {
    public:
        typedef boost::intrusive_ptr<SubPieceContent> pointer;

        SubPieceContent()
            : buffer_(new SubPieceBuffer)
            , nref_(0)
        {
            assert(buffer_);
        }

    public:
//        static size_t get_left_capacity(){return pool_.left_object();};

        static size_t set_pool_capacity(size_t size)
        {
			assert( false );
			return 0;
//            return framework::memory::MemoryPoolObjectNoThrow<SubPieceContent>::set_pool_capacity(size);
        }

    public:
        typedef void (*unspecified_bool_type)();
        static void unspecified_bool_true() {}
        operator unspecified_bool_type() const
        {
            return buffer_ == NULL ? 0 : unspecified_bool_true;
        }

        operator boost::uint8_t *()
        {
            return buffer_->buffer;
        }

        operator boost::uint8_t const *() const
        {
            return buffer_->buffer;
        }

        template <typename T>
        friend T buffer_static_cast(
            SubPieceContent & buf)
        {
            return static_cast<T>(buf.buffer_->buffer);
        }

        template <typename T>
        friend T buffer_static_cast(
            SubPieceContent const & buf)
        {
            return static_cast<T>(static_cast<boost::uint8_t const *>(buf.buffer_->buffer));
        }

    private:
        // non copyable
        SubPieceContent(
            SubPieceContent const &);

        SubPieceContent & operator=(
            SubPieceContent const &);

    private:
        // non passive delete
        ~SubPieceContent()
        {
            delete buffer_;
            buffer_ = NULL;
        }

    private:
        friend void intrusive_ptr_add_ref(
            SubPieceContent * p)
        {
            ++p->nref_;
        }

        friend void intrusive_ptr_release(
            SubPieceContent * p)
        {
            if (--p->nref_ == 0) {
                delete p;
            }
        }

    public:
        static const size_t sub_piece_size = SUB_PIECE_SIZE;

    private:
        struct SubPieceBuffer
//            : framework::memory::MemoryPoolObjectNoThrow<SubPieceBuffer>
        {
            boost::uint8_t buffer[sub_piece_size];
        } * buffer_;

        boost::uint32_t nref_;
    };


}

#endif // _STORAGE_SUB_PIECE_BUFFER_H_
