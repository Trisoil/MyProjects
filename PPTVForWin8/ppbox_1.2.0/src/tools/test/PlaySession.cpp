// PlaySession.cpp

#include "tools/test/Common.h"
#include "tools/test/InputHandler.h"
#include "tools/test/PlaySession.h"
#ifdef WRITE_FILE
#include "tools/test/FileWriter.h"
#endif
#include "tools/test/VideoStreamWriter.h"
#include "tools/test/AudioStreamWriter.h"
#include "OpenManager.h"

#include <framework/system/BytesOrder.h>
#include <framework/string/Format.h>
#include <framework/timer/TimeCounter.h>
using namespace framework::logger;
using namespace framework::string;
using namespace framework::configure;

#include <boost/filesystem/operations.hpp>

#include <iostream>

void PlaySession::play_movie(
    Config & conf, 
    InputHandler & std_in, 
    std::string const & name)
{
    PlaySession session(conf);
    PP_int32 ec = session.open(std_in, name.c_str());
    if ( !ec )
    {
        session.insert_media( 20000, 16580, 1192113, 9462, "http://192.168.45.211/movies/yu_2.mp4" );
    }
    while (!ec) {
        ec = session.play_go_on();
        std::vector<std::string> cmd_args;
        if (std_in.get_one(cmd_args)) {
            if (cmd_args[0] == "seek") {
                if (cmd_args.size() >= 2) {
                    PP_uint32 time = framework::string::parse<PP_uint32>(cmd_args[1]);
                    session.seek(time);
                }
            } else if (cmd_args[0] == "pause") {
                session.pause();
            } else if (cmd_args[0] == "resume") {
                session.resume();
            } else if (cmd_args[0] == "close") {
                break;
            } else if (cmd_args[0] == "ui") {
                std::string msg = "abc";
                session.submit_ui_log(msg.c_str(), msg.length());
                break;
            } else if ( cmd_args[0] == "insert" ) {
                if (cmd_args.size() >= 2) {
                    PP_uint32 insert_time = framework::string::parse<PP_uint32>(cmd_args[1]);
                    session.insert_media( insert_time * 1000, 16580, 1192113, 9462, "http://192.168.45.211/movies/yu_2.mp4" );
                }
            } else if ( cmd_args[0] == "getmedia" ) {
                if (cmd_args.size() >= 2) {
                    PP_uint32 media_id = framework::string::parse<PP_uint32>(cmd_args[1]);
                    session.get_insert_media( media_id );
                }
            } else {
                std::cout << "play_movie: " << "unkonwn command " << cmd_args[0] << std::endl;
            }
        }
    }
    session.close();
    std::cout << "exit play movie" << std::endl;
}

PlaySession::PlaySession(
    framework::configure::Config & conf)
    : fast_mode(false)
    , disp_interval(1)
    , buffer_time(10)
    , buffer_size(20)
    , seek_time(0)
    , async_open(false)
    , open_wait_time(0)
    , write_file(false)
    , output_stream(false)
    , show_sync(false)
    , need_buffer(false)
    , writer_(NULL)
    , video_stream_writer_(NULL)
    , audio_stream_writer_(NULL)
    , video_index_(-1)
    , audio_index_(-1)
    , seconds_(0)
    , milli_seconds_(1000)
    , micro_seconds_(1000000)
    , after_seek_(false)
    , after_pause_(false)
    , blocked_(false)
    , buffer_percent_(0)
    , status_(closed)
    , first_frame_(false)
    , first_sync_frame_(false)
{
    conf.register_module("PlaySession") 
        << CONFIG_PARAM_RDWR(fast_mode) 
        << CONFIG_PARAM_RDWR(disp_interval) 
        << CONFIG_PARAM_RDWR(buffer_time) 
        << CONFIG_PARAM_RDWR(buffer_size) 
        << CONFIG_PARAM_RDWR(seek_time) 
        << CONFIG_PARAM_RDWR(async_open)
        << CONFIG_PARAM_RDWR(open_wait_time)
        << CONFIG_PARAM_RDWR(write_file) 
        << CONFIG_PARAM_RDWR(output_stream) 
        << CONFIG_PARAM_RDWR(show_sync)
        << CONFIG_PARAM_RDWR(need_buffer)
    ;

    blocked_ = need_buffer;

#ifdef WRITE_FILE
    if (write_file)
        writer_ = new FileWriter;
#endif
    video_stream_writer_ = new VideoStreamWriter(output_stream);
    audio_stream_writer_ = new AudioStreamWriter(output_stream);
}

