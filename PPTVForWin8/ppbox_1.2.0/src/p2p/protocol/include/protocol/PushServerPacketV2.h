//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _PROTOCOL_PUSH_SERVER_PACKETV2_H_
#define _PROTOCOL_PUSH_SERVER_PACKETV2_H_

#include "protocol/Packet.h"
#include "struct/serialization.h"
#include "struct/Base.h"
#include "struct/RidInfo.h"
#include "protocol/ServerPacket.h"

#include <util/serialization/stl/string.h>
#include <util/serialization/stl/vector.h>

namespace protocol
{
	struct PlayHistoryItem
	{
		PlayHistoryItem(const std::string& video_name, boost::uint32_t duration, boost::uint16_t segment_num)
			: video_name_(video_name), continuously_played_duration_(duration), downloaded_segment_num_(segment_num) {}

		PlayHistoryItem() {}

		std::string video_name_;
		//continuously played duration of this series video
		boost::uint32_t continuously_played_duration_;
		//downloaded video segment number since last play point, include the segment last play point lies in
		boost::uint16_t downloaded_segment_num_;
		
		bool operator == (const PlayHistoryItem& r) const 
		{
			return video_name_ == r.video_name_ && continuously_played_duration_ == r.continuously_played_duration_
				&& downloaded_segment_num_ == r.downloaded_segment_num_;
		}

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & util::serialization::make_sized<boost::uint16_t>(video_name_);
			ar & continuously_played_duration_;
			ar & downloaded_segment_num_;
		}
	};

	struct PushTaskItem
	{
		RidInfo rid_info_;
		std::string url_;
		std::string refer_url_;
		std::string channel_id_;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & rid_info_;
			ar & util::serialization::make_sized<boost::uint16_t>(url_);
			ar & util::serialization::make_sized<boost::uint16_t>(refer_url_);
			ar & util::serialization::make_sized<boost::uint16_t>(channel_id_);
		}
	};

    struct QueryPushTaskPacketV2
        : public ServerPacketT<0x1C>
    {
        enum error_code
        {
            NO_TASK = 1,
        };

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) 
            {
                ar & request.peer_guid_;
                ar & request.used_disk_size_;
                ar & request.upload_bandwidth_kbs_;
                ar & request.avg_upload_speed_kbs_;
				ar & request.play_history_vec_;
            }
            else 
            {
                if(error_code_ == 0)
                {
					ar & response.push_task_vec_;
                    ar & response.response_push_task_param_;
                }
                else if (error_code_ == NO_TASK)
                {
                    ar & response.push_wait_interval_in_sec_;
                }
            }
        }

        QueryPushTaskPacketV2() {}

        // request
        QueryPushTaskPacketV2(boost::uint32_t transaction_id, const Guid& peer_guid, boost::asio::ip::udp::endpoint endpoint_, 
			boost::uint32_t used_disk_size, boost::uint32_t upload_bandwidth_kbs, boost::uint32_t avg_upload_speed_kbs)
        {
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;

            request.used_disk_size_ = used_disk_size;
            request.upload_bandwidth_kbs_ = upload_bandwidth_kbs;
            request.avg_upload_speed_kbs_ = avg_upload_speed_kbs;
            IsRequest = 1;
        }

        // response
        QueryPushTaskPacketV2(boost::uint32_t transaction_id, const PUSH_TASK_PARAM& push_param, boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            error_code_ = 0;
            IsRequest = 0;

            response.response_push_task_param_ = push_param;       
        }

        QueryPushTaskPacketV2(boost::uint32_t transaction_id, boost::uint8_t errorcode, 
            boost::int32_t push_wait_interval_in_sec, boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            error_code_ = errorcode;
            IsRequest = 0;

            response.push_wait_interval_in_sec_ = push_wait_interval_in_sec;
        }

        struct Request {
            Guid peer_guid_;
            boost::uint32_t used_disk_size_;
            boost::uint32_t upload_bandwidth_kbs_;
            boost::uint32_t avg_upload_speed_kbs_;
			std::vector<PlayHistoryItem> play_history_vec_;
        } request;

        struct Response {
			std::vector<PushTaskItem> push_task_vec_;
            PUSH_TASK_PARAM response_push_task_param_;
            boost::int32_t push_wait_interval_in_sec_;
        } response;
    };


    template <typename PacketHandler>
    void register_push_packetv2(
        PacketHandler & handler)
    {
        handler.template register_packet<QueryPushTaskPacketV2>();
    }

    struct QueryPushTaskPacketV3
        : public ServerPacketT<0x1D>
    {
        enum error_code
        {
            NO_TASK = 1,
        };

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) 
            {
                ar & request.peer_guid_;
                ar & request.used_disk_size_;
                ar & request.upload_bandwidth_kbs_;
                ar & request.avg_upload_speed_kbs_;
                ar & request.total_disk_size_;
                ar & request.online_percent_;
                ar & request.nat_type_;
				ar & request.play_history_vec_;
            }
            else 
            {
                if(error_code_ == 0)
                {
					ar & response.push_task_vec_;
                    ar & response.response_push_task_param_;
                }
                else if (error_code_ == NO_TASK)
                {
                    ar & response.push_wait_interval_in_sec_;
                }
            }
        }

        QueryPushTaskPacketV3() {}

        // request
        QueryPushTaskPacketV3(boost::uint32_t transaction_id, const Guid& peer_guid, boost::asio::ip::udp::endpoint 
            endpoint_, boost::uint32_t used_disk_size, boost::uint32_t upload_bandwidth_kbs, boost::uint32_t avg_upload_speed_kbs, 
            boost::uint32_t total_disk_size, boost::uint32_t online_percent, boost::uint32_t nat_type)
        {
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;

            request.used_disk_size_ = used_disk_size;
            request.upload_bandwidth_kbs_ = upload_bandwidth_kbs;
            request.avg_upload_speed_kbs_ = avg_upload_speed_kbs;
            request.total_disk_size_ = total_disk_size;
            request.online_percent_ = online_percent;
            request.nat_type_ = nat_type;
            IsRequest = 1;
        }

        // response
        QueryPushTaskPacketV3(boost::uint32_t transaction_id, const PUSH_TASK_PARAM& push_param, boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            error_code_ = 0;
            IsRequest = 0;

            response.response_push_task_param_ = push_param;  
        }

        QueryPushTaskPacketV3(boost::uint32_t transaction_id, boost::uint8_t errorcode, 
            boost::int32_t push_wait_interval_in_sec, boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            error_code_ = errorcode;
            IsRequest = 0;

            response.push_wait_interval_in_sec_ = push_wait_interval_in_sec;
        }

        struct Request {
            Guid peer_guid_;
            boost::uint32_t used_disk_size_;
            boost::uint32_t upload_bandwidth_kbs_;
            boost::uint32_t avg_upload_speed_kbs_;
            boost::uint32_t total_disk_size_;
            boost::uint32_t online_percent_;
            boost::uint32_t nat_type_;
			std::vector<PlayHistoryItem> play_history_vec_;
        } request;

        struct Response {
			std::vector<PushTaskItem> push_task_vec_;
            PUSH_TASK_PARAM response_push_task_param_;
            boost::int32_t push_wait_interval_in_sec_;
        } response;
    };

    template <typename PacketHandler>
    void register_push_packetv3(
        PacketHandler & handler)
    {
        handler.template register_packet<QueryPushTaskPacketV3>();
    }
}

#endif  // _PROTOCOL_PUSH_SERVER_PACKET_H_
