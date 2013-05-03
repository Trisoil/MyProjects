//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "RequestsQueue.h"

namespace super_node_test
{
    void RequestsQueue::Push(boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > requests)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        queue_.push_back(requests);
    }

    boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > RequestsQueue::Pop()
    {
        boost::shared_ptr<Requests> requests;

		boost::mutex::scoped_lock lock(this->mutex_);

		if (!queue_.empty())
		{
			requests = queue_.front();
			queue_.pop_front();
		}

        return requests;
    }
}

