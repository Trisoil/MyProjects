//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ErrorResponseTask.h"
#include "UdpServerManager.h"

namespace super_node
{
    ErrorResponseTask::ErrorResponseTask(
        const boost::asio::ip::udp::endpoint& endpoint, 
        boost::uint32_t transaction_id,
        const RID& resource_identifier,
        const framework::string::Uuid& server_guid,
        boost::uint16_t error_code,
        boost::uint16_t dest_protocol_version)
        : ResponseTask(endpoint, transaction_id, resource_identifier, server_guid, dest_protocol_version)
    {
        error_code_ = error_code;
    }

    void ErrorResponseTask::Execute(boost::shared_ptr<UdpServerManager> udp_server_manager)
    {
        protocol::ErrorPacket error_packet(
            transaction_id_, 
            resource_identifier_, 
            server_guid_, 
            error_code_,
            0, 
            "", 
            GetTargetEndpoint());

        udp_server_manager->SendPacket(error_packet, dest_protocol_version_);
    }
}