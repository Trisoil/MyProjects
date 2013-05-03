//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "ActiveBlockDataFetcher.h"
#include "RemoteCache.h"

#undef max
#undef min

namespace live_media
{
    ActiveBlockDataFetcher::ActiveBlockDataFetcher(const channel_id& channel_identifier, boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<IRemoteCache> remote_cache, boost::shared_ptr<FetchResultListener> listener)
        : channel_identifier_(channel_identifier), 
        io_service_(io_service), 
        remote_cache_(remote_cache),
        listener_(listener)
    {
        blocks_fetched_ = 0;
        requesting_ = false;
        ResetBlockId();
    }

    void ActiveBlockDataFetcher::OnTimer()
    {
        if (requesting_)
        {
            return;
        }

        if (ticks_elapsed_since_last_move_.elapsed() >= MaxBlockFetchTimeInSeconds*1000)
        {
            if (blocks_fetched_ == 0)
            {
                if (ticks_elapsed_since_last_move_.elapsed() >= MaxTolerableSystemTimeDifferInSeconds * 1000)
                {
                    ResetBlockId();
                    ticks_elapsed_since_last_move_.reset();

                    LOG4CPLUS_ERROR(Loggers::Operations(), "Fail to download first block: "<<this->channel_identifier_);
                    LOG4CPLUS_ERROR(Loggers::HttpStreamFetcher(), "Fetcher has been failing to download the first block for channel "<<channel_identifier_<<" for the past "<<ticks_elapsed_since_last_move_.elapsed()/1000 <<" minutes. Please make sure the system clock is configured properly.");
                }
            }
            else
            {
                LOG4CPLUS_WARN(Loggers::HttpStreamFetcher(), "Fetcher kept failing to download block "<<current_block_<<" for the last "<<MaxBlockFetchTimeInSeconds<<" seconds, and will jump to a later block.");
                Jump();
            }
        }
        
        requesting_ = true;

        remote_cache_->AsyncReadBlock(
            channel_identifier_, 
            current_block_, 
            FetchDataPriority, 
            io_service_->wrap(
                boost::bind(
                    &ActiveBlockDataFetcher::HandleAsyncGetBlock, 
                    shared_from_this(), _1, _3, _4)));
    }

    void ActiveBlockDataFetcher::HandleAsyncGetBlock(ErrorCode error_code, size_t block_id, boost::shared_ptr<BlockData> block_data)
    {
        requesting_ = false;
        if (block_id != current_block_)
        {
            LOG4CPLUS_ERROR(Loggers::HttpStreamFetcher(), "Receiving a callback for unexpected block ID (expected:"<<current_block_<<", actual:"<<block_id<<").");
            return;
        }

        if (error_code == ErrorCodes::Success)
        {
            listener_->OnBlockFetched(channel_identifier_, current_block_, block_data);
            MoveToNextBlock();
            return;
        }
    }

    void ActiveBlockDataFetcher::ResetBlockId()
    {
        current_block_ = CurrentDefaultDownloadBlockPosition();
    }

    void ActiveBlockDataFetcher::Jump()
    {
        size_t next_block = current_block_ + channel_step_size;
        size_t new_default_block = CurrentDefaultDownloadBlockPosition();

        current_block_ = std::max(
                std::min(
                        std::max(next_block, new_default_block), // next position in normal case
                        current_block_ + (size_t)ticks_elapsed_since_last_move_.elapsed() / 1000 / channel_step_size * channel_step_size // don't jump too far
                ),
                new_default_block - MaxDelayTimeInSeconds / channel_step_size * channel_step_size // don't left too late
              );

        ticks_elapsed_since_last_move_.reset();
    }

    size_t ActiveBlockDataFetcher::CurrentDefaultDownloadBlockPosition()
    {
        size_t position_for_now = ::time(0) - 60;
        return position_for_now - position_for_now%channel_step_size;
    }

    void ActiveBlockDataFetcher::MoveToNextBlock()
    {
        ++blocks_fetched_;

        current_block_ = std::max(
                current_block_ + channel_step_size, // next position in normal case
                CurrentDefaultDownloadBlockPosition() - MaxDelayTimeInSeconds / channel_step_size * channel_step_size // don't left too late
            );

        ticks_elapsed_since_last_move_.reset();
    }
}
