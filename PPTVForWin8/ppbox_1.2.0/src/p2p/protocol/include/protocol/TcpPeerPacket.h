#ifndef TCP_PEER_PACKET_H
#define TCP_PEER_PACKET_H

#include "boost/shared_ptr.hpp"

namespace network
{
    class TcpConnection;
}

namespace protocol
{
    struct TcpCommonPacket
        : Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);

            ar & protocol_version_;
        }

        TcpCommonPacket()
        {
            protocol_version_ = PEER_VERSION;
        }

        boost::uint16_t protocol_version_;
        boost::shared_ptr<network::TcpConnection> tcp_connection_;
    };

    struct TcpAnnounceRequestPacket
        : PacketT<0xB0>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & resource_id_;
        }

        TcpAnnounceRequestPacket()
        {

        }

        TcpAnnounceRequestPacket(const RID & rid)
            : resource_id_(rid)
        {

        }

        RID resource_id_;
    };

    struct TcpAnnounceResponsePacket
        : PacketT<0xB1>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & resource_id_;
            ar & peer_download_info_;
            ar & reserve1_;
            ar & reserve2_;
            ar & reserve3_;
            ar & block_map_;
        }

        TcpAnnounceResponsePacket()
        {

        }

        TcpAnnounceResponsePacket(
            boost::uint32_t transaction_id,
            const RID & rid,
            const PEER_DOWNLOAD_INFO & peer_download_info,
            const BlockMap & block_map)
            : resource_id_(rid)
            , peer_download_info_(peer_download_info)
            , block_map_(block_map)
        {
            transaction_id_ = transaction_id;
        }

        RID resource_id_;
        PEER_DOWNLOAD_INFO peer_download_info_;
        boost::uint8_t reserve1_;
        boost::uint8_t reserve2_;
        boost::uint8_t reserve3_;
        BlockMap block_map_;
    };

    struct TcpSubPieceRequestPacket
        : PacketT<0xB2>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & resource_id_;
            ar & util::serialization::make_sized<boost::uint16_t>(subpiece_infos_);
            ar & priority_;
        }

        TcpSubPieceRequestPacket()
        {

        }

        TcpSubPieceRequestPacket(
            const RID & rid, 
            const std::vector<SubPieceInfo> & subpiece_infos,
            const boost::uint16_t priority)
            : resource_id_(rid)
            , subpiece_infos_(subpiece_infos)
            , priority_(priority)
        {

        }

        RID resource_id_;
        std::vector<SubPieceInfo> subpiece_infos_;
        boost::uint16_t priority_;
    };

    struct TcpSubPieceResponsePacket
        : PacketT<0xB3>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & resource_id_;
            ar & sub_piece_info_;
            ar & sub_piece_length_;

            for (size_t i = 0 ; i < sub_piece_length_ ; i++)
            {
                ar & *(sub_piece_content_->get_buffer() + i);
            }
        }

        TcpSubPieceResponsePacket()
        {

        }

        TcpSubPieceResponsePacket(
            boost::uint32_t transaction_id,
            const RID & rid,
            const SubPieceInfo & sub_piece_info,
            const SubPieceBuffer & sub_piece_buffer)
            : resource_id_(rid)
            , sub_piece_info_(sub_piece_info)
            , sub_piece_length_(sub_piece_buffer.Length())
            , sub_piece_content_(sub_piece_buffer.GetSubPieceBuffer())
        {
            transaction_id_ = transaction_id;
        }

        RID resource_id_;
        SubPieceInfo sub_piece_info_;
        boost::uint16_t sub_piece_length_;
        mutable SubPieceContent::pointer sub_piece_content_;
    };

    struct TcpReportSpeedPacket
        : PacketT<0xB4>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & speed_;
        }

        TcpReportSpeedPacket()
        {

        }

        boost::uint32_t speed_;
    };

    struct TcpErrorPacket
        : PacketT<0xB5>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & error_code_;
        }

        TcpErrorPacket()
        {

        }


        TcpErrorPacket(
            const boost::uint32_t transaction_id,
            const boost::uint16_t error_code_)
            : error_code_(error_code_)
        {
            transaction_id_ = transaction_id;
        }

        boost::uint16_t error_code_;
    };

    struct TcpClosePacket
        : PacketT<0xB6>
        , TcpCommonPacket
    {
        TcpClosePacket()
        {
            PacketAction = 0xB6;
        }
    };

    // herain:这个报文比较特殊，不是远端播放器发送给内核的，而是本地播放器有插播放时发送给内核的
    struct TcpReportStatusPacket
        : PacketT<0xD0>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & resource_id_;
            ar & rest_play_time_in_seconds_;
        }

        TcpReportStatusPacket()
        {

        }

        RID resource_id_;
        boost::uint16_t rest_play_time_in_seconds_;
    };

    struct TcpStartDownloadPacket
        : PacketT<0xD6>
        , TcpCommonPacket
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & util::serialization::make_sized<boost::uint16_t>(download_url_);
            ar & util::serialization::make_sized<boost::uint16_t>(refer_url_);
            ar & util::serialization::make_sized<boost::uint16_t>(user_agent_);
            ar & util::serialization::make_sized<boost::uint16_t>(filename_);
        }

        TcpStartDownloadPacket()
        {

        }

        string download_url_;
        string refer_url_;
        string user_agent_;
        string filename_;
    };

    struct TcpStopDownLoadPacket
        : TcpCommonPacket
        , PacketT<0xD7>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            TcpCommonPacket::serialize(ar);

            ar & util::serialization::make_sized<boost::uint16_t>(stop_url_);
        }

        TcpStopDownLoadPacket()
        {

        }

        string stop_url_;
    };
}

#endif
