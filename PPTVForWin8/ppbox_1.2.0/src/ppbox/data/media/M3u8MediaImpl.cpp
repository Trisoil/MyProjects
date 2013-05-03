
// M3u8Media.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/media/M3u8MediaImpl.h"
#include "ppbox/data/base/SourceBase.h"
#include "ppbox/data/base/SourceError.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Algorithm.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>

#include <sstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.M3u8Media", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        M3u8MediaImpl::M3u8MediaImpl(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : closed_(true)
            , url_(url)
            , noshift_(false)
            , source_(NULL)
            , timer_(io_svc)
            , seq_start_(0)
            , seq_lastest_(0)
            , seq_play_(0)
            , segment_url_subpos_(std::string::npos)
        {
            source_ = SourceBase::create(io_svc, url.protocol());
            info_.flags = MediaBasicInfo::f_segment;
            info_.flags |= MediaBasicInfo::f_smoth;
            info_.flags |= MediaBasicInfo::f_segment_seek;
            info_.format = "ts";

            noshift_ = url_.param("m3u8.noshift") == "true";
        }

        M3u8MediaImpl::~M3u8MediaImpl()
        {
            if (source_) {
                delete source_;
            }
        }

        void M3u8MediaImpl::async_open(
            MediaBase::response_type const & resp)
        {
            closed_ = false;

            resp_ = resp;
            source_->async_open(url_, 0, invalid_size, 
                boost::bind(&M3u8MediaImpl::handle_open, this, _1));
        }

        void M3u8MediaImpl::cancel(
            boost::system::error_code & ec)
        {
            source_->cancel(ec);
        }

        void M3u8MediaImpl::close(
            boost::system::error_code & ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            closed_ = true;

            timer_.cancel(ec);
            source_->cancel(ec);
        }

        bool M3u8MediaImpl::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        bool M3u8MediaImpl::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        void M3u8MediaImpl::on_error(
            boost::system::error_code & ec) const
        {
            if (ec == source_error::no_more_segment && info_.type == MediaBasicInfo::live) {
                ec = boost::asio::error::would_block;
            }
        }

        size_t M3u8MediaImpl::segment_count() const
        {
            boost::mutex::scoped_lock lc(mutex_);

            return info_.type == MediaBasicInfo::live ? size_t(-1) : seq_lastest_ + segments_.size();
        }

        std::string const M3u8MediaImpl::segment_protocol() const
        {
            boost::mutex::scoped_lock lc(mutex_);

            framework::string::Url url;
            if (!segment_urls_.empty())
                url.from_string(segment_urls_[0]);
            return url.protocol();
        }

        bool M3u8MediaImpl::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                std::string const & str = segment_urls_[segment - seq_lastest_];
                LOG_DEBUG("[segment_url] segment: " << segment << ", seq_lastest: " << seq_lastest_ << ", url: " << str);
                complete_url(url, str);
                seq_play_ = segment;
                ec.clear();
                return true;
            } else if (info_.type == MediaBasicInfo::live && segment >= seq_lastest_) {
                ec = boost::asio::error::would_block;
                return false;
            } else {
                ec = framework::system::logic_error::out_of_range;
                return false;
            }
        }

        bool M3u8MediaImpl::segment_info(
            size_t segment, 
            SegmentInfo & info,
            boost::system::error_code & ec) const
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                info = segments_[segment - seq_lastest_];
                ec.clear();
                return true;
            } else if (info_.type == MediaBasicInfo::live && segment >= seq_lastest_) {
                ec = boost::asio::error::would_block;
                return false;
            } else {
                ec = framework::system::logic_error::out_of_range;
                return false;
            }
        }

        void M3u8MediaImpl::handle_open(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            lc.unlock();

            if (ec) {
                handle_read(ec);
                return;
            }
            boost::uint64_t total = source_->total(ec);
            if (ec) {
                boost::asio::async_read(*source_, buf_, 
                    boost::bind(&M3u8MediaImpl::handle_read, this, _1));
            } else {
                boost::asio::async_read(*source_, buf_, boost::asio::transfer_at_least(total), 
                    boost::bind(&M3u8MediaImpl::handle_read, this, _1));
            }
        }

        void M3u8MediaImpl::handle_read(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            if (ec == boost::asio::error::eof) {
                ec.clear();
            }
            boost::system::error_code ec1;
            source_->close(ec1);
            if (!ec) {
                parse_m3u8(ec);
            }

            if (!ec && segments_.empty()) {
                source_->async_open(url_, 0, invalid_size, 
                    boost::bind(&M3u8MediaImpl::handle_open, this, _1));
                return;
            }

            if (info_.type == MediaBasicInfo::live) {
                timer_.expires_from_now(
                    boost::posix_time::seconds(duration_));
                timer_.async_wait(
                    boost::bind(&M3u8MediaImpl::handle_timer, this, _1));
            } else {
                closed_ = true;
            }
            
            lc.unlock();

            if (!resp_.empty()) {
                MediaBase::response_type resp;
                resp.swap(resp_);
                resp(ec);
            }
        }

        void M3u8MediaImpl::handle_timer(
            boost::system::error_code ec)
        {
            boost::mutex::scoped_lock lc(mutex_);

            if (closed_) {
                lc.unlock();
                delete this;
                return;
            }

            source_->async_open(url_, 0, invalid_size, 
                boost::bind(&M3u8MediaImpl::handle_open, this, _1));
        }

        struct M3u8MediaImpl::StreamInfo
        {
        };

        void M3u8MediaImpl::parse_m3u8(
            boost::system::error_code & ec)
        {
            char const * const M3U8_BEGIN = "#EXTM3U";
            char const * const M3U8_TARGETDURATION = "#EXT-X-TARGETDURATION";
            char const * const M3U8_SEQUENCE = "#EXT-X-MEDIA-SEQUENCE";
            char const * const M3U8_EXTSTREAMINF = "#EXT-X-STREAM-INF";
            char const * const M3U8_EXTINF = "#EXTINF";
            char const * const M3U8_END  = "#EXT-X-ENDLIST";

            std::string line;
            std::istream is(&buf_);
            bool begin = false;
            bool end = false;
            size_t line_num = 0;
            boost::uint64_t duration = 0;
            size_t sequence = 0;
            streams_.clear();
            stream_urls_.clear();
            while (std::getline(is, line)) {
                framework::string::trim(line);
                ++line_num;
                std::istringstream iss(line);
                std::string token;
                std::getline(iss, token, ':');
                if (token == M3U8_BEGIN) {
                    begin = true;
                } else if (!begin) {
                    LOG_WARN("[parse_m3u8] line before begin tag: " << line); 
                } else if (end) {
                    LOG_WARN("[parse_m3u8] line before end tag: " << line); 
                } else if (token == M3U8_END) {
                    end = true;
                } else if (token == M3U8_TARGETDURATION) {
                    iss >> duration_;
                } else if (token == M3U8_SEQUENCE) {
                    if (iss >> sequence) {
                        if (seq_start_ == 0) {
                            seq_start_ = sequence;
                        }
                        sequence -= seq_start_;
                    }
                } else if (token == M3U8_EXTSTREAMINF) {
                    StreamInfo info;
                    if (!std::getline(is, line)) {
                        LOG_WARN("[parse_m3u8] no url at line: " << line_num); 
                    }
                    framework::string::trim(line);
                    streams_.push_back(info);
                    stream_urls_.push_back(line);
                } else if (token == M3U8_EXTINF) {
                    SegmentInfo info;
                    if (iss >> info.duration)
                        info.duration *= 1000;
                    duration += info.duration;
                    if (!std::getline(is, line)) {
                        LOG_WARN("[parse_m3u8] no url at line: " << line_num); 
                    }
                    framework::string::trim(line);
                    if (sequence < seq_lastest_ + segments_.size()) {
                    } else {
                        segments_.push_back(info);
                        segment_urls_.push_back(line);
                    }
                    ++sequence;
                }
            }

            if (!streams_.empty()) {
                complete_url(url_, stream_urls_.front());
                segments_.clear();
                segment_urls_.clear();
                return;
            }

            if (sequence > seq_play_) {
                sequence = seq_play_;
            }
            if (seq_lastest_ < sequence) {
                segments_.erase(segments_.begin(), segments_.begin() + (sequence - seq_lastest_));
                segment_urls_.erase(segment_urls_.begin(), segment_urls_.begin() + (sequence - seq_lastest_));
                seq_lastest_ = seq_play_;
            }

            if (end) {
                info_.duration = duration;
            } else {
                info_.type = MediaInfo::live;
                boost::uint32_t delay = 0;
                for (size_t i = segments_.size() - 1; i != size_t(-1) && i + 4 > segments_.size(); --i) {
                    delay += (boost::uint32_t)segments_[i].duration;
                }
                if (noshift_) {
                    duration = delay;
                    size_t k = segments_.size() > 3 ? segments_.size() - 3 : 0;
                    segments_.erase(segments_.begin(), segments_.begin() + k);
                }
                info_.current = duration;
                info_.start_time = time(NULL) - duration / 1000;
                info_.delay = delay;
                info_.shift = duration;
            }
        }

        void M3u8MediaImpl::complete_url(
            framework::string::Url & url,
            std::string const & str) const
        {
            if (segment_url_subpos_ == std::string::npos) {
                if (str.find("://") == std::string::npos) {
                    if (str[0] == '/') {
                        segment_url_prefix_ = url_.protocol() + "://" + url_.host_svc();
                        segment_url_subpos_ = 0;
                    } else {
                        segment_url_prefix_ = url_.protocol() + "://" + url_.host_svc() + url_.path();
                        std::string::size_type pos = segment_url_prefix_.rfind('/');
                        segment_url_prefix_ = segment_url_prefix_.substr(0, pos + 1);
                        segment_url_subpos_ = 0;
                    }
                } else {
                    segment_url_subpos_ = 0;
                }
            }
            if (segment_url_prefix_.empty()) {
                url.from_string(str);
            } else {
                url.from_string(segment_url_prefix_ + str.substr(segment_url_subpos_));
            }
        }

    } // data
} // ppbox
