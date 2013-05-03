//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_DATA_H
#define SUPER_NODE_BLOCK_DATA_H

#include "protocol/Protocol.h"
#include "struct/SubPieceBuffer.h"
#include "count_object_allocate.h"

#ifdef USE_DIRECT_IO
#ifdef BOOST_WINDOWS_API 
#error USE_DIRECT_IO is not supported on windows
#endif
#endif

namespace super_node
{
    class BlockDataAllocator;

    class BlockData
        : public count_object_allocate<BlockData>
    {
    public:
        BlockData(const boost::filesystem::path& block_file_path);
        BlockData(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);

        // [data] is allocated from BlockDataAllocator, and ownership is given to BlockData
        BlockData(char* data, size_t size);

        ~BlockData();

        size_t Size() const 
        { 
#ifndef USE_DIRECT_IO
            return data_.size(); 
#else
            return bytes_;
#endif
        }

        void* Buf() const
        {
#ifndef USE_DIRECT_IO
            return const_cast<char*>(&*data_.begin());
#else
            return block_data_bytes_;
#endif
        }

        bool WriteFile(const boost::filesystem::path& block_file_path);

        void GetSubPiece(boost::uint16_t subpiece_index, protocol::SubPieceBuffer & buf);

        boost::uint16_t GetSubPieceNumber() const;

    private:
        static size_t __MaxBlockSize;
        friend class BlockDataAllocator;

    public:
        static const size_t& MaxBlockSize;

    private:
#ifndef USE_DIRECT_IO
        std::vector<char> data_;
#else
        char* block_data_bytes_;
        size_t bytes_;
#endif
    };
}

#endif //SUPER_NODE_BLOCK_DATA_H
