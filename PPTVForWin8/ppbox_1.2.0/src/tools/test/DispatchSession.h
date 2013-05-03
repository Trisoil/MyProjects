// DispatchSession.h

#ifndef _TOOLS_TEST_DISPATCH_SESSION_H_
#define _TOOLS_TEST_DISPATCH_SESSION_H_

#include <framework/timer/Ticker.h>

#include <fstream>

class DispatchSession
{
public:
    DispatchSession(
        framework::configure::Config & conf);

    ~DispatchSession();

public:
    static void dispatch_movie(
        framework::configure::Config & conf, 
        InputHandler & std_in, 
        std::string const & name);

public:
    PP_int32 open(
        InputHandler & std_in, 
        char const * name);

    PP_int32 go_on();

    PP_int32 seek(
        PP_uint32 time);

    PP_int32 pause();

    PP_int32 resume();

    PP_int32 close();

private:
    void dump_stat();

    void dump_stat(
        PPBOX_DispatchStatistic const & stat);

private:
    bool async_open;
    size_t open_wait_time;
    size_t seek_position;
    bool write_file;

private:
    framework::timer::Ticker ticker_;
    std::ofstream ofs_;
};

#endif // _PLAY_SESSION_H_
