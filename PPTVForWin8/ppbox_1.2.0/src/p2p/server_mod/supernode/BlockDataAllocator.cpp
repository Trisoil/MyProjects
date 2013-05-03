//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockDataAllocator.h"

#ifdef USE_DIRECT_IO
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define __USE_GNU

#include <fcntl.h>
#endif

#include <framework/configure/Config.h>
#include "ConfigManager/ConfigManager.h"

namespace super_node
{
    const size_t MemoryChunk::BlocksPerChunk = 200;
    const size_t MemoryChunk::ChunkSize = BlockData::MaxBlockSize*BlocksPerChunk;

    MemoryChunk::MemoryChunk()
    {
        blocks_ = 0;

#ifdef USE_DIRECT_IO
        int ret_code = posix_memalign(reinterpret_cast<void**>(&blocks_), GetPageSize(), ChunkSize);
        if (ret_code)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "Failed to allocate aligned memory buffer. ret_code="<<ret_code<<".");
            blocks_ = 0;
        }
#else
        blocks_ = reinterpret_cast<char*>(malloc(ChunkSize));
#endif

        if (blocks_)
        {
            for(size_t i = 0; i < BlocksPerChunk; ++i)
            {
                available_blocks_.push_back(blocks_ + i*BlockData::MaxBlockSize);
            }
        }
    }

    MemoryChunk::~MemoryChunk()
    {
        if (blocks_)
        {
            free(blocks_);
        }
    }

    bool MemoryChunk::Owns(void* memory)
    {
        if (blocks_)
        {
            return memory >= blocks_ && memory < blocks_ + ChunkSize;
        }

        return false;
    }

    bool MemoryChunk::Free(void* memory)
    {
        if (Owns(memory))
        {
            available_blocks_.push_back(reinterpret_cast<char*>(memory));
            assert(available_blocks_.size() <= BlocksPerChunk);
            return true;
        }

        return false;
    }

    void* MemoryChunk::TryAllocate()
    {
        void* result = 0;
        if (!available_blocks_.empty())
        {
            result = available_blocks_.back();
            available_blocks_.pop_back();
        }

        return result;
    }

#ifdef USE_DIRECT_IO
    size_t MemoryChunk::GetPageSize()
    {
        static size_t PageSize = 0;

        if (PageSize == 0)
        {
            PageSize = getpagesize();
            assert(PageSize > 0 && BlockData::MaxBlockSize%PageSize == 0);
            LOG4CPLUS_INFO(Loggers::Service(), "system page size = "<<PageSize<<" bytes.");
        }

        return PageSize;
    }
#endif

    void* BlockDataAllocator::Allocate()
    {
        boost::mutex::scoped_lock lock(mutex_);
        void* result = 0;
        for(size_t i = 0; i < chunks_.size(); ++i)
        {
            result = chunks_[i]->TryAllocate();
            if (result)
            {
                break;
            }
        }

        if (!result)
        {
            boost::shared_ptr<MemoryChunk> new_chunk(new MemoryChunk());
            result = new_chunk->TryAllocate();
            assert(result);

            if (result)
            {
                chunks_.push_back(new_chunk);
            }
        }

        return result;
    }

    bool BlockDataAllocator::Free(void* buffer)
    {
        if (!buffer)
        {
            return true;
        }

        boost::mutex::scoped_lock lock(mutex_);

        bool freed = false;
        for(size_t i = 0; i < chunks_.size(); ++i)
        {
            if (chunks_[i]->Owns(buffer))
            {
                freed = chunks_[i]->Free(buffer);
                break;
            }
        }

        return freed;
    }


    void BlockDataAllocator::LoadConfig()
    {
        framework::configure::Config conf(ConfigManager::AllConfigFileName);

        size_t data_block_size_in_16k = 64;

        conf.register_module("MemoryCache")
            << CONFIG_PARAM_NAME_RDONLY("data_block_size_in_16k", data_block_size_in_16k);

        if (data_block_size_in_16k > 0 && data_block_size_in_16k <= 64 * 8)
        {
            BlockData::__MaxBlockSize = data_block_size_in_16k << 14;
            std::cout << "--->data block size: " << data_block_size_in_16k << ", " << BlockData::__MaxBlockSize << std::endl;
        }
    }
}
