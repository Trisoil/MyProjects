// Test.cpp

#include "tools/test/Common.h"
#include "tools/test/InputHandler.h"
#include "tools/test/PlaySession.h"
//#include "tools/test/DispatchSession.h"
#include "tools/test/DownloadSession.h"

#include <framework/string/Slice.h>
#include <framework/system/BytesOrder.h>
#include <framework/configure/Config.h>
using namespace framework::string;
using namespace framework::configure;
using namespace framework::logger;
using namespace framework::network;

#include <boost/thread/thread.hpp>
using namespace boost::system;

#include <iostream>
#include <fstream>

#ifdef BOOST_WINDOWS_API
#  if (defined UNDER_CE) || (defined __MINGW32__)
#    define localtime_r(x, y) *y = *localtime(x)
#  else 
#    define localtime_r(x, y) localtime_s(y, x)
#  endif
#endif

void msg_out(PP_uint32 numbers)
{
    DialogMessage * vector = NULL;
    vector = new DialogMessage[numbers];
    //numbers = PPBOX_DialogMessage(vector, numbers, module.c_str(), 5);
    numbers = PPBOX_DialogMessage(vector, numbers, NULL, 5);
    if (numbers > 0) {
        for (size_t i = 0; i < numbers; ++i) {

            time_t tt = vector[i].time;
            struct tm lt;
            char buf[40];
            localtime_r(&tt, &lt);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &lt);

            std::cout << "msg:" << i << ", [" << buf << "] ["
                << vector[i].module << "] " << vector[i].msg << std::endl;
        }
    }
    delete [] vector;
    vector = NULL;
}

