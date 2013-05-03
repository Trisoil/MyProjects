//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "pre.h"
#include "ChannelSessionManager.h"
#include "channel_manager.h"
#include "channel.h"

namespace live_media
{
    ChannelSessionManager::ChannelSessionManager(boost::shared_ptr<ChannelManager> channel_manager)
        : channel_manager_(channel_manager)
    {
    }

    bool ChannelSessionManager::AcceptsNewSession(const channel_id& channel_identifier)
    {
        ChannelPointer target_channel = channel_manager_->GetChannel(channel_identifier);
        return target_channel && target_channel->AcceptsNewSession();
    }
}