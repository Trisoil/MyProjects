//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "BlockCache.h"
#include "RemoteCache.h"

namespace live_media
{
    BlockCache::BlockCache(boost::shared_ptr<BlockData> block_data, 
        const channel_id& channel_identifier, 
        size_t block_id, 
        boost::shared_ptr<BlockMetaData> block_meta_data, 
        boost::shared_ptr<IRemoteCache> remote_cache,
        boost::shared_ptr<boost::asio::io_service> io_service)
        : remote_cache_(remote_cache), 
        block_data_(block_data), 
        block_meta_data_(block_meta_data), 
        channel_identifier_(channel_identifier), 
        block_id_(block_id),
        io_service_(io_service)
    {
        last_visit_time_ = ::time(0);
    }

    void BlockCache::AsyncGetBlock(ReadBlockCallback callback)
    {
        last_visit_time_ = ::time(0);

        if (block_data_)
        {
            callback(ErrorCodes::Success, channel_identifier_, block_id_, block_data_);
            return;
        }

        pending_read_requests_.push_back(PendingBlockReadRequest(callback));

        if (pending_read_requests_.size() == 1)
        {
            remote_cache_->AsyncReadBlock(channel_identifier_, block_id_, 0, 
                io_service_->wrap(
                boost::bind(
                &BlockCache::HandleGetBlockFromRemoteCache, 
                shared_from_this(), 
                _1, 
                _4)));
        }
    }

    void BlockCache::CancelPendingRequests()
    {
        if (pending_read_requests_.size() == 0)
        {
            return;
        }

        LOG4CPLUS_INFO(Loggers::Service(), "Cancelling all pending requests for block "<<block_id_<<" of channel "<<channel_identifier_);
        HandleGetBlockFromRemoteCache(ErrorCodes::ResourceNotFound, boost::shared_ptr<BlockData>());
    }

    void BlockCache::HandleGetBlockFromRemoteCache(ErrorCode error_code, boost::shared_ptr<BlockData> block_data)
    {
        if (error_code == ErrorCodes::Success)
        {
            assert(block_data);
            block_data_ = block_data;
        }

        while (pending_read_requests_.size() > 0)
        {
            PendingBlockReadRequest& read_request = pending_read_requests_.front();
            read_request.ProcessRequest(error_code, channel_identifier_, block_id_, block_data);
            pending_read_requests_.pop_front();
        }
    }
}