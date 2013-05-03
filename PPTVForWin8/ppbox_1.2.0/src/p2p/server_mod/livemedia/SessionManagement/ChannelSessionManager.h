//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _LIVE_MEDIA_CHANNEL_SESSION_MANAGER_H_
#define _LIVE_MEDIA_CHANNEL_SESSION_MANAGER_H_

namespace live_media
{
    class ChannelManager;

    class IChannelSessionManager
    {
    public:
        virtual bool AcceptsNewSession(const channel_id& channel_identifier) = 0;
        virtual ~IChannelSessionManager(){}
    };

    class ChannelSessionManager
        : public IChannelSessionManager
    {
    public:
        ChannelSessionManager(boost::shared_ptr<ChannelManager> channel_manager);
        bool AcceptsNewSession(const channel_id& channel_identifier);

    private:
        boost::shared_ptr<ChannelManager> channel_manager_;
    };
}

#endif //_LIVE_MEDIA_CHANNEL_SESSION_MANAGER_H_