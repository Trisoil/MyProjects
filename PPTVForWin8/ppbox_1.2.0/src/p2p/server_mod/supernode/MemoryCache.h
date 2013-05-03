//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_MEMORY_CACHE_H
#define SUPER_NODE_MEMORY_CACHE_H

#include "MemoryCache/BlockSetInOrder.h"
#include "MemoryCache/ResourceState.h"
#include "MemoryCache/ResourceSetInCostOrder.h"
#include "MemoryCache/ResourceCacheManager.h"
#include "MemoryCache/GlobalResourceState.h"

namespace super_node
{
    class ILocalDiskCache;
    class ResourceIdentifier;
    class ResourceCache;
    class ComponentStatusItem;

    class MemoryCache
        : public boost::enable_shared_from_this<MemoryCache>,
        public count_object_allocate<MemoryCache>
    {
    public:
        class MemoryCacheSettings
        {
        public:
            MemoryCacheSettings(){}
            MemoryCacheSettings(size_t block_keep_time, size_t dead_block_keep_time, boost::uint16_t statistics_interval,
                boost::uint16_t drop_cache_interval, boost::uint8_t drop_percentage, boost::uint8_t max_exceedance_percentage_allowed,
                size_t max_cache_block_num, size_t block_cost_delim,
                size_t average_block_visit_count_mix_rate, size_t accumulative_block_visit_count_decline_rate,
                size_t average_resource_visit_count_mix_rate, size_t accumulative_resource_visit_count_decline_rate)
                : block_keep_time_(block_keep_time)
                , dead_block_keep_time_(dead_block_keep_time)
                , statistics_interval_(statistics_interval)
                , drop_cache_interval_(drop_cache_interval)
                , drop_percentage_(drop_percentage)
                , max_exceedance_percentage_allowed_(max_exceedance_percentage_allowed)
                , max_cache_block_num_(max_cache_block_num)
                , block_cost_delim_(block_cost_delim)
                , average_block_visit_count_mix_rate_(average_block_visit_count_mix_rate)
                , accumulative_block_visit_count_decline_rate_(accumulative_block_visit_count_decline_rate)
                , average_resource_visit_count_mix_rate_(average_resource_visit_count_mix_rate)
                , accumulative_resource_visit_count_decline_rate_(accumulative_resource_visit_count_decline_rate)
            {
            }

            size_t GetBlockKeepTime() const { return block_keep_time_; }
            size_t GetDeadBlockKeepTime() const { return dead_block_keep_time_; }
            boost::uint16_t GetStatisticsInterval() const { return statistics_interval_; }
            boost::uint16_t GetDropCacheInterval() const { return drop_cache_interval_; }
            boost::uint8_t GetDropPercentage() const { return drop_percentage_; }
            boost::uint8_t GetMaxExceedancePercentageAllowed() const { return max_exceedance_percentage_allowed_; }
            size_t GetMaxCacheBlockNum() const { return max_cache_block_num_; }
            size_t GetResourceCostDelim() const { return resource_cost_delim_; }
            size_t GetBlockCostDelim() const { return block_cost_delim_; }

            size_t GetAverageBlockVisitCountMixRate() const { return average_block_visit_count_mix_rate_; }
            size_t GetAverageResourceVisitCountMixRate() const { return average_resource_visit_count_mix_rate_; }
            size_t GetAccumulativeBlockVisitCountDeclineRate() const { return accumulative_block_visit_count_decline_rate_; }
            size_t GetAccumulativeResourceVisitCountDeclineRate() const { return accumulative_resource_visit_count_decline_rate_; }

            void LoadFromConfig();

        private:
            size_t block_keep_time_;
            size_t dead_block_keep_time_;
            boost::uint16_t statistics_interval_;
            boost::uint16_t drop_cache_interval_;
            boost::uint8_t drop_percentage_;
            boost::uint8_t max_exceedance_percentage_allowed_;
            size_t max_cache_block_num_;
            size_t resource_cost_delim_;
            size_t block_expire_delim_;
            size_t block_cost_delim_;
            size_t average_block_visit_count_mix_rate_;
            size_t accumulative_block_visit_count_decline_rate_;
            size_t average_resource_visit_count_mix_rate_;
            size_t accumulative_resource_visit_count_decline_rate_;
        };

    public:
        MemoryCache(boost::shared_ptr<ILocalDiskCache> local_disk_cache, boost::shared_ptr<boost::asio::io_service> io_service);
        ~MemoryCache();

        void Start();
        void Start(const MemoryCacheSettings& settings);
        void Stop();
        void AsyncReadBlock(const std::string & resource_name, boost::uint32_t block_id,
            boost::uint32_t subpiece_index, DelayReadCallback callback);

        // for unit-testability
        void InsertToResourceMap(const ResourceIdentifier & resource_identifier, boost::shared_ptr<ResourceCache> resource_cache)
        {
            resource_cache_manager_.Insert(resource_identifier, resource_cache);
        }

        void UpdateConfig();

    private:
        void Tick();
        void Statistics();
        void DropCache(size_t keeped_block_cache_num);
        void DropMetadataIfNeeded();
        void HandleReadBlock(ErrorCode error_code, boost::shared_ptr<BlockData> block_data,
            boost::shared_ptr<BlockCache> block, const ResourceIdentifier & resource_identifier);
        void EraseBlockFromTheExactQueue(boost::shared_ptr<BlockCache> block);
        void StatisticState(boost::uint8_t statistics_type, boost::shared_ptr<BlockCache> block, size_t num = 1);

        void ResetCounters();
        void ReportStatus();

        void OnBlockDataRelease(boost::uint32_t lifetime);

    private:
        boost::shared_ptr<ILocalDiskCache> local_disk_cache_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        ResourceCacheManager resource_cache_manager_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        boost::uint16_t time_left_statistics_;
        boost::uint16_t time_left_drop_cache_;
        time_t now_;
        size_t expire_delim_;
        MemoryCacheSettings settings_;
        BlockSetInExpireOrder block_set_in_expire_order_;
        GlobalResourceState global_resource_state_;
        ResourceSetInCostOrder resource_set_in_cost_order_;

        boost::shared_ptr<ComponentStatusItem> visits_;
        boost::shared_ptr<ComponentStatusItem> cache_hit_rate_;
        boost::shared_ptr<ComponentStatusItem> disk_load_success_rate_;
        boost::shared_ptr<ComponentStatusItem> disk_load_rejected_rate_;
        boost::shared_ptr<ComponentStatusItem> average_block_lifetime_;
        boost::shared_ptr<ComponentStatusItem> cached_blocks_count_;
        boost::shared_ptr<ComponentStatusItem> data_block_size_;

        boost::uint32_t successful_disk_loads_;
        boost::uint32_t rejected_disk_loads_;
        boost::uint32_t disk_load_failures_;
        size_t total_block_data_lifetime_;
        boost::uint32_t block_data_releases_;

        static const size_t SubpiecePerBlock;
    };
}

#endif  // SUPER_NODE_MEMORY_CACHE_H
