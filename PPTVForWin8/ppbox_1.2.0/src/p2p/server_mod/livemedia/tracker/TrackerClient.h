#pragma once

#include <protocol/TrackerPacket.h>

#include <framework/string/Uuid.h>

#include <vector>
#include <set>


#define TRACK_TYPE "tracker"


typedef framework::string::Uuid RID;
typedef framework::string::Uuid Guid;

using namespace boost;

class TrackerClient
    : public boost::noncopyable
    , public boost::enable_shared_from_this<TrackerClient>
#ifdef DUMP_OBJECT
    , public count_object_allocate<TrackerClient>
#endif
{
public:

    typedef boost::shared_ptr<TrackerClient> p;

    static p Create(boost::asio::ip::udp::endpoint end_point) 
    {
        return p(new TrackerClient(end_point));
    }

public:

    void Start();

    void Stop();

    void DoList(const RID& rid);

    void DoQueriedPeerCount(const RID& rid);
    void OnListResponsePacket(protocol::ListPacket const & packet);

    void OnReportResponsePacket(protocol::ReportPacket const & packet);

    void OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet);
    /**
    * @brief ����ֵΪTransationID
    */
    uint32_t DoSubmit();

    void SetRidCount(uint32_t rid_count);

    void PPLeave();

    bool IsSync() const { return is_sync_; }

public:

    void SetTrackerInfo(const protocol::TRACKER_INFO& tracker_info);

    const protocol::TRACKER_INFO& GetTrackerInfo() const;

    void SetGroupCount(uint32_t group_count);

    uint32_t GetGroupCount() const;

    std::set<RID> GetClientResource() const;

    //uint32_t DoCommit();

    uint32_t DoReport();

private:
    //uint32_t DoKeepAlive();

private:

    void UpdateIpStatistic(const protocol::SocketAddr& detected_addr);

private:
    TrackerClient() {}
    TrackerClient(boost::asio::ip::udp::endpoint end_point) 
        : end_point_(end_point){}

public:
    boost::asio::ip::udp::endpoint end_point_;

private:
    /**
    * @brief ��¼�ϴ��յ���Tracker���ص��ϱ���Դ���
    */
    uint32_t last_response_rid_count_;

    /**
    * @brief
    */
    uint32_t last_transaction_id_; 

    /**
    * @brief ������Ϊ�ķ�������Դ����
    */
    std::set<RID> local_resources_;
    bool is_sync_;

    /**
    * @brief Tracker Info
    */
    protocol::TRACKER_INFO tracker_info_;

    uint32_t group_count_;

    /**
    * @brief �ϴ�report������
    */
    std::vector<protocol::REPORT_RESOURCE_STRUCT> last_updates_;

public:
    static boost::shared_ptr<protocol::UdpServer> udp_server_;
    static Guid peer_guid_;
    static std::set<RID> client_resource_ids_;

private:
    /**
    *
    */
    static const boost::uint16_t MAX_REQUEST_PEER_COUNT_ = 50;

    static const uint32_t MAX_UINT = 0xFFFFFFFFU;

    /**
    * ÿ�����Report��RID��
    */
    static const uint32_t MAX_REPORT_RID_COUNT = 50;
};
