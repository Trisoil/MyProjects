//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#pragma once

#include <boost/cstdint.hpp>
#include <framework/Framework.h>
#include <util/protocol/http/HttpClient.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>

#include <string>
using std::string;

#include "ErrorCode.h"
#include "ResourceIdentifier.h"
#include "BlockData.h"
#include "Loggers.h"
#include "OperationsLogging.h"

#include "count_object_allocate.h"

#include <log4cplus/logger.h>
using namespace log4cplus;
using namespace log4cplus::helpers;

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;