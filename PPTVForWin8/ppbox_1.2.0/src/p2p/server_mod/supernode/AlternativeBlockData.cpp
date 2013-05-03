//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockData.h"
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

namespace super_node
{
    size_t BlockData::__MaxBlockSize = 1 << 20;
    const size_t& BlockData::MaxBlockSize = BlockData::__MaxBlockSize;

    BlockData::BlockData(char* data, size_t size)
    {
        block_data_bytes_ = data;
        bytes_ = size;
    }

    BlockData::BlockData(const boost::filesystem::path& block_file_path)
    {
        block_data_bytes_ = 0;
        bytes_ = 0;

        int file_descriptor = open(block_file_path.file_string().c_str(), O_RDONLY | O_DIRECT);
        if (file_descriptor < 0)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "Failed to open file "<<block_file_path<<" to read.");
            return;    
        }

        block_data_bytes_ = reinterpret_cast<char*>(BlockDataAllocator::Instance().Allocate());
        if (!block_data_bytes_)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "Failed to allocate aligned memory buffer.");
            close(file_descriptor);
            return;
        }
        
        //read up to MaxBlockSize bytes to the buffer
        size_t offset(0);
        int bytes_read(0);
        int bytes_left(BlockData::MaxBlockSize);
        do
        {
            bytes_read = pread(file_descriptor, block_data_bytes_ + offset, bytes_left, offset);
            if (bytes_read > 0)
            {
                bytes_left -= bytes_read;
                offset += bytes_read;
            }
        }
        while (bytes_left > 0 && bytes_read > 0);
        
        if (bytes_read < 0)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "An error occurred while reading file file "<<block_file_path);
            close(file_descriptor);
            
            BlockDataAllocator::Instance().Free(block_data_bytes_);
            block_data_bytes_ = 0;
            
            return;
        }
        
        close(file_descriptor);
        bytes_ = offset;
    }

    BlockData::BlockData(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
    {
        block_data_bytes_ = 0;

        bytes_ = end - begin;
        assert(bytes_ <= BlockData::MaxBlockSize);

        block_data_bytes_ = reinterpret_cast<char*>(BlockDataAllocator::Instance().Allocate());
        if (block_data_bytes_ == 0)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "Failed to allocate aligned memory buffer.");
            bytes_ = 0;
            return;
        }

        std::copy(begin, end, block_data_bytes_);
    }

    BlockData::~BlockData()
    {
        if (block_data_bytes_)
        {
            BlockDataAllocator::Instance().Free(block_data_bytes_);
            block_data_bytes_ = 0;
        }
    }

    bool BlockData::WriteFile(const boost::filesystem::path& block_file_path)
    {
        if (bytes_ == 0)
        {
            LOG4CPLUS_WARN(Loggers::Service(), "An attempt to write an empty block into file "<<block_file_path<<" is rejected.");
            return false;
        }

        bool direct_io_applicable = ((bytes_ % MemoryChunk::GetPageSize()) == 0);
        int open_flags = O_CREAT | O_WRONLY;
        if (direct_io_applicable)
        {
            open_flags |= (O_DIRECT | O_SYNC);
        }
        
        LOG4CPLUS_TRACE(Loggers::Service(), "DIRECT IO:"<<direct_io_applicable<<", bytes_:"<<bytes_<<", page_size:"<<MemoryChunk::GetPageSize()<<", bytes%pagesize="<<(bytes_%MemoryChunk::GetPageSize()));
        
        int file_descriptor = open(block_file_path.string().c_str(), open_flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

        if (file_descriptor < 0)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "Failed to open file "<<block_file_path<<" to write.");
            return false;
        }

        size_t bytes_left = bytes_;
        size_t offset(0);
        int bytes_written(0);
        do
        {
            bytes_written = pwrite(file_descriptor, block_data_bytes_ + offset, bytes_left, offset);
            if (bytes_written > 0)
            {
                offset += bytes_written;
                bytes_left -= bytes_written;
            }
        }
        while(bytes_written > 0 && bytes_left > 0);

        if (bytes_written < 0)
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "An error occurred while writing to file "<<block_file_path<<".(bytes_="<<bytes_<<", bytes_left="<<bytes_left<<")");
        }

        if (!direct_io_applicable)
        {
            //data would have to be flushed out before it can be wiped from page cache
            if (fdatasync(file_descriptor) == 0)
            {
                //to advise system NOT to keep the data in page cache
                //note that option POSIX_FADV_NOREUSE is no-op on linux, and should not be used
                if (0 != posix_fadvise64(file_descriptor, 0, 0, POSIX_FADV_DONTNEED))
                {
                    LOG4CPLUS_WARN(Loggers::Service(), "Failed to advise system to free file data from page cache.");
                }
            }
            else
            {
                LOG4CPLUS_WARN(Loggers::Service(), "Failed to flush data into memory");
            }
        }

        close(file_descriptor);
        return bytes_left == 0;
    }

    void BlockData::GetSubPiece(boost::uint16_t subpiece_index, protocol::SubPieceBuffer & buf)
    {
        assert(subpiece_index * SUB_PIECE_SIZE + buf.Length() <= Size());

        if (subpiece_index * SUB_PIECE_SIZE + buf.Length() <= Size())
        {
            memcpy(buf.Data(), block_data_bytes_ + subpiece_index * SUB_PIECE_SIZE, buf.Length());
        }
    }

    boost::uint16_t BlockData::GetSubPieceNumber() const
    {
        return static_cast<boost::uint16_t>((Size() + SUB_PIECE_SIZE - 1) / SUB_PIECE_SIZE);
    }
}

#endif
