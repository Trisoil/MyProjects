//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// SubPieceInfo.h

#ifndef _STORAGE_SUB_PIECE_INFO_H_
#define _STORAGE_SUB_PIECE_INFO_H_

namespace protocol
{

    struct SubPieceInfo
    {
        boost::uint16_t block_index_;
        boost::uint16_t subpiece_index_;

        explicit SubPieceInfo(
            boost::uint16_t block_index = 0,
            boost::uint16_t subpiece_index = 0)
            : block_index_(block_index)
            , subpiece_index_(subpiece_index)
        {
        }

        SubPieceInfo(
            const SubPieceInfo & subpiece_info)
            : block_index_(subpiece_info.block_index_)
            , subpiece_index_(subpiece_info.subpiece_index_)
        {
        }

        SubPieceInfo& operator = (
            const SubPieceInfo& subpiece_info)
        {
            block_index_ = subpiece_info.block_index_;
            subpiece_index_ = subpiece_info.subpiece_index_;
            return *this;
        }

        SubPieceInfo GetSubPieceInfoStruct() const
        {
            SubPieceInfo subpiece_info_struct;
            subpiece_info_struct.block_index_ = block_index_;
            subpiece_info_struct.subpiece_index_ = subpiece_index_;
            return subpiece_info_struct;
        }

        PieceInfo GetPieceInfo() const
        {
            return PieceInfo(block_index_, GetPieceIndex());
        }

        static void MakeByPosition(
            boost::uint32_t position,
            boost::uint32_t block_size,
            SubPieceInfo & subpiece_info)
        {
            subpiece_info.block_index_ = (block_size == 0 ? 0 : position / block_size);
            subpiece_info.subpiece_index_ = (block_size == 0 ? 0 : (position % block_size) / SUB_PIECE_SIZE);
        }

        bool operator < (
            const SubPieceInfo& n) const
        {
            if (block_index_ != n.block_index_)
                return block_index_ < n.block_index_;
            else return subpiece_index_ < n.subpiece_index_;
        }

        bool operator <= (
            const SubPieceInfo& n) const
        {
            if (block_index_ != n.block_index_)
                return block_index_ < n.block_index_;
            else return subpiece_index_ <= n.subpiece_index_;
        }

        bool operator == (
            const SubPieceInfo& n) const
        {
            return block_index_ == n.block_index_ && subpiece_index_ == n.subpiece_index_;
        }

        boost::uint32_t GetPieceIndex() const
        {
            return subpiece_index_ / SUB_PIECE_COUNT_PER_PIECE;
        }

        boost::uint32_t GetSubPieceIndexInFile(
            boost::uint32_t block_size) const
        {
            assert(block_size % PIECE_SIZE == 0);
            boost::uint32_t subpiece_count_in_block = block_size / SUB_PIECE_SIZE;
            return subpiece_count_in_block*block_index_ + subpiece_index_;
        }

        boost::uint32_t GetSubPieceIndexInBlock() const
        {
            return subpiece_index_;
        }

        boost::uint32_t GetSubPieceIndexInPiece() const
        {
            return subpiece_index_ % SUB_PIECE_COUNT_PER_PIECE;
        }

        boost::uint32_t GetPosition(
            boost::uint32_t block_size) const
        {
            assert(block_size % PIECE_SIZE == 0);
            return block_index_*block_size + subpiece_index_*SUB_PIECE_SIZE;
        }

        boost::uint32_t GetEndPosition(
            boost::uint32_t block_size) const
        {
            assert(block_size % PIECE_SIZE == 0);
            return GetPosition(block_size) + SUB_PIECE_SIZE;
        }

        friend std::ostream & operator << (
            std::ostream& os,
            const SubPieceInfo& n)
        {
            os << "(" << n.block_index_ << "|sp:" << n.subpiece_index_ << ")";
            return os;
        }
    };

}

#endif  // _STORAGE_SUB_PIECE_INFO_H_
