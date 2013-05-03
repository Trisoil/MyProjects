//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _SN_SESSION_H_
#define _SN_SESSION_H_

namespace super_node
{
    class Session
        :public boost::enable_shared_from_this<Session>
        ,public count_object_allocate<Session>
    {
        static const size_t MaxThreadAffinityId = 100;
    public:
        Session()
        {
            thread_affinity_id_ = rand()%MaxThreadAffinityId;
            idle_time_ = 0;
        }

        void AddTransactionId(boost::uint32_t transaction_id);
        bool IsDuplicateTransactionId(const boost::uint32_t transaction_id) const;

        void Tick()
        {
            ++idle_time_;
        }

        size_t GetThreadAffinityId() const 
        { 
            return thread_affinity_id_;
        }

        boost::uint32_t GetIdleTime() const
        {
            return idle_time_;
        }

        static void SetMaxTransactionIdNum(const boost::uint16_t num)
        {
            max_transaction_id_num = num ?  num : 1;
        }

        void ResetIdleTime()
        {
            idle_time_ = 0;
        }

        boost::uint32_t GetTransactionIdNum() const
        {
            return most_recent_transaction_ids_.size();
        }

    private:
        size_t thread_affinity_id_;
        std::set<boost::uint32_t> most_recent_transaction_ids_;
        boost::uint32_t idle_time_;
        static boost::uint16_t max_transaction_id_num;
    };
}
#endif
