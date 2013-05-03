//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _RESPONSE_TASK_H_
#define _RESPONSE_TASK_H_

namespace super_node
{
    class UdpServerManager;

    class ResponseTask
    {
    protected:
        ResponseTask(
            const boost::asio::ip::udp::endpoint& endpoint,
            boost::uint32_t transaction_id,
            const RID& resource_identifier,
            const framework::string::Uuid& server_guid,
            boost::uint16_t dest_protocol_version
            );

    public:
        const boost::asio::ip::udp::endpoint& GetTargetEndpoint() const { return endpoint_; }
        virtual void Execute(boost::shared_ptr<UdpServerManager> udp_server_manager) = 0;
        virtual ~ResponseTask(){}

    protected:
        boost::uint32_t transaction_id_;
        RID resource_identifier_;
        framework::string::Uuid server_guid_;
        boost::uint16_t dest_protocol_version_;

    private:
        boost::asio::ip::udp::endpoint endpoint_;
    };
}

#endif //_RESPONSE_TASK_H_