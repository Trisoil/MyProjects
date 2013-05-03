// AccServer.cpp : 定义控制台应用程序的入口点。
//

#include "Common.h"

#include "UdpServer.h"
#include "ManageInterface.h"
#include "ManageInterfaceOld.h"
#include "CacheManager.h"
#include "DiskManager.h"
#include "Version.h"

//#include <SapClient.h>
//#include <SapManagerI.h>
#include <PFSErrorMsg.h>

#include <Framework/PeriodAction.h>
#include <Framework/UserIf.h>
#include <Framework/Daemon.h>
#include <Framework/Logger.h>
using namespace Util;

boost::asio::io_service global_io_service;

namespace SharedMemory
{
    ErrorCode conf_set_log_level(
        std::string const & value)
    {
        size_t log_level;
        ErrorCode ret = Parse::parse2(value, log_level);
        if (ret == ERROR_SUCCEED)
            SharedMemoryPool::set_log_level(log_level);
        return ret;
    }

    ErrorCode conf_get_log_level(
        std::string & value)
    {
        size_t log_level = SharedMemoryPool::set_log_level();
        value = Format::format(log_level);
        return ERROR_SUCCEED;
    }

    ErrorCode init(
        Config & conf)
    {
        Config::ConfigItem params[] = {
            {"log_level", Config::allow_get | Config::allow_set, conf_set_log_level, conf_get_log_level}, 
        };

        conf.register_module("SharedMemoryPool", sizeof(params) / sizeof(params[0]), params);

        return ERROR_SUCCEED;
    }

    ErrorCode start(
        boost::asio::io_service &)
    {
        if (SharedMemoryPool::init(SHARED_MEMORY_POOL_NAME) != 0) {
            size_t log_level = SharedMemoryPool::set_log_level();
            logger.Log(Logger::kLevelError, log_level, "SharedMemoryPool init failed!");
            return ERROR_ERROR;
        }

        return ERROR_SUCCEED;
    }

    ErrorCode stop()
    {
        SharedMemoryPool::term();
        return ERROR_SUCCEED;
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'r') {
        // 删除共享内存
        SharedMemoryPool::clear(SHARED_MEMORY_POOL_NAME);
    }

    Daemon::init("ImageServer", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    PFSErrorMsg::init();

    DAEMON_ADD_MODULE(SharedMemory);
    DAEMON_ADD_MODULE(PeriodAction);
    DAEMON_ADD_MODULE(DiskManager);
    DAEMON_ADD_MODULE(CacheManager);
    //DAEMON_ADD_MODULE(SapManagerI);
    DAEMON_ADD_MODULE(UdpServer);
    DAEMON_ADD_MODULE(ManageInterface);
    DAEMON_ADD_MODULE(ManageInterfaceOld);
    DAEMON_ADD_MODULE(UserIf);
    //DAEMON_ADD_MODULE(SapClient);

    //SapClient::add_sap_module("UdpServer", Service::acc);

    int ret = Daemon::main(argc, argv);

    return ret;
}
