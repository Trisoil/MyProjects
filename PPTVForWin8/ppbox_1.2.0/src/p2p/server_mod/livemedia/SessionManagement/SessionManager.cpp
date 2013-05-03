//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "pre.h"
#include "SessionManager.h"
#include "ChannelSessionManager.h"

namespace live_media
{
    void SessionsCollection::Add(const boost::asio::ip::udp::endpoint& end_point, boost::shared_ptr<Session> session)
    {
        assert(!Find(end_point));
        assert(session);
        session_map_[end_point] = session;
        IncrementChannelSessionCount(session->GetChannelId());
    }

    bool SessionsCollection::Remove(const boost::asio::ip::udp::endpoint& end_point)
    {
        boost::shared_ptr<Session> session_to_remove = Find(end_point);
        if (session_to_remove)
        {
            DecrementChannelSessionCount(session_to_remove->GetChannelId());
            session_map_.erase(end_point);
            return true;
        }

        return false;
    }

    size_t SessionsCollection::RemoveExpiredSessions(size_t session_keep_time)
    {
        size_t removed_sessions(0);

        for (SessionsMap::iterator iter = session_map_.begin();
            iter != session_map_.end(); )
        {
            boost::shared_ptr<Session> current_session = iter->second;
            current_session->Tick();
            if(current_session->GetIdleTime() >= session_keep_time)
            {
                DecrementChannelSessionCount(current_session->GetChannelId());
                session_map_.erase(iter++);

                ++removed_sessions;
            }
            else
            {
                ++iter;
            }
        }

        return removed_sessions;
    }

    void SessionsCollection::CaculateUploadSessionsByChannel()
    {
        upload_sessions_by_channel_.clear();
        for(SessionsMap::iterator iter = session_map_.begin();
            iter != session_map_.end(); iter++)
        {

            if (iter->second->GetUploadIdleTime() <= upload_session_time_limit_in_second_ * 1000)
            {
                if(upload_sessions_by_channel_.find(iter->second->GetChannelId()) == upload_sessions_by_channel_.end())
                {
                    upload_sessions_by_channel_.insert(std::make_pair(iter->second->GetChannelId(), 1));
                }
                else
                {
                    ++upload_sessions_by_channel_[iter->second->GetChannelId()];
                }
            }
        }
    }

    void SessionsCollection::IncrementChannelSessionCount(const channel_id& channel_identifier)
    {
        std::map<channel_id, size_t>::iterator iter = sessions_by_channel_.find(channel_identifier);
        if (iter != sessions_by_channel_.end())
        {
            ++iter->second;
        }
        else
        {
            sessions_by_channel_[channel_identifier] = 1;
        }
    }

    void SessionsCollection::DecrementChannelSessionCount(const channel_id& channel_identifier)
    {
        std::map<channel_id, size_t>::iterator iter = sessions_by_channel_.find(channel_identifier);
        if (iter != sessions_by_channel_.end())
        {
            --iter->second;
            if (iter->second == 0)
            {
                sessions_by_channel_.erase(iter);
            }
        }
    }

    size_t SessionsCollection::GetChannelSessionsCount(const channel_id& channel_identifier) const
    {
        std::map<channel_id, size_t>::const_iterator iter = sessions_by_channel_.find(channel_identifier);
        if (iter != sessions_by_channel_.end())
        {
            return iter->second;
        }

        return 0;
    }

    size_t SessionsCollection::GetChannelUploadSessionsCount(const channel_id& channel_identifier) const
    {
        std::map<channel_id, size_t>::const_iterator iter = upload_sessions_by_channel_.find(channel_identifier);
        if (iter != upload_sessions_by_channel_.end())
        {
            return iter->second;
        }

        return 0;
    }

    SessionManager::SessionManager(
        boost::shared_ptr<boost::asio::io_service> io_service, 
        const SessionManagementConfiguration & config, 
        boost::shared_ptr<IChannelSessionManager> channel_session_manager)
        :io_service_(io_service), session_timer_(new boost::asio::deadline_timer(*io_service_)), channel_session_manager_(channel_session_manager)
    {
        UpdateConfiguration(config);
    }
    
    void SessionManager::Start()
    {
        session_timer_->expires_from_now(boost::posix_time::seconds(1));
        session_timer_->async_wait(boost::bind(&SessionManager::OnSessionTimerElapse, shared_from_this(), _1));
    }

    void SessionManager::Stop()
    {
        if (session_timer_)
        {
            boost::system::error_code cancel_error;
            session_timer_->cancel(cancel_error);
            session_timer_.reset();
        }
    }

