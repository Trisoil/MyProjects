//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "RequestsQueue.h"

namespace live_media_test
{
    void RequestsQueue::Push(boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > requests)
    {
        boost::mutex::scoped_lock lock(this->mutex_);
        queue_.push_back(requests);
    }

    boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > RequestsQueue::Pop()
    {
        assert(this->Size() > 0);

        boost::shared_ptr<Requests> requests;
        if (this->Size() > 0)
        {
            boost::mutex::scoped_lock lock(this->mutex_);
            requests = queue_.front();
            queue_.pop_front();
        }

        return requests;
    }
}

