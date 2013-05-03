//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef PRE_H
#define PRE_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/date_time/posix_time/posix_time_types.hpp>


#include <boost/thread/mutex.hpp>

#include <framework/Framework.h>
#include <framework/configure/Profile.h>
#include <framework/string/Uuid.h>

#include <framework/timer/Timer.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/timer/TickCounter.h>

#include <util/serialization/Uuid.h>

#include <protocol/Protocol.h>
#include <protocol/UdpServer.h>
#include <protocol/LivePeerPacket.h>
#include <protocol/PeerPacket.h>

#include <util/Util.h>

#include "Loggers.h"
#include "ErrorCode.h"
#include "BlockData.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
using namespace log4cplus;
using namespace log4cplus::helpers;

typedef framework::string::Uuid channel_id;
typedef framework::string::Uuid peer_id;

using namespace boost;

const std::string SERVER_VERSION = "1.1.0.3";

#ifdef WIN32

const std::string LIVE_MEDIA_SERVER_VERSION = SERVER_VERSION;

#include <direct.h>


#else

const std::string LIVE_MEDIA_SERVER_VERSION  = SERVER_VERSION + std::string(".x64");

#endif

#endif