    void SessionManager::UpdateConfiguration(const SessionManagementConfiguration & config)
    {
        sessions_count_contraint_.SetHardLimit(config.max_session_num_);
        session_keep_time_ = config.sesson_keep_time_;
        sessions_.SetUploadSessionTimeLimit(config.upload_session_time_limit_in_second_);
        Session::SetMaxTransactionIdNum(config.max_transaction_id_num_);
    }

    SessionManager::Type SessionManager::TryAddSession(const boost::asio::ip::udp::endpoint & end_point, const channel_id& channel_identifier, const boost::uint32_t transaction_number)
    {
        boost::shared_ptr<Session> existing_session = sessions_.Find(end_point);
        if (existing_session)
        {
            if (existing_session->IsDuplicateTransactionId(transaction_number))
            {
                return DuplicateRequest;
            }

            existing_session->AddTransactionId(transaction_number);
            existing_session->ResetIdleTime();
            return SessionAlreadyExists;
        }
        
        if (!sessions_count_contraint_.AcceptsNew(sessions_.Count()))
        {
            LOG4CPLUS_TRACE(Loggers::SessionManager(), "Rejected a request as the global sessions limit is reached.");
            return MaxSessionNumberReached;
        }

        if (!channel_session_manager_->AcceptsNewSession(channel_identifier))
        {
            LOG4CPLUS_TRACE(Loggers::SessionManager(), "Rejected a request as the sessions limit for channel "<<channel_identifier<<" is reached.");
            return MaxSessionNumberReached;
        }
       
        boost::shared_ptr<Session> new_session(new Session(channel_identifier));
        new_session->AddTransactionId(transaction_number);
        sessions_.Add(end_point, new_session);
        return NewSessionCreated;
    }

    void SessionManager::CloseSession(const boost::asio::ip::udp::endpoint & end_point)
    {
        sessions_.Remove(end_point);
    }

    SessionManager::Type SessionManager::TryUpdateSession(const boost::asio::ip::udp::endpoint & end_point, boost::uint32_t transaction_number, boost::uint8_t packet_action)
    {
        boost::shared_ptr<Session> target_session = sessions_.Find(end_point);
        if (target_session)
        {
            if (target_session->IsDuplicateTransactionId(transaction_number))
            {
                return DuplicateRequest;
            }

            target_session->ResetIdleTime();
            if (packet_action == protocol::LiveRequestSubPiecePacket::Action)
            {
                target_session->SetUploadTime();
            }

            return SessionAlreadyExists;
        }

        return SessionNotFound;
    }

    void SessionManager::OnSessionTimerElapse(const boost::system::error_code& error)
    {
        sessions_.RemoveExpiredSessions(session_keep_time_);
        sessions_.CaculateUploadSessionsByChannel();

        LOG4CPLUS_INFO(Loggers::SessionManager(), "Current global sessions count:"<<sessions_.Count()<<", limit:"<<sessions_count_contraint_.GetCurrentLimit());

        if (session_timer_)
        {
            session_timer_->expires_from_now(boost::posix_time::seconds(1));
            session_timer_->async_wait(boost::bind(&SessionManager::OnSessionTimerElapse, shared_from_this(), _1));
        }
    }

    void SessionManager::SetMaxSessionNum(boost::uint16_t num)
    {
        sessions_count_contraint_.SetHardLimit(num);
    }

    void SessionManager::SetSessionKeepTime(boost::uint16_t time)
    {
        session_keep_time_ = time;
    }

    size_t SessionManager::GetCurrentSessionNum() const
    {
        return sessions_.Count();
    }

    size_t SessionManager::GetChannelSessionsCount(const channel_id& channel_identifier) const
    {
        return sessions_.GetChannelSessionsCount(channel_identifier);
    }

    size_t SessionManager::GetChannelUploadSessionCount(const channel_id& channel_identifier) const
    {
        return sessions_.GetChannelUploadSessionsCount(channel_identifier);
    }
    void SessionManager::DecreaseSessionsCount()
    {
        LOG4CPLUS_INFO(Loggers::SessionManager(), "Decreasing global sessions limit");
        sessions_count_contraint_.DecreaseLimit(GetCurrentSessionNum());
    }

    void SessionManager::IncreaseSessionsCount()
    {
        LOG4CPLUS_INFO(Loggers::SessionManager(), "Increasing global sessions limit");
        sessions_count_contraint_.IncreaseLimit(GetCurrentSessionNum());
    }
}