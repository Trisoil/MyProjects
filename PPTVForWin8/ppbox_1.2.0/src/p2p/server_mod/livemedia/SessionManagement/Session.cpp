//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "pre.h"
#include "Session.h"

namespace live_media
{
    boost::uint16_t Session::max_transaction_id_num = 0;
    void Session::AddTransactionId(boost::uint32_t transaction_id)
    {
        assert(most_recent_transaction_ids_.find(transaction_id) == most_recent_transaction_ids_.end());
        while (most_recent_transaction_ids_.size() >= max_transaction_id_num)
        {
            std::set<boost::uint32_t>::iterator erase_iter = most_recent_transaction_ids_.begin();
            most_recent_transaction_ids_.erase(erase_iter);
        }
        most_recent_transaction_ids_.insert(transaction_id);
    }

    bool Session::IsDuplicateTransactionId(const boost::uint32_t transaction_id)
    {
        return most_recent_transaction_ids_.find(transaction_id) != most_recent_transaction_ids_.end();
    }
}