#ifndef _TRACKER_GROUP_H_
#define _TRACKER_GROUP_H_



#include "tracker/TrackerClient.h"

class TrackerGroup
    : public boost::noncopyable
    , public boost::enable_shared_from_this<TrackerGroup>
#ifdef DUMP_OBJECT
    , public count_object_allocate<TrackerGroup>
#endif
{
public:
    typedef boost::shared_ptr<TrackerGroup> p;
    static p Create( framework::timer::AsioTimerManager * asio_timer_manager )
    { 
        return p( new TrackerGroup(asio_timer_manager) );
    }

public:
    void Start();

    void Stop();

    void DoList(const RID& rid);

    void DoQueriedPeerCount(const RID& rid);

    boost::uint32_t GetTrackersNum(const RID& rid);
    void SetTrackers(boost::uint32_t group_count, const std::set<protocol::TRACKER_INFO>& trackers);

    const std::vector<protocol::TRACKER_INFO> GetTrackers();

    void DoCommit();

    void PPLeave();

public:
    void OnTimerElapsed(framework::timer::Timer * pointer);

public:

    void OnListResponsePacket( protocol::ListPacket const & packet);

    void OnReportResponsePacket( protocol::ReportPacket const & packet);

    void OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet);
public:

    void SetGroupCount(uint32_t group_count);

    uint32_t GetGroupCount() const;

private:

    void StartAllClients();

    void StopAllClients();

    void ClearAllClients();

private:
    /**
    * @brief ����endpoint������TrackerClient
    *    [Key] endpoint ��tracker �� ip + port��
    *    [Value] TrackerClient ������ָ��
    */
    std::map<boost::asio::ip::udp::endpoint, TrackerClient::p> trackers_;

    /**`
    * @brief ��ǰѡ���� ��¼Tracker�� ������ָ��Ϊ�գ���ôδѡ�� ��¼Tracker
    *     ע����¼Tracker ��ָ ��Ҫ Commit �� KeepAlive ��Tracker
    */
    TrackerClient::p current_tracker_;

public:
    /**
    * @brief ����������һ�� Trackerʹ�õ�,�� trying_tracker����ʹ��
    */
    typedef std::list<TrackerClient::p> TrackerClientList; // // ����ʹ��list
    TrackerClientList tracker_list_;

private:
    /**
    * @brief ����������ڳ���Commit/KeepAlive��Tracker, ��tracker_list_����ʹ��
    **/
    TrackerClient::p trying_tracker_;
    TrackerClientList::iterator trying_tracker_iterator_;

    /**
    * @brief �Ե�ǰTracker Commit����KeepAlive �������������
    */ 
    uint32_t error_times_;

    /**
    * @brief �����ύ Commit/KeepAlive �Ķ�ʱ��
    */
    framework::timer::PeriodicTimer timer_;

    /**`
    * @brief �ϴη������� Trans_id
    */
    uint32_t last_transcation_id_;

    /**
    * @�����İ��Ƿ�, false Ϊû���յ���Ӧ,true Ϊ���յ���Ӧ
    */
    bool is_responsed_;

    /**
    * @��¼�ϴ��յ���Tracker���ص��ϱ���Դ���
    */
    // uint32_t last_response_rid_count_;
    uint32_t group_count_;

private:
    static const uint32_t DEFAULT_INTERVAL_IN_SECONDS_ = 20;
    static const uint32_t DEFAULT_INTERVAL_IN_MILLISECONDS_ = 1000 * DEFAULT_INTERVAL_IN_SECONDS_;

private:
    TrackerGroup(framework::timer::AsioTimerManager * asio_timer_manager)
        : timer_(*asio_timer_manager, 1000 * DEFAULT_INTERVAL_IN_SECONDS_, boost::bind(&TrackerGroup::OnTimerElapsed, this, &timer_))

    {}
};


#endif
