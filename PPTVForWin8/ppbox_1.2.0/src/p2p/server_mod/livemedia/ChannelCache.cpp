//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "ChannelCache.h"
#include "BlockCache.h"
#include "packet_statistics.h"

namespace live_media
{
    ChannelCache::ChannelCache(
        const channel_id& channel_identifier, 
        boost::shared_ptr<IRemoteCache> remote_cache, 
        boost::shared_ptr<boost::asio::io_service> io_service,
        boost::shared_ptr<HitsStatistics> announce_request_relative_to_max_hits_statistics,
        boost::shared_ptr<HitsStatistics> subpiece_request_relative_to_max_hits_statistics)
        : remote_cache_(remote_cache), io_service_(io_service), channel_identifier_(channel_identifier)
    {
        announce_request_relative_to_max_hits_statistics_ = announce_request_relative_to_max_hits_statistics;
        subpiece_request_relative_to_max_hits_statistics_ = subpiece_request_relative_to_max_hits_statistics;
    }

    void ChannelCache::Start()
    {
        fetcher_.reset(new ActiveBlockDataFetcher(channel_identifier_, io_service_, remote_cache_, shared_from_this()));
    }

    bool ChannelCache::TryGetDistanceToMaxBlock(size_t block_id, int& distance) const
    {
        distance = 0;
        if (blocks_.size() > 0)
        {
            size_t max_block_id = blocks_.rbegin()->first;
            distance = static_cast<int64_t>(max_block_id) - block_id;
            return true;
        }

        return false;
    }

    void ChannelCache::AsyncGetBlock(size_t block_id, ReadBlockCallback callback) const
    {
        int distance_to_max_block(0);
        if (TryGetDistanceToMaxBlock(block_id, distance_to_max_block))
        {
            subpiece_request_relative_to_max_hits_statistics_->AddHit(distance_to_max_block);
        }

        BlocksCacheMap::const_iterator target_block = blocks_.find(block_id);
        if (target_block != blocks_.end())
        {
            target_block->second->AsyncGetBlock(callback);
            return;
        }

        callback(ErrorCodes::ResourceNotFound, channel_identifier_, block_id, boost::shared_ptr<BlockData>());
    }

    void ChannelCache::AddBlockCache(size_t block_id, boost::shared_ptr<BlockData> block_data)
    {
        assert(blocks_.find(block_id) == blocks_.end());
        boost::shared_ptr<BlockMetaData> meta_data(new BlockMetaData(block_id, block_data->GetBlockSize()));
        boost::shared_ptr<BlockCache> block_cache(new BlockCache(block_data, channel_identifier_, block_id, meta_data, remote_cache_, io_service_));
        blocks_.insert(std::make_pair(block_id, block_cache));
    }

    void ChannelCache::OnBlockFetched(const channel_id& channel_identifier, size_t block_id, boost::shared_ptr<BlockData> block_data)
    {
        assert(channel_identifier_ == channel_identifier);
        assert(block_data);
        LOG4CPLUS_INFO(Loggers::Channel(), "Channel "<<channel_identifier_<<" received block "<<block_id<<", block_size="<<block_data->GetBlockSize()<<"bytes.");
        AddBlockCache(block_id, block_data);
    }

    //always return true
    bool ChannelCache::TryGetConsecutiveBlocksBitmap(size_t block_id, std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap)
    {
        int distance_to_max_block(0);
        if (TryGetDistanceToMaxBlock(block_id, distance_to_max_block))
        {
            announce_request_relative_to_max_hits_statistics_->AddHit(distance_to_max_block);
        }

        blocks_bitmap.clear();

        size_t prev_block_id = 0;
        for(BlocksCacheMap::iterator i = blocks_.lower_bound(block_id);
            i != blocks_.end();
            ++i)
        {
            if (prev_block_id != 0 && i->first != prev_block_id + channel_step_size)
            {
                break;
            }

            boost::shared_ptr<BlockMetaData> bitmap = i->second->GetMetaData();
            assert(bitmap);

            blocks_bitmap[i->first] = bitmap;
            prev_block_id = i->first;
        }

        return true;
    }

    void ChannelCache::OnTimer()
    {
        fetcher_->OnTimer();
    }

    size_t ChannelCache::GetBlockCacheCount() const
    {
        return blocks_.size();
    }

    size_t ChannelCache::GetBlockDataCount() const
    {
        size_t count(0);
        for (BlocksCacheMap::const_iterator iter = blocks_.begin(); iter != blocks_.end(); ++iter)
        {
            if (iter->second->GetBlockData())
            {
                ++count;
            }
        }

        return count;
    }

    void ChannelCache::CleanUpObsoleteBlockDatas(boost::shared_ptr<ExpirationStrategy> strategy)
    {
        if (blocks_.empty())
        {
            return;
        }

        size_t max = blocks_.rbegin()->second->GetBlockId();
        
        for (BlocksCacheMap::iterator iter = blocks_.begin(); iter != blocks_.end();)
        {
            boost::shared_ptr<BlockCache> block_cache = iter->second;
            if (strategy->NeedCancelBlockCache(block_cache, max))
            {
                LOG4CPLUS_INFO(Loggers::Channel(), "Channel "<<channel_identifier_<<": removing block #" << block_cache->GetBlockId());
                block_cache->CancelPendingRequests();
                blocks_.erase(iter++);
                continue;
            }
            
            if (block_cache->GetBlockData() && strategy->NeedReleaseBlockData(block_cache, max))
            {
                LOG4CPLUS_INFO(Loggers::Channel(), "Channel "<<channel_identifier_<<": removing data for block #" << block_cache->GetBlockId());
                block_cache->ReleaseBlockData();
            }
            
            iter++;
        }

        LOG4CPLUS_INFO(Loggers::Channel(), "blocks cache for channel "<<channel_identifier_<<" - blocks:"<<GetBlockCacheCount()<<", data:"<<GetBlockDataCount());
    }
}