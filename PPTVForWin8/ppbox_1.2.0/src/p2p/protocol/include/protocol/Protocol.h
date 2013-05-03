//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef PROTOCOL_PROTOCOL_H
#define PROTOCOL_PROTOCOL_H

#include <boost/cstdint.hpp>

#include <framework/Framework.h>
#include <framework/string/Uuid.h>

//using boost::uint32_t;
using std::string;

typedef framework::string::Uuid RID;
typedef framework::string::Uuid MD5;
typedef framework::string::Uuid Guid;

const boost::uint32_t SUB_PIECE_SIZE = 1024;
const boost::uint32_t SUB_PIECE_COUNT_PER_PIECE = 128;
const boost::uint32_t PIECE_SIZE = SUB_PIECE_SIZE * SUB_PIECE_COUNT_PER_PIECE;
const boost::uint32_t BLOCK_MIN_SIZE = 2*1024*1024;
const boost::uint32_t BLOCK_MAX_COUNT = 50;

const boost::uint32_t LIVE_SUB_PIECE_SIZE = 1400;

#include "struct/Base.h"
#include "struct/serialization.h"
#include "struct/Structs.h"

#include "protocol/BootstrapPacket.h"
#include "protocol/CachePacket.h"
#include "protocol/IndexPacket.h"
#include "protocol/Packet.h"
#include "protocol/PeerPacket.h"
#include "protocol/LivePeerPacket.h"
#include "protocol/StatisticPacket.h"
#include "protocol/StunServerPacket.h"
#include "TrackerPacket.h"
#include "protocol/PushServerPacket.h"
#include "protocol/PushServerPacketV2.h"
#include "protocol/NotifyPacket.h"

#include "protocol/TcpPeerPacket.h"
#include "protocol/NatCheckPacket.h"

#endif  // PROTOCOL_PROTOCOL_H
