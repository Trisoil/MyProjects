//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _SUBPIECE_RESPONSE_TASK_H_
#define _SUBPIECE_RESPONSE_TASK_H_

#include "ResponseTask.h"

namespace super_node
{
    class SubPieceResponseTask
        : public ResponseTask
    {
        boost::uint16_t subpiece_index_;
        boost::shared_ptr<BlockData> block_data_;
        protocol::SubPieceInfo subpiece_info_;

    public:
        SubPieceResponseTask(
            const boost::asio::ip::udp::endpoint& endpoint,
            boost::shared_ptr<BlockData> block_data,
            boost::uint16_t subpiece_index,
            boost::uint32_t transaction_id,
            const RID& resource_identifier,
            const framework::string::Uuid& server_guid,
            const protocol::SubPieceInfo& subpiece_info, 
            boost::uint16_t dest_protocol_version);

        void Execute(boost::shared_ptr<UdpServerManager> udp_server_manager);
    };
}

#endif //_SUBPIECE_RESPONSE_TASK_H_