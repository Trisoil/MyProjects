#include <pre.h>
#include "IndexManager.h"

#include <framework/logger/Logger.h>

#ifdef NOTIFY_ON
#include "p2sp/notify/NotifyModule.h"
#endif

#include "network/Uri.h"

#include "tracker/TrackerManager.h"


using namespace network;
using namespace protocol;

//#  define LOGX(level, type, message) 

#define COUT(msg)
//#define COUT(msg) std::cout << msg

namespace p2sp
{
    framework::timer::AsioTimerManager * IndexManager::asio_timer_manager_;
    boost::asio::io_service * IndexManager::io_service_;
    protocol::UdpServer * IndexManager::udp_server_;
    Guid IndexManager::peer_guid_;


    IndexManager::p IndexManager::inst_;

    IndexManager::IndexManager(
        boost::asio::io_service & io_svc)
        : io_svc_(io_svc)
        , is_have_tracker_list_(false)
        , is_have_stun_server_list_(false)
        , is_have_index_server_list_(false)
        , is_have_test_url_list_(false)
        , is_have_keyword_list_(false)
#ifdef NOTIFY_ON
        , is_have_notify_server_(false)
#endif
        , is_have_change_domain_(false)
        , is_resolving_(false)
        , is_firsttime_resolve_(true)
        , change_domain_resolver_timer_(*this->asio_timer_manager_, 3000, boost::bind(&IndexManager::OnTimerElapsed, this, &change_domain_resolver_timer_))
        , query_tracker_list_timer_(*this->asio_timer_manager_,INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_tracker_list_timer_))
        , query_stun_server_list_timer_(*this->asio_timer_manager_,INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_stun_server_list_timer_))
        , query_index_server_list_timer_(*this->asio_timer_manager_,INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_index_server_list_timer_))
        , query_keyword_list_timer_(*this->asio_timer_manager_,INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_keyword_list_timer_))
        //, query_needreport_timer_(global_second_timer(),INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_needreport_timer_))
#ifdef NOTIFY_ON
        , query_notify_server_list_timer_(global_second_timer(), INITIAL_QUERY_INTERVAL, boost::bind(&IndexManager::OnTimerElapsed, this, &query_notify_server_list_timer_))
#endif
        , is_running_(false)
        , failed_times_(0)
        , resolve_times_(0)
    {

    }

    void IndexManager::OnResolverSucced(uint32_t ip, boost::uint16_t port)
    {
        if (is_running_ == false)
            return;

        LOG4CPLUS_INFO(Loggers::Service(), "IndexManager: Start OnResolverSucced, index_end_point " << ip << " " << port);

        // IndexServer 的 IP 和 端口设置
        server_list_endpoint_ = boost::asio::ip::udp::endpoint( boost::asio::ip::address_v4(ip), port);
        protocol::SocketAddr index_socket(server_list_endpoint_);

        /*
        statistic::StatisticModule::Inst()->SetBsInfo(server_list_endpoint_);
        statistic::StatisticModule::Inst()->SetIndexServerInfo(index_socket);
        */
        is_resolving_ = false;
        if (false == is_have_tracker_list_) DoQueryTrackerList();
        //        if (false == is_have_stun_server_list_) DoQueryStunServerList();
        //        if (false == is_have_index_server_list_) DoQueryIndexServerList();
#ifdef NOTIFY_ON
        //        if (false == is_have_notify_server_) DoQueryNotifyServerList();
#endif
        if (is_firsttime_resolve_)
        {
            is_firsttime_resolve_ = false;
            //DoQueryNeedReport();
            //DoQueryKeywordList();
            //            DoQueryUploadPicProbability();
        }
    }

    void IndexManager::OnResolverFailed(uint32_t error_code) // 1-Url锟斤拷锟斤拷锟斤拷 2-锟斤拷锟斤拷锟睫凤拷锟斤拷锟斤拷 3-锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 4-锟斤拷锟绞э拷锟?
    {
        if (is_running_ == false) return;    
        change_domain_resolver_timer_.stop();
        is_resolving_ = false;
        // 域名尚未切换至世纪互联
        if (false == is_have_change_domain_)
        {
            ++resolve_times_;
            bool need_change_domain = !(is_have_index_server_list_ || is_have_tracker_list_
                || is_have_stun_server_list_);
            // 解析失败三次并且三个回包一个都没收到，将域名切换至世纪互联，再次解析，不开启定时器
            if (resolve_times_ >= 3 && need_change_domain)
            {
                if (resolver_) { resolver_->Close(); resolver_.reset(); }
                resolver_ = Resolver::create(io_svc_, *this->asio_timer_manager_ , boss_domain_, boss_port_, shared_from_this());
                is_have_change_domain_ = true;
                resolve_times_ = 0;
                resolver_->DoResolver();
                is_resolving_ = true;
            }
            else // 否则，不改变域名再次解析，开启定时器
            {
                resolver_ = Resolver::create(io_svc_, *this->asio_timer_manager_ , domain_, port_, shared_from_this());
                resolver_->DoResolver();
                is_resolving_ = true;
                change_domain_resolver_timer_ .start();
            }
            failed_times_ = 0;
        }
        else // 域名已经切换至世纪互联，此时解析失败，直接设置server_list的ip-port
        {
            boost::system::error_code error;
            boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string(boss_ip_, error);
            server_list_endpoint_ = boost::asio::ip::udp::endpoint(addr, boss_port_);
            protocol::SocketAddr index_socket(server_list_endpoint_);
            //            statistic::StatisticModule::Inst()->SetIndexServerInfo(index_socket);
            DoQueryTrackerList();
            //            DoQueryStunServerList();
            //            DoQueryIndexServerList();
            //DoQueryNeedReport();
            //            DoQueryUploadPicProbability();
        }

        LOG4CPLUS_WARN(Loggers::Service(), "IndexManager ResolverFailed error_code: " << error_code );
    }

    void IndexManager::Start(string domain, boost::uint16_t port)
    {

        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "IndexManager::Start()");
        if( is_running_ == true ) return;

        // 注册 index packets
        protocol::register_bootstrap_packet( *this->udp_server_ );

        is_running_ = true;

        is_have_tracker_list_ = (false);
        is_have_stun_server_list_ = (false);
        is_have_index_server_list_ = (false);
        is_have_test_url_list_ = (false);
        is_have_keyword_list_ = (false);
        is_have_change_domain_ = (false);
        //is_have_needreport_ = false;
