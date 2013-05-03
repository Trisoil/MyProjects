#include "stdafx.h"
#include "protocal/DataCollectionPacket.h"

namespace protocal
{
    //////////////////////////////////////////////////////////////////////////
    // Data Collection Request Packet
    DataCollectionRequestPacket::p DataCollectionRequestPacket::ParseFromBinary(Buffer buffer)
    {
        DataCollectionRequestPacket::p packet;
        if (DataCollectionRequestPacket::Check(buffer))
        {
            packet = DataCollectionRequestPacket::p(new DataCollectionRequestPacket(buffer));
        }
        return packet;
    }
    bool DataCollectionRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ < DataCollectionRequestPacket::HEADER_LENGTH)
            return false;
        return true;
    }
    VERSION_INFO DataCollectionRequestPacket::GetKernelVersion(Buffer buffer)
    {
        DataCollectionRequestPacket::p packet = DataCollectionRequestPacket::ParseFromBinary(buffer);
        if (packet) return packet->GetKernelVersion();
        return VERSION_INFO(0, 0, 0, 0);
    }
    DataCollectionRequestPacket::DataCollectionRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , datacollection_header_(NULL)
    {
        assert(DataCollectionRequestPacket::Check(buffer));
        datacollection_header_ = (REQUEST_DATACOLLECTION_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    DataCollectionRequestPacket::DataCollectionRequestPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version, VERSION_INFO kernel_version, Guid peer_guid)
        : RequestServerPacket(append_length + sizeof(REQUEST_DATACOLLECTION_PACKET), action, transaction_id, peer_version)
        , datacollection_header_(NULL)
    {
        datacollection_header_ = (REQUEST_DATACOLLECTION_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        datacollection_header_->KernelVersion = kernel_version;
        datacollection_header_->PeerGuid = peer_guid;
    }

    //////////////////////////////////////////////////////////////////////////
    // 
    DataOnAppStopRequestPacket::p DataOnAppStopRequestPacket::ParseFromBinary(Buffer buffer)
    {
        DataOnAppStopRequestPacket::p packet;
        if (DataOnAppStopRequestPacket::Check(buffer))
        {
            packet = DataOnAppStopRequestPacket::p(new DataOnAppStopRequestPacket(buffer));
        }
        return packet;
    }
    DataOnAppStopRequestPacket::p DataOnAppStopRequestPacket::CreatePacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONAPPSTOP_INFO& data_onappstop_info, const string& flv_folder_path, const PLATFORM_INFO& platform_info, const ACTION_COUNT_INFO& action_count_info)
    {
        return DataOnAppStopRequestPacket::p(new DataOnAppStopRequestPacket(transaction_id, kernel_version, peer_guid, data_onappstop_info, flv_folder_path, platform_info, action_count_info));
    }
    bool DataOnAppStopRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) return false;
        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;
        if (false == Packet::CheckAction(buffer, DataOnAppStopRequestPacket::ACTION))
            return false;
        offset += sizeof(DATA_ONAPPSTOP_INFO);
        if (buffer.length_ < offset + sizeof(STRING))
            return false;
        STRING * s = (STRING*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(STRING) + s->Length)
            return false;
        offset += sizeof(STRING) + s->Length;

        // PLATFORM_INFO
        UINT32 peer_version = RequestServerPacket::ParseFromBinary(buffer)->GetPeerVersion();
        if (peer_version >= 0x00000008)
        {
            if (buffer.length_ < offset + sizeof(PLATFORM_INFO))
                return false;
            offset += sizeof(PLATFORM_INFO);
        }

        // ACTION_COUNT_INFO
        if (peer_version >= 0x00000009)
        {
            if (buffer.length_ < offset + sizeof(ACTION_COUNT_INFO))
                return false;
            offset += sizeof(ACTION_COUNT_INFO);
        }

        return true;
    }
    DataOnAppStopRequestPacket::DataOnAppStopRequestPacket(Buffer buffer)
        : DataCollectionRequestPacket(buffer)
        , data_onappstop_info_(NULL)
        , flv_folder_path_(NULL)
        , platform_info_(NULL)
        , action_count_info_(NULL)
    {
        assert(DataOnAppStopRequestPacket::Check(buffer));

        // DATA
        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;
        data_onappstop_info_ = (DATA_ONAPPSTOP_INFO*)(buffer_.data_.get() + offset);
        offset += sizeof(DATA_ONAPPSTOP_INFO);

        // flv folder path
        flv_folder_path_ = (STRING*)(buffer_.data_.get() + offset);
        offset += sizeof(STRING) + flv_folder_path_->Length;

        // platform info
        if (GetPeerVersion() >= 0x00000008)
        {
            platform_info_ = (PLATFORM_INFO*)(buffer_.data_.get() + offset);
            offset += sizeof(PLATFORM_INFO);
        }

        // action count info
        if (GetPeerVersion() >= 0x00000009)
        {
            action_count_info_ = (ACTION_COUNT_INFO*)(buffer_.data_.get() + offset);
            offset += sizeof(ACTION_COUNT_INFO);
        }
    }
    DataOnAppStopRequestPacket::DataOnAppStopRequestPacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONAPPSTOP_INFO& data_onappstop_info, const string& flv_folder_path, const PLATFORM_INFO& platform_info, const ACTION_COUNT_INFO& action_count_info)
        : DataCollectionRequestPacket(sizeof(DATA_ONAPPSTOP_INFO) + sizeof(STRING) + flv_folder_path.length() + sizeof(PLATFORM_INFO) + sizeof(ACTION_COUNT_INFO), DataOnAppStopRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION, kernel_version, peer_guid)
        , data_onappstop_info_(NULL)
        , flv_folder_path_(NULL)
        , platform_info_(NULL)
        , action_count_info_(NULL)
    {
        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;

        // DATA_ONAPPSTOP_INFO
        data_onappstop_info_ = (DATA_ONAPPSTOP_INFO*)(buffer_.data_.get() + offset);
        *data_onappstop_info_ = data_onappstop_info;
        offset += sizeof(DATA_ONAPPSTOP_INFO);

        // FLV
        flv_folder_path_ = (STRING*)(buffer_.data_.get() + offset);
        flv_folder_path_->Length = flv_folder_path.length();
        memcpy(flv_folder_path_->StringData, flv_folder_path.c_str(), flv_folder_path.length());
        offset += sizeof(STRING) + flv_folder_path.length();

        // PLATFORM_INFO
        platform_info_ = (PLATFORM_INFO*)(buffer_.data_.get() + offset);
        *platform_info_ = platform_info;
        offset += sizeof(PLATFORM_INFO);

        // ACTION_COUNT_INFO
        action_count_info_ = (ACTION_COUNT_INFO*)(buffer_.data_.get() + offset);
        *action_count_info_ = action_count_info;
        offset += sizeof(ACTION_COUNT_INFO);
    }

    map<UINT32, UINT32> DataOnAppStopRequestPacket::GetActionCountInfoMap() const
    {
        ACTION_COUNT_INFO action_info = GetActionCountInfo();
        map<UINT32, UINT32> action_info_map;
        for (UINT32 i = 0; i < ACTION_NUMBER; ++i)
        {
            if (action_info.ActionCount[2*i] == 0xFFFFFFFFu)
                break;
            action_info_map[ action_info.ActionCount[2*i] ] = action_info.ActionCount[2*i+1];
        }
        return action_info_map;
    }

    //////////////////////////////////////////////////////////////////////////
    // 
    DataOnDownloadStopRequestPacket::p DataOnDownloadStopRequestPacket::ParseFromBinary(Buffer buffer)
    {
        DataOnDownloadStopRequestPacket::p packet;
        if (DataOnDownloadStopRequestPacket::Check(buffer))
        {
            packet = DataOnDownloadStopRequestPacket::p(new DataOnDownloadStopRequestPacket(buffer));
        }
        return packet;
    }
    DataOnDownloadStopRequestPacket::p DataOnDownloadStopRequestPacket::CreatePacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO& data_ondownloadstop_info, const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos)
    {
        UINT32 append_length = sizeof(STRING) + sizeof(STRING) + flv_url.length() + flv_refer_url.length() + sizeof(DATA_ONDOWNLOADSTOP_INFO) + sizeof(PLAY_STOP_INFO_ARRAY) + play_stop_infos.size() * sizeof(PLAY_STOP_INFO);
        return DataOnDownloadStopRequestPacket::p(new DataOnDownloadStopRequestPacket(append_length, transaction_id, kernel_version, peer_guid, data_ondownloadstop_info, flv_url, flv_refer_url, play_stop_infos));
    }
    bool DataOnDownloadStopRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) return false;
        UINT32 offset = DataOnDownloadStopRequestPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;
        if (false == Packet::CheckAction(buffer, DataOnDownloadStopRequestPacket::ACTION))
            return false;
        if (buffer.length_ < offset + sizeof(UINT16))
            return false;

        // url
        STRING* s = (STRING*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(STRING) + s->Length)
            return false;
        offset += sizeof(STRING) + s->Length;

        // refer url
        s = (STRING*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(STRING) + s->Length)
            return false;
        offset += sizeof(STRING) + s->Length;

        // data on download stop info
        if (buffer.length_ < offset + sizeof(DATA_ONDOWNLOADSTOP_INFO))
            return false;
        offset += sizeof(DATA_ONDOWNLOADSTOP_INFO);

        // play stop info
        PLAY_STOP_INFO_ARRAY* arr = (PLAY_STOP_INFO_ARRAY*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(PLAY_STOP_INFO_ARRAY))
            return false;
        offset += sizeof(PLAY_STOP_INFO_ARRAY);
        if (buffer.length_ < offset + arr->PlayStopCount * sizeof(PLAY_STOP_INFO))
            return false;

        return true;
    }
    DataOnDownloadStopRequestPacket::DataOnDownloadStopRequestPacket(Buffer buffer)
        : DataCollectionRequestPacket(buffer)
        , flv_url_(NULL)
        , flv_refer_url_(NULL)
        , data_ondownloadstop_info_(NULL)
        , play_stop_info_array_(NULL)
    {
        assert(DataOnDownloadStopRequestPacket::Check(buffer));

        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;
        // url
        flv_url_ = (STRING*)(buffer_.data_.get() + offset);
        offset += sizeof(STRING) + flv_url_->Length;
        // refer url
        flv_refer_url_ = (STRING*)(buffer_.data_.get() + offset);
        offset += sizeof(STRING) + flv_refer_url_->Length;
        // downloadstop info
        data_ondownloadstop_info_ = (DATA_ONDOWNLOADSTOP_INFO*)(buffer_.data_.get() + offset);
        offset += sizeof(DATA_ONDOWNLOADSTOP_INFO);
        // play stop info
        play_stop_info_array_ = (PLAY_STOP_INFO_ARRAY*)(buffer_.data_.get() + offset);
    }
    DataOnDownloadStopRequestPacket::DataOnDownloadStopRequestPacket(UINT32 append_length, UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO& data_ondownloadstop_info, const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos)
        : DataCollectionRequestPacket(append_length, DataOnDownloadStopRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION, kernel_version, peer_guid)
        , flv_url_(NULL)
        , flv_refer_url_(NULL)
        , data_ondownloadstop_info_(NULL)
        , play_stop_info_array_(NULL)
    {
        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;

        // flv url
        flv_url_ = (STRING*)(buffer_.data_.get() + offset);
        flv_url_->Length = flv_url.length();
        memcpy(flv_url_->StringData, flv_url.c_str(), flv_url.length());
        offset += sizeof(STRING) + flv_url_->Length;

        // refer url
        flv_refer_url_ = (STRING*)(buffer_.data_.get() + offset);
        flv_refer_url_->Length = flv_refer_url.length();
        memcpy(flv_refer_url_->StringData, flv_refer_url.c_str(), flv_refer_url.length());
        offset += sizeof(STRING) + flv_refer_url_->Length;

        // downloadstop info
        data_ondownloadstop_info_ = (DATA_ONDOWNLOADSTOP_INFO*)(buffer_.data_.get() + offset);
        *data_ondownloadstop_info_ = data_ondownloadstop_info;
        offset += sizeof(DATA_ONDOWNLOADSTOP_INFO);

        // play stop info
        play_stop_info_array_ = (PLAY_STOP_INFO_ARRAY*)(buffer_.data_.get() + offset);
        assert(play_stop_infos.size() < 0xFFFF);
        play_stop_info_array_->PlayStopCount = play_stop_infos.size();
        for (UINT8 i = 0; i < play_stop_infos.size(); ++i)
        {
            play_stop_info_array_->PlayStopInfos[i] = play_stop_infos[i];
        }
    }

    //////////////////////////////////////////////////////////////////////////

    DataOnDownloadStopExRequestPacket::p DataOnDownloadStopExRequestPacket::ParseFromBinary(Buffer buffer)
    {
        DataOnDownloadStopExRequestPacket::p packet;
        if (DataOnDownloadStopExRequestPacket::Check(buffer))
        {
            packet = DataOnDownloadStopExRequestPacket::p(new DataOnDownloadStopExRequestPacket(buffer));
        }
        return packet;
    }

    DataOnDownloadStopExRequestPacket::p DataOnDownloadStopExRequestPacket::CreatePacket(
        UINT32 transaction_id, VERSION_INFO kernel_version, 
        Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO_EX& data_ondownloadstop_info_ex, 
        const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos)
    {
        // append length
        UINT32 append_length = 0;
        // flv_url
        append_length += sizeof(STRING) + flv_url.length();
        // flv_refer_url
        append_length += sizeof(STRING) + flv_refer_url.length();
        // data_item_array
        append_length += sizeof(DATA_ITEM_ARRAY) + sizeof(DATA_ITEM) * DATA_TYPE::DATA_TYPE_COUNT + sizeof(DATA_ONDOWNLOADSTOP_INFO_EX);
        // play_stop_infos
        append_length += sizeof(PLAY_STOP_INFO_ARRAY) + play_stop_infos.size() * sizeof(PLAY_STOP_INFO);

        // create
        return DataOnDownloadStopExRequestPacket::p(
            new DataOnDownloadStopExRequestPacket(append_length, transaction_id, kernel_version, 
                peer_guid, data_ondownloadstop_info_ex, flv_url, flv_refer_url, play_stop_infos
            )
        );
    }

    bool DataOnDownloadStopExRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;

        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;
        if (false == Packet::CheckAction(buffer, DataOnDownloadStopExRequestPacket::ACTION))
            return false;

        // flv_url
        if (buffer.length_ < offset + sizeof(STRING))
            return false;
        STRING* url = (STRING*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(STRING) + url->Length)
            return false;
        offset += sizeof(STRING) + url->Length;

        // flv_refer_url
        if (buffer.length_ < offset + sizeof(STRING))
            return false;
        STRING* refer_url = (STRING*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(STRING) + refer_url->Length)
            return false;
        offset += sizeof(STRING) + refer_url->Length;

        // data_item_array
        if (buffer.length_ < offset + sizeof(DATA_ITEM_ARRAY))
            return false;
        DATA_ITEM_ARRAY* data_item_array = (DATA_ITEM_ARRAY*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(DATA_ITEM_ARRAY) + data_item_array->ITEMS_SIZE)
            return false;
        offset += sizeof(DATA_ITEM_ARRAY);
        UINT32 data_item_array_offset = offset + data_item_array->ITEMS_SIZE;
        for (UINT8 i = 0; i < data_item_array->ITEMS_COUNT; ++i)
        {
            if (data_item_array_offset < offset + sizeof(DATA_ITEM))
                return false;
            DATA_ITEM* data_item = (DATA_ITEM*)(buffer.data_.get() + offset);
            if (data_item->ITEM_TYPE >= DATA_TYPE::DATA_TYPE_COUNT)
                return false;
            if (data_item_array_offset < offset + sizeof(DATA_ITEM) + data_item->ITEM_SIZE)
                return false;
            offset += sizeof(DATA_ITEM) + data_item->ITEM_SIZE;
        }
        if (data_item_array_offset != offset)
            return false;

        // play_stop_info_array
        PLAY_STOP_INFO_ARRAY* arr = (PLAY_STOP_INFO_ARRAY*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(PLAY_STOP_INFO_ARRAY))
            return false;
        offset += sizeof(PLAY_STOP_INFO_ARRAY);
        if (buffer.length_ < offset + arr->PlayStopCount * sizeof(PLAY_STOP_INFO))
            return false;

        return true;
    }

    DataOnDownloadStopExRequestPacket::DataOnDownloadStopExRequestPacket(Buffer buffer)
        : DataCollectionRequestPacket(buffer)
        , flv_url_(NULL)
        , flv_refer_url_(NULL)
        , data_item_array_(NULL)
        , play_stop_info_array_(NULL)
    {
        assert(DataOnDownloadStopExRequestPacket::Check(buffer));

        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;

        // flv_url
        flv_url_ = (STRING*)(buffer_.data_.get() + offset);
        offset += sizeof(STRING) + flv_url_->Length;

        // flv_refer_url
        flv_refer_url_ = (STRING*)(buffer_.data_.get() + offset);
        offset += sizeof(STRING) + flv_refer_url_->Length;

        // data_item_array
        data_item_array_ = (DATA_ITEM_ARRAY*)(buffer_.data_.get() + offset);
        offset += sizeof(DATA_ITEM_ARRAY) + data_item_array_->ITEMS_SIZE;

        // play_stop_info_array
        play_stop_info_array_ = (PLAY_STOP_INFO_ARRAY*)(buffer_.data_.get() + offset);
    }

    DataOnDownloadStopExRequestPacket::DataOnDownloadStopExRequestPacket(
        UINT32 append_length, UINT32 transaction_id, 
        VERSION_INFO kernel_version, Guid peer_guid, 
        const DATA_ONDOWNLOADSTOP_INFO_EX& data_ondownloadstop_info_ex, 
        const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos )
        : DataCollectionRequestPacket(append_length, DataOnDownloadStopExRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION, kernel_version, peer_guid)
        , flv_url_(NULL)
        , flv_refer_url_(NULL)
        , data_item_array_(NULL)
        , play_stop_info_array_(NULL)
    {
        UINT32 offset = DataCollectionRequestPacket::HEADER_LENGTH;

        struct DATA_TAG {
            DATA_TYPE Type;
            UINT8     Len;
            UINT8*     Data;
        } tags[] = {
            { DATA_TYPE::IP_POOL_SIZE,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount) },
            { DATA_TYPE::FULL_PEERS_COUNT,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.FullPeersCount) },
            { DATA_TYPE::IS_DOWNLOAD_FINISHED,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.IsDownloadFinished) },
            { DATA_TYPE::AVG_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgDownloadSpeedInKBps) },
            { DATA_TYPE::MAX_HTTP_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.MaxHttpDownloadSpeedInKBps)},
            { DATA_TYPE::MAX_P2P_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.MaxP2PDownloadSpeedInKBps)},
            { DATA_TYPE::AVG_HTTP_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgHttpSpeedInKBps)},
            { DATA_TYPE::AVG_P2P_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgP2PSpeedInKBps)},
            { DATA_TYPE::HTTP_DATA_BYTES,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.HttpDataBytes)},
            { DATA_TYPE::P2P_DATA_BYTES,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.P2PDataBytes)},
            { DATA_TYPE::PACKET_LOST_RATE,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.PacketLostRate)},
            { DATA_TYPE::REDUNDANT_RATE,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.RedundantRate)},
            { DATA_TYPE::DOWNLOAD_STATUS,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.DownloadStatus)},
            { DATA_TYPE::FLV_FILE_LENGTH,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.FlvFileLength)},
            { DATA_TYPE::FLV_DATA_RATE,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.FlvDataRate)},
            { DATA_TYPE::FLV_TIME_DURATION_IN_SECONDS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.FlvTimeDurationInSecond)},
            { DATA_TYPE::HAS_P2P,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.HasP2P) },
            { DATA_TYPE::RESOURCE_ID, sizeof(RID), (UINT8*)(&data_ondownloadstop_info_ex.Rid) },
            { DATA_TYPE::IP_POOL_SIZE_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s1) },
            { DATA_TYPE::IP_POOL_SIZE_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s2) },
            { DATA_TYPE::IP_POOL_SIZE_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s3) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s1, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s1) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s2, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s2) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s3, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s3) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s1) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s2) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s3) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s1) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s2) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s3) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s1) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s2) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s3) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s1) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s2) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s3) },
            { DATA_TYPE::DATA_TYPE_COUNT, 0, NULL }
        };

        // flv url
        flv_url_ = (STRING*)(buffer_.data_.get() + offset);
        flv_url_->Length = flv_url.length();
        memcpy(flv_url_->StringData, flv_url.c_str(), flv_url.length());
        offset += sizeof(STRING) + flv_url_->Length;

        // flv_refer_url
        flv_refer_url_ = (STRING*)(buffer_.data_.get() + offset);
        flv_refer_url_->Length = flv_refer_url.length();
        memcpy(flv_refer_url_->StringData, flv_refer_url.c_str(), flv_refer_url.length());
        offset += sizeof(STRING) + flv_refer_url_->Length;

        // data_item_array
        data_item_array_ = (DATA_ITEM_ARRAY*)(buffer_.data_.get() + offset);
        data_item_array_->ITEMS_COUNT = DATA_TYPE::DATA_TYPE_COUNT;
        data_item_array_->ITEMS_SIZE = sizeof(DATA_ITEM) * data_item_array_->ITEMS_COUNT + sizeof(DATA_ONDOWNLOADSTOP_INFO_EX);
        offset += sizeof(DATA_ITEM_ARRAY);

        DATA_ITEM* item = NULL;
        for (UINT8 i = 0; i <= DATA_TYPE::DATA_TYPE_COUNT; ++i)
        {
            if (tags[i].Type == DATA_TYPE::DATA_TYPE_COUNT || tags[i].Len == 0 || tags[i].Data == NULL)
            {
                assert(tags[i].Type == DATA_TYPE::DATA_TYPE_COUNT && tags[i].Len == 0 && tags[i].Data == NULL);
                assert(i == DATA_TYPE::DATA_TYPE_COUNT);
                break;
            }
            item = (DATA_ITEM*)(buffer_.data_.get() + offset);
            item->ITEM_TYPE = tags[i].Type;
            item->ITEM_SIZE = tags[i].Len;
            memcpy(item->DATA, tags[i].Data, tags[i].Len);
            offset += sizeof(DATA_ITEM) + item->ITEM_SIZE;
        }

        // play_stop_info_array
        play_stop_info_array_ = (PLAY_STOP_INFO_ARRAY*)(buffer_.data_.get() + offset);
        assert(play_stop_infos.size() < 0xFFFF);
        play_stop_info_array_->PlayStopCount = play_stop_infos.size();
        for (UINT8 i = 0; i < play_stop_infos.size(); ++i)
        {
            play_stop_info_array_->PlayStopInfos[i] = play_stop_infos[i];
        }
    }

    DATA_ONDOWNLOADSTOP_INFO_EX DataOnDownloadStopExRequestPacket::GetDataOnDownloadStopInfo() const
    {
        DATA_ONDOWNLOADSTOP_INFO_EX data_ondownloadstop_info_ex;

        struct DATA_TAG {
            DATA_TYPE Type;
            UINT8     Len;
            UINT8*     Data;
        } tags[] = {
            { DATA_TYPE::IP_POOL_SIZE,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount) },
            { DATA_TYPE::FULL_PEERS_COUNT,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.FullPeersCount) },
            { DATA_TYPE::IS_DOWNLOAD_FINISHED,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.IsDownloadFinished) },
            { DATA_TYPE::AVG_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgDownloadSpeedInKBps) },
            { DATA_TYPE::MAX_HTTP_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.MaxHttpDownloadSpeedInKBps)},
            { DATA_TYPE::MAX_P2P_DOWNLOAD_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.MaxP2PDownloadSpeedInKBps)},
            { DATA_TYPE::AVG_HTTP_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgHttpSpeedInKBps)},
            { DATA_TYPE::AVG_P2P_SPEED_IN_KPS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.AvgP2PSpeedInKBps)},
            { DATA_TYPE::HTTP_DATA_BYTES,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.HttpDataBytes)},
            { DATA_TYPE::P2P_DATA_BYTES,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.P2PDataBytes)},
            { DATA_TYPE::PACKET_LOST_RATE,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.PacketLostRate)},
            { DATA_TYPE::REDUNDANT_RATE,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.RedundantRate)},
            { DATA_TYPE::DOWNLOAD_STATUS,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.DownloadStatus)},
            { DATA_TYPE::FLV_FILE_LENGTH,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.FlvFileLength)},
            { DATA_TYPE::FLV_DATA_RATE,  sizeof(UINT32), (UINT8*)(&data_ondownloadstop_info_ex.FlvDataRate)},
            { DATA_TYPE::FLV_TIME_DURATION_IN_SECONDS,  sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.FlvTimeDurationInSecond)},
            { DATA_TYPE::HAS_P2P,  sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.HasP2P) },
            { DATA_TYPE::RESOURCE_ID, sizeof(RID), (UINT8*)(&data_ondownloadstop_info_ex.Rid) },
            { DATA_TYPE::IP_POOL_SIZE_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s1) },
            { DATA_TYPE::IP_POOL_SIZE_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s2) },
            { DATA_TYPE::IP_POOL_SIZE_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.IPPoolSize_s3) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s1, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s1) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s2, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s2) },
            { DATA_TYPE::CONNECTED_PEERS_COUNT_s3, sizeof(UINT8), (UINT8*)(&data_ondownloadstop_info_ex.ConnectedPeersCount_s3) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s1) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s2) },
            { DATA_TYPE::P2P_DOWNLOAD_SPEED_IN_KPS_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.P2PDownloadSpeedInKBps_s3) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s1) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s2) },
            { DATA_TYPE::CONNECT_SEND_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectSendCount_s3) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s1) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s2) },
            { DATA_TYPE::CONNECT_ERR_FULL_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrFullCount_s3) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s1, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s1) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s2, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s2) },
            { DATA_TYPE::CONNECT_ERR_OTHER_COUNT_s3, sizeof(UINT16), (UINT8*)(&data_ondownloadstop_info_ex.ConnectErrOtherCount_s3) },
            { DATA_TYPE::DATA_TYPE_COUNT, 0, NULL }
        };

        UINT8* data_ptr[ DATA_TYPE::DATA_TYPE_COUNT ];

        for (UINT8 i = 0; i < DATA_TYPE::DATA_TYPE_COUNT; ++i)
            data_ptr[ tags[i].Type ] = tags[i].Data;

        assert(data_item_array_);

        // data_item_array
        UINT32 offset = 0;

        DATA_ITEM* item = NULL;
        for (UINT8 i = 0; i < data_item_array_->ITEMS_COUNT; ++i)
        {
            item = (DATA_ITEM*)(data_item_array_->ITEMS + offset);
            assert(item->ITEM_TYPE < DATA_TYPE::DATA_TYPE_COUNT);
            if (item->ITEM_TYPE < DATA_TYPE::DATA_TYPE_COUNT)
            {
                // check type
                memcpy(data_ptr[item->ITEM_TYPE], item->DATA, item->ITEM_SIZE);
                offset += sizeof(DATA_ITEM) + item->ITEM_SIZE;
            }
            else
            {
                break;
            }
        }

        return data_ondownloadstop_info_ex;
    }

}