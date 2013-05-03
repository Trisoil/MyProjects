// SingleSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SingleSource.h"
#include "ppbox/data/base/SourceError.h"
#include "ppbox/data/base/SourceEvent.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/timer/TimeCounter.h>

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SingleSource", framework::logger::Debug);

        SingleSource::SingleSource(
            framework::string::Url const & url, 
            ppbox::data::SourceBase & source, 
            size_t total_req)
            : util::stream::Source(source.get_io_service())
            , max_try_(size_t(-1))
            , max_req_(total_req)
            , time_out_(0)
            , url_(url)
            , source_(source)
            , num_try_(0)
            , source_open_(false)
            , source_is_open_(false)
            , seek_end_(0)
            , expire_pause_time_(framework::timer::Time::now())
        {
            write_range_.big_offset = invalid_size;
        }

        SingleSource::~SingleSource()
        {
        }

        boost::system::error_code SingleSource::seek(
            boost::uint64_t offset, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            //assert(segment.byte_range.big_pos() != write_.byte_range.big_pos() || seek_end_ == write_.byte_range.big_pos());

            close_source(ec);

            write_range_.pos = offset;
            write_range_.end = write_range_.big_offset;
            seek_end_ = (size == invalid_size) ? invalid_size : write_range_.pos + size;
            assert(seek_end_ >= write_range_.pos);
            if (seek_end_ < write_range_.end) {
                write_range_.end = seek_end_;
            }
            ec.clear();
            return ec;
        }

        void SingleSource::reset()
        {
            boost::system::error_code ec;
            seek(0, ec);
        }

        void SingleSource::pause(
            boost::uint32_t time)
        {
            boost::system::error_code ec;
            close_source(ec);
            expire_pause_time_ = framework::timer::Time::now() + framework::timer::Duration::milliseconds(time);
        }

        void SingleSource::set_time_out(
            boost::uint32_t time_out)
        {
            time_out_ = time_out / 1000;
            boost::system::error_code ec;
            source_.set_time_out(time_out, ec);
        }

        boost::system::error_code SingleSource::cancel(
            boost::system::error_code & ec)
        {
            source_error_ = boost::asio::error::operation_aborted;
            return source_.cancel(ec);
        }

        boost::system::error_code SingleSource::close(
            boost::system::error_code & ec)
        {
            close_source(ec);
            return ec;
        }

        std::size_t SingleSource::private_read_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            return prepare(buffers, ec);
        }

        void SingleSource::private_async_read_some(
            buffers_t const & buffers, 
            handler_t const & handler)
        {
            async_prepare(buffers, handler);
        }

        size_t SingleSource::prepare(
            buffers_t const & buffers, 
            boost::system::error_code & ec)
        {
            size_t bytes_transferred = 0;
            ec = source_error_;
            while (1) {
                if (ec) {
                } else if (write_range_.pos >= write_range_.end) {
                    ec = boost::asio::error::eof;
                } else if (!source_open_ && !open_source(ec)) {
                } else if (source_is_open_ || source_is_open(ec)) {
                    framework::timer::TimeCounter tc;
                    bytes_transferred = source_.read_some(buffers, ec);
                    if (tc.elapse() >= 20) {
                        LOG_DEBUG("[prepare] read elapse: " << tc.elapse() 
                            << " bytes_transferred: " << bytes_transferred);
                    }
                    increase_bytes(bytes_transferred);
                    write_range_.pos += bytes_transferred;
                    if (ec && !source_.continuable(ec)) {
                        LOG_WARN("[prepare] read_some: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
                        if (ec == boost::asio::error::eof) {
                            LOG_DEBUG("[prepare] read eof, offset: " << write_range_.pos
                                << " end: " << write_range_.end);
                        }
                    }
                } else {
                    if (!source_.continuable(ec)) {
                        LOG_WARN("[prepare] open_source: ec: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
                    } else {
                        increase_bytes(0);
                    }
                }
                if (source_error_) {
                    ec = source_error_;
                }
                if (!ec) {
                    break;
                }

                bool is_error = handle_error(ec);
                if (is_error) {
                    open_source(ec);
                } else {
                    break;
                }
            }
            return bytes_transferred;
        }

        // 返回false表示不能再继续了
        bool SingleSource::handle_error(
            boost::system::error_code& ec)
        {
            if (source_.continuable(ec)) {
                boost::uint32_t time_block_ = get_zero_interval();
                if (time_out_ > 0 && time_block_ > time_out_) {
                    LOG_WARN("source.read_some: timeout" << 
                        " --- failed " << num_try_ << " times");
                    ec = boost::asio::error::timed_out;
                    if (num_try_ < max_try_) {
                        return true;
                    }
                } else {
                    return false;
                }
            } else if (ec == boost::asio::error::eof) {
                if (write_range_.pos >= write_range_.end) {
                    return false;
                } else if (write_range_.big_offset == invalid_size) {
                    write_range_.big_offset = write_range_.end = write_range_.pos;
                    LOG_INFO("[handle_error] guess segment size " << write_range_.big_offset);
                    return false;
                } else if (num_try_ < max_try_) {
                    ec = boost::asio::error::connection_aborted;
                    return true;
                }
            } else if(source_.recoverable(ec)) {
                if (num_try_ < max_try_) {
                    return true;
                }
            }
            if (ec == boost::asio::error::would_block) {
                pause(5000);
                ec.clear();
            }
            if (ec)
                source_error_ = ec;
            return !ec;
        }

        void SingleSource::async_prepare(
            buffers_t const & buffers, 
            handler_t const & handler)
        {
            handle_async(buffers, handler, boost::system::error_code(), 0);
        }

        void SingleSource::handle_async(
            buffers_t const & buffers, 
            handler_t const & handler, 
            boost::system::error_code const & ecc, 
            size_t bytes_transferred)
        {
            boost::system::error_code ec = ecc;
            bool is_open_callback = false;
            if (bytes_transferred == (size_t)-1) {
                is_open_callback = true;
                bytes_transferred = 0;
            }
            if (ec && !source_.continuable(ec)) {
                if (is_open_callback) {
                    if (ec != source_error::no_more_segment) {
                        LOG_WARN("[handle_async] open_source: ec: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
                    }
                }
                if (source_is_open_) {
                    LOG_WARN("[handle_async] read_some: " << ec.message() << 
                        " --- failed " << num_try_ << " times");
                    if (ec == boost::asio::error::eof) {
                        LOG_DEBUG("[handle_async] read eof, offset: " << write_range_.pos
                            << " end: " << write_range_.end);
                    }
                }
            }
            increase_bytes(bytes_transferred);
            write_range_.pos += bytes_transferred;
            if (source_error_) {
                ec = source_error_;
            }
            if (ec) {
                bool is_error = handle_error(ec);
                if (is_error) {
                    reset_zero_interval();
                        async_open_segment(boost::bind(&SingleSource::handle_async, this, buffers, handler, _1, (size_t)-1));
                } else {
                    boost::system::error_code ec1;
                    close_source(ec1);
                    response(handler, ec, 0);
                }
            } else if (bytes_transferred > 0) {
                response(handler, ec, bytes_transferred);
            } else if (write_range_.pos >= write_range_.end) {
                ec = boost::asio::error::eof;
                handle_async(buffers, handler, ec, 0);
            } else if (!source_open_) {
                async_open_segment(boost::bind(&SingleSource::handle_async, this, buffers, handler, _1, (size_t)-1));
            } else {
                update_size(ec);
                source_.async_read_some(buffers, boost::bind(&SingleSource::handle_async, this, buffers, handler, _1, _2));
            }
        }

        void SingleSource::response(
            handler_t const & handler, 
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            boost::system::error_code ecc;
            handler(ec, bytes_transferred);
        }

        void SingleSource::update_size(
            boost::system::error_code & ec)
        {
            if (write_range_.big_offset == invalid_size) {
                write_range_.big_offset = source_.total(ec);
                if (ec) {
                    write_range_.big_offset = invalid_size; // source_.total()失败时返回0，需要重置
                } else {
                    write_range_.end = write_range_.big_offset;
                    if (write_range_.end > write_range_.big_offset)
                        write_range_.end = write_range_.big_offset;
                }
            }
        }

        bool SingleSource::open_source(
            boost::system::error_code & ec)
        {
            close_source(ec);

            reset_zero_interval();

            if ((framework::timer::Time::now() - expire_pause_time_) < framework::timer::Duration::milliseconds(0)) {
                ec = boost::asio::error::would_block;
                return false;
            }

            ++num_try_;

            write_range_.beg = write_range_.pos; // 记录开始位置

            source_open_ = true;

            source_.open(
                url_, 
                write_range_.pos, 
                write_range_.big_offset == write_range_.end ? invalid_size : write_range_.end, ec);

            if (ec && !source_.continuable(ec)) {
                LOG_WARN("[open_source] ec: " << ec.message() << 
                    " --- failed " << num_try_ << " times");
                return false;
            }

            LOG_DEBUG("[open_source] range: " << write_range_.pos << " - " << write_range_.end);

            return true;
        }

        void SingleSource::async_open_segment(
            SourceBase::response_type const & resp)
        {
            boost::system::error_code ec;

            close_source(ec);

            reset_zero_interval();

            ++num_try_;

            write_range_.beg = write_range_.pos; // 记录开始位置

            source_open_ = true;

            source_.async_open(
                url_, 
                write_range_.pos, 
                write_range_.end == write_range_.big_offset ? invalid_size : write_range_.end, 
                resp);
        }

        bool SingleSource::source_is_open(
            boost::system::error_code & ec)
        {
            if (!source_is_open_ && source_.is_open(ec)) {
                update_size(ec);
                source_is_open_ = true;
            }
            return source_is_open_;
        }

        bool SingleSource::close_source(
            boost::system::error_code & ec)
        {
            if (source_open_) {
                LOG_DEBUG("[close_source] range: " << write_range_.beg << " - "<< write_range_.end);
                source_.close(ec);
                source_open_ = false;
                source_is_open_ = false;
            }
            ec.clear();
            return true;
        }

    } // namespace demux
} // namespace ppbox
