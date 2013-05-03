//-------------------------------------------------------------
//     Copyright (c) 2011 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------

#include "TrackerPack.h"
#include <stdio.h>


namespace ns_pplive
{
    namespace ns_tracker
    {

        TrackerPack::TrackerPack()
        {

        };
        TrackerPack::~TrackerPack()
        {
        };

        int TrackerPack::PackList(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
        {
            protocol::ListPacket packet;
            packet.PacketAction = protocol::ListPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.request.resource_id_ = resource_id;
            packet.request.peer_guid_ = peer_guid;
            packet.request.request_peer_count_ = request_peer_count;
            packet.end_point = endpoint_;
            PackSerialize(send_buffer,protocol::ListPacket::Action,packet);
            return 0;
        };



        int TrackerPack::UnPackList(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info)
        {
            protocol::ListPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            packet.response.peer_infos_.swap(candidate_peer_info);

            return 0;
        };

        int TrackerPack::PackListTcp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
        {
            protocol::ListTcpPacket packet;
            packet.PacketAction = protocol::ListTcpPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.request.resource_id_ = resource_id;
            packet.request.peer_guid_ = peer_guid;
            packet.request.request_peer_count_ = request_peer_count;
            packet.end_point = endpoint_;

            PackSerialize(send_buffer,protocol::ListTcpPacket::Action,packet);
            return 0;
        };

        int TrackerPack::UnPackListTcp(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info)
        {
            protocol::ListTcpPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            packet.response.peer_infos_.swap(candidate_peer_info);

            return 0;
        };

        int TrackerPack::PackListTcpWithIp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,unsigned request_ip,
            boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
        {
            protocol::ListTcpWithIpPacket packet;
            packet.PacketAction = protocol::ListTcpWithIpPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.request.resource_id_ = resource_id;
            packet.request.peer_guid_ = peer_guid;
            packet.request.request_peer_count_ = request_peer_count;
            packet.request.reqest_ip_ = request_ip;
            packet.end_point = endpoint_;            

            PackSerialize(send_buffer,protocol::ListTcpWithIpPacket::Action,packet);
            return 0;
        };

        int TrackerPack::UnPackListTcpWithIp(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info)
        {
            protocol::ListTcpWithIpPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            packet.response.peer_infos_.swap(candidate_peer_info);
            return 0;
        };

        int TrackerPack::PackListWithIp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,unsigned request_ip,
            boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer)
        {
            protocol::ListWithIpPacket packet;
            packet.PacketAction = protocol::ListWithIpPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.request.resource_id_ = resource_id;
            packet.request.peer_guid_ = peer_guid;
            packet.request.request_peer_count_ = request_peer_count;
            packet.request.reqest_ip_ = request_ip;
            packet.end_point = endpoint_;            

            PackSerialize(send_buffer,protocol::ListWithIpPacket::Action,packet);
            return 0;
        };

        int TrackerPack::UnPackListWithIp(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info)
        {
            protocol::ListWithIpPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            packet.response.peer_infos_.swap(candidate_peer_info);
            return 0;
        };


        int TrackerPack::PackReoprt( boost::uint32_t transaction_id,const Guid& peer_guid,boost::uint16_t local_resource_count,	boost::uint16_t server_resource_count,
            boost::uint16_t udp_port,boost::uint32_t stun_peer_ip,boost::uint16_t stun_peer_udpport,boost::uint32_t stun_detected_ip,boost::uint16_t stun_detected_udp_port,const std::vector<boost::uint32_t>& real_ips,
            const std::vector<REPORT_RESOURCE_STRUCT>& resource_ids,boost::uint8_t nat_type,boost::uint8_t upload_priority,boost::uint8_t idle_time_in_mins,boost::int32_t upload_bandwidth_kbs,
            boost::int32_t upload_limit_kbs,boost::int32_t upload_speed_kbs,boost::asio::ip::udp::endpoint endpoint_,/*boost::uint16_t upnp_tcp_port,*/UdpBuffer & send_buffer)
        {
            protocol::ReportPacket packet(transaction_id,PEER_VERSION,peer_guid,local_resource_count,server_resource_count,udp_port,stun_peer_ip,stun_peer_udpport,
                stun_detected_ip,stun_detected_udp_port,real_ips,resource_ids,nat_type,upload_priority,idle_time_in_mins,upload_bandwidth_kbs,upload_limit_kbs,upload_speed_kbs,endpoint_/*,upnp_tcp_port*/);

            PackSerialize(send_buffer,protocol::ReportPacket::Action,packet);
            return 0;
        };

