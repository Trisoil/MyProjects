//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _CHANNEL_CACHE_H_
#define _CHANNEL_CACHE_H_

#include "BlockCache.h"
#include "ActiveBlockDataFetcher.h"
#include "CacheManager.h"

namespace live_media
{
    class BlockCache;
    class IRemoteCache;
    class ActiveBlockDataFetcher;
    class HitsStatistics;
    typedef std::map<size_t, boost::shared_ptr<BlockCache> > BlocksCacheMap;

    class IChannelCache
    {
    public:
        virtual void OnTimer()
        {}

        virtual void AsyncGetBlock(size_t block_id, ReadBlockCallback callback) const
        {}

        virtual bool TryGetConsecutiveBlocksBitmap(size_t block_id, std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap)
        {
            return true;
        }

        virtual void CleanUpObsoleteBlockDatas(boost::shared_ptr<ExpirationStrategy>)
        {}
        
        virtual const BlocksCacheMap& GetBlocks() const = 0;
        virtual size_t GetBlockCacheCount() const = 0;
        virtual size_t GetBlockDataCount() const = 0;
        

        virtual ~IChannelCache()
        {}
    };

    class ChannelCache
        : public boost::enable_shared_from_this<ChannelCache>,
        public FetchResultListener,
        public IChannelCache
    {
        const static size_t channel_step_size = 5;
       
    public:
        ChannelCache(
            const channel_id& channel_identifier, 
            boost::shared_ptr<IRemoteCache> remote_cache, 
            boost::shared_ptr<boost::asio::io_service> io_service,
            boost::shared_ptr<HitsStatistics> announce_request_relative_to_max_hits_statistics,
            boost::shared_ptr<HitsStatistics> subpiece_request_relative_to_max_hits_statistics);
        void Start();

        void CleanUpObsoleteBlockDatas(boost::shared_ptr<ExpirationStrategy> strategy); 

        void AsyncGetBlock(size_t block_id, ReadBlockCallback callback) const;

        void AddBlockCache(size_t block_id, boost::shared_ptr<BlockData> block_data);

        //always return true
        bool TryGetConsecutiveBlocksBitmap(size_t block_id, std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap);

        void OnTimer();

        void OnBlockFetched(const channel_id& channel_identifier, size_t block_id, boost::shared_ptr<BlockData> block_data);

        size_t GetBlockCacheCount() const;

        size_t GetBlockDataCount() const;

    protected:
        //for testability
        const BlocksCacheMap& GetBlocks() const
        {
            return blocks_;
        }

        bool TryGetDistanceToMaxBlock(size_t block_id, int& distance) const;

    private:
        boost::shared_ptr<HitsStatistics> announce_request_relative_to_max_hits_statistics_;
        boost::shared_ptr<HitsStatistics> subpiece_request_relative_to_max_hits_statistics_;
        BlocksCacheMap blocks_;
        channel_id channel_identifier_;
        boost::shared_ptr<IRemoteCache> remote_cache_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<ActiveBlockDataFetcher> fetcher_;
    };
}

#endif //_CHANNEL_CACHE_H_