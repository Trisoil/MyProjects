#pragma once

#include "TrackerLogger.h"
#include "TrackerModel.h"
#include "TrackerStatistic.h"
#include "framework/configure/Config.h"
#include "framework/timer/Timer.h"
#include "framework/timer/AsioTimerManager.h"

//#include "UdpServer.h"
#include "protocol/UdpServer.h"

#include "protocol/TrackerPacket.h"
#include "protocol/Packet.h"

//#include "Timer.h"
using namespace std;

namespace udptracker
{
    enum CheckResult
    {
        PEERID_CHECK_FAILED = -1,
        FREQUENT_CHECK_FAILED = -2,
        RESOURCEID_CHECK_FAILED = -3,
    };

    class UdpTrackerModule 
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpTrackerModule>
        , public protocol::IUdpServerListener
        //		, public ITimerListener
    {
    public:

        typedef boost::shared_ptr<UdpTrackerModule> p;

    public:

        void Start(
            u_short local_udp_port                                // 本地监听UDP端口
            );

        void Stop();

        void InitConfigValues();

        //        void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf);
        virtual void OnUdpRecv(protocol::Packet const & packet);

        template<typename T>
        void DoSendPacket(const boost::asio::ip::udp::endpoint& end_point, T packet , boost::uint16_t peer_version )
        {
            if( !is_running_ )
            {
                LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::DoSendPacket: TracerModule is not running. Return.");
                RELEASE_LOG("UdpTrackerModule::DoSendPacket: TracerModule is not running. Return.");
                return;
            }

            //按请求者的版本进行回复
            packet.peer_version_ = peer_version;

            this->udp_server_->send_packet( packet , peer_version );
        }

        template<typename T>
        void DoSendPacket(const boost::asio::ip::udp::endpoint& end_point, boost::shared_ptr<T> & packet , boost::uint16_t peer_version )
        {
            this->DoSendPacket( end_point , *packet , peer_version );
        }

        void OnTimerElapsed(const Timer::pointer * pointer, unsigned times);

    public:

        unsigned GetResourceCount() const { return tracker_model_->TotalResourcesCount(); }

        unsigned GetPeersCount() const { return tracker_model_->TotalPeersCount(); }

        void ConsoleLogListRIDCount() {CONSOLE_OUTPUT("$Resource Count: " << GetResourceCount());}

        void ConsoleLogListPeerGuidCount() {CONSOLE_OUTPUT("$Peer Count: " << GetPeersCount());}

        void ConsoleLogListRID() {tracker_model_->ConsoleLogListRID();CONSOLE_OUTPUT("$Resource Count: " << GetResourceCount());}

        void ConsoleLogListPeerGuid() {tracker_model_->ConsoleLogListPeerGuid();CONSOLE_OUTPUT("$Peer Count: " << GetPeersCount());}

        void ConsoleLogPeerGuid(Guid &peer_guid) {tracker_model_->ConsoleLogPeerGuid(peer_guid);}

        void ConsoleLogRID(RID &rid) {tracker_model_->ConsoleLogRID(rid);}

        void ConsoleLogShowEmptyPeer() {tracker_model_->ConsoleLogShowEmptyPeer();}

        void ConsoleLogShowStatistic() 
        { 
            CONSOLE_LOG(tracker_statistic_->ReportStatistic());
        }

        void ConsoleLogShowConfig() { CONSOLE_LOG(""); }
        //        void ConsoleLogShowConfig() { CONSOLE_LOG(""); CONSOLE_OUTPUT(Config::Inst().ToString()); }

        void DumpResources();

        void DumpPeers();

        void DumpSampleResources();

        void SetDumpOn(bool on) { dump_on_ = on; }

        void SetCheckResources(bool on) { check_resources_on_ = on; }

        void ConsoleLogDumpStatus() { CONSOLE_OUTPUT( (dump_on_ ? "on" : "off") ); }

        void ConsoleLogCheckResourcesStatus() { CONSOLE_OUTPUT( (check_resources_on_ ? "on" : "off") ); }

        void LoadSampleResources();

        void DelayDump(unsigned time_in_seconds);

        size_t BackupFile(std::string file_path);

        size_t MoveFile(std::string source_file, std::string dest_file, size_t flags);

        TrackerStatistic::p GetStatistic() { return tracker_statistic_; }

    protected:
        //check 成功 返回 0，check失败返回负数
        int PeerIdCheck(const Guid& peer_id,bool check_frequent);
        int PeerIdRidCheck(const Guid& peer_id,bool check_frequent,const RID& rid);

        virtual void OnListRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::ListPacket & list_request);

