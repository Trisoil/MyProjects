//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------
#include "PushPack.h"
#include <stdio.h>

namespace ns_pplive
{
	namespace ns_push
	{
        PushPack::PushPack( void )
        {
        }

        PushPack::~PushPack( void )
        {
        }

        int PushPack::PackQueryPushTask(int pversion, boost::uint32_t transaction_id, const Guid& peer_guid, 
                                         boost::asio::ip::udp::endpoint endpoint_, boost::uint32_t avg_upload_speed_kbs, 
                                         boost::uint32_t used_disk_size, boost::uint32_t upload_bandwidth_kbs,
                                         boost::uint32_t total_disk_size, boost::uint32_t nat_type,
                                         boost::uint32_t online_percent,
                                         UdpBuffer & send_buffer, std::vector<protocol::PlayHistoryItem>& play_history_vec)
        {
            if (pversion == 2) {
                protocol::QueryPushTaskPacketV2 packet;
                packet.PacketAction = protocol::QueryPushTaskPacketV2::Action;
                packet.transaction_id_ = transaction_id;
                packet.request.avg_upload_speed_kbs_ = avg_upload_speed_kbs;
                packet.request.peer_guid_ = peer_guid;
                packet.request.play_history_vec_.swap(play_history_vec);
                packet.request.upload_bandwidth_kbs_ = upload_bandwidth_kbs;
                packet.request.used_disk_size_ = used_disk_size;
                packet.end_point = endpoint_;
                PackSerialize(send_buffer, protocol::QueryPushTaskPacketV2::Action, packet);
            }else{
                protocol::QueryPushTaskPacketV3 packet;
                packet.PacketAction = protocol::QueryPushTaskPacketV3::Action;
                packet.transaction_id_ = transaction_id;
                packet.request.avg_upload_speed_kbs_ = avg_upload_speed_kbs;
                packet.request.peer_guid_ = peer_guid;
                packet.request.play_history_vec_.swap(play_history_vec);
                packet.request.upload_bandwidth_kbs_ = upload_bandwidth_kbs;
                packet.request.used_disk_size_ = used_disk_size;
                packet.request.total_disk_size_ = total_disk_size;
                packet.request.nat_type_ = nat_type;
                packet.request.online_percent_ = online_percent;
                packet.end_point = endpoint_;
                PackSerialize(send_buffer, protocol::QueryPushTaskPacketV3::Action, packet);
            }
            return 0;
        }

        int PushPack::UnPackQueryPushTask(int pversion, UdpBuffer& recv_buffer, boost::uint32_t transaction_id, 
            boost::uint8_t& error_code, std::vector<protocol::PushTaskItem>& push_task_vec)
        {
            int ret;
            if (pversion == 3) {
                protocol::QueryPushTaskPacketV3 packet;
                ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
                if (ret != 0)
                {
                    return ret;
                }
                packet.response.push_task_vec_.swap(push_task_vec);
            }else{
                protocol::QueryPushTaskPacketV2 packet;
                ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);
                if (ret != 0)
                {
                    return ret;
                }
                packet.response.push_task_vec_.swap(push_task_vec);
            }
            return 0;
        }

        std::string PushPack::GetErrMsg()
        {
            return err_msg_.str();
        }
    }
}