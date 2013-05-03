//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _BLOCK_CACHE_H_
#define _BLOCK_CACHE_H_

namespace live_media
{
    class IRemoteCache;
    class BlockData;
    class BlockMetaData;
    
    typedef boost::function<
        void (ErrorCode, const channel_id& channel_identifier, size_t block_index, boost::shared_ptr<BlockData> block_data)
    > ReadBlockCallback;

    class PendingBlockReadRequest
    {
    public:
        PendingBlockReadRequest(ReadBlockCallback callback)
            : callback_(callback)
        {}

        void ProcessRequest(ErrorCode error_code, const channel_id& channel_identifier, size_t block_id, boost::shared_ptr<BlockData> block_data)
        {
            callback_(error_code, channel_identifier, block_id, block_data);
        }

    private:
        ReadBlockCallback callback_;
        boost::posix_time::ptime request_time_;
    };

    class BlockCache:
        public boost::enable_shared_from_this<BlockCache>
    {
    public:
        BlockCache(boost::shared_ptr<BlockData> block_data, 
            const channel_id& channel_identifier, 
            size_t block_id, 
            boost::shared_ptr<BlockMetaData> block_meta_data, 
            boost::shared_ptr<IRemoteCache> remote_cache,
            boost::shared_ptr<boost::asio::io_service> io_service);

        void AsyncGetBlock(ReadBlockCallback callback);

        boost::shared_ptr<BlockMetaData> GetMetaData() const
        {
            return block_meta_data_;
        }

        size_t GetLastVisitTime() const
        {
            return last_visit_time_;
        }

        void ReleaseBlockData()
        {
            block_data_.reset();
        }

        void CancelPendingRequests();

        boost::shared_ptr<BlockData> GetBlockData() const
        {
            return block_data_;
        }

        size_t GetBlockId()
        {
            return block_id_;
        }

    private:
        void HandleGetBlockFromRemoteCache(ErrorCode error_code, boost::shared_ptr<BlockData> block_data);

    private:
        boost::shared_ptr<IRemoteCache> remote_cache_;
        channel_id channel_identifier_;

        boost::shared_ptr<BlockData> block_data_;
        size_t block_id_;
        boost::shared_ptr<BlockMetaData> block_meta_data_;

        std::deque<PendingBlockReadRequest> pending_read_requests_;

        boost::posix_time::ptime last_visit_;
        boost::posix_time::ptime last_download_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        size_t last_visit_time_;
    };
}

#endif //_BLOCK_CACHE_H_