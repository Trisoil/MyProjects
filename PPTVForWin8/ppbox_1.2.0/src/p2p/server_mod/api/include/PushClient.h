/******************************************************************************
*
* Copyright (c) 2012 PPLive Inc.  All rights reserved
* 
* PushClient.h
* 
* Description: 和push进行交互的命令
*             
* 
* --------------------
* 2011-02-06, youngky create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_PUSH_CLINET_H__
#define __PPLIVE_PUSH_CLINET_H__
#include <sstream>
#include <string>

#include "PushPack.h"
#include <boost/thread/mutex.hpp>

extern unsigned int g_total_send;
extern unsigned int g_total_recv;
//extern boost::mutex g_mu;

namespace ns_pplive
{
	namespace ns_push
	{
		class PushClient
		{
		public:
            int QueryPushTask(int pversion, boost::asio::io_service& ios, boost::asio::ip::udp::endpoint& push_server_endpoint, 
                std::vector<PlayHistoryItem>& play_history_vec, boost::uint32_t transaction_id, const Guid& peer_guid, 
                boost::uint32_t avg_upload_speed_kbs, boost::uint32_t used_disk_size, 
                boost::uint32_t upload_bandwidth_kbs, boost::uint32_t total_disk_size, 
                boost::uint32_t nat_type, boost::uint32_t online_percent,
                std::vector<PushTaskItem>& push_task_vec,
                boost::uint8_t& error_code, double timeout);
		
		public:
			PushClient(void);
			~PushClient(void);

		private:
            std::ostringstream err_msg_;
            PushPack pack_;

		public:
            std::string GetErrMsg();
            static unsigned int send_count_;
            static unsigned int recv_count_;
		};
	}
}

#endif /*__PPLIVE_PUSH_CLINET_H__*/