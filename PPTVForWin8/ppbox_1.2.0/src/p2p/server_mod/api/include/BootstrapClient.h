/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved.
*
* BootstrapClient.h
* 
* Description: bs的对外接口
*             
* 
* --------------------
* 2011-11-22,  dingchangnie create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_BOOTSTRAP_CLIENT_H_NDC_20111122__
#define __PPLIVE_BOOTSTRAP_CLIENT_H_NDC_20111122__
#include "BootstrapPack.h"
//#include <xstring>



namespace ns_pplive
{
	namespace ns_bootstrap
	{
		class BootstrapClient
		{
		public:
			int QueryTrackerList(std::vector<TRACKER_INFO>& tracker_info,boost::uint32_t transaction_id,const Guid& peer_guid,
				                 boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);   //0 success 1 fail
			int QueryTrackerForListing(std::vector<TRACKER_INFO>& tracker_info,boost::uint32_t transaction_id,const Guid& peer_guid,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout); 
			int QueryStunServerList(std::vector<STUN_SERVER_INFO>& stunserver_info,boost::uint32_t transaction_id,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout );
			int QueryIndexServerList(std::vector<INDEX_SERVER_INFO>& tracker_info,boost::uint32_t transaction_id,const Guid& peer_guid,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);
			int QueryNotifyList(std::vector< NOTIFY_SERVER_INFO>& notify_info,boost::uint32_t transaction_id,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout );
			int QueryLiveTrackerList(std::vector< TRACKER_INFO>& livetracker_info,boost::uint32_t transaction_id,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout );
			int QueryConfigString(std::string& config_string,boost::uint32_t transaction_id,
				boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout );
		public:
			BootstrapClient();
			~BootstrapClient();
		private:
			BootstrapPack pack_;
			ostringstream err_msg_;
		public:
			string GetErrMsg();
			void SetTraceLog(bool trace);
		};
	};
};

#endif