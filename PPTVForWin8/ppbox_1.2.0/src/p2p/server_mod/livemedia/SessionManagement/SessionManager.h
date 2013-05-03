//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _LIVE_MEDIA_SESSION_MANAGER_H_
#define _LIVE_MEDIA_SESSION_MANAGER_H_

#include "Session.h"
#include "SessionsCountConstraint.h"
#include "Configuration.h"
#include "hash_map.h"

namespace live_media
{
    class IChannelSessionManager;

    class SessionsCollection
    {
        struct endpoint_hash
        {
            enum
            {
                bucket_size = 4,	// 0 < bucket_size
                min_buckets = 1 << 16   // minimum number of buckets, power of 2, >0 
            };

            size_t operator()(const boost::asio::ip::udp::endpoint& end_point) const 
            { 
                protocol::SocketAddr sock_addr(end_point);
                return (sock_addr.IP*59)^sock_addr.Port;
            }

            bool operator()(const boost::asio::ip::udp::endpoint& left, const boost::asio::ip::udp::endpoint& right) const 
            {
                return left < right;
            }
        };

        typedef hash_map<boost::asio::ip::udp::endpoint, boost::shared_ptr<Session>, endpoint_hash> SessionsMap;

    public:
        boost::shared_ptr<Session> Find(const boost::asio::ip::udp::endpoint& end_point) const
        {
            SessionsMap::const_iterator iter = session_map_.find(end_point);
            if (iter != session_map_.end())
            {
                return iter->second;
            }

            return boost::shared_ptr<Session>();
        }

        size_t Count() const { return session_map_.size(); }

        size_t GetChannelSessionsCount(const channel_id& channel_identifier) const;
        size_t GetChannelUploadSessionsCount(const channel_id& channel_identifier) const;

        void Add(const boost::asio::ip::udp::endpoint& end_point, boost::shared_ptr<Session> session);
        bool Remove(const boost::asio::ip::udp::endpoint& end_point);
        size_t RemoveExpiredSessions(size_t session_keep_time);
        void CaculateUploadSessionsByChannel();
        void SetUploadSessionTimeLimit(boost::uint32_t limit)
        {
            upload_session_time_limit_in_second_ = limit;
        }

    private:
        void IncrementChannelSessionCount(const channel_id& channel_identifier);
        void DecrementChannelSessionCount(const channel_id& channel_identifier);

    private:
        SessionsMap session_map_;
        std::map<channel_id, size_t> sessions_by_channel_;
        std::map<channel_id, size_t> upload_sessions_by_channel_;
        boost::uint32_t upload_session_time_limit_in_second_;
    };

    class SessionManager
        :public boost::enable_shared_from_this<SessionManager>
    {
    public:
        enum Type
        {
           SessionAlreadyExists,
           NewSessionCreated,
           MaxSessionNumberReached,
           DuplicateRequest,
           SessionNotFound
        };

        SessionManager(boost::shared_ptr<boost::asio::io_service> io_service, const SessionManagementConfiguration& config, boost::shared_ptr<IChannelSessionManager> channel_session_manager);
        SessionManager::Type TryAddSession(const boost::asio::ip::udp::endpoint & end_point, const channel_id& channel_identifier, const boost::uint32_t transaction_number);

        SessionManager::Type TryUpdateSession(const boost::asio::ip::udp::endpoint & end_point, boost::uint32_t transaction_number, boost::uint8_t packet_action);

        void Start();
        void Stop();
        void UpdateConfiguration(const SessionManagementConfiguration & config);
        void CloseSession(const boost::asio::ip::udp::endpoint & end_point);
        void SetMaxSessionNum(const boost::uint16_t num);
        void SetSessionKeepTime(const boost::uint16_t time);
        size_t GetCurrentSessionNum() const;

        boost::shared_ptr<Session> GetSession(const boost::asio::ip::udp::endpoint & end_point) const
        {
            return sessions_.Find(end_point);
        }

        void DecreaseSessionsCount();
        void IncreaseSessionsCount();

        size_t GetChannelSessionsCount(const channel_id& channel_identifier) const;
        size_t GetChannelUploadSessionCount(const channel_id& channel_identifier) const;

    private:
        void OnSessionTimerElapse(const boost::system::error_code& error);
        SessionsCountConstraint sessions_count_contraint_;

    private:
        boost::uint16_t session_keep_time_;
        SessionsCollection sessions_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> session_timer_;
        boost::shared_ptr<IChannelSessionManager> channel_session_manager_;
    };
}

#endif //_LIVE_MEDIA_SESSION_MANAGER_H_