//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_REQUESTS_QUEUE_H
#define LIVE_MEDIA_TEST_REQUESTS_QUEUE_H

using namespace protocol;

#include <deque>

namespace live_media_test
{
    class Request;

    class RequestsQueue
    {
        typedef std::vector<boost::shared_ptr<Request> > Requests;
    public:
        void Push(boost::shared_ptr<Requests> requests);
        boost::shared_ptr<Requests> Pop();

        bool IsEmpty() 
        {
            boost::mutex::scoped_lock lock(this->mutex_);
            return queue_.empty(); 
        }

        size_t Size()
        { 
            boost::mutex::scoped_lock lock(this->mutex_);
            return queue_.size(); 
        }
    private:
        std::deque<boost::shared_ptr<Requests> > queue_;
        boost::mutex mutex_;
    };
}

#endif //LIVE_MEDIA_TEST_REQUESTS_QUEUE_H