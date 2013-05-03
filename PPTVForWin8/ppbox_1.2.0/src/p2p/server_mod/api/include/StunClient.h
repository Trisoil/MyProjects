/******************************************************************************
*
* Copyright (c) 2012 PPLive Inc.  All rights reserved.
*
* StunClient.h
* 
* Description: stun的对外接口
*             
* 
* --------------------
* 2012-03-14,  dingchangnie create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_STUN_CLIENT_H_NDC_20120314__
#define __PPLIVE_STUN_CLIENT_H_NDC_20120314__

#include "StunPack.h"

namespace ns_pplive
{
	namespace ns_stun
	{
		class StunClient
		{
		public:
			int StunHandShake(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_, boost::asio::io_service& ios, double timeout);
			int StunKPL(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_, boost::asio::io_service& ios, double timeout);
			int StunInvoke(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint& endpoint_, boost::asio::io_service& ios, double timeout);
		public:
			StunClient();
			~StunClient();
		private:
			StunPack pack_;
			ostringstream err_msg_;
		public:
			string GetErrMsg();
			void SetTraceLog(bool trace);
		};
	};
};









#endif