#ifdef NOTIFY_ON
        is_have_notify_server_ = false;
#endif

        boss_domain_ = "ppvaindex.pplive.com"; // 世纪互联
        boss_ip_ = "60.28.216.149";
        boss_port_ = 6400;

        domain_ = domain;
        port_ = port;

        //         domain = "ppvabs.pplive.com";
        //         port = 6400;

        //port = 6400;
        need_report_ = false;
        upload_pic_probability_ = 1.0f;

        //         boost::system::error_code error;
        //         boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4::from_string("60.28.216.149", error);
        //         server_list_endpoint_ = boost::asio::ip::udp::endpoint(addr, 6400);

        //         SocketAddr index_socket(server_list_endpoint_);
        //         statistic::StatisticModule::Inst()->SetIndexServerInfo(index_socket);

        last_querytrackerlist_intervaltimes_= INITIAL_QUERY_INTERVAL;
        last_querystunlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
        last_queryindexlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
        last_querytesturllist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
        last_querykeywordlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
        last_queryindexlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
#ifdef NOTIFY_ON
        last_querynotifyserverlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;
#endif
        /*
        query_tracker_list_timer_ = OnceTimer::create( last_querytrackerlist_intervaltimes_, shared_from_this() );
        query_stun_server_list_timer_= OnceTimer::create( last_querystunlist_intervaltimes_, shared_from_this() );
        query_index_server_list_timer_ = OnceTimer::create(last_queryindexlist_intervaltimes_, shared_from_this());
        //query_keyword_list_timer_ = OnceTimer::create(last_querykeywordlist_intervaltimes_, shared_from_this());
        query_needreport_timer_ = OnceTimer::create(last_queryindexlist_intervaltimes_, shared_from_this());
        */
        resolver_ = Resolver::create(io_svc_, *this->asio_timer_manager_ , domain_, port_, shared_from_this());
        resolver_->DoResolver();
        is_resolving_ = true;
        failed_times_ = 0;

        //      change_domain_resolver_timer_ = OnceTimer::create(3000, shared_from_this() );
        change_domain_resolver_timer_.start();

    }

    void IndexManager::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "IndexManager::Stop()");

        if( is_running_ == false ) return;

        // 停止定时器
        query_tracker_list_timer_.stop(); 
        query_stun_server_list_timer_.stop(); 
        query_index_server_list_timer_.stop();
        query_keyword_list_timer_.stop(); 
        //query_needreport_timer_.stop();
#ifdef NOTIFY_ON
        query_notify_server_list_timer_.stop();
