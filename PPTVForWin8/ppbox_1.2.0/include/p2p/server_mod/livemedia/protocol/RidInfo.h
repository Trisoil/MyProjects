// RidInfo.h

#ifndef _STORAGE_RID_INFO_H_
#define _STORAGE_RID_INFO_H_

namespace storage
{

    template<typename T>
    inline T UpperDiv(const T& a, const T& b)
    {
        return (a + b - 1) / b;
    }

    struct RidInfo
    {
    public:
        RID rid_;
        uint32_t file_length_;
        uint32_t block_size_;
        uint32_t block_count_;

    public:
        std::vector<MD5> block_md5_s_;

        explicit RidInfo() : file_length_(0), block_size_(0), block_count_(0) {}

        bool HasRID() const
        {
            return ! rid_.is_empty();
        }

        RID GetRID() const
        {
            return rid_;
        }

        uint32_t GetFileLength() const
        {
            return file_length_;
        }

        uint32_t GetBlockSize() const
        {
            return block_size_;
        }

        uint32_t GetBlockCount() const
        {
            return block_count_;
        }

        void InitByFileLength(
            uint32_t file_length)
        {
            file_length_ = file_length;

            if( file_length_ <= BLOCK_MIN_SIZE * BLOCK_MAX_COUNT )
            {
                block_size_ = BLOCK_MIN_SIZE;
                block_count_ = UpperDiv<uint32_t>(file_length_, block_size_);
            }
            else
            {
                block_size_ = static_cast<uint32_t>( UpperDiv<boost::uint64_t>( file_length_, BLOCK_MAX_COUNT ) );
                block_size_ = UpperDiv<uint32_t>( block_size_, PIECE_SIZE ) * PIECE_SIZE;
                block_count_ = static_cast<uint32_t>( UpperDiv<boost::uint64_t>( file_length_, block_size_) );
            }
            assert( block_size_ %  PIECE_SIZE == 0 );
        }

        bool operator == (const RidInfo& n) const
        {
            return rid_ == n.GetRID() && file_length_ == n.GetFileLength()
                && block_size_ == n.GetBlockSize() && block_count_ == n.GetBlockCount()
                && block_md5_s_ == n.block_md5_s_ ;
        }

        friend std::ostream& operator << (
            std::ostream& os, 
            const RidInfo& rid_info)
        {
            return os << " ResourceID: " << rid_info.GetRID()
                << ", FileLength: " << rid_info.GetFileLength()
                << ", BlockSize: " << rid_info.GetBlockSize()
                << ", BlockCount: " << rid_info.GetBlockCount()
                //<<", BlockMD5s: [" << rid_info.block_md5_s_ << "]"
                ;
        }
    };

}

#endif // _STORAGE_RID_INFO_H_
