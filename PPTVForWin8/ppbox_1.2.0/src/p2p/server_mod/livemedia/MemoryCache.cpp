//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "MemoryCache.h"
#include "ChannelCache.h"
#include "channel.h"
#include "channel_manager.h"

namespace live_media
{
    MemoryCache::MemoryCache(boost::shared_ptr<ChannelManager> channels_mgmt)
    {
        channels_mgmt_ = channels_mgmt;
    }

    void MemoryCache::Start()
    {
    }

    void MemoryCache::Stop()
    {
    }

    void MemoryCache::AsyncGetBlock(const channel_id& channel_identifier, size_t block_id, ReadBlockCallback callback)
    {
        ChannelPointer target_channel = channels_mgmt_->GetChannel(channel_identifier);

        if (!target_channel)
        {
            callback(ErrorCodes::ResourceNotFound, channel_identifier, block_id, boost::shared_ptr<BlockData>());
            return;
        }
        
        target_channel->OnVisit();
        target_channel->GetCache()->AsyncGetBlock(block_id, callback);
    }

    bool MemoryCache::TryGetConsecutiveBlocksBitmap(const channel_id& channel_identifier, size_t block_id, std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap)
    {
        ChannelPointer target_channel = channels_mgmt_->GetChannel(channel_identifier);
        if (!target_channel)
        {
            return false;
        }

        return target_channel->GetCache()->TryGetConsecutiveBlocksBitmap(block_id, blocks_bitmap);
    }
}
