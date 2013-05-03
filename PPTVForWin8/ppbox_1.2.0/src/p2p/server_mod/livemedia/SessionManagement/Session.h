//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _LIVE_MEDIA_SESSION_H_
#define _LIVE_MEDIA_SESSION_H_


namespace live_media
{
    class Session
        :public boost::enable_shared_from_this<Session>
    {
        static const size_t MaxThreadAffinityId = 100;
    public:
        Session(const channel_id& channel_identifier)
            : channel_identifier_(channel_identifier)
        {
            thread_affinity_id_ = rand()%MaxThreadAffinityId;
            idle_time_ = 0;
        }

        void AddTransactionId(boost::uint32_t transaction_id);
        bool IsDuplicateTransactionId(const boost::uint32_t transaction_id);

        void Tick()
        {
            ++idle_time_;
        }

        boost::uint32_t GetIdleTime() const
        {
            return idle_time_;
        }
        
        boost::uint32_t GetUploadIdleTime() const
        {
            boost::posix_time::ptime current_time(boost::posix_time::microsec_clock::local_time());

            if ((current_time - last_upload_time_).total_milliseconds() > 0 )
            {
                return (current_time - last_upload_time_).total_milliseconds();
            }
            else
            {
                assert(false);
                return 0;
            }
        }

        size_t GetThreadAffinityId() const 
        { 
            return thread_affinity_id_;
        }

        static void SetMaxTransactionIdNum(const boost::uint16_t num)
        {
            max_transaction_id_num = num;
        }

        const channel_id& GetChannelId() const 
        { 
            return channel_identifier_;
        }

        void ResetIdleTime()
        {
            idle_time_ = 0;
        }
        
        void SetUploadTime()
        {
            last_upload_time_ = boost::posix_time::microsec_clock::local_time();
        }

        boost::uint32_t GetTransactionIdNum()
        {
            return most_recent_transaction_ids_.size();
        }

    private:
        size_t thread_affinity_id_;
        std::set<boost::uint32_t> most_recent_transaction_ids_;
        boost::uint32_t idle_time_;
        boost::posix_time::ptime last_upload_time_;
        static boost::uint16_t max_transaction_id_num;
        channel_id channel_identifier_;
    };
}

#endif //_LIVE_MEDIA_SESSION_H_