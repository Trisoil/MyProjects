// Common.h

#ifndef _MUTEX_SERVER_COMMON_H_
#define _MUTEX_SERVER_COMMON_H_

#define _CHECKSUM_
#define _MEV_
#define _STDLOG_

#include <framework/Framework.h>
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerFormatRecord.h>
#include <framework/logger/LoggerStreamRecord.h>
#include <framework/logger/LoggerSection.h>
using namespace framework::logger;

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace mutexserver
{
    const boost::uint8_t KEEPALIVE_REQUEST_ACTION = 0x52;
    const boost::uint8_t KEEPALIVE_RESPONSE_ACTION = 0x72;

    const boost::uint8_t LEAVE_REQUEST_ACTION = 0x53;
    const boost::uint8_t KICKOUT_RESPONSE_ACTION = 0x75;

    void update_time();
    boost::uint16_t get_config_interval();
    void set_interval(
        boost::uint16_t interval);
    boost::uint64_t get_last_time();
    std::string byteArray2Str(
        const boost::uint8_t * arr, size_t len);
}

#endif // _MUTEX_SERVER_COMMON_H_
