//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef CHANNEL_H
#define CHANNEL_H

#include "UploadSpeedConstraint.h"
#include "SessionManagement/SessionsCountConstraint.h"
#include "BandwidthAdjustmentHandler.h"
#include "ChannelCache.h"
#include "Configuration.h"

namespace live_media
{
    class ChannelServer;
    class SessionManager;
    class UploadManager;
    class SpeedStatistics;

    class IChannel
    {
    public:
        virtual channel_id GetChannelId() const = 0;
        virtual boost::shared_ptr<IChannelCache> GetCache() const = 0;
        virtual ~IChannel(){}
    };

    class Channel 
        : public IChannel,
        public BandwidthAdjustmentHandler,
        public boost::enable_shared_from_this<Channel> 
    {
    public:
        Channel(const channel_id & channel_identifier, ChannelServer* channel_server, boost::shared_ptr<boost::asio::io_service> io_service_);
        ~Channel();

        channel_id GetChannelId() const;

        bool Initalize();
        bool Uninitalize();

        boost::shared_ptr<IChannelCache> GetCache() const
        {
            return channel_cache_;
        }

        void SetChannelConfig(const ChannelConfiguration & channel_config);

        void OnTimer();

        void OnIncreaseBandwidth();
        void OnDecreaseBandwidth();
        

        bool AcceptsNewSession() const;

        void OnVisit() { last_visit_time_ = ::time(0); }
        time_t GetLastVisitTime() const { return last_visit_time_; }

    public:
        boost::shared_ptr<SpeedStatistics> speed_statistics_;

    private:
        boost::shared_ptr<UploadManager> upload_manager_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        channel_id channel_id_;

        boost::shared_ptr<ChannelCache> channel_cache_;

        boost::shared_ptr<SessionManager> session_manager_;
        SessionsCountConstraint channel_sessions_count_constraint_;
        time_t last_visit_time_;

    public:
        // 
        ChannelConfiguration channel_config_;
    };

    typedef boost::shared_ptr<Channel> ChannelPointer;
    typedef boost::shared_ptr<IChannel> IChannelPointer;
}

#endif //CHANNEL_H
