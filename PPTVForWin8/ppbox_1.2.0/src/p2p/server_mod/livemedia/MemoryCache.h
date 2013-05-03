//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _MEMORY_CACHE_H_
#define _MEMORY_CACHE_H_

#include "BlockCache.h"

namespace live_media
{
    class ChannelManager;

    class MemoryCache
        : public boost::enable_shared_from_this<MemoryCache>
    {
    public:
        MemoryCache(boost::shared_ptr<ChannelManager> channels_mgmt);

        void Start();
        void Stop();

        void AsyncGetBlock(const channel_id& channel_identifier, size_t block_id, ReadBlockCallback callback);
        bool TryGetConsecutiveBlocksBitmap(const channel_id& channel_identifier, size_t block_id, std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap);

    private:
        boost::shared_ptr<ChannelManager> channels_mgmt_;
    };
}

#endif //_MEMORY_CACHE_H_
