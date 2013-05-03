/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* TrackerApi.h
* 
* Description: 负载均衡，优先返回状态好的tracker
*             
* 
* --------------------
* 2011-12-27, dingchangnie create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_LOAD_BALANCE_H_NDC_20111220__
#define __PPLIVE_LOAD_BALANCE_H_NDC_20111220__

#include "BootstrapClient.h"
#include <map>
#include <set>

namespace ns_pplive
{
	namespace ns_loadbalance
	{
		const int LOADBALANCE_SUCCESS = 0;
		const int LOADBALANCE_FAIL_CAN_RETRY = -1;
		const int LOADBALANCE_FAIL_NO_RETRY = -2; //选择的set的大小为0
		struct SelectParam{
			double average_time;
			int total_success_times;
			int total_times;
			double score;
			SelectParam():average_time(0.0),total_success_times(0),total_times(0),score(1.0){};
		};
		typedef  map<boost::asio::ip::udp::endpoint, SelectParam>::iterator map_iter;
		class LoadBalance
		{
		public:
			LoadBalance();
			LoadBalance(const LoadBalance& lb);
			~LoadBalance();
			bool AddEndpoint(const boost::asio::ip::udp::endpoint& endpoint_);
			void ClearEndpoint();
			int UpdatePara(const boost::asio::ip::udp::endpoint& endpoint_, int ret, double time);     
			int GetResult(set<boost::asio::ip::udp::endpoint>& candidate_endpoint, int neednumber) const;
			int GetResult(boost::asio::ip::udp::endpoint& candidate_endpoint) const;     //0 success , -1 fail but return 1 tracker
			int GetResult(set<boost::asio::ip::udp::endpoint>& candidate_endpoint, set<boost::asio::ip::udp::endpoint>& exclude_endpoint, int neednumber);//0 success  
			int GetResult(boost::asio::ip::udp::endpoint& candidate_endpoint, set<boost::asio::ip::udp::endpoint>& exclude_endpoint);                     //-2 neednumber is too greater,so all tracker is returned
			int Size() const;
			string GetLoadBalanceInfo();

		private:
			void RangeForSelect(set<boost::asio::ip::udp::endpoint>& select_endpoint, set<boost::asio::ip::udp::endpoint>& exclude_endpoint) const;
			bool SelectOne(boost::asio::ip::udp::endpoint& endpoint_, set<boost::asio::ip::udp::endpoint>& select_endpoint) const;
			double GetScore(const boost::asio::ip::udp::endpoint& endpoint_) const;
			map<boost::asio::ip::udp::endpoint, SelectParam> tracker_param_;	
			//ostringstream			select_info_;
			//ostringstream update_info_;
			double average_time;
			double total_success_times;
		};
	};

};

#endif