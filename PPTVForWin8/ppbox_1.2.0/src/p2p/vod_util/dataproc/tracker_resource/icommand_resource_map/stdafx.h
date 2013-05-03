// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/function/function_base.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <framework/Framework.h>
#include <framework/string/Uuid.h>
#include <framework/string/Format.h>
#include <framework/string/Parse.h>
#include <framework/timer/TickCounter.h>
#include <framework/timer/Timer.h>
#include <framework/timer/TimerQueue.h>
#include <framework/configure/Config.h>
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerStreamRecord.h>

#include <util/Util.h>
#include <util/serialization/Uuid.h>

#include "protocol/UdpServer.h"

#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>

#include <iostream>

#include "protocol/TrackerPacket.h"

// TODO: reference additional headers your program requires here