#endif

        change_domain_resolver_timer_.stop(); 
        if (resolver_) { resolver_->Close(); resolver_.reset(); }

        trans_url_map_.clear();    

        is_running_ = false;

        inst_.reset();
    }
    /*
    void IndexManager::DoQueryRidByUrl(string url, string refer)
    {
    if( is_running_ == false ) return;
    if( is_have_index_server_list_ == false) return;

    // 根据url拼出 QureyRidByUrlRequestPacket 包，然后发送
    boost::uint32_t transaction_id_ = protocol::Packet::NewTransactionID();
    protocol::QueryRidByUrlPacket  query_rid_by_url_request_packet(
    transaction_id_, 
    protocol::PEER_VERSION, transaction_id_, url, refer, 
    AppModule::Inst()->GetUniqueGuid(),
    GetIndexEndPoint(url, refer)); 
    AppModule::Inst()->DoSendPacket( query_rid_by_url_request_packet);

    statistic::StatisticModule::Inst()->SubmitQueryRIDByUrlRequest();

    trans_url_map_.insert(std::make_pair(transaction_id_, storage::UrlInfo(url, refer)));
    // 注意：
    //   发包时 将 TransactionID 和 url 的对应关系 保存在 trans_url_map_ 中
    }

    void IndexManager::DoQueryRidByContent(string url, string refer, MD5 content_md5, uint32_t content_bytes, uint32_t file_length)
    {
    if( is_have_index_server_list_ == false) return;
    LOG(__EVENT, "index", "DoQueryRidByContent url:"<<url);

    LOG(__DEBUG, "user", "QueryContent: " << refer << " From: " << GetIndexEndPoint(content_md5));

    uint32_t transaction_id_ = protocol::Packet::NewTransactionID();
    protocol::QueryRidByContentPacket packet(transaction_id_, protocol::PEER_VERSION, content_md5, content_bytes, file_length, AppModule::Inst()->GetUniqueGuid(),GetIndexEndPoint(content_md5));
    trans_url_map_.insert(std::make_pair(transaction_id_, UrlInfo(url, refer)));
    AppModule::Inst()->DoSendPacket( packet);
    }

    void IndexManager::DoQueryhttpServerByRid(RID rid)
    {
    //放弃p2s
    return;

    LOG(__EVENT, "index", "DoQueryhttpServerByRid rid:"<<rid);

    if( is_running_ == false ) return;
    if( is_have_index_server_list_ == false) return;

    // 根据rif拼出 QureyHttpServerByRidRequestPacket 包，然后发送

    uint32_t transaction_id_ = protocol::Packet::NewTransactionID();
    protocol::QueryHttpServerByRidPacket query_http_server_by_rid_request_packet (transaction_id_, protocol::PEER_VERSION, rid,GetIndexEndPoint(rid));

    AppModule::Inst()->DoSendPacket(query_http_server_by_rid_request_packet);
    statistic::StatisticModule::Inst()->SubmitQueryHttpServersByRIDRequest();
    }
    */
    void IndexManager::DoQueryTrackerList()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "IndexManager::DoQueryTrackerList()");
        //COUT("DoQueryTrackerList" << server_list_endpoint_ << ", last_querytrackerlist_intervaltimes_=" << last_querytrackerlist_intervaltimes_);

        if( is_running_ == false ) return;


        query_tracker_list_timer_.interval(last_querytrackerlist_intervaltimes_);
        query_tracker_list_timer_.start();


        // 直接发送 QueryTrackerListRequestPacket 包

        uint32_t transaction_id_ = protocol::Packet::NewTransactionID();

        Guid unique_guid = this->peer_guid_;

        protocol::QueryLiveTrackerListPacket query_tracker_list_request_packet(
            transaction_id_, protocol::PEER_VERSION, this->peer_guid_, server_list_endpoint_);

        this->udp_server_->send_packet( query_tracker_list_request_packet , query_tracker_list_request_packet.peer_version_ );
        //        AppModule::Inst()->DoSendPacket( query_tracker_list_request_packet);

        //        statistic::StatisticModule::Inst()->SubmitQueryTrackerListRequest();

    }
    /*
    void IndexManager::DoQueryStunServerList()
    {
    LOG(__EVENT, "index", "DoQueryStunServerList");

    if( is_running_ == false ) return;

    query_stun_server_list_timer_.interval(last_querystunlist_intervaltimes_);
    query_stun_server_list_timer_.start();

    // 直接发送 QueryTrackerListRequestPacket 包

    uint32_t transaction_id_ = protocol::Packet::NewTransactionID();

    protocol::QueryStunServerListPacket query_stun_server_list_request_packet(transaction_id_, protocol::PEER_VERSION,server_list_endpoint_);
    AppModule::Inst()->DoSendPacket( query_stun_server_list_request_packet);
    //StatisticModule::Inst()->SubmitQueryTrackerListRequest();
    }

    void IndexManager::DoAddUrlRid(storage::UrlInfo url_info, storage::RidInfo rid_info, MD5 content_md5, uint32_t content_bytes, int flag)
    {
    LOG(__EVENT, "index", "DoAddUrlRid ,url_info:"<<url_info<<" rid_info:"<<rid_info);
    if( is_running_ == false ) return;
    if( is_have_index_server_list_ == false) return;

    if (url_info.url_.size() >= 1024 || url_info.refer_url_.size() >= 1024)
    {
    //忽略这次
    LOG(__WARN, "index", "DoAddUrlRid url_info.url_.size() >= 1024 || url_info.refer_url_.size() >= 1024");
    return;
    }

    //!
    // 根据 url, rid 拼出 AddRidUrlRequestPacket 包，然后发送
    boost::uint32_t transaction_id_ = protocol::Packet::NewTransactionID();
    // boost::uint16_t http_server_info_length = 2 + 1 + 2 + 2 + url_info.url_.length() + url_info.refer_url_.length();
    // boost::uint16_t resource_info_length_ = 2 + 16 + 4 + 2 + 4 + 16 * rid_info.GetBlockCount() + 2 + http_server_info_length;
    // boost::uint16_t this_length_ = 16 + resource_info_length_;

    std::vector<storage::UrlInfo> http_server_s_;
    http_server_s_.push_back(url_info);

    //!

    boost::asio::ip::udp::endpoint indexs_endpoint;
    if (flag == protocol::RID_BY_URL)
    {
    indexs_endpoint = GetIndexEndPoint(url_info.url_, url_info.refer_url_);
    }
    else if (flag == protocol::RID_BY_CONTENT)
    {
    indexs_endpoint = GetIndexEndPoint(content_md5);
    }
    protocol::AddRidUrlPacket add_rid_url_request_packet (transaction_id_, protocol::PEER_VERSION, 
    AppModule::Inst()->GetUniqueGuid(),rid_info.GetRID(), rid_info.GetFileLength(), rid_info.GetBlockSize(), rid_info.block_md5_s_,  http_server_s_, content_md5, content_bytes, indexs_endpoint);

    AppModule::Inst()->DoSendPacket(add_rid_url_request_packet);
    statistic::StatisticModule::Inst()->SubmitAddUrlRIDRequest();
    }
    */
    void IndexManager::OnUdpRecv(protocol::ServerPacket const & packet)
    {
        if( is_running_ == false ) 
            return;

        switch(packet.PacketAction)
        {
        case protocol::QueryRidByUrlPacket::Action:
            //            OnQueryRidByUrlPacket((protocol::QueryRidByUrlPacket const &)packet);
            break;
        case protocol::QueryRidByContentPacket::Action:
            //            OnQueryRidByContentPacket((protocol::QueryRidByContentPacket const &)packet);
            break;
        case protocol::QueryHttpServerByRidPacket::Action:
            //            OnQueryHttpServerByRidPacket((protocol::QueryHttpServerByRidPacket const &)packet);
            break;
        case protocol::QueryLiveTrackerListPacket::Action:
            OnQueryTrackerListPacket((protocol::QueryLiveTrackerListPacket const &)packet);
            break;
        case protocol::AddRidUrlPacket::Action:
            //            OnAddRidUrlPacket((protocol::AddRidUrlPacket const &)packet);
            break;
        case protocol::QueryStunServerListPacket::Action:
            //            OnQueryStunServerListPacket((protocol::QueryStunServerListPacket const &)packet);
            break;
        case protocol::QueryIndexServerListPacket::Action:
            //            OnQueryIndexServerListPacket((protocol::QueryIndexServerListPacket const &)packet);
            break;
            //         case protocol::QueryNeedReportPacket::Action:
            //             OnQueryNeedReportPacket((protocol::QueryNeedReportPacket const &)packet);
            //             break;
        case protocol::QueryKeyWordListPacket::Action:
            //            OnQueryKeywordListPacket((protocol::QueryKeyWordListPacket const &)packet);
            break;
            /*case protocol::QueryDataCollectionServerPacket::Action:
            OnQueryDataCollectionServerPacket((protocol::QueryDataCollectionServerPacket const &) packet);
            break;*/
        case protocol::QueryUploadPicProbabilityPacket::Action:
            //            OnQueryUploadPicProbabilityPacket(( protocol::QueryUploadPicProbabilityPacket const &)packet );
            break;
#if 0
        case protocol::QueryPushTaskPacket:
            {
                protocol::QueryPushTaskPacket const & response_packet =
                    protocol::QueryPushTaskPacket::ParseFromBinary(buf);
                if (!response_packet) {
                    assert(!"QueryPushTaskPacket Error!");
                }
                //MainThread::Post(boost::bind(&PushModule::OnPushTaskResponse, PushModule::Inst(), end_point, response_packet));
                PushModule::Inst()->OnPushTaskResponse(end_point, response_packet);
            }
            break;
#endif
#ifdef NOTIFY_ON
        case protocol::QueryNotifyListPacket::Action:
            {
                LOG(__DEBUG, "index", "收到QueryNotifyListResponsePacket");

                is_have_notify_server_ = true;
                LOG(__DEBUG, "index", "post OnGetNotifyServerList");
                global_io_svc().post(boost::bind(&p2sp::NotifyModule::OnGetNotifyServerList, NotifyModule::Inst(), ( protocol::QueryNotifyListPacket const &)packet));
            }
            break;
#endif
        default:
            assert(0);
        }
    }
    /*
    void IndexManager::OnQueryRidByUrlPacket(protocol::QueryRidByUrlPacket const & packet)
    {
    LOG(__EVENT, "index", "OnQueryRidByUrlPacket");

    if( is_running_ == false ) return;

    statistic::StatisticModule::Inst()->SubmitQueryRIDByUrlResponse();

    // 判断是否 出错
    //if( packet->ErrorCode() == 0 )
    if(packet.error_code_ == 0)
    {   // 正确
    // if( trans_url_map_ 找不到 trans_id )
    if (trans_url_map_.find(packet.transaction_id_) == trans_url_map_.end())
    {
    LOG(__EVENT, "index", "OnQueryRidByUrlPacket  Can Not Find TransactionID  return");
    return ;
    }
    //       不管
    // 找到 trans_id 对应的url
    storage::RidInfo rid_info ;//= packet->GetRidInfo();
    rid_info.rid_ = packet.response.rid_;
    rid_info.file_length_ = packet.response.file_length_;
    rid_info.block_size_ = packet.response.block_size_;
    rid_info.block_count_ = packet.response.block_md5_.size();
    rid_info.block_md5_s_ = packet.response.block_md5_;


    LOG(__EVENT, "index", "rid: " << rid_info.GetRID());
    LOG(__EVENT, "user", "QueryUrl Succeed.");

    storage::UrlInfo url_info = trans_url_map_.find(packet.transaction_id_)->second;

    LOG(__DEBUG, "storage", "AppModule::Inst()->AttachRidInfoToUrl, " << url_info.url_);
    AppModule::Inst()->AttachRidInfoToUrl(url_info.url_, rid_info, packet.response.content_sense_md5_,
    packet.response.content_bytes_, protocol::RID_BY_URL);
    }
    else
    {
    LOG(__EVENT, "index", "OnQueryRidByUrlPacket error no found");
    LOG(__ERROR, "user", "QueryUrl Failed.");
    // if( 服务器没有找到这个资源 )
    //     开启 纯下模式
    // else 
    //       其他错误处理
    }
    }

    void IndexManager::OnQueryRidByContentPacket(protocol::QueryRidByContentPacket const & packet)
    {
    LOG(__EVENT, "index", "OnQueryRidByContentPacket");

    if( is_running_ == false ) return;

    //! StatisticModule::Inst()->SubmitQueryRIDByUrlResponse();


    // 判断是否出错

    if( packet.error_code_ == 0 )
    {   // 正确
    // if( trans_url_map_ 找不到 trans_id )
    if (trans_url_map_.find(packet.transaction_id_) == trans_url_map_.end())
    {
    LOG(__EVENT, "index", "OnQueryRidByContentPacket  Can Not Find TransactionID  return");
    return ;
    }
    //       不管
    // 找到 trans_id 对应的url
    LOG(__EVENT, "user", "QueryContent Succeed.");
    storage::RidInfo rid_info ;//= packet->GetRidInfo();
    rid_info.rid_ = packet.response.resource_id_;
    rid_info.file_length_ = packet.response.file_length_;
    rid_info.block_size_ = packet.response.block_size_;
    rid_info.block_count_ = packet.response.block_md5_.size();
    rid_info.block_md5_s_ = packet.response.block_md5_;


    storage::UrlInfo url_info = trans_url_map_.find(packet.transaction_id_)->second;
    AppModule::Inst()->AttachRidInfoToUrl(url_info.url_, rid_info, packet.response.content_sense_md5_,
    packet.response.content_bytes_, protocol::RID_BY_CONTENT);
    //AppModule::Inst()->AttachRidToUrl( url, packet.GetRidInfo());
    }
    else if (packet.error_code_ == 2 )
    {
    LOG(__EVENT, "index", "OnQueryRidByContentPacket error no found");

    if (trans_url_map_.find(packet.transaction_id_) == trans_url_map_.end())
    {
    LOG(__EVENT, "index", "OnQueryRidByContentPacket  Can Not Find TransactionID  return");
    return ;
    }
    //       不管
    // 找到 trans_id 对应的url
    storage::UrlInfo url_info = trans_url_map_.find(packet.transaction_id_)->second;
    AppModule::Inst()->AttachContentStatusByUrl(url_info.url_, false);

    LOG(__ERROR, "user", "QueryContent Failed, No such Item.");
    // if( 服务器没有找到这个资源 )
    //     开启 纯下模式
    // else 
    //       其他错误处理
    //
    }
    else
    {
    LOG(__EVENT, "user", "QueryContent Failed, No such Resource.");
    }
    }

    void IndexManager::OnQueryHttpServerByRidPacket(protocol::QueryHttpServerByRidPacket const & packet)
    {
    LOG(__EVENT, "index", "OnQueryHttpServerByRidPacket");

    if( is_running_ == false ) return;

    statistic::StatisticModule::Inst()->SubmitQueryHttpServersByRIDResponse();

    // 判断时都出错
    if(packet.error_code_ == 0)
    {
    // 根据返回情况调用
    // 从packet中,解出每个 HttpServer 然后 合成一个 http_server_vector
    AppModule::Inst()->AddUrlInfo(packet.response.rid_,packet.response.url_infos_);
    }
    else
    {
    LOG(__EVENT, "index", "OnQueryHttpServerByRidPacket ERROR");
    // 不管
    }
    }
    */
    void IndexManager::OnQueryTrackerListPacket(protocol::QueryLiveTrackerListPacket const & packet)
    {
        LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnQueryTrackerListPacket");
        COUT("IndexManager::OnQueryTrackerListPacket ");

        if( is_running_ == false ) return;

        //        statistic::StatisticModule::Inst()->SubmitQueryTrackerListResponse();


        // 判断时都出错
        if(packet.error_code_ == 0)
        {
            // 成功收到服务器回复，定时器时间设置为比较长的时间
            query_tracker_list_timer_.interval( DEFAULT_QUERY_INTERVAL );
            query_tracker_list_timer_.start();
            is_have_tracker_list_ = true;
            failed_times_ = 0;
            resolve_times_ = 0;
            last_querytrackerlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;

            // 根据返回情况调用
            // 从packet中,解出每个 TrackerInfo 到 tracker_vector
            // TrackerManager::Inst()->SetTrackerList(group_count,tracker_vector);
            for (uint32_t i = 0; i < packet.response.tracker_info_.size(); i ++)
            {
                LOG4CPLUS_INFO(Loggers::Service(), "IndexManager Tracker List :[" << i << "]ModNo:" << packet.response.tracker_info_[i].ModNo << " IP:" << packet.response.tracker_info_[i].IP);
                COUT("Tracker List :[" <<i<<"] ModNo:"<<packet.response.tracker_info_[i].ModNo<<" IP: "<<packet.response.tracker_info_[i].IP);
            }
            TrackerManager::Inst()->SetTrackerList(packet.response.tracker_group_count_, packet.response.tracker_info_);
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::Service(), "IndexManager::OnQueryTrackerListPacketERROR");
        }
    }
    /*
    void IndexManager::OnAddRidUrlPacket(protocol::AddRidUrlPacket const & packet)
    {
    if( is_running_ == false ) return;

    statistic::StatisticModule::Inst()->SubmitAddUrlRIDResponse();

    }
    */

    void IndexManager::OnTimerElapsed(framework::timer::Timer * pointer)
    {
        if( is_running_ == false ) return;
        uint32_t times = pointer->times();

        if( pointer == &query_tracker_list_timer_ )
        {   // QueryTrackerList 定时器出发
            OnQueryTrackerListTimerElapsed(times);
        }
        else if( pointer == &query_stun_server_list_timer_ )
        {   // QueryTrackerList 定时器出发
            OnQueryStunServerListTimerElapsed(times);
        }
        else if (pointer == &query_index_server_list_timer_)
        {
            OnQueryIndexServerListTimerElapsed(times);
        }
        else if (pointer == &query_keyword_list_timer_)
        {
            OnQueryKeywordListTimerElapsed(times);
        }
        //         else if (pointer == &query_needreport_timer_)
        //         {
        //             OnQueryNeedReportTimerElapsed(times);
        //         }
        /*
        else if (pointer == &query_datacollectionserver_timer_)
        {
        OnQueryDataCollectionServerTimerElapsed(times);
        }*/
#ifdef NOTIFY_ON
        else if (pointer == &query_notify_server_list_timer_)
        {
            OnQueryNotifyServerTimerElapsed(times);
        }
#endif
        else if( pointer == &change_domain_resolver_timer_)
        {
            LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnTimerElapsed DoResolver");
            //COUT("IndexManager::OnTimerElapsed DoResolver failed_times_=" << failed_times_ << " " << is_have_change_domain_);

            assert(resolver_);
            // 收包失败，需要再次解析或者切换域名(如果已经切换域名，则不该有此定时器)
            if (failed_times_ >= 3 && false == is_have_change_domain_)
            {
                bool need_change_domain = !(is_have_index_server_list_ || is_have_tracker_list_
                    || is_have_stun_server_list_);
                ++resolve_times_;
                //COUT("resolve_times_=" << resolve_times_ << " need_change_domain=" << need_change_domain);
                // 解析三次都收包失败，并且三个回包一个都没收到，切换域名，关闭定时器
                if (resolve_times_>=3 && need_change_domain)
                {
                    if (resolver_) { resolver_->Close(); resolver_.reset(); }
                    resolver_ = Resolver::create(io_svc_, *this->asio_timer_manager_ , boss_domain_, port_, shared_from_this());
                    is_have_change_domain_ = true;
                    resolve_times_ = 0;
                    change_domain_resolver_timer_.stop();
                }
                else    // 不改变域名，再次尝试解析，开启一次性定时器
                {
                    change_domain_resolver_timer_.start();
                    failed_times_ = 0;
                }
                // 锟劫次斤拷锟斤拷
                resolver_->DoResolver();
                is_resolving_ = true;
                LOG4CPLUS_INFO(Loggers::Service(), "Resolve again.");
            }
            else    // 不需要切换域名，也不需要解析，只是开启一次性定时器
            {
                change_domain_resolver_timer_.start();
            }
        }
        else
        {    // 本类中不存在这个定时器
            //assert(!"No Such framework::timer::Timer");
            LOG4CPLUS_ERROR(Loggers::Service(), "IndexManager::OnTimerElapsed No Such framework::timer::Timer, Ignored");
        }
    }
    /*
    void IndexManager::OnQueryStunServerListPacket(protocol::QueryStunServerListPacket const & packet)
    {
    LOG(__INFO, "index", "IndexManager::OnQueryStunServerListPacket");

    if( is_running_ == false ) return;

    //StatisticModule::Inst()->SubmitQueryTrackerListResponse();

    // 判断时都出错
    if(packet.error_code_ == 0)
    {
    // 成功收到服务器回复，定时器时间设置为比较长的时间
    query_stun_server_list_timer_.interval( DEFAULT_QUERY_INTERVAL ); // 定义成为4个小时
    query_stun_server_list_timer_.start();
    is_have_stun_server_list_ = true;
    failed_times_ = 0;
    resolve_times_ = 0;
    last_querystunlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;

    // 根据返回情况调用
    // 从packet中,解出每个 TrackerInfo 到 tracker_vector
    // TrackerManager::Inst()->SetTrackerList(group_count,tracker_vector);
    StunModule::Inst()->SetStunServerList(packet.response.stun_infos_);
    }
    else
    {
    // 不管
    LOG(__INFO, "index", "IndexManager::OnQueryTrackerListPacketERROR");
    LOG(__ERROR, "user", "QuerySList Failed.");
    }

    }
    */
    /*
    void IndexManager::OnQueryNeedReportPacket( protocol::QueryNeedReportPacket const & packet )
    {
    LOG(__INFO, "index", "IndexManager::OnQueryNeedReportPacket");

    if( is_running_ == false ) return;

    // 判断是否出错
    if(packet.error_code_ == 0)
    {
    if (packet.response.need_report_ != 1 && packet.response.need_report_ != 0)
    {
    LOG(__ERROR, "index", "IndexManager::OnQueryNeedReportPacket, packet error!");
    assert(0);
    return ;
    }

    need_report_ = packet.response.need_report_;
    interval_time_ = packet.response.interval_time_;
    is_have_needreport_ = true;

    // need report, query data collection server
    if (NeedReport())
    {
    // prepare retry timer
    //query_datacollectionserver_timer_.interval(last_querydatacollectionserver_intervaltimes_);

    //    OnceTimer::create(last_querydatacollectionserver_intervaltimes_, shared_from_this());
    // query
    //DoQueryDataCollectionServer();
    }
    #ifdef NEED_LOG
    interval_time_ = 5;
    #endif
    DACStatisticModule::Inst()->SetIntervalTime(interval_time_);
    }
    else
    {
    // 不管
    LOG(__INFO, "index", "IndexManager::OnQueryNeedReportPacket");
    }
    }
    */
    /*
    void IndexManager::OnQueryUploadPicProbabilityPacket( protocol::QueryUploadPicProbabilityPacket const & packet )
    {
    if( is_running_ == false ) return;

    // 判断是否出错
    if(packet.error_code_ == 0)
    {
    upload_pic_probability_ = packet.response.upload_pic_probability_;
    LIMIT_MIN_MAX(upload_pic_probability_, 0.0f, 1.0f);
    }
    else
    {
    // 不管
    }

    }
    */

    void IndexManager::OnQueryKeywordListPacket(protocol::QueryKeyWordListPacket const & packet)
    {
        LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnQueryKeywordListPacket");
        if( is_running_ == false ) return;

        // 判断时都出错
        if(packet.error_code_ == 0)
        {
            // 成功收到服务器回复，定时器时间设置为比较长的时间
            query_keyword_list_timer_->interval( DEFAULT_QUERY_INTERVAL ); // 定义成为4个小时
            query_keyword_list_timer_.start();
            is_have_keyword_list_ = true;
            last_querykeywordlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;

            // extract
            keyword_list_.clear();
            keyword_list_.insert(keyword_list_.end(), packet.response.keywords_.begin(), packet.response.keywords_.end());
        }
        else
        {
            // 不管
            LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnQueryKeywordListPacketNoSucceed. error_code: " << packet.error_code_);
        }
    }
    /*
    void IndexManager::OnQueryDataCollectionServerPacket(protocol::QueryDataCollectionServerPacket const & packet)
    {
    if (false == is_running_)
    return ;

    if (packet.error_code_ == 0)
    {
    DATACOLLECTION_SERVER_INFO info = packet.response.server_info_;
    data_collection_server_endpoint_ = framework::network::Endpoint(info.IP, info.Port);

    is_have_datacollection_server_ = true;
    }
    else
    {
    // nothing
    }
    }
    */
    void IndexManager::OnQueryTrackerListTimerElapsed(uint32_t times)
    {
        if (false == is_running_)
            return ;

        // 如果是成功状态
        //     定时时间4个小时

        if (is_resolving_) return;
        ++failed_times_;

        //COUT("OnQueryTrackerListTimerElapsed failed_times_=" << failed_times_);

        DoQueryTrackerList();

        // 指数增长
        last_querytrackerlist_intervaltimes_ *= 2;
        if (last_querytrackerlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
            last_querytrackerlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;

        //         if (true == is_have_tracker_list_/*锟角凤拷锟秸碉拷锟斤拷trackerlist*/)
        //         {
        //             LOG(__INFO, "index", "IndexManager::OnQueryTrackerListTimerElapsed is_have_tracker_list_"<<is_have_tracker_list_);
        //             DoQueryTrackerList();
        //         }
        //         else
        //         {
        //             LOG(__INFO, "index", "IndexManager::OnQueryTrackerListTimerElapsed is_have_tracker_list_"<<is_have_tracker_list_);
        //             DoQueryTrackerList();
        //             last_querytrackerlist_intervaltimes_ *= 2;
        //             if (last_querytrackerlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
        //                 last_querytrackerlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
        //         }

        // 如果是失败状态
        //   指数退避，
    }

    void IndexManager::OnQueryStunServerListTimerElapsed(uint32_t times)
    {
        if (false == is_running_)
            return ;

        // 如果是成功状态
        //     定时时间4个小时

        if (is_resolving_) return;
        ++failed_times_;
        //        DoQueryStunServerList();

        // 指数增长
        last_querystunlist_intervaltimes_ *= 2;
        if (last_querystunlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
            last_querystunlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;

        //         if (true == is_have_stun_server_list_/*锟角凤拷锟秸碉拷锟斤拷trackerlist*/)
        //         {
        //             LOG(__INFO, "index", "IndexManager::OnQueryStunServerListTimerElapsed is_have_stun_server_list_"<<is_have_stun_server_list_);
        //             DoQueryStunServerList();
        //         }
        //         else
        //         {
        //             LOG(__INFO, "index", "IndexManager::OnQueryStunServerListTimerElapsed is_have_stun_server_list_"<<is_have_stun_server_list_);
        //             DoQueryStunServerList();
        // 
        //             last_querystunlist_intervaltimes_ *= 2;
        //             if (last_querystunlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
        //                 last_querystunlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
        //         }

        // 如果是失败状态
        //   指数退避，
    }

    void IndexManager::OnQueryIndexServerListTimerElapsed(uint32_t times)
    {
        if (false == is_running_)
            return ;

        if (is_resolving_) return;
        ++failed_times_;

        //        DoQueryIndexServerList();
        // 指数增长
        last_queryindexlist_intervaltimes_ *= 2;
        if (last_queryindexlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
            last_queryindexlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
    }

    void IndexManager::OnQueryKeywordListTimerElapsed(uint32_t times)
    {
        if (false == is_running_)
            return ;

        //        DoQueryKeywordList();

        last_querykeywordlist_intervaltimes_ *= 2;
        if (last_querykeywordlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
            last_querykeywordlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
    }
    /*
    void IndexManager::OnQueryNeedReportTimerElapsed(uint32_t times)
    {
    if (false == is_running_)
    return ;

    // 只查询一次
    if (true == is_have_needreport_) 
    return ;

    DoQueryNeedReport();

    last_queryneedreport_intervaltimes_ *= 2;
    if (last_queryneedreport_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
    last_queryneedreport_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
    }*/
    /*
    void IndexManager::OnQueryDataCollectionServerTimerElapsed(uint32_t times)
    {
    if (false == is_running_)
    return ;

    if (true == is_have_datacollection_server_)
    return ;

    DoQueryDataCollectionServer();

    last_querydatacollectionserver_intervaltimes_ *= 2;
    if (last_querydatacollectionserver_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
    last_querydatacollectionserver_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
    }
    */
#ifdef NOTIFY_ON
    void IndexManager::OnQueryNotifyServerTimerElapsed(boost::uint32_t times)
    {
        if (false == is_running_)
        {
            return;
        }

        if (is_have_notify_server_)
        {
            return;
        }

        //        DoQueryNotifyServerList();

        last_querynotifyserverlist_intervaltimes_ *= 2;
        if (last_querynotifyserverlist_intervaltimes_ > DEFAULT_QUERY_INTERVAL)
            last_querynotifyserverlist_intervaltimes_ = DEFAULT_QUERY_INTERVAL;
    }
#endif

#ifdef NOTIFY_ON
    void IndexManager::DoQueryNotifyServerList()
    {
        LOG(__EVENT, "index", "DoQueryNotifyServerList");

        if (!is_running_)
        {
            return;
        }

        query_notify_server_list_timer_.interval(last_querynotifyserverlist_intervaltimes_);
        query_notify_server_list_timer_.start();

        boost::uint32_t transaction_id = protocol::Packet::NewTransactionID();

        protocol::QueryNotifyListPacket query_notify_server_list_request_packet(transaction_id, protocol::PEER_VERSION, AppModule::Inst()->GetUniqueGuid(), server_list_endpoint_);

        AppModule::Inst()->DoSendPacket(query_notify_server_list_request_packet);
    }
#endif

    void IndexManager::DoQueryIndexServerList()
    {
        LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::DoQueryIndexServerList()");

        if( is_running_ == false ) return;

        query_index_server_list_timer_.interval(last_queryindexlist_intervaltimes_);
        query_index_server_list_timer_.start();

        // 直接发送 QueryIndexServerListRequestPacket 包

        uint32_t transaction_id = protocol::Packet::NewTransactionID();

        protocol::QueryIndexServerListPacket query_index_server_list_request_packet(transaction_id, 
            protocol::PEER_VERSION, this->peer_guid_,server_list_endpoint_);
        this->udp_server_->send_packet( 
            query_index_server_list_request_packet , query_index_server_list_request_packet.peer_version_ );
        //        AppModule::Inst()->DoSendPacket(query_index_server_list_request_packet);
    }

    //     void IndexManager::DoQueryNeedReport()
    //     {
    //         if (false == is_running_) return ;
    // 
    //         if (true == is_have_needreport_) return ;
    // 
    //         query_needreport_timer_.interval(last_querykeywordlist_intervaltimes_);
    //         query_needreport_timer_.start();
    // 
    //         uint32_t transaction_id = protocol::Packet::NewTransactionID();
    // 
    //         protocol::QueryNeedReportPacket query_need_report_request_packet (transaction_id, AppModule::Inst()->GetUniqueGuid(),server_list_endpoint_);
    //         AppModule::Inst()->DoSendPacket( query_need_report_request_packet);
    //     }
    /*
    void IndexManager::DoQueryKeywordList()
    {
    if (false == is_running_) return ;

    query_keyword_list_timer_->interval(last_querykeywordlist_intervaltimes_);
    query_keyword_list_timer_.start();

    uint32_t transaction_id = protocol::Packet::NewTransactionID();

    protocol::QueryKeyWordListPacket query_keyword_list_request_packet (transaction_id, AppModule::Inst()->GetUniqueGuid(),server_list_endpoint_);
    AppModule::Inst()->DoSendPacket( query_keyword_list_request_packet);
    }
    */
    /*
    void IndexManager::DoQueryDataCollectionServer()
    {
    if (false == is_running_) return ;

    if (true == is_have_datacollection_server_)
    return ;

    query_datacollectionserver_timer_.interval(last_querydatacollectionserver_intervaltimes_);
    query_datacollectionserver_timer_.start();

    uint32_t transaction_id = protocol::Packet::NewTransactionID();

    protocol::QueryDataCollectionServerPacket query_data_collection_server_request_ (transaction_id, AppModule::Inst()->GetUniqueGuid(),server_list_endpoint_);
    AppModule::Inst()->DoSendPacket( query_data_collection_server_request_);
    }
    */
    /*
    void IndexManager::DoQueryUploadPicProbability()
    {
    if (false == is_running_) return ;

    uint32_t transaction_id = protocol::Packet::NewTransactionID();

    protocol::QueryUploadPicProbabilityPacket  request_packet(transaction_id, AppModule::Inst()->GetUniqueGuid(),server_list_endpoint_);
    AppModule::Inst()->DoSendPacket( request_packet);
    }
    */
    void IndexManager::OnQueryIndexServerListPacket( protocol::QueryIndexServerListPacket const & packet )
    {
        LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnQueryIndexServerListPacket");

        if( is_running_ == false ) return;

        /*StatisticModule::Inst()->SubmitQueryTrackerListResponse();*/

        // 判断时都出错
        if(packet.error_code_ == 0)
        {
            // 成功收到服务器回复，定时器时间设置为比较长的时间
            query_index_server_list_timer_.interval( DEFAULT_QUERY_INTERVAL ); // 定义成为4个小时
            query_index_server_list_timer_.start();
            is_have_index_server_list_ = true;
            failed_times_ = 0;
            resolve_times_ = 0;
            last_queryindexlist_intervaltimes_ = INITIAL_QUERY_INTERVAL;

            // 根据返回情况调用
            mod_index_map_ = vector<boost::uint8_t>(packet.response.mod_index_map_, packet.response.mod_index_map_ + INDEX_LIST_MOD_COUNT);
            index_servers_ = packet.response.index_servers_;
            LOG4CPLUS_INFO(Loggers::Service(), "IndexManager mod_index_map_.size()= " << mod_index_map_.size() << ", index_servers_.size()" << index_servers_.size());
        }
        else
        {
            // 不管
            LOG4CPLUS_INFO(Loggers::Service(), "IndexManager::OnQueryIndexServerListPacketERROR");
        }
    }

    boost::asio::ip::udp::endpoint IndexManager::GetIndexEndPoint(string url, string refer)
    {
        uint32_t mod = boost::hash_value(Url2Mini(url, refer)) % 256;
        return ModToEndPoint(mod);
    }

    boost::asio::ip::udp::endpoint IndexManager::GetIndexEndPoint(Guid guid )
    {
        uint32_t mod = boost::hash_value(guid) % 256;
        return ModToEndPoint(mod);
    }

    inline boost::asio::ip::udp::endpoint IndexManager::ModToEndPoint( uint32_t mod )
    {
        protocol::INDEX_SERVER_INFO isi = index_servers_[ mod_index_map_[mod] ];
        return framework::network::Endpoint(isi.IP, isi.Port);
    }

    string IndexManager::Url2Mini(string url, string refer)
    {
        string url_tmp;
        if (refer.find("youku") == string::npos)
        {
            url_tmp = network::Uri(url).getrequest();
        }
        else
        {
            url_tmp = network::Uri(url).getfilerequest();
        }
        return url_tmp;
    }

    void IndexManager::SubmitDataOnDownloadStop(const protocol::DATA_ONDOWNLOADSTOP_INFO_EX& info, const protocol::UrlInfo& url_info, const std::vector<protocol::PLAY_STOP_INFO>& play_stop_infos)
    {
        if (false == is_running_)
            return ;

        if (NeedReport())
        {//need to be fixed
            /*
            uint32_t transaction_id = Packet::NewTransactionID();
            protocol::DataOnDownloadStopExRequestPacket const & request_packet =
            DataOnDownloadStopExRequestPacket::CreatePacket(transaction_id, AppModule::GetKernelVersionInfo(), AppModule::Inst()->GetUniqueGuid(), info, url_info.url_, url_info.refer_url_, play_stop_infos);
            // Send
            AppModule::Inst()->DoSendPacket(GetDataCollectionServerEndpoint(), request_packet);
            */
        }
    }

    bool IndexManager::NeedReport() const
    {
        if (false == is_running_)
            return false;
#ifdef NEED_LOG
        return true;
#else
        return need_report_;
#endif//NEED_LOG
    }
}
