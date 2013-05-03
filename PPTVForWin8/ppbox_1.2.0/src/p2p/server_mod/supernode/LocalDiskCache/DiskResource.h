//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DISK_RESOURCE_H
#define SUPER_NODE_DISK_RESOURCE_H

#include "Common.h"

namespace super_node
{
    typedef boost::dynamic_bitset<boost::uint8_t> BlocksMap;

    class DiskResource
        : public count_object_allocate<DiskResource>
    {
    public:
        DiskResource(const ResourceIdentifier& resource_identifier, bool new_resource, const BlocksMap& blocks_map)
            : disk_index_(0), resource_identifier_(resource_identifier), resource_size_(0), visit_count_(0), blocks_map_(blocks_map)
        {
            new_resource_count_down_ = new_resource ? MaxCountDown : 0;
        }

        void SetDiskIndex(boost::uint8_t disk_index)
        {
            disk_index_ = disk_index;
        }
        void SetSize(size_t resource_size) { resource_size_ = resource_size; }

        boost::uint8_t GetDiskIndex() const { return disk_index_; }
        boost::uint32_t GetImportanceScore() const;
        const ResourceIdentifier& GetResourceIdentifier() const { return resource_identifier_; }
        void OnMinuteElapsed(size_t minutes_elapsed);

        size_t GetSize() const { return resource_size_; } // resource_size_ == 0 means undetermined

        void Visit()
        {
            ++visit_count_;
        }

        bool HasBlock(size_t block_index) const
        {
            if (block_index < blocks_map_.size())
            {
                return  blocks_map_[block_index];
            }

            return false;
        }

        void AddBlock(size_t block_index);

        size_t GetBlocksCount() const
        {
            return blocks_map_.count();
        }

        const BlocksMap& GetBlocksBitmap() const { return blocks_map_; }

    private:
        ResourceIdentifier resource_identifier_;
        size_t resource_size_;
        boost::uint32_t visit_count_;
        boost::uint8_t disk_index_;
        boost::uint8_t new_resource_count_down_;
        BlocksMap blocks_map_;

        static const boost::uint8_t MaxCountDown = 4*60;
    };

    inline boost::uint32_t DiskResource::GetImportanceScore() const
    {
        boost::uint32_t score = visit_count_;
        if (new_resource_count_down_ > MaxCountDown - 60)
        {
            score += 50;
        }
        else if (new_resource_count_down_ > 0)
        {
            score += 10;
        }

        return score;
    }

    inline void DiskResource::OnMinuteElapsed(size_t minutes_elapsed)
    {
        if (new_resource_count_down_ > 0)
        {
            --new_resource_count_down_;
        }

        if (minutes_elapsed%(4*60) == 0)
        {
            if (visit_count_ > 0)
            {
                visit_count_ = visit_count_*8/10;
            }
        }
    }

    inline void DiskResource::AddBlock(size_t block_index)
    {
        if(block_index >= blocks_map_.size())
        {
            blocks_map_.resize(block_index + 1);
        }

        if (!blocks_map_[block_index])
        {
            blocks_map_.set(block_index);
            new_resource_count_down_ = MaxCountDown;
        }
    }
}

#endif //SUPER_NODE_DISK_RESOURCE_H