        int TrackerPack::UnPackReport(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,boost::uint16_t& keepalive_interval,
            boost::uint32_t& detected_ip,boost::uint16_t& detected_udp_port,boost::uint16_t& resource_count)
        {
            protocol::ReportPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            keepalive_interval = packet.response.keep_alive_interval_;
            detected_ip = packet.response.detected_ip_;
            detected_udp_port = packet.response.detected_udp_port_;
            resource_count = packet.response.resource_count_;
            return 0;
        };

        int TrackerPack::PackLeave(boost::uint32_t transaction_id,const Guid& peer_guid,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer & send_buffer)
        {
            protocol::LeavePacket packet(transaction_id,PEER_VERSION,peer_guid,endpoint_);

            PackSerialize(send_buffer,protocol::LeavePacket::Action,packet);

            return 0;
        };

        int TrackerPack::PackQueryPeerCount(boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer)
        {
            protocol::QueryPeerCountPacket packet(transaction_id,PEER_VERSION,resource_id,endpoint_);

            PackSerialize(send_buffer,protocol::QueryPeerCountPacket::Action,packet);

            return 0;
        }

        int TrackerPack::UnPackQueryPeerCount(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,boost::uint32_t& peer_count)
        {
            protocol::QueryPeerCountPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            peer_count = packet.response.peer_count_;

            return 0;
        };

        int TrackerPack::PackQueryPeerResources(boost::uint32_t transaction_id,const Guid& peer_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer)
        {
            protocol::QueryPeerResourcesPacket packet(transaction_id,PEER_VERSION,peer_id,endpoint_);

            PackSerialize(send_buffer,protocol::QueryPeerResourcesPacket::Action,packet);

            return 0;
        }
        int TrackerPack::UnPackQueryPeerResources(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,vector<RID>& resources)
        {
            protocol::QueryPeerResourcesPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            resources.swap(packet.response.resources_);

            return 0;	
        }

        int TrackerPack::PackQueryTrackerStatistic(boost::uint32_t transaction_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer)
        {
            protocol::QueryTrackerStatisticPacket packet(transaction_id,PEER_VERSION,endpoint_);

            PackSerialize(send_buffer,protocol::QueryTrackerStatisticPacket::Action,packet);

            return 0;
        }
        int TrackerPack::UnPackQueryTrackerStatistic(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::string& tracker_statistic)
        {
            protocol::QueryTrackerStatisticPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }
            tracker_statistic = packet.response.tracker_statistic_;

            return 0;	
        }

        int TrackerPack::PackInternalCommand(boost::uint32_t transaction_id, const boost::asio::ip::udp::endpoint& endpoint_, UdpBuffer &send_buffer, boost::uint16_t cmd, boost::uint32_t arg)
        {
            protocol::InternalCommandPacket packet(transaction_id, INTERNAL_COMMAND_MAGIC_NUMBER, cmd, endpoint_);
            packet.reserved_ = arg;
            PackSerialize(send_buffer, protocol::InternalCommandPacket::Action, packet);

            return 0;
        }

        int TrackerPack::UnPackInternalCommand(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code)
        {
            protocol::InternalCommandPacket packet;
            int ret = UnpackUnSerialize(recv_buffer, transaction_id, packet, error_code);

            if (ret != 0)
            {
                return ret;
            }

            return 0;
        }


        string TrackerPack::GetErrMsg()
        {
            return err_msg_.str();
        };

        void TrackerPack::SetTraceLog(bool trace)
        {
        };

    };

};


