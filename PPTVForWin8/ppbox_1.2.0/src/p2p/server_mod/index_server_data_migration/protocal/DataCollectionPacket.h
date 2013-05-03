#pragma once

#include "protocal/Packet.h"
#include "protocal/structs.h"

namespace protocal
{
    //////////////////////////////////////////////////////////////////////////
    class DataCollectionRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<DataCollectionRequestPacket> p;
        virtual ~DataCollectionRequestPacket() {}
        static p ParseFromBinary(Buffer buffer);
        static const size_t HEADER_LENGTH = sizeof(REQUEST_DATACOLLECTION_PACKET) + RequestServerPacket::HEADER_LENGTH;

    public:
        VERSION_INFO GetKernelVersion() const { assert(datacollection_header_); return datacollection_header_->KernelVersion; }
        Guid GetPeerGuid() const { assert(datacollection_header_); return datacollection_header_->PeerGuid; }
    private:
        REQUEST_DATACOLLECTION_PACKET* datacollection_header_;
    protected:
        static bool Check(Buffer buffer);
        static VERSION_INFO GetKernelVersion(Buffer buffer);
    protected:
        DataCollectionRequestPacket(Buffer buffer);
        DataCollectionRequestPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version, VERSION_INFO kernel_version, Guid peer_guid);
    };

    //////////////////////////////////////////////////////////////////////////
    class DataOnAppStopRequestPacket : public DataCollectionRequestPacket
    {
    public:
        typedef boost::shared_ptr<DataOnAppStopRequestPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONAPPSTOP_INFO& data_onappstop_info, const string& flv_folder_path, const PLATFORM_INFO& platform_info, const ACTION_COUNT_INFO& action_count_info);
    public:
        static const UINT8 ACTION = 0xB1;
        string GetFlvFolderPath() const { assert(flv_folder_path_); return string((const char*)(flv_folder_path_->StringData), flv_folder_path_->Length); }
        DATA_ONAPPSTOP_INFO GetDataOnAppStopInfo() const { assert(data_onappstop_info_); return *data_onappstop_info_; }
        PLATFORM_INFO GetPlatformInfo() const { if (GetPeerVersion() < 0x00000008) return PLATFORM_INFO(); assert(platform_info_); return *platform_info_; }
        ACTION_COUNT_INFO GetActionCountInfo() const { if (GetPeerVersion() < 0x00000009) return ACTION_COUNT_INFO(); assert(action_count_info_); return *action_count_info_; }
        map<UINT32, UINT32> GetActionCountInfoMap() const;

    private:
        static bool Check(Buffer buffer);
        DATA_ONAPPSTOP_INFO* data_onappstop_info_;
        STRING* flv_folder_path_;
        PLATFORM_INFO* platform_info_;
        ACTION_COUNT_INFO* action_count_info_;

    protected:
        DataOnAppStopRequestPacket(Buffer buffer);
        DataOnAppStopRequestPacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONAPPSTOP_INFO& data_onappstop_info, const string& flv_folder_path, const PLATFORM_INFO& platform_info, const ACTION_COUNT_INFO& action_count_info);
    };

    //////////////////////////////////////////////////////////////////////////
    class DataOnDownloadStopRequestPacket : public DataCollectionRequestPacket
    {
    public:
        typedef boost::shared_ptr<DataOnDownloadStopRequestPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO& data_ondownloadstop_info, const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos);

    public:
        static const UINT8 ACTION = 0xB2;
        string GetFlvUrl() const { assert(flv_url_); return string((const char*)(flv_url_->StringData), flv_url_->Length); }
        string GetFlvReferUrl() const { assert(flv_refer_url_); return string((const char*)(flv_refer_url_->StringData), flv_refer_url_->Length); }
        DATA_ONDOWNLOADSTOP_INFO GetDataOnDownloadStopInfo() const { assert(data_ondownloadstop_info_); return *data_ondownloadstop_info_; }
        vector<PLAY_STOP_INFO> GetPlayStopInfoArray() const { assert(play_stop_info_array_); return vector<PLAY_STOP_INFO>(play_stop_info_array_->PlayStopInfos, play_stop_info_array_->PlayStopInfos + play_stop_info_array_->PlayStopCount); }
        UINT8 GetPlayStopInfoCount() const { assert(play_stop_info_array_); return play_stop_info_array_->PlayStopCount; }

    private:
        static bool Check(Buffer buffer);
        STRING* flv_url_;
        STRING* flv_refer_url_;
        DATA_ONDOWNLOADSTOP_INFO* data_ondownloadstop_info_;
        PLAY_STOP_INFO_ARRAY* play_stop_info_array_;

    protected:
        DataOnDownloadStopRequestPacket(Buffer buffer);
        DataOnDownloadStopRequestPacket(
            UINT32 append_length, UINT32 transaction_id, VERSION_INFO kernel_version, 
            Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO& data_ondownloadstop_info, 
            const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos
        );
    };

    //////////////////////////////////////////////////////////////////////////
    class DataOnDownloadStopExRequestPacket : public DataCollectionRequestPacket
    {
    public:
        typedef boost::shared_ptr<DataOnDownloadStopExRequestPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, VERSION_INFO kernel_version, Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO_EX& data_ondownloadstop_info, const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos);

    public:
        static const UINT8 ACTION = 0xB3;
        string GetFlvUrl() const { assert(flv_url_); return string((const char*)(flv_url_->StringData), flv_url_->Length); }
        string GetFlvReferUrl() const { assert(flv_refer_url_); return string((const char*)(flv_refer_url_->StringData), flv_refer_url_->Length); }
        DATA_ONDOWNLOADSTOP_INFO_EX GetDataOnDownloadStopInfo() const;
        vector<PLAY_STOP_INFO> GetPlayStopInfoArray() const { assert(play_stop_info_array_); return vector<PLAY_STOP_INFO>(play_stop_info_array_->PlayStopInfos, play_stop_info_array_->PlayStopInfos + play_stop_info_array_->PlayStopCount); }
        UINT8 GetPlayStopInfoCount() const { assert(play_stop_info_array_); return play_stop_info_array_->PlayStopCount; }

    private:
        static bool Check(Buffer buffer);
        STRING* flv_url_;
        STRING* flv_refer_url_;
        DATA_ITEM_ARRAY* data_item_array_;
        PLAY_STOP_INFO_ARRAY* play_stop_info_array_;

    protected:
        DataOnDownloadStopExRequestPacket(Buffer buffer);
        DataOnDownloadStopExRequestPacket(
            UINT32 append_length, UINT32 transaction_id, VERSION_INFO kernel_version, 
            Guid peer_guid, const DATA_ONDOWNLOADSTOP_INFO_EX& data_ondownloadstop_info_ex, 
            const string& flv_url, const string& flv_refer_url, const vector<PLAY_STOP_INFO>& play_stop_infos
        );
    };
}
