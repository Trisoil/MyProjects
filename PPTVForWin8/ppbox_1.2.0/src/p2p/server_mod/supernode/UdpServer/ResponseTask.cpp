//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResponseTask.h"
#include "UdpServerManager.h"

namespace super_node
{
    ResponseTask::ResponseTask(
        const boost::asio::ip::udp::endpoint& endpoint,
        boost::uint32_t transaction_id,
        const RID& resource_identifier,
        const framework::string::Uuid& server_guid,
        boost::uint16_t dest_protocol_version
        )
        :endpoint_(endpoint), resource_identifier_(resource_identifier), server_guid_(server_guid)
    {
        transaction_id_ = transaction_id;
        dest_protocol_version_ = dest_protocol_version;
    }
}