PlaySession::~PlaySession()
{
#ifdef WRITE_FILE
    if (writer_) {
        delete writer_;
        writer_ = NULL;
    }
#endif
    if (video_stream_writer_) {
        delete video_stream_writer_;
        video_stream_writer_ = NULL;
    }
    if (audio_stream_writer_) {
        delete audio_stream_writer_;
        audio_stream_writer_ = NULL;
    }
}

PP_int32 PlaySession::open(
                           InputHandler & std_in, 
                           char const * name)
{
    std::cout << "open: " << name << std::endl;
    PPBOX_SetPlayBufferTime(buffer_time * 1000);
    PPBOX_SetDownloadBufferSize(buffer_size * 1024 * 1024);
    Time start_time = Time::now();
    OpenManager openm(std_in, async_open, open_wait_time);
    PP_int32 ec = openm.open(name);
    if (ppbox_success == ec) {
        start_time_ = Time::now();

        std::cout << "open succeed (" << (start_time_ - start_time).total_milliseconds() << " milliseconds)" << std::endl;

#ifdef WRITE_FILE
        if (write_file)
            writer_->open(name);
#endif
        std::cout << "Duration: " << PPBOX_GetDuration() / 1000.0 << std::endl;
        size_t stream_count = PPBOX_GetStreamCount();
        std::cout << "StreamCount: " << stream_count << std::endl;
        for (size_t i = 0; i < stream_count; ++i) {
            PPBOX_StreamInfoEx stream_info;
            PPBOX_GetStreamInfoEx(i, &stream_info);
            std::cout << "Stream " << i << std::endl;
            if (stream_info.type == ppbox_video) {
                video_index_ = i;
                std::cout << "Type: Video" << std::endl;
                std::cout << "Format Type: " << stream_info.format_type << std::endl;
                video_stream_writer_->open(i, stream_info);
            } else {
                audio_index_ = i;
                std::cout << "Type: Audio" << std::endl;
                std::cout << "Format Type: " << stream_info.format_type << std::endl;
                audio_stream_writer_->open(i, stream_info);
            }
        }
        status_ = active;
        if (seek_time)
            seek(seek_time);
    } else {
        std::cout << "open: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
    }

    return ec;
}

PP_int32 PlaySession::play_go_on()
{
    if (status_ != active) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        return ppbox_success;
    }

    if (blocked_) {
        PPBOX_PlayStatistic stat;
        framework::timer::TimeCounter tc;
        PPBOX_GetPlayMsg(&stat);
        if (tc.elapse() >= 20) {
            std::cout << "[play_go_on] PPBOX_GetPlayMsg: " << tc.elapse() << std::endl;
        }
        buffer_percent_ = stat.buffering_present;
        PP_int32 ec = PPBOX_GetLastError();
        if (ec == ppbox_success 
            || ec == ppbox_would_block) {
                if (buffer_percent_ != stat.buffering_present) {
                    dump_stat(stat);
                }
                if (ec == ppbox_would_block) {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
                }
                if (buffer_percent_ < 100) {
                    //boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                    return ppbox_success;
                }
        } else {
            std::cout << "play_go_on: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
            return ec;
        }
        blocked_ = false;
        std::cout << " ok (" << (Time::now() - start_time_).total_milliseconds() << "ms)" << std::endl;
#ifdef WRITE_FILE
        start_time_ = Time::now() - Duration::milliseconds(milli_seconds_);
#else
        start_time_ = Time::now() - Duration::microseconds(micro_seconds_);
#endif
    }

    framework::timer::TimeCounter tc;
#ifdef WRITE_FILE
    PPBOX_SampleEx sample;
    PP_int32 ec = PPBOX_ReadSampleEx(&sample);
#else
    PPBOX_SampleEx2 sample;
    PP_int32 ec = PPBOX_ReadSampleEx2(&sample);
