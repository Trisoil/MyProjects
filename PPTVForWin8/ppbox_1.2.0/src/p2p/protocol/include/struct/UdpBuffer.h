//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// UdpBuffer.h

#ifndef _PEER_NETWORK_UDP_BUFFER_H_
#define _PEER_NETWORK_UDP_BUFFER_H_

#ifndef PEER_PC_CLIENT
    #include <framework/memory/MemoryPoolObject.h>
#endif

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/udp.hpp>

#include <struct/SubPieceBuffer.h>

namespace boost
{
    namespace asio
    {
        class mutable_buffers_3
        {
        public:
            typedef mutable_buffer value_type;

            typedef const mutable_buffer* const_iterator;

            mutable_buffers_3(
                mutable_buffer const & buffer0,
                mutable_buffer const & buffer1,
                mutable_buffer const & buffer2)
            {
                buffers[0] = buffer0;
                buffers[1] = buffer1;
                buffers[2] = buffer2;
            }

            mutable_buffers_3(
                mutable_buffer const & buffer0,
                mutable_buffer const & buffer1)
            {
                buffers[0] = buffer0;
                buffers[1] = buffer1;
            }

            mutable_buffers_3(
                mutable_buffer const & buffer0)
            {
                buffers[0] = buffer0;
            }

            const_iterator begin() const
            {
                return buffers;
            }

            const_iterator end() const
            {
                return buffers + 3;
            }

        private:
            mutable_buffer buffers[3];
        };

        class const_buffers_3
        {
        public:
            typedef const_buffer value_type;

            typedef const const_buffer* const_iterator;

            const_buffers_3(
                const_buffer const & buffer0)
            {
                buffers[0] = buffer0;
            }

            const_buffers_3(
                const_buffer const & buffer0,
                const_buffer const & buffer1)
            {
                buffers[0] = buffer0;
                buffers[1] = buffer1;
            }

            const_buffers_3(
                const_buffer const & buffer0,
                const_buffer const & buffer1,
                const_buffer const & buffer2)
            {
                buffers[0] = buffer0;
                buffers[1] = buffer1;
                buffers[2] = buffer2;
            }

            const_iterator begin() const
            {
                return buffers;
            }

            const_iterator end() const
            {
                if (boost::asio::buffer_cast<void const *>(buffers[2]))
                    return buffers + 3;
                else if (boost::asio::buffer_cast<void const *>(buffers[1]))
                    return buffers + 2;
                else
                    return buffers + 1;
            }

        private:
            const_buffer buffers[3];
        };

    }
}

namespace protocol
{
    typedef std::basic_istream<boost::uint8_t> IUdpBufferStream;
    typedef std::basic_ostream<boost::uint8_t> OUdpBufferStream;

