// IndexManager.h

#ifndef _P2SP_INDEX_INDEX_MANAGER_H_
#define _P2SP_INDEX_INDEX_MANAGER_H_

#include "network/Resolver.h"
#include "bootstrap/BootstrapPacket.h"
#include "stun/IndexPacket.h"

#include <struct/RidInfo.h>
#include <struct/UrlInfo.h>

#include <framework/timer/AsioTimerManager.h>

#ifndef Guid
typedef framework::string::Uuid Guid;
typedef framework::string::Uuid MD5;
typedef framework::string::Uuid RID;
#endif

using namespace std;
using namespace framework::string;

namespace p2sp
{
    class IndexManager 
        : public boost::enable_shared_from_this<IndexManager>
        , public network::IResolverListener
#ifdef DUMP_OBJECT
        , public count_object_allocate<IndexManager>
#endif
    {
        friend class AppModule;
    public:
        typedef boost::shared_ptr<IndexManager> p;

        /**
        * @brief ������ģ��
        */

        void Start(string domain, boost::uint16_t port);

        /**
        * @brief ֹͣ��ģ��
        */
        void Stop();

        /**
        * @brief ͨ��url���RID (�첽)
        */
        void DoQueryRidByUrl(string url, string refer);

        void DoQueryRidByContent(string url, string refer, MD5 content_md5, uint32_t content_bytes, uint32_t file_length);

        void DoQueryhttpServerByRid(RID rid);

        void DoQueryTrackerList();

        void DoQueryStunServerList();

        void DoQueryIndexServerList();

        //void DoQueryNeedReport();

        void DoQueryKeywordList();

        //void DoQueryDataCollectionServer();

#ifdef NOTIFY_ON
        void DoQueryNotifyServerList();
#endif
        void DoQueryUploadPicProbability();

        void DoAddUrlRid(protocol::UrlInfo url_info, protocol::RidInfo rid_info, MD5 content_md5, uint32_t content_bytes, int flag);

        void OnUdpRecv(protocol::ServerPacket const & packet_header);

    public:

        bool NeedReport() const;
        boost::asio::ip::udp::endpoint GetDataCollectionServerEndpoint() const { return data_collection_server_endpoint_; }
        float UploadPicProbability() const { return upload_pic_probability_; }

    public:
        //�ӿ�
        virtual void OnResolverSucced(uint32_t ip, boost::uint16_t port);
        virtual void OnResolverFailed(uint32_t error_code); // 1-Url������ 2-�����޷����� 3-������������ 4-����ʧ��

    public:
        void SubmitDataOnDownloadStop(
            const protocol::DATA_ONDOWNLOADSTOP_INFO_EX& info, 
            const protocol::UrlInfo& url_info, 
            const std::vector<protocol::PLAY_STOP_INFO>& play_stop_infos);

    protected:

        void OnQueryRidByUrlPacket(protocol::QueryRidByUrlPacket const & packet);

        void OnQueryRidByContentPacket(protocol::QueryRidByContentPacket const & packet);

        void OnQueryHttpServerByRidPacket(protocol::QueryHttpServerByRidPacket const & packet);

        void OnQueryTrackerListPacket(protocol::QueryLiveTrackerListPacket const & packet);

        void OnAddRidUrlPacket(protocol::AddRidUrlPacket const & packet);

        void OnQueryStunServerListPacket(protocol::QueryStunServerListPacket const & packet);

        void OnQueryIndexServerListPacket(protocol::QueryIndexServerListPacket const & packet);

        //void OnQueryNeedReportPacket(protocol::QueryNeedReportPacket const & packet);

        void OnQueryKeywordListPacket(protocol::QueryKeyWordListPacket const & packet);

        //void OnQueryDataCollectionServerPacket(protocol::QueryDataCollectionServerPacket const & packet);

        void OnQueryUploadPicProbabilityPacket(protocol::QueryUploadPicProbabilityPacket const & packet);

    protected:
        
        void OnTimerElapsed(
            framework::timer::Timer * pointer);

        void OnQueryTrackerListTimerElapsed(uint32_t times);
        
        void OnQueryStunServerListTimerElapsed(uint32_t times);

        void OnQueryIndexServerListTimerElapsed(uint32_t times);

        void OnQueryTestUrlListTimerElapsed(uint32_t times);

        void OnQueryKeywordListTimerElapsed(uint32_t times);

        //void OnQueryNeedReportTimerElapsed(uint32_t times);

        //void OnQueryDataCollectionServerTimerElapsed(uint32_t times);
#ifdef NOTIFY_ON
        void OnQueryNotifyServerTimerElapsed(boost::uint32_t times);
#endif
    private:
        boost::asio::io_service & io_svc_;
        /**
        * @brief ���ڼ�¼ ������ QureyRidByUrl �� TransactionID �� url �Ķ�Ӧ��ϵ
        */
        std::map<uint32_t, protocol::UrlInfo> trans_url_map_; 

        bool is_have_tracker_list_;
        bool is_have_stun_server_list_;
        bool is_have_index_server_list_;
        bool is_have_test_url_list_;
        bool is_have_keyword_list_;
        bool is_have_change_domain_;
        bool is_resolving_;
        bool is_firsttime_resolve_;
        //bool is_have_needreport_;
        //bool is_have_datacollection_server_;
        bool is_have_notify_server_;

        bool need_report_;
        boost::uint8_t interval_time_;
        float upload_pic_probability_;
        boost::asio::ip::udp::endpoint data_collection_server_endpoint_;

        framework::timer::OnceTimer change_domain_resolver_timer_;
        framework::timer::OnceTimer query_tracker_list_timer_;
        framework::timer::OnceTimer query_stun_server_list_timer_;
        framework::timer::OnceTimer query_index_server_list_timer_;
        framework::timer::OnceTimer query_keyword_list_timer_;
        //framework::timer::OnceTimer query_needreport_timer_;
        //framework::timer::OnceTimer query_datacollectionserver_timer_;
#ifdef NOTIFY_ON
        framework::timer::OnceTimer query_notify_server_list_timer_;
#endif

        string query_rid_url_, query_rid_refer_;
        volatile bool is_running_;

	public:
        boost::asio::ip::udp::endpoint server_list_endpoint_;

        uint32_t last_querytrackerlist_intervaltimes_;
        uint32_t last_querystunlist_intervaltimes_;
        uint32_t last_queryindexlist_intervaltimes_;
        uint32_t last_querytesturllist_intervaltimes_;
        uint32_t last_querykeywordlist_intervaltimes_;
        //uint32_t last_queryneedreport_intervaltimes_;
        //uint32_t last_querydatacollectionserver_intervaltimes_;
#ifdef NOTIFY_ON
        boost::uint32_t last_querynotifyserverlist_intervaltimes_;
#endif

        //Resolver
        network::Resolver::p resolver_;
        std::vector<boost::uint8_t> mod_index_map_;
        std::vector<protocol::INDEX_SERVER_INFO> index_servers_;

        // test url std::list
        std::vector<string> test_url_list_;
        // keyword std::list
        std::vector<string> keyword_list_;

        string domain_;
        boost::uint16_t port_;
        string boss_ip_;
        string boss_domain_;
        boost::uint16_t boss_port_;
        boost::uint16_t failed_times_;
        boost::uint16_t resolve_times_;

    private:
        static IndexManager::p inst_;
        IndexManager(
            boost::asio::io_service & io_svc);

        boost::asio::ip::udp::endpoint GetIndexEndPoint(string url, string refer);
        boost::asio::ip::udp::endpoint GetIndexEndPoint(Guid guid);
        boost::asio::ip::udp::endpoint ModToEndPoint(uint32_t mod);
        string Url2Mini(string url, string refer);

    private:

        static const uint32_t INITIAL_QUERY_INTERVAL = 15*1000 ;
        static const uint32_t DEFAULT_QUERY_INTERVAL = 4 * 60 * 60*1000U;

    public:
        static IndexManager::p CreateInst(
            boost::asio::io_service & io_svc)
        {
            inst_.reset(new IndexManager(io_svc));
            return inst_;
        }

        static IndexManager::p Inst() { return inst_; };

		static framework::timer::AsioTimerManager * asio_timer_manager_;
		static boost::asio::io_service * io_service_;
		static protocol::UdpServer * udp_server_;
		static Guid peer_guid_;
    };
}

#endif // _P2SP_INDEX_INDEX_MANAGER_H_
