// BitsIStream.h

#ifndef _PPBOX_AVFORMAT_STREAM_BITS_I_STREAM_H_
#define _PPBOX_AVFORMAT_STREAM_BITS_I_STREAM_H_

#include "ppbox/avformat/stream/BitsType.h"

#include <util/archive/StreamIArchive.h>

namespace ppbox
{
    namespace avformat
    {

        template <
            typename _Elem = char, 
            typename _Traits = std::char_traits<_Elem>
        >
        class BitsIStream;

    } // namespace avformat
} // namespace ppbox

namespace util
{
    namespace serialization
    {

        template<
            typename _Elem, 
            typename _Traits, 
            size_t n
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::I<n> >
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits, 
            size_t n
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::U<n> >
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits, 
            typename V
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::IV<V> >
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits, 
            typename V
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::UV<V> >
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::SE>
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::UE>
            : boost::true_type
        {
        };

        template<
            typename _Elem, 
            typename _Traits
        >
        struct is_primitive<ppbox::avformat::BitsIStream<_Elem, _Traits>, ppbox::avformat::P>
            : boost::true_type
        {
        };

    } // namespace serialization
} // namespace util

namespace ppbox
{
    namespace avformat
    {

        template <
            typename _Elem, 
            typename _Traits
        >
        class BitsIStream
            : public util::archive::StreamIArchive<BitsIStream<_Elem, _Traits>, _Elem, _Traits>
        {
            friend class util::archive::StreamIArchive<BitsIStream<_Elem, _Traits>, _Elem, _Traits>;
        public:
            BitsIStream(
                std::basic_istream<_Elem, _Traits> & is)
                : util::archive::StreamIArchive<BitsIStream<_Elem, _Traits>, _Elem, _Traits>(*is.rdbuf())
                , this_byte_(0)
                , size_this_byte_(0)
                , mask_this_byte_(0)
            {
            }

            BitsIStream(
                std::basic_streambuf<_Elem, _Traits> & buf)
                : util::archive::StreamIArchive<BitsIStream<_Elem, _Traits>, _Elem, _Traits>(buf)
                , this_byte_(0)
                , size_this_byte_(0)
                , mask_this_byte_(0)
            {
            }

        public:
            /// 从流中读出变量
            template<class T>
            void load(
                T & t)
            {
                boost::uint32_t l = sizeof(T) * 8;
                boost::uint32_t v = 0;
                this->read_bits_flc(l, v);
                t = (T)v;
            }

            template<size_t N>
            void load(
                ppbox::avformat::I<N> & i)
            {
                boost::uint32_t l = N;
                boost::uint32_t v = 0;
                this->read_bits_flc(l, v);
                i = v;
            }

            template<size_t N>
            void load(
                ppbox::avformat::U<N> & u)
            {
                boost::uint32_t l = N;
                boost::uint32_t v = 0;
                this->read_bits_flc(l, v);
                u = v;
            }

            template<typename V>
            void load(
                ppbox::avformat::IV<V> & i)
            {
                boost::uint32_t l = i();
                boost::uint32_t v = 0;
                this->read_bits_flc(l, v);
                i = v;
            }

            template<typename V>
            void load(
                ppbox::avformat::UV<V> & u)
            {
                boost::uint32_t l = u();
                boost::uint32_t v = 0;
                this->read_bits_flc(l, v);
                u = v;
            }

            void load(
                ppbox::avformat::SE & se)
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                this->read_bits_vlc(l, v);
                se.parse(l, v);
            }

            void load(
                ppbox::avformat::UE & ue)
            {
                boost::uint32_t l = 0;
                boost::uint32_t v = 0;
                this->read_bits_vlc(l, v);
                ue.parse(l, v);
            }

            void load(
                ppbox::avformat::P & p)
            {
                if (size_this_byte_ == 0)
                    next_byte();
                p.data(this_byte_ & mask_this_byte_);
                p.size(size_this_byte_);
            }

            using util::archive::StreamIArchive<BitsIStream<_Elem, _Traits>, _Elem, _Traits>::load;

            bool more()
            {
                if (size_this_byte_ == 0)
                    next_byte();
                return this->buf_->sgetc() != _Traits::eof();
            }

            bool byte_aligned()
            {
                return size_this_byte_ == 0;
            }

        private:
            void read_bits_flc(
                boost::uint32_t l, 
                boost::uint32_t & v)
            {
                //std::cout << "read_bits_flc beg: this_byte = " << (size_t)this_byte_ << ", size_this_byte = " << size_this_byte_ << ", l = " << l << std::endl;
                if (this->state()) {
                    return;
                }
                v = 0;
                while (l > size_this_byte_) {
                    v = (v << size_this_byte_) | (((boost::uint32_t)(this_byte_)) & mask_this_byte_);
                    l -= size_this_byte_;
                    next_byte();
                    if (this->state()) {
                        return;
                    }
                }
                if (l) {
                    v = (v << l) | ((((boost::uint32_t)(this_byte_)) >> (size_this_byte_ - l)) & ((1 << l) - 1));
                    size_this_byte_ -= l;
                    mask_this_byte_ >>= l;
                }
                //std::cout << "read_bits_flc end: this_byte = " << (size_t)this_byte_ << ", size_this_byte = " << size_this_byte_ << ", v = " << v << std::endl;
            }

            // 0001001 : l = 3, v = 001
            void read_bits_vlc(
                boost::uint32_t & l, 
                boost::uint32_t & v)
            {
                //std::cout << "read_bits_vlc beg: this_byte = " << (size_t)this_byte_ << ", size_this_byte = " << size_this_byte_ << std::endl;
                if (this->state()) {
                    return;
                }
                v = ((boost::uint32_t)(this_byte_)) & mask_this_byte_;
                l = 0;
                while (true) {
                    size_t n = 0;
                    while (v) {
                        ++n;
                        v >>= 1;
                    }
                    if (n) {
                        n = size_this_byte_ - n;
                        l += n++;
                        size_this_byte_ -= n;
                        mask_this_byte_ >>= n;
                        break;
                    } else {
                        l += size_this_byte_;
                        next_byte();
                        if (this->state()) {
                            return;
                        }
                        v = (boost::uint32_t)(this_byte_);
                    }
                }
                read_bits_flc(l, v);
                //std::cout << "read_bits_vlc end: this_byte = " << (size_t)this_byte_ << ", size_this_byte = " << size_this_byte_ << std::endl;
            }
 
            void next_byte()
            {
                this->load_binary(&this_byte_, 1);
                if (this->state())
                    return;
                size_this_byte_ = 8;
                mask_this_byte_ = 0xff;
                this_byte_ = this_byte_ & mask_this_byte_;
            }
            
        private:
            _Elem this_byte_;
            boost::uint32_t size_this_byte_;
            boost::uint32_t mask_this_byte_;
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_STREAM_BITS_I_STREAM_H_
