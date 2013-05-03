//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _ERROR_RESPONSE_TASK_H_
#define _ERROR_RESPONSE_TASK_H_

#include "ResponseTask.h"

namespace super_node
{
    class ErrorResponseTask
        : public ResponseTask
    {
    private:
        boost::uint16_t error_code_;

    public:
        ErrorResponseTask(
            const boost::asio::ip::udp::endpoint& endpoint, 
            boost::uint32_t transaction_id,
            const RID& resource_identifier,
            const framework::string::Uuid& server_guid,
            boost::uint16_t error_code,
            boost::uint16_t dest_protocol_version);

        void Execute(boost::shared_ptr<UdpServerManager> udp_server_manager);
    };
}

#endif //_ERROR_RESPONSE_TASK_H_