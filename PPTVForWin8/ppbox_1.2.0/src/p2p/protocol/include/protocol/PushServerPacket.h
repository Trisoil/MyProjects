//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _PROTOCOL_PUSH_SERVER_PACKET_H_
#define _PROTOCOL_PUSH_SERVER_PACKET_H_

#include "protocol/Packet.h"
#include "struct/serialization.h"
#include "struct/Base.h"
#include "struct/RidInfo.h"
#include "protocol/ServerPacket.h"

#include <util/serialization/stl/string.h>
#include <util/serialization/stl/vector.h>

namespace protocol
{
    struct QueryPushTaskPacket
        : public ServerPacketT<0x1A>
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
            }
            else 
            {
                if(error_code_ == 0)
                {
                    ar & response.rid_info_;
                    ar & util::serialization::make_sized<boost::uint16_t>(response.response_url_);
                    ar & util::serialization::make_sized<boost::uint16_t>(response.response_refer_url_);
                    ar & response.response_push_task_param_;
                }
                else if (error_code_ == NO_TASK)
                {
                    ar & response.push_wait_interval_in_sec_;
                }
            }
        }

        // herain:默认构造函数是为UdpServer反序列时使用
        QueryPushTaskPacket() {}

        // request
        QueryPushTaskPacket(boost::uint32_t transaction_id, const Guid& peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_, 
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
        QueryPushTaskPacket(boost::uint32_t transaction_id, const RidInfo& rid_info, const std::string& url,
            const std::string& refer_url,
            const PUSH_TASK_PARAM& push_param, boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            error_code_ = 0;
            IsRequest = 0;

            response.rid_info_ = rid_info;
            response.response_url_ = url;
            response.response_refer_url_ = refer_url;
            response.response_push_task_param_ = push_param;       
        }

        QueryPushTaskPacket(boost::uint32_t transaction_id, boost::uint8_t errorcode, 
            boost::int32_t push_wait_interval_in_sec, const boost::asio::ip::udp::endpoint & endpoint_)
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
        } request;
        struct Response {
            RidInfo rid_info_;
            std::string response_url_;
            std::string response_refer_url_;
            PUSH_TASK_PARAM response_push_task_param_;
            boost::int32_t push_wait_interval_in_sec_;
        } response;
    };

    struct ReportPushTaskCompletedPacket
        : public ServerPacketT<0x1B>
    {
        ReportPushTaskCompletedPacket(){}

        ReportPushTaskCompletedPacket(boost::uint32_t transaction_id, const Guid & peer_guid, const RidInfo & rid_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            IsRequest = true;

            request.peer_guid_ = peer_guid;
            request.rid_info_ = rid_info;            
        }

        ReportPushTaskCompletedPacket(boost::uint32_t transaction_id, const RidInfo & rid_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;

            IsRequest = false;
            error_code_ = 0;

            response.rid_info_ = rid_info;            
        }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
                ar & request.peer_guid_;
                ar & request.rid_info_;
            }
            else
            {
                ar & response.rid_info_;
            }
        }

        struct Request{
            Guid peer_guid_;
            RidInfo rid_info_;
        } request;

        struct Response {
            RidInfo rid_info_;
        } response;
    };

    template <typename PacketHandler>
    void register_push_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<QueryPushTaskPacket>();
        handler.template register_packet<ReportPushTaskCompletedPacket>();
    }
}

#endif  // _PROTOCOL_PUSH_SERVER_PACKET_H_