#endif

    if (tc.elapse() >= 20) {
        std::cout << "[play_go_on] PPBOX_ReadSampleEx2: " << tc.elapse() << std::endl;
    }

    if (ec == ppbox_success) {
        //std::cout << "sample start_time: " << sample.start_time 
        //    << " buffer_length: " << sample.buffer_length << std::endl;
#ifdef WRITE_FILE
        if (write_file)
            writer_->handle(sample);
#endif
        if (!first_frame_) {
            first_frame_ = true;
            std::cout << " ok first frame (" << (Time::now() - start_time_).total_milliseconds() << "ms)" << std::endl;
        }
        if (!first_sync_frame_ && sample.is_sync) {
            first_sync_frame_ = true;
            std::cout << " ok first sync frame (" << (Time::now() - start_time_).total_milliseconds() << "ms)" << std::endl;
        }

        if (sample.stream_index == video_index_) {
            // avc nalus
            if (!video_stream_writer_->handle(sample)) {
                std::cout << "invalid video sample ";
                std::cout << " start_time: " << sample.start_time 
                    << " buffer_length: " << sample.buffer_length << std::endl;
                //LOG_DATA(Debug, sample.buffer, sample.buffer_length);
            }
            if (show_sync && sample.is_sync) {
                std::cout << "sync sample ";
                std::cout << " start_time: " << sample.start_time 
                    << " buffer_length: " << sample.buffer_length << std::endl;
            }
        } else if (sample.stream_index == audio_index_) {
            if (!audio_stream_writer_->handle(sample)) {
                std::cout << "invalid audio sample ";
                std::cout << " start_time: " << sample.start_time 
                    << " buffer_length: " << sample.buffer_length << std::endl;
                //LOG_DATA(Debug, sample.buffer, sample.buffer_length);
            }
            // aac
            //LOG_DATA(Debug, 0, sample.buffer, sample.buffer_length);
        } else {
            std::cout << "sample other stream " << sample.stream_index;
            std::cout << " start_time: " << sample.start_time 
                << " buffer_length: " << sample.buffer_length << std::endl;
        }
#ifndef WRITE_FILE
        if (sample.is_discontinuity) {
            std::cout << "discontinuity sample ";
            std::cout << " start_time: " << sample.start_time 
                << " buffer_length: " << sample.buffer_length << std::endl;
        }
#endif
        if (after_seek_ || after_pause_) {
#ifdef WRITE_FILE
            seconds_ = sample.start_time / 1000;
            milli_seconds_ = (seconds_ + 1) * 1000;
            start_time_ = Time::now() - Duration::milliseconds(sample.start_time);
#else
            seconds_ = sample.start_time / 1000000;
            micro_seconds_ = (boost::uint64_t)(seconds_ + 1) * (boost::uint64_t)1000000;
            start_time_ = Time::now() - Duration::microseconds(sample.start_time);
#endif
            std::cout << "after seek/pause sample " << sample.stream_index;
            std::cout << " start_time: " << sample.start_time 
                << " buffer_length: " << sample.buffer_length << std::endl;
            after_seek_ = false;
            after_pause_ = false;
        }
#ifdef WRITE_FILE
        if (sample.start_time >= milli_seconds_) {
#else
        if (sample.start_time >= micro_seconds_) {
#endif
            size_t loop = 0;
            do {
                if (++loop > 1) {
#ifdef WRITE_FILE
                    loop = (sample.start_time - milli_seconds_) / 1000;
#else
                    loop = (sample.start_time - micro_seconds_) / 1000000;
#endif
                    std::cout << "error: skip " << loop << " seconds" << std::endl;
                    loop = loop / disp_interval * disp_interval;
                    seconds_ += loop;
                    milli_seconds_ += 1000 * loop;
                    micro_seconds_ += 1000000 * loop;
#ifdef WRITE_FILE
                    start_time_ = Time::now() - Duration::milliseconds(milli_seconds_);
#else
                    start_time_ = Time::now() - Duration::microseconds(micro_seconds_);
#endif
                    break;
                }
                seconds_ += disp_interval;
                milli_seconds_ += 1000 * disp_interval;
                micro_seconds_ += 1000000 * disp_interval;
#ifdef WRITE_FILE
            } while (sample.start_time > milli_seconds_);
#else
            } while (sample.start_time > micro_seconds_);
#endif

            dump_stat();
        }
#ifdef WRITE_FILE
        if (!fast_mode && start_time_ + Duration::milliseconds(sample.start_time) > Time::now() + Duration::milliseconds(500)) {
#else
        if (!fast_mode && start_time_ + Duration::microseconds(sample.start_time) > Time::now() + Duration::milliseconds(500)) {
#endif
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        }
    } else if (ec == ppbox_would_block) {
        std::cout << "play_go_on: ppbox_would_block" << std::endl;
        start_time_ = Time::now();
        if (!fast_mode) {
            blocked_ = true;
            buffer_percent_ = 0;
            dump_stat();
            if (tc.elapse() > 10) {
                std::cout << "[play_go_on] dump_stat: " << tc.elapse() << std::endl;
            }
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        ec = ppbox_success;
    } else if (ec == ppbox_stream_end) {
        std::cout << "play_go_on: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
    } else {
        std::cout << "play_go_on: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
    }
    return ec;
}

PP_int32 PlaySession::seek(
                           PP_uint32 time)
{
    std::cout << "seek: time = " << time << std::endl;
    PP_int32 ec = PPBOX_Seek(time * 1000);
    if (ppbox_success == ec || ppbox_would_block == ec) {
        ec = ppbox_success;
        if (!after_seek_) {
            after_seek_ = true;
        }
        if (blocked_) {
            buffer_percent_ = 0;
        }
        seconds_ = time;
#ifdef WRITE_FILE
        if (write_file)
            writer_->seek();
#endif
        video_stream_writer_->seek();
        audio_stream_writer_->seek();
    } else {
        std::cout << "seek: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
    }
    return ec;
}

PP_int32 PlaySession::pause()
{
    std::cout << "pause" << std::endl;
    PP_int32 ec = PPBOX_Pause();
    if (ppbox_success == ec) {
        status_ = paused;
        after_pause_ = true;
    } else {
        std::cout << "pause: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
    }
    return ec;
}

PP_int32 PlaySession::resume()
{
    std::cout << "resume" << std::endl;
    status_ = active;
    return ppbox_success;
}

PP_int32 PlaySession::close()
{
    std::cout << "close" << std::endl;
    PPBOX_Close();
#ifdef WRITE_FILE
    if (write_file)
        writer_->close();
#endif
    if (output_stream) {
        video_stream_writer_->close();
        audio_stream_writer_->close();
    }
    status_ = closed;
    return ppbox_success;
}

void PlaySession::set_config(
                             char const * module, 
                             char const * section, 
                             char const * key, 
                             char const * value)
{
    PPBOX_SetConfig(module, section, key, value);
}

void PlaySession::insert_media(
                               PP_uint64 insert_time,
                               PP_uint64 media_duration,
                               PP_uint64 media_size,
                               PP_uint64 head_size,
                               char const * url )
{
    InsertMedia media;
    media.id                = 0;
    media.insert_time       = insert_time;
    media.media_duration    = media_duration;
    media.media_size        = media_size;
    media.head_size         = head_size;
    media.report            = 0;
    media.url               = url;
    media.report_begin_url  = NULL;
    media.report_end_url    = NULL;

    //PPBOX_InsertMedia(1, &media);
}

void PlaySession::get_insert_media(
                               PP_uint32 media_id )
{
    //InsertMediaEvent media_event;
    //media_event.media_id = media_id;

    //PPBOX_GetInsertMediaEvent( &media_event );
}

void PlaySession::submit_ui_log(
                   char const * msg, 
                   PP_int32 size)
{
    PPBOX_SubmitMessage(msg, size);
}

void PlaySession::dump_stat()
{
    PPBOX_PlayStatistic stat;
    framework::timer::TimeCounter tc;
    PPBOX_GetPlayMsg(&stat);
    if (tc.elapse() >= 20) {
        std::cout << "[dump_stat] PPBOX_GetPlayMsg: " << tc.elapse() << std::endl;
    }
    dump_stat(stat);
}

void PlaySession::dump_stat(
    PPBOX_PlayStatistic const & stat)
{
    PPBOX_DownloadSpeedMsg msg;
    PPBOX_GetDownSedMsg(&msg);
#ifdef WIN32
#else
    std::cout << "\r";
    std::cout << "\033[K";
#endif
#ifdef WIN32
#else
    if (stat.play_status == ppbox_closed) {
        std::cout << "\033[1;35m";
    } else if (stat.play_status == ppbox_playing) {
        std::cout << "\033[1;32m";
    } else if (stat.play_status == ppbox_buffering) {
        std::cout << "\033[1;31m";
    } else if (stat.play_status == ppbox_paused) {
        std::cout << "\033[0;37m";
    }
#endif
    std::cout << seconds_ 
        << "\t" << stat.buffer_time / 1000 << "(" << stat.buffering_present << "%)" 
        << "\t" << msg.second_download_speed;
#ifdef WIN32
    std::cout << std::endl;
#else
    std::cout << "\033[m";
    std::cout << "\r";
    std::cout << std::flush;
#endif
}
