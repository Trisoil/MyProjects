// M3u8MediaImpl.h

#ifndef _PPBOX_DATA_MEDIA_M3U8_MEDIA_IMPL_H_
#define _PPBOX_DATA_MEDIA_M3U8_MEDIA_IMPL_H_

#include "ppbox/data/base/MediaBase.h"
#include "ppbox/data/segment/SegmentInfo.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/mutex.hpp>

namespace ppbox
{
    namespace data
    {

        class SourceBase;

        class M3u8MediaImpl
        {
        public:
            M3u8MediaImpl(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~M3u8MediaImpl();

        public:
            void async_open(
                MediaBase::response_type const & resp);

            void cancel(
                boost::system::error_code & ec);

            void close(
                boost::system::error_code & ec);

        public:
            bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            void on_error(
                boost::system::error_code & ec) const;

        public:
            size_t segment_count() const;

            std::string const segment_protocol() const;

            bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const;

            bool segment_info(
                size_t segment, 
                SegmentInfo & info,
                boost::system::error_code & ec) const;

        private:
            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec);

            void handle_timer(
                boost::system::error_code ec);

            void parse_m3u8(
                boost::system::error_code & ec);

            void complete_url(
                framework::string::Url & url,
                std::string const & str) const;

        private:
            mutable boost::mutex mutex_;
            bool closed_;

            framework::string::Url url_;
            bool noshift_;

            MediaInfo info_;
            SourceBase * source_;
            boost::asio::deadline_timer timer_;
            boost::asio::streambuf buf_;

            struct StreamInfo;

            size_t seq_start_;
            size_t seq_lastest_;
            mutable size_t seq_play_;
            boost::uint64_t duration_;
            std::vector<StreamInfo> streams_;
            std::vector<std::string> stream_urls_;
            std::vector<SegmentInfo> segments_;
            std::vector<std::string> segment_urls_;
            mutable std::string segment_url_prefix_;
            mutable std::string::size_type segment_url_subpos_;

            MediaBase::response_type resp_;
        };

    } // data
} // ppbox

#endif // _PPBOX_DATA_MEDIA_M3U8_MEDIA_IMPL_H_
