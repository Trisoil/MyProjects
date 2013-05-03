//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#pragma once   
#include <framework/Framework.h>

#include <util/protocol/http/HttpClient.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <time.h>

#include <ctime>

#include "Loggers.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
using namespace log4cplus;
using namespace log4cplus::helpers;


#include <string>
using std::string;

