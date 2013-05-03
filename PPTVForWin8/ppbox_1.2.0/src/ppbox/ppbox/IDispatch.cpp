// IDispatch.cpp

#include "ppbox/ppbox/Common.h"
#define PPBOX_SOURCE
#include "ppbox/ppbox/IDispatch.h"

#include <ppbox/merge/MergeModule.h>
#include <ppbox/merge/MergerBase.h>
#include <ppbox/merge/MergeError.h>
using namespace ppbox::merge;

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>
#include <framework/system/LogicError.h>
#include <util/buffers/BufferCopy.h>
using namespace framework::timer;

#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.IDispatch", framework::logger::Debug);

namespace ppbox
{

    class IDispatch
    {
    private:
        struct Cache
        {
            Cache()
                : close_token(0)
                , paused(false)
                , sample_buffer(NULL)
            {
            }

            boost::uint8_t const * copy_sample_data()
            {
                if (sample.data.size() == 1) {
                    return boost::asio::buffer_cast<boost::uint8_t const *>(sample.data[0]);
                } else {
                    if (sample_buffer.size() < sample.size)
                        sample_buffer.resize(sample.size);
                    util::buffers::buffer_copy(boost::asio::buffer(sample_buffer), sample.data);
                    return sample_buffer.size() ? &sample_buffer.at(0) : NULL;
                }
            }

            size_t close_token;
            MergerBase * merger;
            ppbox::avformat::Sample sample;
            bool paused;
            std::vector<boost::uint8_t> sample_buffer;
        };

    public:
        IDispatch()
            : merge_mod_(util::daemon::use_module<ppbox::merge::MergeModule>(global_daemon()))
        {
        }

        ~IDispatch()
        {
        }

        error::errors open(
            char const * playlink)
        {
            LOG_SECTION();
            LOG_INFO("open playlink: " << playlink);

            error_code ec;
            if (cache_) {
                ec = ppbox::merge::error::already_open;
            } else {
                boost::shared_ptr<Cache> cache(new Cache);
                cache_ = cache;
                framework::string::Url play_link(playlink);
                cache->merger = merge_mod_.open(play_link, cache->close_token, ec);
            }
            return last_error(__FUNCTION__, ec);
        }

        void async_open(
            char const * playlink, 
            PPBOX_DispatchOpen_Callback callback)
        {
            LOG_SECTION();
            LOG_INFO("async_open playlink: " << playlink);

            error_code ec;
            if (cache_) {
                ec = ppbox::merge::error::already_open;
            } else {
                cache_.reset(new Cache);
                framework::string::Url play_link(playlink);
                merge_mod_.async_open(play_link, cache_->close_token, 
                    boost::bind(&IDispatch::open_call_back, cache_, callback, _1, _2));
            }
            if (ec) {
                boost::thread th(boost::bind(callback, async_last_error(__FUNCTION__, ec)));
                (void)th;
            }
        }

        static void open_call_back(
            boost::shared_ptr<Cache> & cache, 
            PPBOX_DispatchOpen_Callback callback, 
            error_code const & ec, 
            MergerBase * merger)
        {
            cache->merger = merger;
            callback(async_last_error(__FUNCTION__, ec));
        }

        bool is_open(
            error_code & ec) const
        {
            if (cache_ && cache_->merger) {
                ec.clear();
                return true;
            } else {
                ec = ppbox::merge::error::not_open;
                return false;
            }
        }

        error::errors seek(
            boost::uint64_t time)
        {
            LOG_SECTION();
            LOG_INFO("seek time: " << time);

            error_code ec;
            if (is_open(ec)) {
                cache_->merger->byte_seek(time, ec);
            }
            return last_error(__FUNCTION__, ec);
        }

        error::errors pause()
        {
            LOG_SECTION();
            LOG_INFO("pause");
            error_code ec;
            if (is_open(ec)) {
                // cache_->merger->pause(ec);
                if (!ec)
                    cache_->paused = true;
            }
            return last_error(__FUNCTION__, ec);
        }

