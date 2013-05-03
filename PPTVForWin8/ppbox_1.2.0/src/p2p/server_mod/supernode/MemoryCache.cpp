//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "MemoryCache.h"
#include "LocalDiskCache.h"
#include "ResourceIdentifier.h"
#include "MemoryCache/ResourceCache.h"
#include "ServiceHealthMonitor.h"
#include "ConfigManager/ConfigManager.h"
#include <framework/configure/Config.h>

namespace super_node
{
    const size_t MemoryCache::SubpiecePerBlock = BlockData::MaxBlockSize / SUB_PIECE_SIZE;

    MemoryCache::MemoryCache(boost::shared_ptr<ILocalDiskCache> local_disk_cache, boost::shared_ptr<boost::asio::io_service> io_service)
        : local_disk_cache_(local_disk_cache), io_service_(io_service), timer_(new boost::asio::deadline_timer(*io_service))
    {
        ResetCounters();
    }

    MemoryCache::~MemoryCache()
    {
        resource_cache_manager_.Clear();
    }

    void MemoryCache::Start()
    {
        MemoryCacheSettings settings;
        settings.LoadFromConfig();
        Start(settings);
    }

    void MemoryCache::Start(const MemoryCacheSettings& settings)
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::MemoryCache(), "MemoryCache::Start()");

        settings_ = settings;

        now_ = time(NULL);
        expire_delim_ = static_cast<size_t>(now_ - static_cast<size_t>(settings.GetDeadBlockKeepTime()));
        time_left_statistics_ = settings.GetStatisticsInterval();
        time_left_drop_cache_ = settings.GetDropCacheInterval();

        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&MemoryCache::Tick, shared_from_this()));

        boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(ServiceComponents::MemoryCache);
        visits_ = service_component->RegisterStatusItem("Requests", "visits/s");
        cache_hit_rate_ = service_component->RegisterStatusItem("Memory Hit Rate", "%");
        disk_load_success_rate_ = service_component->RegisterStatusItem("Disk Load Success Rate", "%");
        disk_load_rejected_rate_ = service_component->RegisterStatusItem("Disk Load Rejected Rate", "%");
        average_block_lifetime_ = service_component->RegisterStatusItem("Average Block Lifetime", "s");
        cached_blocks_count_ = service_component->RegisterStatusItem("Cached Blocks", "");
        data_block_size_ = service_component->RegisterStatusItem("Data Block Size", "KB");
        data_block_size_->SetStatus(BlockData::MaxBlockSize >> 10);
    }

    void MemoryCache::Stop()
    {
        ServiceHealthMonitor::Instance()->UnregisterComponent(ServiceComponents::MemoryCache);

        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            timer_.reset();
        }
    }

    void MemoryCache::MemoryCacheSettings::LoadFromConfig()
    {
        framework::configure::Config conf(ConfigManager::AllConfigFileName);
        conf.register_module("MemoryCache")
            << CONFIG_PARAM_NAME_RDONLY("block_keep_time_in_second", block_keep_time_)
            << CONFIG_PARAM_NAME_RDONLY("dead_block_keep_time_in_second", dead_block_keep_time_)
            << CONFIG_PARAM_NAME_RDONLY("statistics_interval_in_second", statistics_interval_)
            << CONFIG_PARAM_NAME_RDONLY("drop_cache_interval_in_second", drop_cache_interval_)
            << CONFIG_PARAM_NAME_RDONLY("drop_percentage", drop_percentage_)
            << CONFIG_PARAM_NAME_RDONLY("max_exceedance_percentage_allowed", max_exceedance_percentage_allowed_)
            << CONFIG_PARAM_NAME_RDONLY("max_cache_block_num", max_cache_block_num_)
            << CONFIG_PARAM_NAME_RDONLY("resource_cost_delim", resource_cost_delim_)
            << CONFIG_PARAM_NAME_RDONLY("block_cost_delim", block_cost_delim_)
            << CONFIG_PARAM_NAME_RDONLY("average_block_visit_count_mix_rate", average_block_visit_count_mix_rate_)
            << CONFIG_PARAM_NAME_RDONLY("average_resource_visit_count_mix_rate", average_resource_visit_count_mix_rate_)
            << CONFIG_PARAM_NAME_RDONLY("accumulative_block_visit_count_decline_rate", accumulative_block_visit_count_decline_rate_)
            << CONFIG_PARAM_NAME_RDONLY("accumulative_resource_visit_count_decline_rate", accumulative_resource_visit_count_decline_rate_);
    }

    void MemoryCache::AsyncReadBlock(const std::string & resource_name, boost::uint32_t block_id,
        boost::uint32_t subpiece_index, DelayReadCallback callback)
    {
        ResourceIdentifier resource_identifier(resource_name);

        boost::shared_ptr<BlockCache> block;
        boost::shared_ptr<ResourceCache> resource;

        if (!resource_cache_manager_.FindBlock(resource_identifier, block_id, resource, block, settings_.GetResourceCostDelim(),
            settings_.GetBlockCostDelim()))
        {
            resource_set_in_cost_order_.Insert(resource);
        }

        StatisticState(State::VISIT, block);

        if (block->GetBlockData())
        {
            StatisticState(State::HIT, block);

            time_t t = now_ + settings_.GetBlockKeepTime() - settings_.GetBlockKeepTime() * subpiece_index / SubpiecePerBlock;

            if (block->GetExpire() < t)
            {
                if (block->GetExpire() < expire_delim_)
                {
                    StatisticState(State::CONTINUOUS_HIT, block);
                }
                EraseBlockFromTheExactQueue(block);
                block->SetExpire(t);
                block_set_in_expire_order_.Insert(block);
            }
            callback(ErrorCodes::Success, block->GetBlockData());
        }
        else
        {
            StatisticState(State::LOAD, block);

            resource->AddLoadingBlockNum(1);

            block->PushBackToTheDelayReadCallbackDeque(callback);
            if (block->GetDelayReadCallbackDequeSize() == 1)
            {
                local_disk_cache_->AsyncReadBlock(resource_identifier, block_id, 20,
                    boost::bind(&MemoryCache::HandleReadBlock, shared_from_this(), _1, _2, block, resource_identifier));
            }
        }
    }

    void MemoryCache::HandleReadBlock(ErrorCode error_code, boost::shared_ptr<BlockData> block_data,
        boost::shared_ptr<BlockCache> block, const ResourceIdentifier & resource_identifier)
    {
        size_t delay_read_queue_size = block->GetDelayReadCallbackDequeSize();
        assert(block->GetLoadingBlockNum() >= delay_read_queue_size);
        block->SubtractLoadingBlockNum(delay_read_queue_size);

        block->CallBackDelayRead(error_code, block_data);

        if (error_code == ErrorCodes::Success)
        {
            ++successful_disk_loads_;
            StatisticState(State::LOADED, block, delay_read_queue_size);

            global_resource_state_.AddCacheBlockNum(1);
            block->GetMyResource()->AddCacheBlockNum(1);

            time_t t = now_ + settings_.GetBlockKeepTime();
            block->SetExpire(t);
            block->SetBlockData(block_data);
            block_set_in_expire_order_.Insert(block);

            const size_t MaxCacheBlockNum = settings_.GetMaxCacheBlockNum();
            if (global_resource_state_.GetCacheBlockNum() > MaxCacheBlockNum * (100 + settings_.GetMaxExceedancePercentageAllowed()) / 100)
            {
                DropCache(global_resource_state_.GetCacheBlockNum() - (MaxCacheBlockNum * (100 - settings_.GetDropPercentage()) / 100));
            }
        }
        else
        {
            if (error_code == ErrorCodes::ResourceNotFound || 
                error_code == ErrorCodes::ServiceBusy)
            {
                ++rejected_disk_loads_;
            }
            else
            {
                ++disk_load_failures_;
            }
            
            StatisticState(State::REFUSED, block, delay_read_queue_size);
        }
    }

    void MemoryCache::EraseBlockFromTheExactQueue(boost::shared_ptr<BlockCache> block)
    {
        if (block->IsInTheExpireQueue(expire_delim_))
        {
            block_set_in_expire_order_.Erase(block);
        }
        else
        {
            block->EraseFromTheSetInCostOrder();
        }
    }

    void MemoryCache::Tick()
    {
        ++now_;
        expire_delim_ = static_cast<size_t>(now_ - static_cast<size_t>(settings_.GetDeadBlockKeepTime()));

        block_set_in_expire_order_.MoveDeadBlockToBlockSetInCostOrder(expire_delim_);

        if (time_left_statistics_ && --time_left_statistics_ == 0)
        {
            Statistics();
            time_left_statistics_ = settings_.GetStatisticsInterval();
            ReportStatus();
            ResetCounters();
        }

        if (time_left_drop_cache_ && --time_left_drop_cache_ == 0)
        {
            const size_t MaxCacheBlockNum = settings_.GetMaxCacheBlockNum();

            if (global_resource_state_.GetCacheBlockNum() > MaxCacheBlockNum)
            {
                DropCache(global_resource_state_.GetCacheBlockNum() - MaxCacheBlockNum * (100 - settings_.GetDropPercentage()) / 100);
            }

            DropMetadataIfNeeded();
            time_left_drop_cache_ = settings_.GetDropCacheInterval();
        }

        if (timer_)
        {
            timer_->expires_from_now(boost::posix_time::seconds(1));
            timer_->async_wait(boost::bind(&MemoryCache::Tick, shared_from_this()));
        }
    }

    void MemoryCache::Statistics()
    {
        resource_cache_manager_.Statistics(
            settings_.GetAverageResourceVisitCountMixRate(),
            settings_.GetAccumulativeResourceVisitCountDeclineRate(),
            settings_.GetAverageBlockVisitCountMixRate(),
            settings_.GetAccumulativeBlockVisitCountDeclineRate());

        resource_set_in_cost_order_.ReArrangeTheSet();
        global_resource_state_.AllTick(90, 80);
    }

    void MemoryCache::DropCache(size_t need_drop_num)
    {
        size_t drop_left_num = need_drop_num - resource_set_in_cost_order_.DropBlockCacheInOrder(need_drop_num,
            boost::bind(&MemoryCache::OnBlockDataRelease, shared_from_this(), _1));

        if (drop_left_num > 0)
        {
            drop_left_num -= block_set_in_expire_order_.DropDeadBlockCacheInExpireOrder(drop_left_num, now_,
                boost::bind(&MemoryCache::OnBlockDataRelease, shared_from_this(), _1));
        }

        if (drop_left_num > need_drop_num / 2)
        {
            drop_left_num -= block_set_in_expire_order_.DropAliveBlockCacheInExpireOrder(drop_left_num - need_drop_num / 2, now_,
                boost::bind(&MemoryCache::OnBlockDataRelease, shared_from_this(), _1));
        }

        global_resource_state_.SubtractCacheBlockNum(need_drop_num - drop_left_num);
    }

    void MemoryCache::DropMetadataIfNeeded()
    {
        resource_set_in_cost_order_.DropMetadataIfNeeded();

        resource_cache_manager_.DropMetadataIfNeeded();
    }

    void MemoryCache::StatisticState(boost::uint8_t statistics_type, boost::shared_ptr<BlockCache> block, size_t num)
    {
        global_resource_state_.Statistic(statistics_type, num);

        assert(block);
        block->Statistic(statistics_type, num);

        assert(block->GetMyResource());
        block->GetMyResource()->Statistic(statistics_type, num);
    }

    void MemoryCache::ResetCounters()
    {
        successful_disk_loads_ = 0;
        rejected_disk_loads_ = 0;
        disk_load_failures_ = 0;
        total_block_data_lifetime_ = 0;
        block_data_releases_ = 0;
    }

    void MemoryCache::ReportStatus()
    {
        size_t visits = global_resource_state_.GetVisit().GetRate();
        size_t hits = global_resource_state_.GetHit().GetRate();
        assert(visits >= hits);

        visits_->SetStatus(settings_.GetStatisticsInterval() > 0 ? visits/settings_.GetStatisticsInterval() : visits);
        cache_hit_rate_->SetStatus(visits > 0 ? 100*hits/visits : 0);

        size_t disk_loads = successful_disk_loads_ + rejected_disk_loads_ + disk_load_failures_;
        disk_load_success_rate_->SetStatus(disk_loads > 0 ? successful_disk_loads_*100/disk_loads : 0);
        disk_load_rejected_rate_->SetStatus(disk_loads > 0 ? rejected_disk_loads_*100/disk_loads : 0);
        average_block_lifetime_->SetStatus(block_data_releases_ > 0 ? total_block_data_lifetime_ / block_data_releases_ : 0);
        cached_blocks_count_->SetStatus(global_resource_state_.GetCacheBlockNum());
    }

    void MemoryCache::OnBlockDataRelease(boost::uint32_t lifetime)
    {
        ++block_data_releases_;
        total_block_data_lifetime_ += lifetime;
    }

    void MemoryCache::UpdateConfig()
    {
        settings_.LoadFromConfig();
    }
}
