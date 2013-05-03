//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockData.h"
#include "BlockDataAllocator.h"

#ifndef USE_DIRECT_IO

namespace super_node
{
    size_t BlockData::__MaxBlockSize = 1 << 20;
    const size_t& BlockData::MaxBlockSize = BlockData::__MaxBlockSize;

    BlockData::BlockData(char* data, size_t size)
    {
        data_.assign(data, data + size);
        BlockDataAllocator::Instance().Free(data);
    }

    BlockData::BlockData(const boost::filesystem::path& block_file_path)
    {
        std::ifstream block_file_stream(block_file_path.string().c_str(), std::ios::binary);

        if (block_file_stream)
        {
            std::copy(
                std::istreambuf_iterator<char>(block_file_stream), 
                std::istreambuf_iterator<char>(), 
                std::back_inserter(data_));

            assert(data_.size() <= MaxBlockSize);
        }
    }

    BlockData::BlockData(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
        : data_(begin, end)
    {
    }

    BlockData::~BlockData()
    {
    }

    bool BlockData::WriteFile(const boost::filesystem::path& block_file_path)
    {
        std::ofstream output_file(block_file_path.string().c_str(), std::ios::binary);
        if (!output_file)
        {
            return false;
        }
        
        std::copy(data_.begin(), data_.end(), std::ostreambuf_iterator<char>(output_file));
        return true;
    }

    void BlockData::GetSubPiece(boost::uint16_t subpiece_index, protocol::SubPieceBuffer & buf)
    {
        assert(subpiece_index * SUB_PIECE_SIZE + buf.Length() <= data_.size());

        if (subpiece_index * SUB_PIECE_SIZE + buf.Length() <= data_.size())
        {
            memcpy(buf.Data(), &data_[0] + subpiece_index * SUB_PIECE_SIZE, buf.Length());
        }
    }

    boost::uint16_t BlockData::GetSubPieceNumber() const
    {
        return static_cast<boost::uint16_t>((Size() + SUB_PIECE_SIZE - 1) / SUB_PIECE_SIZE);
    }
}

#endif //USE_DIRECT_IO