        error::errors resume()
        {
            LOG_SECTION();
            LOG_INFO("resume");

            error_code ec;
            if (is_open(ec)) {
                // cache_->merger->resume(ec);
                if (!ec)
                    cache_->paused = false;
            }
            return last_error(__FUNCTION__, ec);
        }

        error::errors close()
        {
            LOG_SECTION();
            LOG_INFO("close");

            error_code ec;
            if (cache_) {
                while (true) {
                    merge_mod_.close(cache_->close_token, ec);
                    if (ec == framework::system::logic_error::item_not_exist) {
                        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                    } else {
                        break;
                    }
                }
                cache_->merger = NULL;
                cache_.reset();
            } else {
                ec = ppbox::merge::error::not_open;
            }
            return last_error(__FUNCTION__, ec);
        }

        error::errors get_media_info(
            PPBOX_MediaInfo & info)
        {
            error_code ec;
            return last_error(__FUNCTION__, ec);
        }

        error::errors read(
            PPBOX_DispatchSample & sample)
        {
            error_code ec;
            if (is_open(ec)) {
                if (cache_->paused) {
                    cache_->paused = false;
                }
                if (cache_->merger->read(cache_->sample, ec)) {
                    sample.start_position = (boost::uint32_t)cache_->sample.ustime;
                    sample.buffer_length = cache_->sample.size;
                    sample.buffer = cache_->copy_sample_data();
                }
            }
            return last_error(__FUNCTION__, ec);
        }

        error::errors get_statistic(
            PPBOX_DispatchStatistic & stat)
        {
            error_code ec;
            if (is_open(ec)) {
                //stat.total_size = cache_->merger->total_size();
                //stat.current = cache_->merger->get_current();
                stat.buffer_size = cache_->merger->get_buffer_size();
                //stat.buffer_time = cache_->merger->get_buffer_time(ec);
            }
            return last_error(__FUNCTION__, ec);
        }

        static error::errors async_last_error(
            char const * log_title, 
            error_code const & ec)
        {
            if (ec && ec != boost::asio::error::would_block) {
                LOG_WARN(log_title << ": " << ec.message());
            }
            return ppbox::error::last_error_enum(ec);
        }

        static error::errors last_error(
            char const * title, 
            error_code const & ec)
        {
            ppbox::error::last_error(ec);
            return async_last_error(title, ec);
        }

    private:
        ppbox::merge::MergeModule & merge_mod_;
        boost::shared_ptr<Cache> cache_;
    };
}

static ppbox::IDispatch & merger()
{
    static ppbox::IDispatch the_merger;
    return the_merger;
}

#if __cplusplus
extern "C" {
#endif // __cplusplus

    PPBOX_DECL boost::int32_t PPBOX_DispatchOpen(
        char const * playlink)
    {
        return merger().open(playlink);
    }

    PPBOX_DECL void PPBOX_DispatchAsyncOpen(
        char const * playlink, 
        PPBOX_DispatchOpen_Callback callback)
    {
        merger().async_open(playlink, callback);
    }

    PPBOX_DECL void PPBOX_DispatchClose()
    {
        merger().close();
    }

    PPBOX_DECL boost::int32_t PPBOX_DispatchPause()
    {
        return merger().pause();
    }

    PPBOX_DECL boost::int32_t PPBOX_DispatchSeek(
        boost::uint32_t start_time)
    {
        return merger().seek(start_time);
    }

    PPBOX_DECL boost::int32_t PPBOX_DispatchGetMediaInfo(
        PPBOX_MediaInfo * info)
    {
        return merger().get_media_info(*info);
    }

    PPBOX_DECL boost::int32_t PPBOX_DispatchRead(
        PPBOX_DispatchSample * sample)
    {
        return merger().read(*sample);
    }

    PPBOX_DECL PP_int32 PPBOX_DispatchGetStatistic(
        PPBOX_DispatchStatistic * statistic)
    {
        return merger().get_statistic(*statistic);
    }

#if __cplusplus
}
#endif // __cplusplus
