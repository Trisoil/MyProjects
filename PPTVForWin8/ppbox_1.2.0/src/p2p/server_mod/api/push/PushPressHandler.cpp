//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------
#include "PushPressHandler.h"
#include <fstream>

#define PVERSION_PRESSURE 2

namespace ns_pplive
{
    namespace ns_push
    {
        PushPressHandler::PushPressHandler(int pool_count, const boost::asio::ip::udp::endpoint& end_point) : PPliveThreadPool(pool_count), endpoint_(end_point)
        {
            FillPlayHistoryVec();
        }

        PushPressHandler::~PushPressHandler(void)
        {
        }

        void PushPressHandler::DoIt()
        {
            while(true)
            {
                QueryPushTask(); 
            }
        }

        void PushPressHandler::FillPlayHistoryVec( )
        {
            ifstream fin("play_history.txt");
            while (!fin.eof())
            {
                PlayHistoryItem item;
                fin >> item.video_name_;
                fin >> item.downloaded_segment_num_;
                fin >> item.continuously_played_duration_;
                play_history_vec_.push_back(item);
            }

            cout <<"size: " << play_history_vec_.size() << endl;
            fin.close();
        }

        void PushPressHandler::QueryPushTask()
        {
            //boost::asio::ip::address_v4 av4;
            //av4 = boost::asio::ip::address_v4::from_string(ip);
            //boost::asio::ip::udp::endpoint endpoint_(boost::asio::ip::address(av4),port);
            vector<PlayHistoryItem> play_history_vec;
            GeneratePlayHistoryVec(play_history_vec);
            vector<PushTaskItem> push_task_vec;

            ns_pplive::ns_push::PushClient push_client;
            boost::uint8_t error_code = 0;

            ++g_total_send;
            int ret = push_client.QueryPushTask(PVERSION_PRESSURE, ios_, endpoint_, play_history_vec,boost::uint32_t(rand()), Guid(GeneratePeerID()),100, 0, 100, 2*1024*1024, 0, 50, push_task_vec, error_code, 0.0);
            if (1 != ret)
            {
                cout<<"process failed,msg:"<<push_client.GetErrMsg()<<endl;
                return;
            }

            ++g_total_recv;
        }

        std::string PushPressHandler::GeneratePeerID()
        {
            stringstream ss;
            ss << rand()%1000000;
            return ss.str();
        }

        void PushPressHandler::GeneratePlayHistoryVec( vector<PlayHistoryItem>& play_history_vec )
        {
            unsigned int vec_size = rand()%16;
            assert(vec_size < 16);
            unsigned int all_size = play_history_vec_.size();
            for (unsigned int i = 0; i < vec_size; ++i)
            {
                unsigned int index = rand() % all_size;
                assert(index < all_size);
                play_history_vec.push_back(play_history_vec_[index]);
            }
        }
    }
}
