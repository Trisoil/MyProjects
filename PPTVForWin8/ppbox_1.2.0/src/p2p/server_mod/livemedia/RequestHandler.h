//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "MemoryCache.h"
class channel_manager;
class channel_server;

namespace live_media
{
    class ChannelManager;
    class ChannelServer;

    class SessionManager;
    class MemoryCache;
    class ResponseHandler;
    class ChannelCache;

    class RequestHandler
    {
    public:
        RequestHandler(boost::shared_ptr<ChannelManager> manager, boost::shared_ptr<SessionManager> session_manager);

        void HandleConnectRequest(boost::shared_ptr<protocol::ConnectPacket> connect_request);
        void HandleSubPieceRequest(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request);
        void HandleAnnounceRequest(boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request);
        
        void HandlePeerHeartBeat(const boost::asio::ip::udp::endpoint& peer, boost::uint32_t transaction_number, boost::uint8_t packet_action);
        void HandleCloseSession(const boost::asio::ip::udp::endpoint& peer);

        void Stop();

    private:
        boost::shared_ptr<ChannelManager> manager_;
        boost::shared_ptr<SessionManager> session_manager_;
        boost::shared_ptr<MemoryCache> memory_cache_;
        boost::shared_ptr<ResponseHandler> response_handler_;
        ChannelServer* server_;
    };
}

#endif //_REQUEST_HANDLER_H_
