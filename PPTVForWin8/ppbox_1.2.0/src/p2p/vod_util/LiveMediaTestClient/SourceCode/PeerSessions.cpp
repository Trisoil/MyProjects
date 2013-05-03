//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "PeerSessions.h"

namespace live_media_test
{
    PeerSessions::PeerSessions(boost::shared_ptr<boost::asio::io_service> io_service)
        : io_service_(io_service)
    {
    }

    void PeerSessions::RemoveExpiredSessions()
    {
        for(std::map<int, boost::shared_ptr<ClientSession> >::iterator iter = active_sessions_.begin();
            iter != active_sessions_.end();)
        {
            if (iter->second->Expires())
            {
                active_sessions_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }

    boost::uint32_t PeerSessions::NewTransactionId(int peer_id)
    {
        std::map<int, boost::shared_ptr<ClientSession> >::iterator iter = active_sessions_.find(peer_id);
        if (iter == active_sessions_.end())
        {
            active_sessions_.insert(std::make_pair(peer_id, boost::shared_ptr<ClientSession>(new ClientSession(io_service_))));
        }

        return active_sessions_[peer_id]->NewTransactionId();
    }

    void PeerSessions::SendPacket(int peer_id, const UdpBuffer& send_buffer)
    {
        std::map<int, boost::shared_ptr<ClientSession> >::const_iterator iter = active_sessions_.find(peer_id);
        if (iter == active_sessions_.end())
        {
            //session should have created (for NewTransactionId)
            assert(false);
            active_sessions_.insert(std::make_pair(peer_id, boost::shared_ptr<ClientSession>(new ClientSession(io_service_))));
        }

        active_sessions_[peer_id]->Send(send_buffer);
    }
}