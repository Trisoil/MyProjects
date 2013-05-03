//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockSetInOrder.h"
#include "BlockCache.h"

#include "ResourceCache.h"

namespace super_node
{
    void BlockSetInExpireOrder::MoveDeadBlockToBlockSetInCostOrder(size_t expire_delim)
    {
        for (std::set<boost::shared_ptr<BlockCache>, BlockExpireCompare>::iterator iter = block_order_.begin();
            iter != block_order_.end();)
        {
            if ((*iter)->GetExpire() < expire_delim)
            {
                (*iter)->GetMyResource()->InsertBlockCacheToSetInCostOrder(*iter);
                Erase(*(iter++));
            }
            else
            {
                ++iter;
            }
        }
    }

    size_t BlockSetInExpireOrder::DropDeadBlockCacheInExpireOrder(size_t need_drop_num, time_t now, ReleaseBlockDataCallback callback)
    {
        size_t droped_num = 0;
        for (std::set<boost::shared_ptr<BlockCache>, BlockExpireCompare>::iterator iter = block_order_.begin();
            iter != block_order_.end() && (*iter)->IsBlockDead(now) && need_drop_num > droped_num;)
        {
            (*iter)->ReleaseBlockData(callback);
            Erase(*(iter++));
            ++droped_num;
        }
        return droped_num;
    }

    size_t BlockSetInExpireOrder::DropAliveBlockCacheInExpireOrder(size_t need_drop_num, time_t now, ReleaseBlockDataCallback callback)
    {
        size_t droped_num = 0;
        for (std::set<boost::shared_ptr<BlockCache>, BlockExpireCompare>::iterator iter = block_order_.begin();
            iter != block_order_.end() && need_drop_num > droped_num;)
        {
            if (!(*iter)->IsBlockDead(now))
            {
                (*iter)->ReleaseBlockData(callback);
                Erase(*(iter++));
                ++droped_num;
            }
            else
            {
                ++iter;
            }
        }
        return droped_num;
    }
}