// Test.cpp

#include "tools/test/Common.h"
#include "tools/test/InputHandler.h"
#include "tools/test/DownloadSession.h"

#include <framework/configure/Config.h>
using namespace framework::configure;

#include <boost/thread/thread.hpp>

void download_movie(
                Config & conf, 
                InputHandler & std_in, 
                std::string const & name, 
                std::string const & format, 
                std::string const & path)
{
#ifndef PPBOX_DISABLE_DOWNLOAD

    PPBOX_Download_Handle h = 
        PPBOX_DownloadOpen(name.c_str(), format.c_str(), path.c_str(), NULL);
    if (h == PPBOX_INVALID_DOWNLOAD_HANDLE) {
        std::cout << "download open failed: " << PPBOX_GetLastErrorMsg() << std::endl;
    } else {
        std::cout << "download open succeeded: " << h << std::endl;
    }
    while (true) {
        PPBOX_DownloadStatistic stat;
        PP_int32 ec = PPBOX_GetDownloadInfo(h, &stat);
        if (ec != ppbox_success) {
            std::cout << "get download stat failed: " << PPBOX_GetLastErrorMsg() << std::endl;
        } else {
            std::cout << stat.total_size << "\t" << stat.finish_size << "\t" << stat.speed << "    \r" << std::flush;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));

        if (stat.total_size > 0 && stat.total_size == stat.finish_size) {
            PPBOX_DownloadClose(h);
            break;
        }

        std::vector<std::string> cmd_args;
        if (std_in.get_one(cmd_args)) {
            if (cmd_args[0] == "dclose") {
                PPBOX_DownloadClose(h);
                break;
            } else {
                std::cout << "download_movie: " << "unkonwn command " << cmd_args[0] << std::endl;
            }
        }
    }
    std::cout << std::endl;

    std::cout << "exit download movie" << std::endl;

#else

    std::cout << "download is disabled by strategy" << std::endl;

#endif
}
