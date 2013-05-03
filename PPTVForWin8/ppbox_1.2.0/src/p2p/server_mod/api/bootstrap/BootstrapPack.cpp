#include "BootstrapPack.h"
#include <stdio.h>

namespace ns_pplive
{
	namespace ns_bootstrap
	{
		BootstrapPack::BootstrapPack()
		{

		};
		BootstrapPack::~BootstrapPack()
		{

		};
		int BootstrapPack::PackQueryTrackerList(boost::uint32_t transaction_id,const Guid& peer_guid,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryTrackerListPacket packet;
			packet.PacketAction = protocol::QueryTrackerListPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.request.peer_guid_ = peer_guid;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryTrackerListPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::PackQueryTrackerForListing(boost::uint32_t transaction_id,const Guid& peer_guid,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryTrackerForListingPacket packet;
			packet.PacketAction = protocol::QueryTrackerForListingPacket::Action;
			packet.transaction_id_ = transaction_id;
			//packet.request.peer_guid_ = peer_guid;
			packet.tracker_type_ = protocol::QueryTrackerForListingPacket::VOD_TRACKER_FOR_LISTING;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryTrackerForListingPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::PackQueryStunServerList(boost::uint32_t transaction_id,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryStunServerListPacket packet;
			packet.PacketAction = protocol::QueryStunServerListPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryStunServerListPacket::Action, packet);
			return 0;

		};
		int BootstrapPack::PackQueryIndexServerList(boost::uint32_t transaction_id,const Guid& peer_guid,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryIndexServerListPacket packet;
			packet.PacketAction = protocol::QueryIndexServerListPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryIndexServerListPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::PackQueryNotifyList(boost::uint32_t transaction_id,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryNotifyListPacket packet;
			packet.PacketAction = protocol::QueryNotifyListPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryNotifyListPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::PackQueyLiveTrackerList(boost::uint32_t transaction_id,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryNotifyListPacket packet;
			packet.PacketAction = protocol::QueryLiveTrackerListPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryLiveTrackerListPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::PackQueryConfigString(boost::uint32_t transaction_id,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
		{
			protocol::QueryConfigStringPacket packet;
			packet.PacketAction = protocol::QueryConfigStringPacket::Action;
			packet.transaction_id_ = transaction_id;
			packet.end_point = endpoint_;
			PackSerialize(send_buffer, protocol::QueryConfigStringPacket::Action, packet);
			return 0;
		};
		int BootstrapPack::UnPackQueryTrackerList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< TRACKER_INFO>& trackerinfo)
		{
			protocol::QueryTrackerListPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.tracker_info_.swap(trackerinfo);
			return 0;
		};
		int BootstrapPack::UnPackQueryTrackerForListing(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< TRACKER_INFO>& trackerinfo)
		{
			protocol::QueryTrackerForListingPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.tracker_info_.swap(trackerinfo);
			return 0;
		};
		int BootstrapPack::UnPackQueryStunServerList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< STUN_SERVER_INFO>& stunserverinfo)
		{
			protocol::QueryStunServerListPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.stun_infos_.swap(stunserverinfo);
			return 0;

		};
		int BootstrapPack::UnPackQueryIndexServerList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< INDEX_SERVER_INFO>& indexserverinfo)
		{
			protocol::QueryIndexServerListPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.index_servers_.swap(indexserverinfo);
			return 0;
		};
		int BootstrapPack::UnPackQueryNotifyList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< NOTIFY_SERVER_INFO>& notifyserverinfo)
		{
			protocol::QueryNotifyListPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.notify_server_info_.swap(notifyserverinfo);
			return 0;
		};
		int BootstrapPack::UnPackQueyLiveTrackerList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector< TRACKER_INFO>& livetrackerinfo)
		{
			protocol::QueryLiveTrackerListPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.tracker_info_.swap(livetrackerinfo);
			return 0;
		};
		int BootstrapPack::UnPackQueryConfigString(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::string& configstring)
		{
			protocol::QueryConfigStringPacket packet;
			int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
			if (ret != 0)
			{
				return ret;
			}
			packet.response.config_string_.swap(configstring);
			return 0;
		};
		string BootstrapPack::GetErrMsg()
		{
			return err_msg_.str();
		};
		void BootstrapPack::SetTraceLog(bool trace)
		{

		};

	};

};