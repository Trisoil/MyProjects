#include "StunPack.h"
#include <stdio.h>

namespace ns_pplive
{
	namespace ns_stun
	{
		StunPack::StunPack()
		{

		};
		StunPack::~StunPack()
		{

		};
		int StunPack::PackStunHandShake(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::StunHandShakePacket packet;
			packet.PacketAction = protocol::StunHandShakePacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::StunHandShakePacket::Action, packet);

			return 0;
		};
		int StunPack::UnPackStunHandShake(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code)
		{
			protocol::StunHandShakePacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			//cout<<packet.response.detect_udp_port_<<" "<<boost::asio::ip::address_v4(packet.response.detected_ip_).to_string()<<" "<<packet.response.keep_alive_interval_<<endl;

			return 0;
		};
		int StunPack::PackStunKPL(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::StunKPLPacket packet;
			packet.PacketAction = protocol::StunKPLPacket::Action;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::StunKPLPacket::Action, packet);

			return 0;

		};
		int StunPack::PackStunInvoke(boost::uint32_t transaction_id, boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::StunInvokePacket packet;
			packet.PacketAction = protocol::StunInvokePacket::Action;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::StunInvokePacket::Action, packet);

			return 0;

		}
		string StunPack::GetErrMsg()
		{
			return err_msg_.str();
		};
		void StunPack::SetTraceLog(bool trace)
		{

		};

	};

};