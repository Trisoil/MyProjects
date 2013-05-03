/******************************************************************************
*
* Copyright (c) 2012 PPLive Inc.  All rights reserved
* 
* PushPack.h
* 
* Description: 用于对Push进行压力测试，模拟Push的客户端发包
*             
* 
* --------------------
* 2011-02-10, youngky create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_PUSH_PRESS_HANDLER_H__
#define __PPLIVE_PUSH_PRESS_HANDLER_H__
#include "PushClient.h"
#include "PPliveThreadPool.h"

using namespace std;

namespace ns_pplive
{
    namespace ns_push
    {
        class PushPressHandler :
            public PPliveThreadPool
        {
        public:
            PushPressHandler(int pool_count, const boost::asio::ip::udp::endpoint& end_point);
            virtual ~PushPressHandler(void);

        private:
            void DoIt();
            void FillPlayHistoryVec();
            void QueryPushTask();
            std::string GeneratePeerID();
            void GeneratePlayHistoryVec(vector<PlayHistoryItem>& play_history_vec);

        private:
            boost::asio::io_service ios_;
            boost::asio::ip::udp::endpoint endpoint_;
            std::vector<PlayHistoryItem> play_history_vec_;
        };
    }
}
#endif