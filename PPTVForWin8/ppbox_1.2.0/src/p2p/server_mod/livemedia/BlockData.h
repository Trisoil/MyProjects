//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_BLOCK_DATA_H
#define LIVE_MEDIA_BLOCK_DATA_H

namespace live_media
{
    class HeaderSubPiece;

    struct sub_piece
    {
        sub_piece() : length(0), content(0)
        {
        }

        size_t length;
        char * content;
    };

    class BlockData
    {
    public:
        const static size_t LiveSubPiecesCountPerPiece = 16;
        const static size_t LiveSubPieceSizeInBytes = 1400;

        BlockData(const char* buffer, size_t bytes);

        size_t GetSubPiecesCount() const
        {
            return subpieces_.size();
        }

        ~BlockData();

        bool IsValid() const;

        size_t GetBlockSize() const { return block_size_in_bytes_; }

        size_t GetSubPieceSize(boost::uint16_t subpiece_index) const;
        const char * GetSubPieceContent(boost::uint16_t subpiece_index) const;
    private:
        bool IsPieceValid(boost::shared_ptr<HeaderSubPiece> header_subpiece, size_t data_piece_index) const;

    private:
        std::vector<sub_piece> subpieces_;
        size_t block_size_in_bytes_;
        char* buffer_;
    };

    class BlockMetaData
    {
    public:
        BlockMetaData(size_t block_id, size_t block_data_size)
            : block_id_(block_id), block_data_size_(block_data_size)
        {}

        size_t GetBlockId() const
        {
            return block_id_;
        }

        size_t GetPiecesCount() const
        {
            return BlockMetaData::GetPiecesCount(GetSubPiecesCount());
        }

        static size_t GetPiecesCount(size_t subpieces_count)
        {
            if (subpieces_count == 0)
            {
                return 0;
            }

            return 1 + (subpieces_count - 1 + BlockData::LiveSubPiecesCountPerPiece - 1)/BlockData::LiveSubPiecesCountPerPiece;
        }

        size_t GetSubPiecesCount() const
        {
            return (block_data_size_ + BlockData::LiveSubPieceSizeInBytes - 1)/BlockData::LiveSubPieceSizeInBytes;
        }

    private:
        size_t block_id_;
        size_t block_data_size_;
    };
}
#endif //LIVE_MEDIA_BLOCK_DATA_H