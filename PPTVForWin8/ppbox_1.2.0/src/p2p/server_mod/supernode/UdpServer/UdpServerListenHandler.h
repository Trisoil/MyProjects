//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _UDP_SERVER_LISTEN_HANDLER_H_
#define _UDP_SERVER_LISTEN_HANDLER_H_

#include "protocol/UdpServer.h"

using protocol::IUdpServerListener;

namespace super_node
{
    class MemoryCache;
    class UdpServerManager;
    class SessionManager;
    class UdpServerStatistics;
    class ResponseSender;
    class ResponseTask;

    class UdpServerListenHandler
        :public IUdpServerListener,
        public boost::enable_shared_from_this<UdpServerListenHandler>,
        public count_object_allocate<UdpServerListenHandler>
    {
    public:
        UdpServerListenHandler(boost::shared_ptr<MemoryCache> memory_cache, boost::shared_ptr<SessionManager> session_manager, size_t response_senders_count);

        virtual void OnUdpRecv(protocol::Packet const & packet);
        void Start(boost::shared_ptr<UdpServerManager> udp_server);
        void Stop();

    private:
        void HandleReadBlock(const ErrorCode & error_code, const boost::shared_ptr<BlockData> & block,
            const RID & resource_id, boost::uint32_t transaction_id, boost::uint16_t block_index,
            const std::set<boost::uint16_t> & subpiece_indexs, boost::uint16_t dest_protocol_version,
            const boost::asio::ip::udp::endpoint & end_point);

        void AddResponseTask(boost::shared_ptr<ResponseTask> response_task);
        size_t GetResponseSenderIndex(boost::shared_ptr<ResponseTask> response_task);

    private:
        boost::shared_ptr<UdpServerManager> server_;
        boost::shared_ptr<SessionManager> session_manager_;
        boost::shared_ptr<MemoryCache> memory_cache_;
        framework::string::Uuid guid_;

        std::vector<boost::shared_ptr<ResponseSender> > response_senders_;
        size_t fallback_sender_index_;
        size_t response_senders_count_;

        boost::shared_ptr<UdpServerStatistics> statistics_;
        framework::timer::TimeCounter time_counter_;
    };
}
#endif// UDP_SERVICE_LISTEN_HANDLER_H