//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _CACHE_MANAGER_H_
#define _CACHE_MANAGER_H_

namespace live_media
{
    class IChannelManager;
    class IChannelCache;
    class BlockCache;
    struct CacheExpirationConfiguration;

    class ExpirationStrategy
        :public boost::enable_shared_from_this<ExpirationStrategy>
    {
    public:
        ExpirationStrategy()
        {
            check_time_interval_in_second_ = 1;
            block_data_obsolete_limit_in_second_ = 120;
            block_cache_obsolete_limit_in_second_ = 600;
            last_visit_limit_in_second_ = 5;
        }
        
       void SetStrategy(const CacheExpirationConfiguration& config);

       bool NeedCancelBlockCache(boost::shared_ptr<BlockCache> block_cache, size_t max);

       bool NeedReleaseBlockData(boost::shared_ptr<BlockCache> block_cache, size_t max);

       boost::uint32_t GetCheckTimeIntervalInSecond()
       {
           return check_time_interval_in_second_;
       }

    private:
        boost::uint32_t block_data_obsolete_limit_in_second_;
        boost::uint32_t block_cache_obsolete_limit_in_second_;
        boost::uint32_t last_visit_limit_in_second_;
        boost::uint32_t check_time_interval_in_second_;
    };

    class CacheManager
        : public boost::enable_shared_from_this<CacheManager>
    {
    public:
        CacheManager(boost::shared_ptr<IChannelManager> channel_manager, boost::shared_ptr<boost::asio::io_service> io_service);
        void Start();
        void SetCacheExpirationStrategy(boost::shared_ptr<ExpirationStrategy> strategy);

    private:
        void OnTimer();
        void CleanUpObsoleteDatas();

    private:
        boost::shared_ptr<IChannelManager> channel_manager_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<ExpirationStrategy> strategy_;
    };
}

#endif