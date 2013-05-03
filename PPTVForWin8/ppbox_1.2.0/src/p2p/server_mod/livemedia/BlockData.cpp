//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "BlockData.h"
#include "HeaderSubPiece.h"
#include "protocol/CheckSum.h"

namespace live_media
{
    BlockData::BlockData(const char* buffer, size_t bytes)
    {
        block_size_in_bytes_ = bytes;
        size_t subpieces_count = (bytes + LiveSubPieceSizeInBytes - 1)/LiveSubPieceSizeInBytes;
        subpieces_.resize(subpieces_count);

        buffer_ = new char[bytes];
        memcpy(buffer_, buffer, bytes);

        size_t last_subpiece_size = block_size_in_bytes_ % LiveSubPieceSizeInBytes;
        if (last_subpiece_size == 0)
        {
            last_subpiece_size = LiveSubPieceSizeInBytes;
        }

        for(size_t i = 0; i < subpieces_count; ++i)
        {
            subpieces_[i].content = buffer_ + i*LiveSubPieceSizeInBytes;
            subpieces_[i].length = (i == subpieces_count - 1) ? last_subpiece_size : LiveSubPieceSizeInBytes;
        }
    }

    BlockData::~BlockData()
    {
        delete[] buffer_;
        buffer_ = 0;
    }

    bool BlockData::IsPieceValid(boost::shared_ptr<HeaderSubPiece> header_subpiece, size_t data_piece_index) const
    {
        uint32_t first_subpiece_index = HeaderSubPiece::Constants::SubPiecesPerPiece * data_piece_index + 1;
        assert(first_subpiece_index < subpieces_.size());

        uint32_t last_subpiece_index = first_subpiece_index + HeaderSubPiece::Constants::SubPiecesPerPiece - 1;
        if (last_subpiece_index >= subpieces_.size())
        {
            last_subpiece_index = subpieces_.size() - 1;
        }

        assert(last_subpiece_index >= first_subpiece_index);

        //memcpy的原因是为了确保4字节对齐，以符合checksum算法的要求(checksum为了在box中也有好的性能而有此要求)。
        uint8_t buffer[HeaderSubPiece::Constants::SubPiecesPerPiece*HeaderSubPiece::Constants::SubPieceSizeInBytes];
        uint8_t* buffer_write_pos = buffer;

        for(uint32_t subpiece_index = first_subpiece_index;
            subpiece_index <= last_subpiece_index;
            ++subpiece_index)
        {
            assert(subpieces_[subpiece_index].length <= HeaderSubPiece::Constants::SubPieceSizeInBytes);

            memcpy(
                buffer_write_pos, 
                subpieces_[subpiece_index].content, 
                subpieces_[subpiece_index].length);

            buffer_write_pos += subpieces_[subpiece_index].length;
        }

        uint32_t expected_checksum = header_subpiece->GetPieceChecksum(data_piece_index);
        uint32_t actual_checksum = check_sum_new(boost::asio::const_buffers_1(buffer, buffer_write_pos - buffer));

        return expected_checksum == actual_checksum;
    }

    bool BlockData::IsValid() const
    {
        bool is_header_subpiece_valid = false;

        boost::shared_ptr<HeaderSubPiece> header_subpiece;
        if (subpieces_.size() > 1)
        {
            header_subpiece = HeaderSubPiece::Load(subpieces_[0].content, subpieces_[0].length);
            if (header_subpiece && 
                header_subpiece->IsValid() && 
                header_subpiece->GetDataLength() + BlockData::LiveSubPieceSizeInBytes == block_size_in_bytes_)
            {
                is_header_subpiece_valid = true;
            }
        }

        if (!is_header_subpiece_valid)
        {
            LOG4CPLUS_WARN(Loggers::Operations(), "InvalidBlockData:BadHeader");
            return false;
        }

        assert(header_subpiece);

        size_t piece_count = BlockMetaData::GetPiecesCount(this->GetSubPiecesCount());
        for(size_t data_piece_index = 0; data_piece_index < piece_count - 1; ++data_piece_index)
        {
            if (!IsPieceValid(header_subpiece, data_piece_index))
            {
                LOG4CPLUS_WARN(Loggers::Operations(), "InvalidBlockData:BadPieceChecksum");
                return false;
            }
        }

        return true;
    }

    size_t BlockData::GetSubPieceSize(boost::uint16_t subpiece_index) const
    {
        assert(subpiece_index < subpieces_.size());
        return subpieces_[subpiece_index].length;
    }

    const char * BlockData::GetSubPieceContent(boost::uint16_t subpiece_index) const
    {
        assert(subpiece_index < subpieces_.size());
        return subpieces_[subpiece_index].content;
    }
}

