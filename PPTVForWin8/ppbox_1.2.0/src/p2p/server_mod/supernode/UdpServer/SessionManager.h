//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _SN_SESSION_MANAGER_H_
#define _SN_SESSION_MANAGER_H_

#include "SuperNodeServiceStruct.h"

namespace super_node
{
    class Session;

    class SessionManager
        :public boost::enable_shared_from_this<SessionManager>,
        public count_object_allocate<SessionManager>
    {
        typedef std::map<boost::asio::ip::udp::endpoint, boost::shared_ptr<Session> > SessionMap;
    public:
        enum Type
        {
           SessionAlreadyExists,
           NewSessionCreated,
           MaxSessionNumberReached,
           DuplicateRequest,
        };

        SessionManager(boost::shared_ptr<boost::asio::io_service> io_service, const SessionManagerConfig & config);
        SessionManager::Type TryAddSession(const boost::asio::ip::udp::endpoint & end_point, const boost::uint32_t transaction_number);
        
        boost::shared_ptr<Session> GetSession(const boost::asio::ip::udp::endpoint & end_point) const;

        void Start();
        void Stop();
        void CloseSession(const boost::asio::ip::udp::endpoint & end_point);
        void SetMaxSessionNum(const boost::uint16_t num);
        void SetSessionKeepTime(const boost::uint16_t time);
        boost::uint16_t GetCurrentSessionNum();
        void UpdateConfig();

    private:
        void OnSessionTimerElapse(const boost::system::error_code& error);

    private:
        boost::uint16_t max_session_num_;
        boost::uint16_t session_keep_time_;
        SessionMap session_map_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> session_timer_;
    };
}
#endif