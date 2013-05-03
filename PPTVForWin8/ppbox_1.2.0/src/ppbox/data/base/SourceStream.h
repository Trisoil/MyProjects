// SourceStream.h

#ifndef _PPBOX_DATA_BASE_SOURCE_STREAM_H_
#define _PPBOX_DATA_BASE_SOURCE_STREAM_H_

#include "ppbox/data/base/DataBase.h"
#include "ppbox/data/base/Buffer.h"

#include <util/event/Event.h>
#include <util/stream/StreamBuffers.h>

#include <streambuf>

namespace ppbox
{
    namespace data
    {

        class SingleSource;

        class SourceStream
            : public Buffer
            , public std::basic_streambuf<boost::uint8_t>
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &,
                size_t)
            > prepare_response_type;

        public:
            SourceStream(
                ppbox::data::SingleSource & source, 
                boost::uint32_t buffer_size, 
                boost::uint32_t prepare_size);

            ~SourceStream();

        public:
            bool seek(
                boost::uint64_t pos, 
                boost::uint64_t size, 
                boost::system::error_code & ec);

            bool seek(
                boost::uint64_t pos, 
                boost::system::error_code & ec);

            size_t prepare(
                size_t amount, 
                boost::system::error_code & ec);

            size_t prepare_some(
                size_t amount, 
                boost::system::error_code & ec)
            {
                return prepare(amount > prepare_size_ ? amount : prepare_size_, ec);
            }

            size_t prepare_some(
                boost::system::error_code & ec)
            {
                return prepare(prepare_size_, ec);
            }

            size_t prepare_at_least(
                size_t amount, 
                boost::system::error_code & ec);

            void async_prepare(
                size_t amount, 
                prepare_response_type const & resp);

            void async_prepare_some(
                size_t amount, 
                prepare_response_type const & resp)
            {
                async_prepare(amount > prepare_size_ ? amount : prepare_size_, resp);
            }

            bool fetch(
                boost::uint64_t offset, 
                boost::uint32_t size, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            bool drop(
                boost::system::error_code & ec);

            bool drop_to(
                boost::uint64_t offset, 
                boost::system::error_code & ec);

            bool read_seek(
                boost::uint64_t pos, 
                boost::system::error_code & ec);

        public:
            ppbox::data::SingleSource const & source() const
            {
                return source_;
            }

            // 获取最后一次错误的错误码
            boost::system::error_code last_error() const
            {
                return last_ec_;
            }

            void pause_stream();

            void resume_stream();

        private:
            void on_event(
                util::event::Event const & event);

        private:
            bool source_seek(
                boost::system::error_code & ec);

            void handle_async(
                boost::system::error_code const & ecc, 
                size_t bytes_transferred);

        private:
            boost::uint64_t position();

            void update();

        private:
            virtual int_type underflow();

            virtual pos_type seekoff(
                off_type off, 
                std::ios_base::seekdir dir,
                std::ios_base::openmode mode);

            virtual pos_type seekpos(
                pos_type position, 
                std::ios_base::openmode mode);

        private:
            ppbox::data::SingleSource & source_;
            boost::uint32_t prepare_size_;  // 下载一次，最大的下载数据大小
            util::stream::StreamMutableBuffers prepare_buffers_;
            prepare_response_type resp_;
            boost::uint64_t stream_pos_;
            boost::uint64_t total_size_;
            boost::system::error_code last_ec_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_SOURCE_STREAM_H_
