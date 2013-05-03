//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_SET_IN_ORDER_HPP
#define SUPER_NODE_BLOCK_SET_IN_ORDER_HPP

#include "BlockSetInOrder.h"
#include "BlockCache.h"

namespace super_node
{
    template <typename BlockCompareStruct>
    void BlockSetInOrder<BlockCompareStruct>::Insert(boost::shared_ptr<BlockCache> block)
    {
        block_order_.insert(block);
    }

    template <typename BlockCompareStruct>
    void BlockSetInOrder<BlockCompareStruct>::Erase(boost::shared_ptr<BlockCache> block)
    {
        block_order_.erase(block);
    }

    template <typename BlockCompareStruct>
    void BlockSetInOrder<BlockCompareStruct>::ReArrangeTheSet()
    {
        std::set<boost::shared_ptr<BlockCache> > tmp_block_set;
        typename std::set<boost::shared_ptr<BlockCache>, BlockCompareStruct>::const_iterator iter = block_order_.begin();
        for (; iter != block_order_.end(); ++iter)
        {
            boost::shared_ptr<BlockCache> tmp_block_cache = *iter;
            tmp_block_set.insert(tmp_block_cache);
        }

        block_order_.clear();

        for (std::set<boost::shared_ptr<BlockCache> >::const_iterator iter = tmp_block_set.begin();
            iter != tmp_block_set.end(); ++iter)
        {
            Insert(*iter);
        }
    }

    template <typename BlockCompareStruct>
    bool BlockSetInOrder<BlockCompareStruct>::CheckOrder() const
    {
        if (block_order_.empty())
            return true;

        typename std::set<boost::shared_ptr<BlockCache>, BlockCompareStruct>::const_iterator iter = block_order_.begin();
        typename std::set<boost::shared_ptr<BlockCache>, BlockCompareStruct>::const_iterator next_iter = block_order_.begin();
        ++next_iter;

        BlockCompareStruct block_compare;
        for (; next_iter != block_order_.end(); ++iter, ++next_iter)
        {
            if (!block_compare(*iter, *next_iter))
            {
                return false;
            }
        }
        return true;
    }

    template <typename BlockCompareStruct>
    void BlockSetInOrder<BlockCompareStruct>::Clear()
    {
        block_order_.clear();
    }

    template <typename BlockCompareStruct>
    size_t BlockSetInOrder<BlockCompareStruct>::Size() const
    {
        return block_order_.size();
    }
}

#endif  // SUPER_NODE_BLOCK_SET_IN_ORDER_HPP