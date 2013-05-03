//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockSetInOrder.h"
#include "BlockCache.h"

namespace super_node
{
    size_t BlockSetInCostOrder::DropBlockCacheInCostOrder(size_t need_drop_num, ReleaseBlockDataCallback callback)
    {
        size_t droped_num = 0;
        for (std::set<boost::shared_ptr<BlockCache>, BlockCostCompare>::iterator iter = block_order_.begin();
            iter != block_order_.end() && need_drop_num > droped_num;)
        {
            (*iter)->ReleaseBlockData(callback);
            Erase(*(iter++));
            ++droped_num;
        }
        return droped_num;
    }
}