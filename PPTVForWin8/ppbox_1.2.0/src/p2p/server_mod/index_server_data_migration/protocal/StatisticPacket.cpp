#include "stdafx.h"
#include "protocal/StatisticPacket.h"

namespace protocal
{
    //////////////////////////////////////////////////////////////////////////
    // StatisticPacket
    bool StatisticRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(STATISTIC_HEADER))
            return false;
        return true;
    }

    StatisticRequestPacket::StatisticRequestPacket(framework::Buffer buffer)
        : RequestServerPacket(buffer)
        , statistic_packet_header_(NULL)
    {
        assert( StatisticRequestPacket::Check(buffer) );
        statistic_packet_header_ = (STATISTIC_HEADER*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }

    StatisticRequestPacket::StatisticRequestPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action)
        : RequestServerPacket(append_length + sizeof(STATISTIC_HEADER), action, transaction_id, peer_version)
    {
        statistic_packet_header_ = (STATISTIC_HEADER*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        statistic_packet_header_->TimeStamp = time_stamp;
        statistic_packet_header_->PeerGuid = peer_guid;
        statistic_packet_header_->PeerAction = peer_action;
    }

    StatisticRequestPacket::p StatisticRequestPacket::ParseFromBinary(Buffer buffer)
    {
        StatisticRequestPacket::p packet;
        if (StatisticRequestPacket::Check(buffer))
        {
            packet = StatisticRequestPacket::p(new StatisticRequestPacket(buffer));
        }
        return packet;
    }

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexPeerPacket
    bool StatisticIndexPeerPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ != StatisticRequestPacket::HEADER_LENGTH + 0)
            return false;
        return Packet::CheckAction(buffer, ACTION);
    }
    StatisticIndexPeerPacket::p StatisticIndexPeerPacket::ParseFromBinary(Buffer buffer)
    {
        StatisticIndexPeerPacket::p packet;
        if (StatisticIndexPeerPacket::Check(buffer))
        {
            packet = StatisticIndexPeerPacket::p(new StatisticIndexPeerPacket(buffer));
        }
        return packet;
    }
    StatisticIndexPeerPacket::p StatisticIndexPeerPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action)
    {
        return StatisticIndexPeerPacket::p(new StatisticIndexPeerPacket(transaction_id, peer_version, time_stamp, peer_guid, peer_action));
    }
    StatisticIndexPeerPacket::StatisticIndexPeerPacket(Buffer buffer)
        : StatisticRequestPacket(buffer)
    {
        assert(StatisticIndexPeerPacket::Check(buffer));
    }
    StatisticIndexPeerPacket::StatisticIndexPeerPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action)
        : StatisticRequestPacket(0, ACTION, transaction_id, peer_version, time_stamp, peer_guid, peer_action)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexUrlPacket

    bool StatisticIndexUrlPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < StatisticRequestPacket::HEADER_LENGTH + sizeof(STATISTIC_INDEX_URL))
            return false;

        UINT16 url_length = *(UINT16*)(buffer.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        if (buffer.length_ != StatisticRequestPacket::HEADER_LENGTH + sizeof(STATISTIC_INDEX_URL) + url_length)
            return false;

        return Packet::CheckAction(buffer, ACTION);
    }
    StatisticIndexUrlPacket::p StatisticIndexUrlPacket::ParseFromBinary(Buffer buffer)
    {
        StatisticIndexUrlPacket::p packet;
        if (StatisticIndexUrlPacket::Check(buffer))
        {
            packet = StatisticIndexUrlPacket::p(new StatisticIndexUrlPacket(buffer));
        }
        return packet;
    }
    StatisticIndexUrlPacket::p StatisticIndexUrlPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& url)
    {
        return StatisticIndexUrlPacket::p(new StatisticIndexUrlPacket(transaction_id, peer_version, time_stamp, peer_guid, peer_action, url));
    }
    StatisticIndexUrlPacket::StatisticIndexUrlPacket(Buffer buffer)
        : StatisticRequestPacket(buffer)
        , statistic_index_url_(NULL)
    {
        assert(StatisticIndexUrlPacket::Check(buffer));
        statistic_index_url_ = (STATISTIC_INDEX_URL*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
    }
    StatisticIndexUrlPacket::StatisticIndexUrlPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& url)
        : StatisticRequestPacket(sizeof(STATISTIC_INDEX_URL) + url.length(), ACTION, transaction_id, peer_version, time_stamp, peer_guid, peer_action)
    {
        statistic_index_url_ = (STATISTIC_INDEX_URL*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        statistic_index_url_->UrlLength = url.length();
        memcpy(statistic_index_url_->UrlString, url.c_str(), url.length());
    }

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexContentPacket

    bool StatisticIndexContentPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < StatisticRequestPacket::HEADER_LENGTH + sizeof(STATISTIC_INDEX_CONTENT))
            return false;

        return Packet::CheckAction(buffer, ACTION);
    }
    StatisticIndexContentPacket::p StatisticIndexContentPacket::ParseFromBinary(Buffer buffer)
    {
        StatisticIndexContentPacket::p packet;
        if (StatisticIndexContentPacket::Check(buffer))
        {
            packet = StatisticIndexContentPacket::p(new StatisticIndexContentPacket(buffer));
        }
        return packet;
    }
    StatisticIndexContentPacket::p StatisticIndexContentPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid content_id)
    {
        return StatisticIndexContentPacket::p(new StatisticIndexContentPacket(transaction_id, peer_version, time_stamp, peer_guid, peer_action, content_id));
    }
    StatisticIndexContentPacket::StatisticIndexContentPacket(Buffer buffer)
        : StatisticRequestPacket(buffer)
        , statistic_index_content_(NULL)
    {
        assert(StatisticIndexContentPacket::Check(buffer));
        statistic_index_content_ = (STATISTIC_INDEX_CONTENT*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
    }
    StatisticIndexContentPacket::StatisticIndexContentPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid content_id)
        : StatisticRequestPacket(sizeof(STATISTIC_INDEX_CONTENT), ACTION, transaction_id, peer_version, time_stamp, peer_guid, peer_action)
    {
        statistic_index_content_ = (STATISTIC_INDEX_CONTENT*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        statistic_index_content_->ContentID = content_id;
    }


    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexContentPacket

    bool StatisticIndexResourcePacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < StatisticRequestPacket::HEADER_LENGTH + sizeof(STATISTIC_INDEX_RESOURCE))
            return false;

        return Packet::CheckAction(buffer, ACTION);
    }
    StatisticIndexResourcePacket::p StatisticIndexResourcePacket::ParseFromBinary(Buffer buffer)
    {
        StatisticIndexResourcePacket::p packet;
        if (StatisticIndexResourcePacket::Check(buffer))
        {
            packet = StatisticIndexResourcePacket::p(new StatisticIndexResourcePacket(buffer));
        }
        return packet;
    }
    StatisticIndexResourcePacket::p StatisticIndexResourcePacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid resource_id)
    {
        return StatisticIndexResourcePacket::p(new StatisticIndexResourcePacket(transaction_id, peer_version, time_stamp, peer_guid, peer_action, resource_id));
    }
    StatisticIndexResourcePacket::StatisticIndexResourcePacket(Buffer buffer)
        : StatisticRequestPacket(buffer)
        , statistic_index_resource_(NULL)
    {
        assert(StatisticIndexResourcePacket::Check(buffer));
        statistic_index_resource_ = (STATISTIC_INDEX_RESOURCE*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
    }
    StatisticIndexResourcePacket::StatisticIndexResourcePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid resource_id)
        : StatisticRequestPacket(sizeof(STATISTIC_INDEX_RESOURCE), ACTION, transaction_id, peer_version, time_stamp, peer_guid, peer_action)
    {
        statistic_index_resource_ = (STATISTIC_INDEX_RESOURCE*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        statistic_index_resource_->ResourceID = resource_id;
    }

    //////////////////////////////////////////////////////////////////////////
    // StatisticStringPacket

    bool StatisticStringPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < StatisticRequestPacket::HEADER_LENGTH + sizeof(STRING))
            return false;

        UINT16 str_length = *(UINT16*)(buffer.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        if (buffer.length_ != StatisticRequestPacket::HEADER_LENGTH + sizeof(STRING) + str_length)
            return false;

        return Packet::CheckAction(buffer, ACTION);
    }
    StatisticStringPacket::p StatisticStringPacket::ParseFromBinary(Buffer buffer)
    {
        StatisticStringPacket::p packet;
        if (StatisticStringPacket::Check(buffer))
        {
            packet = StatisticStringPacket::p(new StatisticStringPacket(buffer));
        }
        return packet;
    }
    StatisticStringPacket::p StatisticStringPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& data)
    {
        StatisticStringPacket::p packet;
        if (data.length() <= 0xFFFFu) {
            packet = StatisticStringPacket::p(new StatisticStringPacket(transaction_id, peer_version, time_stamp, peer_guid, peer_action, data));
        }
        return packet;
    }
    StatisticStringPacket::StatisticStringPacket(Buffer buffer)
        : StatisticRequestPacket(buffer)
        , string_(NULL)
    {
        assert(StatisticStringPacket::Check(buffer));
        string_ = (STRING*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
    }
    StatisticStringPacket::StatisticStringPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& data)
        : StatisticRequestPacket(sizeof(STRING) + data.length(), ACTION, transaction_id, peer_version, time_stamp, peer_guid, peer_action)
    {
        string_ = (STRING*)(buffer_.data_.get() + StatisticRequestPacket::HEADER_LENGTH);
        string_->FromString(data);
    }

}
