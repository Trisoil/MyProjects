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
#include <boost/filesystem.hpp>

#include <fstream>
#include <time.h>

#include <ctime>

#include <string>
using std::string;

#include <protocol/Protocol.h>
#include <framework/configure/Config.h>
#include <struct/SubPieceInfo.h>
#include <struct/SubPieceBuffer.h>
#include <protocol/CheckSum.h>
#include <struct/UdpBuffer.h>
#include <util/buffers/SubBuffers.h>
#include <util/buffers/BufferCopy.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