    class UdpBuffer
        : public std::basic_streambuf<boost::uint8_t>
        // use QuickMemoryPoolT<UdpBuffer, PrivateMemory>, framework::thread::NullLock
#ifndef PEER_PC_CLIENT
        , public framework::memory::MemoryPoolObjectNoThrow<UdpBuffer>
#endif
    {
    public:
        UdpBuffer()
            : sub_piece_buffer_(new SubPieceContent)
        {
            assert(sub_piece_buffer_);
            setp(head_buffer_, head_buffer_ + head_size);
            setg(head_buffer_, head_buffer_, head_buffer_);
        }

    public:
        boost::asio::ip::udp::endpoint & end_point()
        {
            return endpoint_;
        }

        boost::asio::ip::udp::endpoint const & end_point() const
        {
            return endpoint_;
        }

        void end_point(
            boost::asio::ip::udp::endpoint const & ep)
        {
            endpoint_ = ep;
        }

    public:
        // 要想获取SubPieceContent，需要拿一个新的来交换
        void swap_sub_piece_buffer(
            SubPieceContent::pointer & buf)
        {
            assert(buf);
            sub_piece_buffer()[-1] = head_buffer_[head_size - 1];
            boost::uint8_t t = sub_piece_buffer()[sub_piece_size() - 1];
            sub_piece_buffer_.swap(buf);
            head_buffer_[head_size - 1] = sub_piece_buffer()[-1];
            sub_piece_buffer()[sub_piece_size() - 1] = t;
        }

        typedef void (*unspecified_bool_type)();
        static void unspecified_bool_true() {}
        operator unspecified_bool_type() const
        {
            return (sub_piece_buffer_) ? unspecified_bool_true : 0;
        }

    public:
        boost::asio::mutable_buffers_3 prepare()
        {
            assert(pptr() == head_buffer_);
            if (pbase() == head_buffer_) {
                return boost::asio::mutable_buffers_3(
                    boost::asio::mutable_buffer(pptr(), epptr() - pptr()),
                    boost::asio::mutable_buffer(sub_piece_buffer(), sub_piece_size()),
                    boost::asio::mutable_buffer(tail_buffer_, tail_size));
            } else if (pbase() == sub_piece_buffer()) {
                return boost::asio::mutable_buffers_3(
                    boost::asio::mutable_buffer(pptr(), epptr() - pptr()),
                    boost::asio::mutable_buffer(tail_buffer_, tail_size));
            } else {
                return boost::asio::mutable_buffers_3(
                    boost::asio::mutable_buffer(pptr(), epptr() - pptr()));
            }
        }

        void commit(
            boost::uint32_t size)
        {
            if ((boost::uint32_t)(epptr() - pptr()) >= size) {
                pbump(static_cast<int>(size));
            } else {
                size -= epptr() - pptr();
                if (pbase() == head_buffer_) {
                    if (size <= sub_piece_size()) {
                        setp(sub_piece_buffer(), sub_piece_buffer() + sub_piece_size());
                        pbump(static_cast<int>(size));
                        return;
                    } else {
                        size -= sub_piece_size();
                    }
                }
                assert(size <= tail_size);
                setp(tail_buffer_, tail_buffer_ + tail_size);
                pbump(static_cast<int>(size));
            }
        }

        boost::asio::const_buffers_3 data() const
        {
            if (eback() == head_buffer_) {
                if (pbase() == head_buffer_) {
                    return boost::asio::const_buffers_3(
                        boost::asio::const_buffer(gptr(), pptr() - gptr()));
                } else if (pbase() == sub_piece_buffer()) {
                    return boost::asio::const_buffers_3(
                        boost::asio::const_buffer(gptr(), head_buffer_ + head_size - gptr()),
                        boost::asio::const_buffer(sub_piece_buffer(), pptr() - sub_piece_buffer()));
                } else {
                    return boost::asio::const_buffers_3(
                        boost::asio::const_buffer(gptr(), head_buffer_ + head_size - gptr()),
                        boost::asio::const_buffer(sub_piece_buffer(), sub_piece_size()),
                        boost::asio::const_buffer(tail_buffer_, pptr() - tail_buffer_));
                }
            } else if (eback() == sub_piece_buffer()) {
                if (pbase() == sub_piece_buffer()) {
                    return boost::asio::const_buffers_3(
                        boost::asio::const_buffer(gptr(), pptr() - gptr()));
                } else {
                    return boost::asio::const_buffers_3(
                        boost::asio::const_buffer(gptr(), sub_piece_buffer() + sub_piece_size() - gptr()),
                        boost::asio::const_buffer(tail_buffer_, pptr() - tail_buffer_));
                }
            } else {
                return boost::asio::const_buffers_3(
                    boost::asio::const_buffer(gptr(), pptr() - gptr()));
            }
        }

        boost::uint32_t size() const
        {
            boost::uint32_t n = 0;
            if (pbase() == head_buffer_) {
                n = (pptr() - head_buffer_);
            } else if (pbase() == sub_piece_buffer()) {
                n = head_size + (pptr() - sub_piece_buffer());
            } else {
                n = head_size + sub_piece_size() + (pptr() - tail_buffer_);
            }
            if (eback() == head_buffer_) {
                n -= gptr() - head_buffer_;
            } else if (eback() == sub_piece_buffer()) {
                n -= head_size + (gptr() - sub_piece_buffer());
            } else {
                n -= head_size + sub_piece_size() + (gptr() - tail_buffer_);
            }
            return n;
        }

        void consume(
            boost::uint32_t size)
        {
            if ((boost::uint32_t)(egptr() - gptr()) >= size) {
                gbump(static_cast<int>(size));
            } else {
                size -= egptr() - gptr();
                do {
                    if (eback() == head_buffer_) {
                        if (egptr() + size <= head_buffer_ + head_size) {
                            setg(head_buffer_, gptr() + size, head_buffer_ + head_size);
                            break;
                        } else {
                            size -= head_buffer_ + head_size - egptr();
                        }
                        if (size <= sub_piece_size()) {
                            setg(sub_piece_buffer(), sub_piece_buffer() + size, sub_piece_buffer() + sub_piece_size());
                            break;
                        } else {
                            size -= sub_piece_size();
                        }
                        assert(size <= tail_size);
                        setg(tail_buffer_, tail_buffer_ + size, tail_buffer_ + tail_size);
                    } else if (eback() == sub_piece_buffer()) {
                        if (egptr() + size <= sub_piece_buffer() + sub_piece_size()) {
                            setg(sub_piece_buffer(), gptr() + size, sub_piece_buffer() + sub_piece_size());
                            break;
                        } else {
                            size -= head_buffer_ + head_size - egptr();
                        }
                        assert(size <= tail_size);
                        setg(tail_buffer_, tail_buffer_ + size, tail_buffer_ + tail_size);
                    } else {
                        assert(size <= tail_size);
                        setg(tail_buffer_, tail_buffer_ + size, tail_buffer_ + tail_size);
                    }
                    break;
                } while (1);
                if (eback() == pbase()) {
                    assert(gptr() <= pptr());
                    setg(eback(), gptr(), pptr());
                }
            }
        }

        void reset()
        {
            setp(head_buffer_, head_buffer_ + head_size);
            setg(head_buffer_, head_buffer_, head_buffer_);
        }

        boost::uint8_t * GetHeadBuffer()
        {
            return head_buffer_;
        }

    protected:
        boost::uint8_t * sub_piece_buffer()
        {
            if (sub_piece_buffer_ && *sub_piece_buffer_)
                return buffer_static_cast<boost::uint8_t *>(*sub_piece_buffer_) + 1;
            else
                return NULL;
        }

        boost::uint8_t const * sub_piece_buffer() const
        {
            if (sub_piece_buffer_ && *sub_piece_buffer_)
                return buffer_static_cast<boost::uint8_t const *>(*sub_piece_buffer_) + 1;
            return NULL;
        }

        boost::uint32_t sub_piece_size() const
        {
            return SubPieceContent::sub_piece_size;
        }

    protected:
        int_type underflow()
        {
            if (gptr() != pptr())
            {
                if (eback() == head_buffer_) {
                    if (egptr() == head_buffer_ + head_size)
                        setg(sub_piece_buffer(), sub_piece_buffer(), sub_piece_buffer() + sub_piece_size());
                    else
                        setg(head_buffer_, head_buffer_, head_buffer_ + head_size);
                } else if (eback() == sub_piece_buffer()) {
                    if (egptr() == sub_piece_buffer() + sub_piece_size())
                        setg(tail_buffer_, tail_buffer_, tail_buffer_ + tail_size);
                    else
                        setg(sub_piece_buffer(), sub_piece_buffer(), sub_piece_buffer() + sub_piece_size());
                } else {
                    setg(tail_buffer_, tail_buffer_, tail_buffer_ + tail_size);
                }
                if (eback() == pbase()) {
                    setg(eback(), gptr(), pptr());
                }
                return traits_type::to_int_type(*gptr());
            }
            else
            {
                return traits_type::eof();
            }
        }

        int_type overflow(int_type c)
        {
            if (!traits_type::eq_int_type(c, traits_type::eof()))
            {
                assert(pbase() != tail_buffer_);
                if (pbase() == head_buffer_)
                    setp(sub_piece_buffer(), sub_piece_buffer() + sub_piece_size());
                else
                    setp(tail_buffer_, tail_buffer_ + tail_size);
                *pptr() = traits_type::to_char_type(c);
                pbump(1);
                return c;
            }
            return traits_type::not_eof(c);
        }

    private:
        // non copyable
        UdpBuffer(
            UdpBuffer const &);

        UdpBuffer & operator = (
            UdpBuffer const &);

    private:
        static const boost::uint32_t head_size = 4 + 1 + 4 + 4 + 16 + 16 + 4 + 2 + 1;
        static const boost::uint32_t tail_size = 1024;

    private:
        boost::asio::ip::udp::endpoint endpoint_;
        boost::uint8_t head_buffer_[head_size];
        SubPieceContent::pointer sub_piece_buffer_;
        boost::uint8_t tail_buffer_[tail_size];
    };

}

#endif  // _PEER_NETWORK_UDP_BUFFER_H_
