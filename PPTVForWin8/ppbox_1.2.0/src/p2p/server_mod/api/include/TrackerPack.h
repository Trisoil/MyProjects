/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved.
*
* TrackerPack.h
* 
* Description: 和tracker进行交互的命令
*             
* 
* --------------------
* 2011-08-29, kelvinchen create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_TRACKER_PACK_H_CK_20110829__
#define __PPLIVE_TRACKER_PACK_H_CK_20110829__
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
    namespace ns_tracker
    {

        class TrackerPack
        {
        public:
            TrackerPack();
            ~TrackerPack();

        public:
            int PackList(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
            int UnPackList(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info);

            int PackListTcp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
            int UnPackListTcp(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info);


            int PackListTcpWithIp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,unsigned request_ip,
                boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
            int UnPackListTcpWithIp(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info);

            int PackListWithIp(boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,unsigned request_ip,
                boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);
            int UnPackListWithIp(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::vector<CandidatePeerInfo>& candidate_peer_info);


            //这个命令字需要的信息好多啊。。
            int PackReoprt( boost::uint32_t transaction_id,const Guid& peer_guid,boost::uint16_t local_resource_count,	boost::uint16_t server_resource_count,
                boost::uint16_t udp_port,boost::uint32_t stun_peer_ip,boost::uint16_t stun_peer_udpport,boost::uint32_t stun_detected_ip,boost::uint16_t stun_detected_udp_port,const std::vector<boost::uint32_t>& real_ips,
                const std::vector<REPORT_RESOURCE_STRUCT>& resource_ids,boost::uint8_t nat_type,boost::uint8_t upload_priority,boost::uint8_t idle_time_in_mins,
                boost::int32_t upload_bandwidth_kbs,boost::int32_t upload_limit_kbs,boost::int32_t upload_speed_kbs,boost::asio::ip::udp::endpoint endpoint_,UdpBuffer & send_buffer);

            int UnPackReport(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,boost::uint16_t& keepalive_interval,
                boost::uint32_t& detected_ip,boost::uint16_t& detected_udp_port,boost::uint16_t& resource_count);

            int PackLeave(boost::uint32_t transaction_id,const Guid& peer_guid,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer & send_buffer);

            int PackQueryPeerCount(boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer);
            int UnPackQueryPeerCount(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,boost::uint32_t& peer_count);

            int PackQueryPeerResources(boost::uint32_t transaction_id,const Guid& peer_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer);
            int UnPackQueryPeerResources(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,vector<RID>& resources);

            int PackQueryTrackerStatistic(boost::uint32_t transaction_id,const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer &send_buffer);
            int UnPackQueryTrackerStatistic(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code,std::string& tracker_statictis);

            int PackInternalCommand(boost::uint32_t transaction_id, const boost::asio::ip::udp::endpoint& endpoint_, UdpBuffer &send_buffer, boost::uint16_t cmd, boost::uint32_t arg = 0);
            int UnPackInternalCommand(UdpBuffer& recv_buffer,boost::uint32_t transaction_id,boost::uint8_t& error_code);

        private:
            template <typename PackType> 
                void PackSerialize(UdpBuffer & send_buffer,boost::uint8_t action,const PackType& packet)
            {
                util::archive::LittleEndianBinaryOArchive<boost::uint8_t> oa(send_buffer);
                oa <<int(0)<< action << packet;

                boost::uint32_t & chk_sum = const_cast<boost::uint32_t &>(
                    *boost::asio::buffer_cast<boost::uint32_t const *>(*send_buffer.data().begin()));

                if (packet.peer_version_ < protocol::PEER_VERSION_V5)
                {
                    chk_sum = check_sum_old(util::buffers::sub_buffers(send_buffer.data(), 4));
                }
                else
                {
                    chk_sum = check_sum_new(util::buffers::sub_buffers(send_buffer.data(), 4));
                }        

                //chk_sum = check_sum_new(util::buffers::sub_buffers(send_buffer.data(), 4));
            }

            template <typename PackType> 
                int UnpackUnSerialize(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,PackType& packet,boost::uint8_t& error_code)
            {
                std::basic_istream<boost::uint8_t> is(&recv_buffer);
                boost::uint32_t chk_sum = 0;
                is.read((boost::uint8_t *)&chk_sum, sizeof(boost::uint32_t));	

                //这里需要chk_sum
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

#endif /* __PPLIVE_TRACKER_PACK_H_CK_20110829__ */