// SegmentStream.h

#ifndef _PPBOX_DATA_SEGMENT_SEGMENT_STREAM_H_
#define _PPBOX_DATA_SEGMENT_SEGMENT_STREAM_H_

#include "ppbox/data/segment/SegmentBuffer.h"

#include <util/buffers/BufferSize.h>
#include <util/buffers/StlBuffer.h>

#include <boost/asio/buffer.hpp>

namespace ppbox
{
    namespace data
    {

        class SegmentStream
            : public util::buffers::StlStream<boost::uint8_t>
        {
        public:
            friend class SegmentBuffer;

            typedef util::buffers::StlBuffer<
                util::buffers::detail::_read, boost::uint8_t, std::char_traits<boost::uint8_t> > buffer_type;

        public:
            SegmentStream(
                SegmentBuffer & buffer, 
                bool merge)
                : buffer_(buffer)
                , segment_(NULL)
                , merge_(merge)
                , pos_(0)
                , buf_(*this)
            {
                setg(NULL, NULL, NULL);
                // TODO: 有可能已经有数据了
            }

        private:
            bool update(
                SegmentBuffer::PositionType::Enum type, 
                pos_type pos)
            {
                boost::uint64_t pos64 = pos;
                boost::uint64_t off32 = 0;
                boost::asio::const_buffer buf;;
                bool ok = buffer_.segment_buffer(*segment_, type, merge_, pos64, off32, buf);
                pos_ = pos64;
                buf_ = buf;
                gbump(off32);
                return ok;
            }

            void update()
            {
                pos_type pos = pos_ + off_type(gptr() - eback());
                update(SegmentBuffer::PositionType::set, pos);
            }

            void close()
            {
                setg(NULL, NULL, NULL);
                pos_ = 0;
            }

        private:
            boost::uint64_t position()
            {
                pos_type pos = pos_ + off_type(gptr() - eback());
                return pos;
            }

            pos_type position2()
            {
                pos_type pos = (merge_ ? segment_->byte_range.big_offset : 0) + pos_ + off_type(gptr() - eback());
                return pos;
            }

            void drop_all()
            {
                update(SegmentBuffer::PositionType::set, 0);
            }

            void seek(
                boost::uint64_t pos)
            {
                update(SegmentBuffer::PositionType::set, pos);
            }

            void change_to(
                SegmentPosition & segment)
            {
                segment_ = &segment;
                setg(NULL, NULL, NULL);
                update(SegmentBuffer::PositionType::set, position2());
            }

        private:
            virtual int_type underflow()
            {
                pos_type pos = position2();
                if (update(SegmentBuffer::PositionType::set, pos) && (gptr() < egptr())) {
                    return *gptr();
                } else {
                    return traits_type::eof();
                }
            }

            virtual pos_type seekoff(
                off_type off, 
                std::ios_base::seekdir dir,
                std::ios_base::openmode mode)
            {
                if (dir == std::ios_base::cur) {
                    pos_type pos = position2();
                    if (off == 0) {
                        return pos;
                    }
                    pos += off;
                    return seekpos(pos, mode);
                } else if (dir == std::ios_base::beg) {
                    return seekpos(off, mode);
                } else if (dir == std::ios_base::end) {
                    assert(off <= 0);
                    if (!update(SegmentBuffer::PositionType::end, pos_type(-off))) {
                        return pos_type(-1);
                    }
                    return position2();
                } else {
                    return pos_type(-1);
                }
            }

            virtual pos_type seekpos(
                pos_type position, 
                std::ios_base::openmode mode)
            {
                assert(position != pos_type(-1));
                if (mode == (std::ios_base::in | std::ios_base::out)) {
                    if (!buffer_.segment_seek(*segment_, merge_, position)) {
                        return pos_type(-1);
                    }
                    mode = std::ios_base::in;
                }
                if (mode != std::ios_base::in) {
                    return pos_type(-1);// 模式错误
                }
                if (!update(SegmentBuffer::PositionType::set, position)) {
                    return pos_type(-1);// 有效位置之前
                }
                return position;
            }

        private:
            SegmentBuffer & buffer_;
            SegmentPosition * segment_;
            bool merge_;            // 是否合并分段数据
            pos_type pos_;          // 与iter_对应分段的开头
            buffer_type buf_;       // 当前的内存段
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_SEGMENT_STREAM_H_
