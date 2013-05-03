//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "CacheManager.h"
#include "ChannelCache.h"
#include "channel_manager.h"
#include "Configuration.h"

namespace live_media
{
    void ExpirationStrategy::SetStrategy(const CacheExpirationConfiguration& config)
    {
        block_cache_obsolete_limit_in_second_ = config.block_cache_obsolete_limit_in_second_;
        block_data_obsolete_limit_in_second_ = config.block_data_obsolete_limit_in_second_;
        last_visit_limit_in_second_ = config.last_visit_limit_in_second_;
    }

    bool ExpirationStrategy::NeedCancelBlockCache(boost::shared_ptr<BlockCache> block_cache, size_t max)
    {
        if (max > block_cache_obsolete_limit_in_second_ + block_cache->GetBlockId())
        {
            return true;
        }
        
        return false;
    }

    bool ExpirationStrategy::NeedReleaseBlockData(boost::shared_ptr<BlockCache> block_cache, size_t max)
    {
        if (max > block_data_obsolete_limit_in_second_ + block_cache->GetBlockId() 
            && ::time(0) - block_cache->GetLastVisitTime() > last_visit_limit_in_second_)
        {
            return true;
        }

        return false;
    }

    CacheManager::CacheManager(boost::shared_ptr<IChannelManager> channel_manager, boost::shared_ptr<boost::asio::io_service> io_service)
        :channel_manager_(channel_manager), io_service_(io_service)
    {

    }

    void CacheManager::Start()
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        timer_->expires_from_now(boost::posix_time::seconds(strategy_->GetCheckTimeIntervalInSecond()));
        timer_->async_wait(boost::bind(&CacheManager::OnTimer, shared_from_this()));
    }

    void CacheManager::SetCacheExpirationStrategy(boost::shared_ptr<ExpirationStrategy> strategy)
    {
        strategy_ = strategy;
    }

    void CacheManager::OnTimer()
    {
        CleanUpObsoleteDatas();

        timer_->expires_from_now(boost::posix_time::seconds(strategy_->GetCheckTimeIntervalInSecond()));
        timer_->async_wait(boost::bind(&CacheManager::OnTimer, shared_from_this()));
    }

    void CacheManager::CleanUpObsoleteDatas()
    {
        std::vector<IChannelPointer> channels = channel_manager_->GetChannels();
        for(size_t i = 0; i < channels.size(); ++i)
        {
            channels[i]->GetCache()->CleanUpObsoleteBlockDatas(strategy_);
        }
    }
}