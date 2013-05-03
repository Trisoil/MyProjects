/******************************************************************************
*
* Copyright (c) 2012 PPLive Inc.  All rights reserved.
*
* StunPack.h
* 
* Description: stun的内部实现接口
*             
* 
* --------------------
* 2012-03-14,  dingchangnie create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_STUN_PACK_H_NDC_20120314__
#define __PPLIVE_STUN_PACK_H_NDC_20120314__
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Protocol.h"
#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
#include "protocol/CheckSum.h"
#include <util/buffers/SubBuffers.h>

using namespace std;
using namespace protocol;

namespace ns_pplive
{
	namespace ns_stun
	{
		class StunPack
		{
		public:
			StunPack();
			~StunPack();

		public:
			int PackStunHandShake(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
			int UnPackStunHandShake(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code);
			int PackStunKPL(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
			int PackStunInvoke(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
			
		private:
			template <typename PackType> 
			void PackSerialize(UdpBuffer & send_buffer,boost::uint8_t action,const PackType& packet)
			{
				util::archive::LittleEndianBinaryOArchive<boost::uint8_t> oa(send_buffer);
				oa <<int(0)<< action << packet;

				boost::uint32_t & chk_sum = const_cast<boost::uint32_t &>(
					*boost::asio::buffer_cast<boost::uint32_t const *>(*send_buffer.data().begin()));

				chk_sum = check_sum_new(util::buffers::sub_buffers(send_buffer.data(), 4));
			}

			template <typename PackType> 
			int UnpackUnSerialize(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,PackType& packet,boost::uint8_t& error_code)
			{
				std::basic_istream<boost::uint8_t> is(&recv_buffer);
				boost::uint32_t chk_sum = 0;
				is.read((boost::uint8_t *)&chk_sum, sizeof(boost::uint32_t));	

				//杩欓噷闇�瑕乧hk_sum
				boost::uint8_t action = is.get();
				if (action != PackType::Action)
				{
					err_msg_.str("");
					err_msg_<<"action != PackType::Action,action is:"<<int(action)<<"PackType::Action is:"<<int(PackType::Action);
					return -1;
				}

				((protocol::Packet &)packet).PacketAction = PackType::Action;
				util::archive::LittleEndianBinaryIArchive<boost::uint8_t> ia(recv_buffer);
				ia >> packet;

				if (transaction_id != packet.transaction_id_)
				{
					err_msg_.str("");
					err_msg_<<"transaction_id != packet.transaction_id_,transaction_id is:"<<transaction_id<<"packet.transaction_id_ is:"<<packet.transaction_id_;
					return -1;
				}
				error_code = packet.error_code_;
				return 0;
			}

			ostringstream			err_msg_;

		public:	
			string GetErrMsg();
			void SetTraceLog(bool trace);

		};

	};

};

#endif /* __PPLIVE_STUN_PACK_H_NDC_20120314__ */