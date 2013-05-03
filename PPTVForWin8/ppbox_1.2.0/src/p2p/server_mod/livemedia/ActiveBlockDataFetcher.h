//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _ACTIVE_BLOCK_DATA_FETCHER_H
#define _ACTIVE_BLOCK_DATA_FETCHER_H

#include <framework/timer/TickCounter.h>

namespace live_media
{
    class BlockCache;
    class IRemoteCache;

    class FetchResultListener
    {
    public:
        void virtual OnBlockFetched(const channel_id& channel_identifier, size_t block_id, boost::shared_ptr<BlockData> block_data) = 0;
        virtual ~FetchResultListener(){}
    };

    class ActiveBlockDataFetcher
        : public boost::enable_shared_from_this<ActiveBlockDataFetcher>       
    {
        const static size_t FetchDataPriority = 20;
        const static size_t channel_step_size = 5;
        const static size_t MaxBlockFetchTimeInSeconds = 120;
        const static size_t MaxTolerableSystemTimeDifferInSeconds = 15 * 60;
        const static size_t MaxDelayTimeInSeconds = 5 * 60;
    public:
        ActiveBlockDataFetcher(const channel_id& channel_identifier, boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<IRemoteCache> remote_cache, boost::shared_ptr<FetchResultListener> listener);
        
        void OnTimer();

        static size_t CurrentDefaultDownloadBlockPosition();

    private:
        void HandleAsyncGetBlock(ErrorCode error_code, size_t block_id, boost::shared_ptr<BlockData> block_data);

        void ResetBlockId();

        void MoveToNextBlock();

        void Jump();

    private:
        boost::shared_ptr<FetchResultListener> listener_;
        size_t current_block_;
        boost::shared_ptr<IRemoteCache> remote_cache_;
        channel_id channel_identifier_;
        bool requesting_;
        size_t blocks_fetched_;
        framework::timer::TickCounter ticks_elapsed_since_last_move_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
    };

}

#endif //_ACTIVE_BLOCK_DATA_FETCHER_H