        virtual void OnListTcpRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::ListTcpPacket & list_request);

        virtual void OnListWithIpRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::ListWithIpPacket & list_request);

        virtual void OnListTcpWithIpRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::ListTcpWithIpPacket & list_request);

       virtual void OnLeaveRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::LeavePacket & leave_request);

        virtual void OnInternalCommandRequest( const boost::asio::ip::udp::endpoint& end_point, 
            protocol::InternalCommandPacket & icommand_request);

        virtual void OnReportRequest( const boost::asio::ip::udp::endpoint& end_point,
            protocol::ReportPacket & report_request);

        virtual void OnQueryPeerCountRequest( const boost::asio::ip::udp::endpoint& end_point,
            protocol::QueryPeerCountPacket & query_peer_count_request);

        virtual void OnQueryPeerResourcesRequest(const boost::asio::ip::udp::endpoint&  end_point, 
            protocol::QueryPeerResourcesPacket& query_peer_resources_request);

        virtual void OnQueryTrackerStatisticRequest(const boost::asio::ip::udp::endpoint&  end_point, 
            protocol::QueryTrackerStatisticPacket& query_tracker_statistic_request);

    private:

        void DispatchAction(const boost::asio::ip::udp::endpoint& end_point, boost::uint8_t action, ::protocol::ServerPacket const & packet);

    private:

        void WriteStatisticHeader();

    private: // Configs

        //////////////////////////////////////////////////////////////////////////
        // Keep Alive

        static unsigned GetKeepAliveIntervalInSeconds();

        static unsigned GetCheckAliveCheckIntervalInSeconds(); // tracker.check_alive_interval_in_seconds

        //////////////////////////////////////////////////////////////////////////
        // Dump

        static unsigned GetDumpResourceIntervalInSeconds(); // tracker.dump_interval_in_seconds

        static unsigned GetDumpResourceCount(); // tracker.dump_resource_count

        static unsigned GetInternalCommandMagicNumber(); //tracker.internal_command_magic_number

        //////////////////////////////////////////////////////////////////////////
        // udp

        static unsigned GetUdpServerReceiveCount(); // tracker.udp_server_receive_count

        static unsigned GetListMaxPeerCount(); // tracker.list_max_peer_count

        //////////////////////////////////////////////////////////////////////////
        // Statistic

        static unsigned GetStatisticLogIntervalInHours(); // tracker.statistic_log_interval_in_hours

        //////////////////////////////////////////////////////////////////////////
        // Copy Resources

        static unsigned GetCopyResourceIntervalInSeconds();

        static unsigned GetCopyResourceMinPeerCount();

        static std::string GetCopyResourceDestination();

        //////////////////////////////////////////////////////////////////////////
        // Sample

        static unsigned GetSampleIntervalInSeconds();

    private:

        boost::shared_ptr<protocol::UdpServer> udp_server_;

        volatile bool is_running_;

        TrackerModel::p tracker_model_;

        //        TickCounter64 tick_counter_;

        ResourcePeerCountHandler resource_dump_handler_;

        volatile bool dump_on_;

        bool dump_resource_map_;
        bool dump_peer_list_;
        boost::uint32_t dump_peer_list_last_sec_;
        boost::uint32_t dump_resource_map_last_sec_;


        volatile bool check_resources_on_;

        //ck 20120627 add .
        //report的时候，是否使用stun测试出来的ip。注意，小运营商由于tracker是部署在内网，stun是外网，因此这个数值必须是false
        //而公网上的部署，这个数值要设置为true
        bool use_stun_detect_ip_;

        volatile bool sample_resource_dump_on_;

        set<RID> sample_resources_;

        set<RID> sample_resources_random_;

    private:

        framework::timer::PeriodicTimer::pointer check_alive_timer_;

        framework::timer::PeriodicTimer::pointer resource_dump_timer_;

        framework::timer::PeriodicTimer::pointer statistic_timer_;

        framework::timer::PeriodicTimer::pointer copy_resource_timer_;

        framework::timer::PeriodicTimer::pointer sample_resource_dump_timer_;

        framework::timer::OnceTimer::pointer delay_dump_timer_;

		framework::timer::PeriodicTimer::pointer peer_statistic_timer_;

        static size_t check_alive_interval_in_seconds_;
        static size_t dump_interval_in_seconds_;
        static size_t dump_resource_count_;
        static size_t udp_server_receive_count_;
        static size_t statistic_log_interval_in_hours_;
        static size_t copy_resource_interval_in_seconds_;
        static size_t copy_resource_min_peer_count_;
        static std::string copy_resource_destination_;
        static size_t sample_interval_in_seconds_;
        static size_t internal_command_magic_number_;

    private:

        TrackerStatistic::p tracker_statistic_;

    private:

        static const unsigned RECEIVE_COUNT = 50;

        static const boost::uint16_t DEFAULT_COMMIT_KPL_INTERVAL_IN_SECONDS = 20;

        static const time_t DEFAULT_CHECK_ALIVE_INTERVAL_IN_SECONDS = TrackerModel::KEEP_ALIVE_INTERVAL_IN_SECONDS * 10;

        //////////////////////////////////////////////////////////////////////////
        // resource dump

        static const unsigned DEFAULT_RESOURCE_DUMP_INTERVAL_IN_SECONDS = 5;

        static const unsigned DEFAULT_RESOURCE_DUMP_COUNT = 1000;

        //////////////////////////////////////////////////////////////////////////
        // Statistic

        static const unsigned DEFAULT_STATISTIC_LOG_INTERVAL_IN_HOURS = 24;

        //////////////////////////////////////////////////////////////////////////
        // Copy Resource

        static const unsigned DEFAULT_COPY_RESOURCE_INTERVAL_IN_SECONDS = 600;

        static const unsigned DEFAULT_COPY_RESOURCE_MIN_PEER_COUNT = 2;

    private:

        UdpTrackerModule();

        void out_put_count_stats();

        static UdpTrackerModule::p inst_;

    public:

        static UdpTrackerModule::p Inst() 
        { 
            if( !inst_ )
            {
                inst_.reset(new UdpTrackerModule());
            }

            return inst_; 
        }

        AsioTimerManager * asio_timer_manager_;
        //		boost::asio::io_service ios_;
    };
}
