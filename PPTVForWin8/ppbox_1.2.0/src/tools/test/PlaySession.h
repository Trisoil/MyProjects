// PlaySession.h

#ifndef _PLAY_SESSION_H_
#define _PLAY_SESSION_H_

#include <framework/network/NetName.h>
#include <framework/timer/ClockTime.h>

struct FileWriter;
struct VideoStreamWriter;
struct AudioStreamWriter;

class PlaySession
{
public:
    PlaySession(
        framework::configure::Config & conf);

    ~PlaySession();

public:
    static void play_movie(
        framework::configure::Config & conf, 
        InputHandler & std_in, 
        std::string const & name);

public:
    PP_int32 open(
        InputHandler & std_in, 
        char const * name);

    PP_int32 play_go_on();

    PP_int32 seek(
        PP_uint32 time);

    PP_int32 pause();

    PP_int32 resume();

    PP_int32 close();

    void set_config(
        char const * module, 
        char const * section, 
        char const * key, 
        char const * value);

    void insert_media(
        PP_uint64 insert_time,
        PP_uint64 media_duration,
        PP_uint64 media_size,
        PP_uint64 head_size,
        char const * url );

    void get_insert_media(
        PP_uint32 media_id );

    void submit_ui_log(
        char const * msg, 
        PP_int32 size);

private:
    void dump_stat();

    void dump_stat(
        PPBOX_PlayStatistic const & stat);

private:
    bool fast_mode;
    size_t disp_interval;
    size_t buffer_time;
    size_t buffer_size;
    size_t seek_time;
    bool async_open;
    size_t open_wait_time;
    bool write_file;
    bool output_stream;
    bool show_sync;
    bool need_buffer;

private:
    FileWriter * writer_;
    VideoStreamWriter * video_stream_writer_;
    AudioStreamWriter * audio_stream_writer_;

private:
    boost::uint32_t video_index_;
    boost::uint32_t audio_index_;
    boost::uint32_t seconds_;
    boost::uint32_t milli_seconds_;
    boost::uint64_t micro_seconds_;
    framework::timer::Time start_time_;
    bool after_seek_;
    bool after_pause_;
    bool blocked_;
    boost::uint32_t buffer_percent_;
    enum StatusEnum
    {
        closed, 
        active, 
        paused
    } status_;

    bool first_frame_;
    bool first_sync_frame_;
};

#endif // _PLAY_SESSION_H_
