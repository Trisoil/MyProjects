#ifndef __PUSH_SERVER_STDAFX_H__
#define __PUSH_SERVER_STDAFX_H__                     

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <framework/Framework.h>
#include <framework/string/Uuid.h>
#include <framework/timer/TickCounter.h>
#include <framework/timer/Timer.h>
#include <framework/timer/TimerQueue.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerStreamRecord.h>

#include <util/Util.h>

#include <protocol/UdpServer.h>

namespace framework
{
    typedef framework::string::Uuid Guid;
}

using std::string;
using std::vector;
using framework::Guid;

using boost::int8_t;
using boost::int16_t;
using boost::int32_t;
using boost::int64_t;

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

#endif /* __PUSH_SERVER_STDAFX_H__ */