int main(int argc, char * argv[])
{
    Config conf("ppbox_test.conf");
    framework::logger::load_config(conf);

    bool is_debug = false;

    std::string gid;
    std::string pid;
    std::string auth;

    conf.register_module("Certify")
        << CONFIG_PARAM_RDWR(gid)
        << CONFIG_PARAM_RDWR(pid)
        << CONFIG_PARAM_RDWR(auth);

    std::string play_list;
    if (argc >= 2) {
        if (argv[1][0] == '-') {
            if (argv[1][1] == 'c') {
                NetName addr("127.0.0.1:1111");
                conf.register_module("Console")
                    << CONFIG_PARAM_RDWR(addr);
                if (argc >= 3) {
                    addr.from_string(argv[2]);
                }
                InputHandler std_out(addr, false);
                std::string cmd;
                PP_uint32 numbers = 1;
                while (std::getline(std::cin, cmd)) {
                    std::vector<std::string> cmd_args;
                    slice<std::string>(cmd, std::inserter(cmd_args, cmd_args.end()), " ");
                    if (is_debug) {
                        if (cmd_args.empty()) {
                            msg_out(numbers);
                            continue;
                        } else if (cmd_args[0] == "xxx") {
                            is_debug = false;
                            PPBOX_DebugMode(false);
                            continue;
                        } else if (isdigit(cmd_args[0][0])) {
                            numbers = framework::string::parse<PP_uint32>(cmd_args[0]);
                            if (numbers <= 0) {
                                numbers = 1;
                            }

                            msg_out(numbers);
                            continue;
                        }
                    }

                    if (cmd_args.empty()) {
                    } else if (cmd_args[0] == "debug") {
                        is_debug = true;
                        PPBOX_DebugMode(true);
                        {
                            //std_out.put_one("open 1d7c1WCcaMih2c7j4a5amciYzMuUx2JamMmYzqCUymtamciY0aGUyWJam5SYzZ6Ux5ZamZmYz6DOx6GtZdHjnw%3d%3d");
                        }
                    } else if (cmd_args[0] == "start") {
                        PP_int32 ec = PPBOX_StartP2PEngine(gid.c_str(), pid.c_str(), auth.c_str());
                        if (ppbox_success != ec) {
                            std::cout << "start p2p engine: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
                            break;
                        }
                    } else if (cmd_args[0] == "config") {
                    } else {
                        std_out.put_one(cmd);
                    }
                }
                return 0;
            } else if (argv[1][1] == 'l') {
                if (argc >= 3) {
                    play_list = argv[2];
                }
            }
        } else {
            conf.profile().pre_set(std::string("Console.name=") + argv[1]);
        }
    }

    std::string name = 
        "ppvod://1d7c1WCcaMih2c7j4a5amZuYz8yUx2hamZubkMukipaWZ5WYzcyUxpheZdHjnw%3d%3d";
    NetName addr("0.0.0.0:1111");
    size_t buffer_size = 20;

    conf.register_module("Console")
        << CONFIG_PARAM_RDWR(name)
        << CONFIG_PARAM_RDWR(addr)
        << CONFIG_PARAM_RDWR(buffer_size);

    InputHandler std_in(addr);

    PP_int32 ec = PPBOX_StartP2PEngine(gid.c_str(), pid.c_str(), auth.c_str());
    if (ppbox_success != ec) {
        std::cout << "start p2p engine: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
        return 0;
    }

    PPBOX_SetDownloadBufferSize(buffer_size * 1024 * 1024);

    if (!play_list.empty()) {
        std::ifstream ifs(play_list.c_str());
        while (std::getline(ifs, name) && !name.empty()) {
            if (name[0] == '#')
                continue;
            if (name.at(name.size() - 1) == '\r') {
                name.erase(name.size() - 1);
            }

            boost::thread th(
                boost::bind(PlaySession::play_movie, boost::ref(conf), boost::ref(std_in), name));
            th.join();
        }
    }

    if (!name.empty()) {
        boost::thread th(
            boost::bind(PlaySession::play_movie, boost::ref(conf), boost::ref(std_in), name));
            //boost::bind(DispatchSession::dispatch_movie, boost::ref(conf), boost::ref(std_in), name));
        th.join();
    }

    /************************************************************************/
    InsertMedia media;
    media.id                = 0;
    media.insert_time       = 20000;
    media.media_duration    = 16580;
    media.media_size        = 1192113;
    media.head_size         = 9462;
    media.report            = 0;
    media.url               = "http://192.168.45.211/movies/yu_2.mp4";
    media.report_begin_url  = NULL;
    media.report_end_url    = NULL;

    //PPBOX_InsertMedia(1, &media);
    /************************************************************************/

    bool exit = false;
    while (1) {
        std::vector<std::string> cmd_args;
        if (std_in.get_one(cmd_args)) {
            if (cmd_args[0] == "open") {
                if (cmd_args.size() >= 2) {
                    name = cmd_args[1];
                }

                boost::thread th(
                    boost::bind(PlaySession::play_movie, boost::ref(conf), boost::ref(std_in), name));
                th.join();
                //play_movie(conf, std_in, name);
            } else if (cmd_args[0] == "stop") {
                break;
            } else if (cmd_args[0] == "dopen") {
                if (cmd_args.size() >= 4) {
                    download_movie(conf, std_in, cmd_args[1], cmd_args[2], cmd_args[3]);
                }
                break;
            } else if (cmd_args[0] == "popen") {
                if (cmd_args.size() >= 2) {
                    //DispatchSession::dispatch_movie(conf, std_in, cmd_args[1]);
                }
                break;
            } else if (cmd_args[0] == "exit") {
                exit = true;
                break;
            } else if (cmd_args[0] == "restart") {
                std::cout << "restart" << std::endl;
                PPBOX_StopP2PEngine();
                ec = PPBOX_StartP2PEngine(gid.c_str(), pid.c_str(), auth.c_str());
                if (ppbox_success != ec) {
                    std::cout << "start p2p engine: (" << ec << ")" << PPBOX_GetLastErrorMsg() << std::endl;
                    break;
                }
            }  else {
                    std::cout << "main: " << "unkonwn command " << cmd_args[0] << std::endl;
            }
        } else {
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        }
    }

    if (exit) {
        std::cout << "exit" << std::endl;
    } else {
        std::cout << "stop" << std::endl;
        PPBOX_StopP2PEngine();
    }

    //std::cout << "press any key to continue..." << std::flush;
    //std::cin.get();

    return 0;
}
