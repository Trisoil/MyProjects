#pragma once

#include "protocal/Packet.h"
#include "protocal/structs.h"

namespace protocal
{

    //////////////////////////////////////////////////////////////////////////
    // StatisticRequestPacket 
    class StatisticRequestPacket : public RequestServerPacket
    {
    public:
        enum PEER_ACTION
        {
            PEER_ONLINE                 = 0x11,
            PEER_QUERY_URL              = 0x12,
            PEER_QUERY_CONTENT          = 0x13,
            PEER_QUERY_URL_SUCCESS      = 0x21,
            PEER_QUERY_CONTENT_SUCCESS  = 0x22,
            PEER_ADD_RESOURCE           = 0x23,
        };
    public:
        typedef boost::shared_ptr<StatisticRequestPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static const size_t HEADER_LENGTH = RequestServerPacket::HEADER_LENGTH + sizeof(STATISTIC_HEADER);
    public:
        UINT64 GetTimeStamp() const { assert(statistic_packet_header_); return statistic_packet_header_->TimeStamp; }
        Guid GetPeerGuid() const { assert(statistic_packet_header_); return statistic_packet_header_->PeerGuid; }
        UINT8 GetPeerAction() const { assert(statistic_packet_header_); return statistic_packet_header_->PeerAction; }
    private:
        STATISTIC_HEADER* statistic_packet_header_;
    protected:
        static bool Check(Buffer buffer);
    protected:
        StatisticRequestPacket(Buffer buffer);
        StatisticRequestPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action);
    };

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexPeerPacket
    class StatisticIndexPeerPacket : public StatisticRequestPacket
    {
    public:
        typedef boost::shared_ptr<StatisticIndexPeerPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action);
    public:
        static const UINT8 ACTION = 0xD1;
    private:
        static bool Check(Buffer buffer);
    private:
        StatisticIndexPeerPacket(Buffer buffer);
        StatisticIndexPeerPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action);
    };

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexUrlPacket
    class StatisticIndexUrlPacket : public StatisticRequestPacket
    {
    public:
        typedef boost::shared_ptr<StatisticIndexUrlPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& url);
    public:
        static const UINT8 ACTION = 0xD2;
    public:
        string GetUrl() const { assert(statistic_index_url_); return string((char*)statistic_index_url_->UrlString, statistic_index_url_->UrlLength); }
    private:
        STATISTIC_INDEX_URL* statistic_index_url_;
        static bool Check(Buffer buffer);
    private:
        StatisticIndexUrlPacket(Buffer buffer);
        StatisticIndexUrlPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& url);
    };

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexContentPacket
    class StatisticIndexContentPacket : public StatisticRequestPacket
    {
    public:
        typedef boost::shared_ptr<StatisticIndexContentPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid content_id);
    public:
        static const UINT8 ACTION = 0xD3;
    public:
        Guid GetContentID() const { assert(statistic_index_content_); return statistic_index_content_->ContentID; }
    private:
        STATISTIC_INDEX_CONTENT* statistic_index_content_;
        static bool Check(Buffer buffer);
    private:
        StatisticIndexContentPacket(Buffer buffer);
        StatisticIndexContentPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid content_id);
    };

    //////////////////////////////////////////////////////////////////////////
    // StatisticIndexResourcePacket
    class StatisticIndexResourcePacket : public StatisticRequestPacket
    {
    public:
        typedef boost::shared_ptr<StatisticIndexResourcePacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid resource_id);
    public:
        static const UINT8 ACTION = 0xD4;
    public:
        Guid GetResourceID() const { assert(statistic_index_resource_); return statistic_index_resource_->ResourceID; }
    private:
        STATISTIC_INDEX_RESOURCE* statistic_index_resource_;
        static bool Check(Buffer buffer);
    private:
        StatisticIndexResourcePacket(Buffer buffer);
        StatisticIndexResourcePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, Guid resource_id);
    };

    //////////////////////////////////////////////////////////////////////////
    // StatisticStringPacket
    class StatisticStringPacket : public StatisticRequestPacket
    {
    public:
        typedef boost::shared_ptr<StatisticStringPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& data);
    public:
        static const UINT8 ACTION = 0xD5;
    public:
        string GetString() const { assert(string_); return string_ == NULL ? string() : string_->AsString(); }
    private:
        STRING* string_;
        static bool Check(Buffer buffer);
    private:
        StatisticStringPacket(Buffer buffer);
        StatisticStringPacket(UINT32 transaction_id, UINT32 peer_version, UINT64 time_stamp, Guid peer_guid, PEER_ACTION peer_action, const string& data);
    };

}
