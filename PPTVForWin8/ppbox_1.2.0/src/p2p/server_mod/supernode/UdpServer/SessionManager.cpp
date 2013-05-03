//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "SessionManager.h"
#include "Session.h"

namespace super_node
{
    SessionManager::SessionManager(boost::shared_ptr<boost::asio::io_service> io_service, const SessionManagerConfig & config)
        :io_service_(io_service),session_timer_(new boost::asio::deadline_timer(*io_service_))
    {
        max_session_num_ = config.max_session_num_;
        session_keep_time_ = config.sesson_keep_time_;
        Session::SetMaxTransactionIdNum(config.max_transaction_id_num_);
    }
    
    void SessionManager::Start()
    {
        session_timer_->expires_from_now(boost::posix_time::seconds(1));
        session_timer_->async_wait(boost::bind(&super_node::SessionManager::OnSessionTimerElapse, shared_from_this(), _1));
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

    SessionManager::Type SessionManager::TryAddSession(const boost::asio::ip::udp::endpoint & end_point, const boost::uint32_t transaction_number)
    {
        SessionMap::iterator iter = session_map_.find(end_point);
        if (iter != session_map_.end())
        {
            if ((iter->second)->IsDuplicateTransactionId(transaction_number))
            {
                return DuplicateRequest;
            } 
          
            (iter->second)->AddTransactionId(transaction_number);
            (iter->second)->ResetIdleTime();
            return SessionAlreadyExists;
        }

        if (session_map_.size() >= max_session_num_)
        {
            return MaxSessionNumberReached;
        }
       
        boost::shared_ptr<Session> new_session(new Session());
        new_session->AddTransactionId(transaction_number);
        session_map_.insert(std::make_pair(end_point, new_session));
        return NewSessionCreated;
    }

    boost::shared_ptr<Session> SessionManager::GetSession(const boost::asio::ip::udp::endpoint & end_point) const
    {
        SessionMap::const_iterator iter = session_map_.find(end_point);
        if (iter != session_map_.end())
        {
            return iter->second;
        }

        return boost::shared_ptr<Session>();
    }


    void SessionManager::CloseSession(const boost::asio::ip::udp::endpoint & end_point)
    {
        if (session_map_.find(end_point) != session_map_.end())
        {
            session_map_.erase(session_map_.find(end_point));
        }
        else
        {
//            assert(false);
        }
    }

    void SessionManager::OnSessionTimerElapse(const boost::system::error_code& error)
    {
        for (SessionMap::iterator iter = session_map_.begin();
            iter != session_map_.end(); )
        {
            (iter->second)->Tick();
            if((iter->second)->GetIdleTime() >= session_keep_time_)
            {
                session_map_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }

        if (session_timer_)
        {
            session_timer_->expires_from_now(boost::posix_time::seconds(1));
            session_timer_->async_wait(boost::bind(&super_node::SessionManager::OnSessionTimerElapse, shared_from_this(), _1));
        }
    }

    void SessionManager::SetMaxSessionNum(boost::uint16_t num)
    {
        max_session_num_ = num;
    }

    void SessionManager::SetSessionKeepTime(boost::uint16_t time)
    {
        session_keep_time_ = time;
    }

    boost::uint16_t SessionManager::GetCurrentSessionNum()
    {
        return session_map_.size();
    }

    void SessionManager::UpdateConfig()
    {
        SessionManagerConfig settings;
        settings.LoadConfig();
        SetMaxSessionNum(settings.max_session_num_);
        SetSessionKeepTime(settings.sesson_keep_time_);
        Session::SetMaxTransactionIdNum(settings.max_transaction_id_num_);
    }
}
