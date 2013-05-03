// SourceStream.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SourceStream.h"

#include <ppbox/data/base/SingleSource.h>
#include <ppbox/data/base/SourceEvent.h>
#include <ppbox/data/base/SourceError.h>

#include <framework/system/LogicError.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SourceStream", framework::logger::Debug);

        SourceStream::SourceStream(
            ppbox::data::SingleSource & source, 
            boost::uint32_t buffer_size, 
            boost::uint32_t prepare_size)
            : Buffer(buffer_size)
            , source_(source)
            , prepare_size_(prepare_size)
            , stream_pos_(0)
            , total_size_(invalid_size)
        {
        }

        SourceStream::~SourceStream()
        {
        }

        // 目前只发生在，seek到一个分段，还没有该分段头部数据时，
        // 此时size为head_size_头部数据大小 
        // TO BE FIXED
        bool SourceStream::seek(
            boost::uint64_t pos, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            bool write_change = Buffer::seek(pos, size);

            if (write_change) {
                source_seek(ec);
            }

            read_seek(pos, ec);

            last_ec_ = ec;

            return !ec;
        }

        bool SourceStream::seek(
            boost::uint64_t pos, 
            boost::system::error_code & ec)
        {
            return seek(pos, invalid_size, ec);
        }

        size_t SourceStream::prepare(
            size_t amount, 
            boost::system::error_code & ec)
        {
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    next_write_hole();
                    source_seek(ec);
                }
                if (ec) {
                    last_ec_ = ec;
                    return 0;
                }
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            amount = source_.read_some(prepare_buffers_, ec);
            commit(amount);
            last_ec_ = ec;
            return amount;
        }

        size_t SourceStream::prepare_at_least(
            size_t amount, 
            boost::system::error_code & ec)
        {
            ec = last_ec_;
            size_t n = 0;
            do {
                n += prepare_some(amount - n, ec);
            } while (!ec && amount > n);
            return n;
        }

        void SourceStream::async_prepare(
            size_t amount, 
            prepare_response_type const & resp)
        {
            resp_ = resp;
            boost::system::error_code ec;
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    next_write_hole();
                    source_seek(ec);
                }
            }
            if (ec) {
                last_ec_ = ec;
                resp_(ec, 0);
                return;
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            source_.async_read_some(prepare_buffers_, 
                boost::bind(&SourceStream::handle_async, this, _1, _2));
        }

        void SourceStream::handle_async(
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            last_ec_ = ec;
            commit(bytes_transferred);
            prepare_response_type resp;
            resp.swap(resp_);
            resp(ec, 0);
        }

        bool SourceStream::fetch(
            boost::uint64_t offset, 
            boost::uint32_t size, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            assert(offset >= in_position() && offset + size <= total_size_);
            if (offset < in_position()) {
                ec = framework::system::logic_error::out_of_range;
            } else if (offset + size > total_size_) {
                ec = boost::asio::error::eof;
            } else {
                if (offset + size > out_position()) {
                    ec = last_ec_;
                    if (!ec) {
                        prepare_at_least((boost::uint32_t)(offset + size - out_position()), ec);
                    }
                }
                if (offset + size <= out_position()) {
                    Buffer::read_buffer(offset, offset + size, data);
                    ec.clear();
                }
            }
            return !ec;
        }

        bool SourceStream::drop(
            boost::system::error_code & ec)
        {
            if (consume((size_t)(position() - in_position()))) {
                stream_pos_ = in_position();
                setg(gptr(), gptr(), egptr());
                ec.clear();
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return !ec;
        }

        bool SourceStream::drop_to(
            boost::uint64_t offset, 
            boost::system::error_code & ec)
        {
            if (offset < in_position()) {
                ec = framework::system::logic_error::invalid_argument;
                return false;
            } else if (consume((size_t)(offset - in_position()))) {
                read_seek(offset, ec);
                return true;
            } else {
                ec = framework::system::logic_error::out_of_range;
                return false;
            }
        }

        bool SourceStream::read_seek(
            boost::uint64_t pos, 
            boost::system::error_code & ec)
        {
            boost::uint64_t beg = in_position();
            boost::uint64_t end = out_position();
            if (pos < beg) {
                pos = beg;
                ec = framework::system::logic_error::out_of_range;
            } else if (pos >= end) {
                if (pos > total_size_) {
                    pos = total_size_;
                    ec = framework::system::logic_error::out_of_range;
                }
                if (pos >= out_position()) {
                    boost::system::error_code ec1 = last_ec_;
                    if (!ec1) {
                        prepare_at_least((size_t)(pos - out_position()), ec1); // 尽量让buffer有数据
                    }
                    if (pos > out_position()) { // 如果没有，也不算失败
                        pos = out_position();
                        if (!ec) ec = ec1;
                    }
                    end = out_position();
                }
                assert(pos <= end);
            }
            boost::uint8_t * ptr = (boost::uint8_t *)read_buffer(beg, pos, end);
            pos -= beg;
            end -= beg;
            stream_pos_ = beg;
            setg(ptr, ptr + pos, ptr + end);
            return !ec;
        }

        void SourceStream::pause_stream()
        {
            if (!last_ec_)
                last_ec_ = boost::asio::error::would_block;
        }

        void SourceStream::resume_stream()
        {
            if (last_ec_ == boost::asio::error::would_block)
                last_ec_.clear();
        }

        bool SourceStream::source_seek(
            boost::system::error_code & ec)
        {
            boost::uint64_t size = write_hole_size();
            if (size == 0) {
                ec = source_error::at_end_point;
            }
            source_.seek(out_position(), size, ec);
            return !ec;
        }

        void SourceStream::on_event(
            util::event::Event const & ec)
        {
        }

        boost::uint64_t SourceStream::position()
        {
            pos_type pos = stream_pos_ + off_type(gptr() - eback());
            return pos;
        }

        void SourceStream::update()
        {
            boost::system::error_code ec;
            read_seek(position(), ec);
            assert(!ec);
        }

        SourceStream::int_type SourceStream::underflow()
        {
            update();
            if (gptr() < egptr()) {
                return *gptr();
            } else {
                return traits_type::eof();
            }
        }

        SourceStream::pos_type SourceStream::seekoff(
            off_type off, 
            std::ios_base::seekdir dir,
            std::ios_base::openmode mode)
        {
            if (dir == std::ios_base::cur) {
                pos_type pos = position();
                if (off == 0) {
                    return pos;
                }
                pos += off;
                return seekpos(pos, mode);
            } else if (dir == std::ios_base::beg) {
                return seekpos(off, mode);
            } else if (dir == std::ios_base::end) {
                assert(off <= 0);
                return seekpos((pos_type)out_position() + off, mode);
            } else {
                return pos_type(-1);
            }
        }

        SourceStream::pos_type SourceStream::seekpos(
            pos_type position, 
            std::ios_base::openmode mode)
        {
            assert(position != pos_type(-1));
            if (mode == (std::ios_base::in | std::ios_base::out)) {
                boost::system::error_code ec;
                if (!seek(position, ec)) {
                    return pos_type(-1);
                }
                mode = std::ios_base::in;
            }
            if (mode != std::ios_base::in) {
                return pos_type(-1);// 模式错误
            }
            boost::system::error_code ec;
            if (!read_seek(position, ec)) {
                return pos_type(-1);// 有效位置之前
            }
            return position;
        }

    } // namespace data
} // namespace ppbox
