#include "Common.h"
#include "UdpTrackerModule.h"
#include "TrackerLogger.h"

#include "protocol/UdpServer.h"
//#include "protocol/Cryptography.h"

#include "MainThread.h"
#include <fstream>
extern log4cplus::Logger g_logger;

using namespace framework;

namespace udptracker
{
    size_t UdpTrackerModule::check_alive_interval_in_seconds_;
    size_t UdpTrackerModule::dump_interval_in_seconds_;
    size_t UdpTrackerModule::dump_resource_count_;
    size_t UdpTrackerModule::internal_command_magic_number_;
    size_t UdpTrackerModule::udp_server_receive_count_;
    size_t UdpTrackerModule::statistic_log_interval_in_hours_;
    size_t UdpTrackerModule::copy_resource_interval_in_seconds_;
    size_t UdpTrackerModule::copy_resource_min_peer_count_;
    std::string UdpTrackerModule::copy_resource_destination_;
    size_t UdpTrackerModule::sample_interval_in_seconds_;

    UdpTrackerModule::p UdpTrackerModule::inst_;//(new UdpTrackerModule());

    UdpTrackerModule::UdpTrackerModule()
        : is_running_(false)
    {
        this->InitConfigValues();

        tracker_model_ = TrackerModel::Create(resource_dump_handler_);
    }

    void UdpTrackerModule::Start(u_short local_udp_port)
    {
        TRACK_INFO( "Starting UDP Tracker Module...");
        CONSOLE_LOG("正在启动Tracker模块...");

        if( is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"Tracker server is running");
            return;
        }

        ::protocol::UdpBuffer recv_buffer;
        ::protocol::IUdpArchive is(recv_buffer );
        framework::string::Uuid uid;
        ::protocol::ListPacket cpi;
        is >> cpi;

        asio_timer_manager_ = new AsioTimerManager( ::MainThread::IOS() , boost::posix_time::seconds(1) );
        asio_timer_manager_->start();

        // 启动Udp服务器
        udp_server_.reset(new protocol::UdpServer(::MainThread::IOS(), shared_from_this()));

        // 设置最小支持的版本号
        int mini_support_protocol_version = Config::Inst().GetInteger("tracker.min_support_protocol_version", 0);
        udp_server_->set_minimal_protocol_verion( (boost::uint16_t) mini_support_protocol_version );

        protocol::register_tracker_packet(*this->udp_server_);
        //			UdpServer::create(shared_from_this());
        TRACK_INFO( "UDP Server has been Created.");

        TRACK_INFO( "Try to listen on port " << local_udp_port);
        if( !udp_server_->Listen(local_udp_port) )
        {
            //! Release 打印不出来
            LOG4CPLUS_WARN(g_logger,"Udp Listen To port" << local_udp_port << ". Failed");
            CONSOLE_LOG("监听 " << local_udp_port << " 端口失败");
            udp_server_->Close();

            //  一旦监听失败，可以直接退出了
            exit( -1 );
            return;
        }

        TRACK_INFO( "Listen on port " << local_udp_port << " successfully.");
        CONSOLE_LOG("监听 " << local_udp_port << " 端口成功");

        sample_resource_dump_on_ = Config::Inst().GetBoolean("tracker.sample_resources_dump", false);

        dump_resource_map_ = false;
        dump_peer_list_ = false;
        dump_peer_list_last_sec_ = 0;
        dump_resource_map_last_sec_ = 0;

        // Statistic
        size_t res_hash_map_size = Config::Inst().GetInteger("tracker.res_hash_map_size",301057);
        if (res_hash_map_size > 1000*10000 && res_hash_map_size<1000)
        {
            res_hash_map_size = 301057;
        }
        tracker_statistic_ = TrackerStatistic::Inst();
        tracker_statistic_->Start(res_hash_map_size);

        // check alive timer
        check_alive_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , GetCheckAliveCheckIntervalInSeconds() * 1000 , 
            boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->check_alive_timer_ , 0 ) );

        // resource dump timer
        resource_dump_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , GetDumpResourceIntervalInSeconds() * 1000 , 
            boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->resource_dump_timer_ , 0 ) );

        // statistic timer
        statistic_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 1000 , 
            boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->statistic_timer_ , 0 ) );

		peer_statistic_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 60 * 1000 , 
			boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->peer_statistic_timer_ , 0 ) );

        copy_resource_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , GetCopyResourceIntervalInSeconds() * 1000 , 
            boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &copy_resource_timer_ , 0 ) );

        if (sample_resource_dump_on_)
        {
            sample_resource_dump_timer_ =new framework::timer::PeriodicTimer( * this->asio_timer_manager_ , GetSampleIntervalInSeconds() * 1000 , 
                boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->sample_resource_dump_timer_ , 0 ) );
        }

        // dump
        dump_on_ = false;

        // check resources
        check_resources_on_ = Config::Inst().GetBoolean("tracker.check_resources", false);

        //report报文的时候，是否使用stun发现的ip作为该节点的出口ip
        use_stun_detect_ip_ = Config::Inst().GetBoolean("tracker.use_stun_detect_ip", true);

        LOG4CPLUS_INFO(g_logger, "use_stun_detect_ip_:"<<use_stun_detect_ip_);

        // 初始化Tracker Model
        tracker_model_->Start();

        // Start to receive packages
        TRACK_INFO( "Start to Receive packets: " << GetUdpServerReceiveCount());

        CONSOLE_LOG("开始收包,队列长度:" << GetUdpServerReceiveCount());
        udp_server_->Recv( GetUdpServerReceiveCount() );

        // sample resources
        LoadSampleResources();

        // 定时器
        resource_dump_timer_->start();
        check_alive_timer_->start();
        statistic_timer_->start();
        copy_resource_timer_->start();
        peer_statistic_timer_->start();
        if (sample_resource_dump_on_)
        {
            sample_resource_dump_timer_->start();
        }

        // Running状态
        is_running_ = true;

        LOG4CPLUS_DEBUG(g_logger, "UDP Tracker Module is started correctly." );
        CONSOLE_LOG("UDP Tracker模块启动完毕");
    }

    void UdpTrackerModule::Stop()
    {
        LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::Stop");
        CONSOLE_LOG("UDP Tracker模块正在停止...");

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"Tracker模块没有运行");
            return;
        }

        // 定时器
        if (check_alive_timer_) check_alive_timer_->cancel();
        if (resource_dump_timer_) resource_dump_timer_->cancel();
        if (statistic_timer_) statistic_timer_->cancel();
        if (copy_resource_timer_) copy_resource_timer_->cancel();
        if (sample_resource_dump_timer_) sample_resource_dump_timer_->cancel();
        if (peer_statistic_timer_) peer_statistic_timer_->cancel();

        udp_server_->Close();

        tracker_model_->Stop();

        tracker_statistic_->Stop();

        is_running_ = false;

        LOG4CPLUS_DEBUG(g_logger,"Tracker Module stops successfully.");
        CONSOLE_LOG("Tracker模块成功停止...");
    }

    void UdpTrackerModule::OnUdpRecv(protocol::Packet const & packet)
    {
        TRACK_DEBUG("UdpTrackerModule::OnUdpRecv: " << packet.end_point);

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnUdpRecv: Tracker is not running. Return.");
            return;
        }

        TRACK_DEBUG( "UDP Received, TID: " << packet.TransactionID);

        // Tracker 相关协议
        if( packet.PacketAction >= 0x30 && packet.PacketAction < 0x50 )
        {
            LOG4CPLUS_DEBUG(g_logger, "Dispatching action: 0x" << std::hex << packet.PacketAction);
            DispatchAction(packet.end_point, packet.PacketAction, (protocol::ServerPacket &)packet);
        }
        else
        {
            RELEASE_LOG("Unknown Action：0x" << std::hex << packet.PacketAction << ", from " << packet.end_point);
        }
    }

    void UdpTrackerModule::DispatchAction(const boost::asio::ip::udp::endpoint& end_point, boost::uint8_t action, ::protocol::ServerPacket const & packet)
    {
        LOG4CPLUS_TRACE(g_logger,"UdpTrackerModule::DispatchAction: " << end_point << ", action: 0x" << std::hex << int(action));

        //added by youngkyyang
        tracker_statistic_->AddCmdTimes(action);

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::DispatchAction: Tracker Module is not running. Return.");
            return;
        }

        if (!( action >= 0x30 && action < 0x50 ))
        {
            LOG4CPLUS_WARN(g_logger, "UdpTrackerModule::DispatchAction: Action 0x" << std::hex << action << " is not accepted by UdpTrackerServer.");
            return ;
        }

        if (false == packet.IsRequest)
        {
            LOG4CPLUS_WARN(g_logger, "UdpTrackerModule::DispatchAction: This Packet is NOT a Request!");
            return ;
        }

        // 解析 Tracker 协议
        switch(action)
        {
        case protocol::ListPacket::Action:
            {
                // statistic
                tracker_statistic_->SubmitListRequest( packet.length() );
                LOG4CPLUS_TRACE(g_logger,"Post to OnListRequest");
                OnListRequest(end_point, (protocol::ListPacket &)packet);
                break;
            }
        case protocol::ListTcpPacket::Action:
            {
                tracker_statistic_->SubmitListRequest( packet.length() );	
                LOG4CPLUS_DEBUG(g_logger, "Post to OnListTcpRequest");
                OnListTcpRequest(end_point, (protocol::ListTcpPacket &)packet);
                break;
            }           
        case protocol::ListWithIpPacket::Action:
            {
                tracker_statistic_->SubmitListRequest( packet.length() );	
                LOG4CPLUS_DEBUG(g_logger, "Post to OnListWithIpRequest");
                OnListWithIpRequest(end_point, (protocol::ListWithIpPacket &)packet);
                break;
            }
        case protocol::ListTcpWithIpPacket::Action:
            {
                tracker_statistic_->SubmitListRequest( packet.length() );	
                LOG4CPLUS_DEBUG(g_logger, "Post to OnListTcpWithIpRequest");
                OnListTcpWithIpRequest(end_point, (protocol::ListTcpWithIpPacket &)packet);
                break;
            }     
  
        case protocol::LeavePacket::Action:
            {
                // statistic
                tracker_statistic_->SubmitLeaveRequest( packet.length() );

                LOG4CPLUS_TRACE(g_logger, "Post to OnLeaveRequest");
                OnLeaveRequest(end_point, (protocol::LeavePacket&)packet);
                break;
            }
        case protocol::ReportPacket::Action:
            {
                // statistic
                tracker_statistic_->SubmitReportRequest( packet.length() );
                LOG4CPLUS_TRACE(g_logger, "Post to OnReportRequest");
                OnReportRequest(end_point, (protocol::ReportPacket&)packet);
                break;
            }
        case protocol::QueryPeerCountPacket::Action:
            {			
                OnQueryPeerCountRequest( end_point, (protocol::QueryPeerCountPacket&)packet );
                break;
            }
        case protocol::InternalCommandPacket::Action:
            {
                LOG4CPLUS_WARN(g_logger, "Post to OnInternalCommand");
                OnInternalCommandRequest(end_point, (protocol::InternalCommandPacket&)packet);
                break;
            }
        case protocol::QueryPeerResourcesPacket::Action:
            {
                LOG4CPLUS_INFO(g_logger,"QueryPeerResources,Action:0x"<<std::hex<<action);
                OnQueryPeerResourcesRequest(end_point, (protocol::QueryPeerResourcesPacket&)packet);
                break;
            }
        case protocol::QueryTrackerStatisticPacket::Action:
            {
                LOG4CPLUS_INFO(g_logger,"QueryTrackerStatisticPacket,Action:0x"<<std::hex<<action);
                OnQueryTrackerStatisticRequest(end_point,(protocol::QueryTrackerStatisticPacket&)packet);
                break;
            }
        default:
            {
                tracker_statistic_->SubmitRequestError(packet.length());
                LOG4CPLUS_WARN(g_logger, "Ignoring unknown action: 0x" << std::hex << int(action));
                RELEASE_LOG("Ignoring unknown action: 0x" << std::hex << action << ",from " << end_point);
                break;
            }
        }
    }

    int UdpTrackerModule::PeerIdCheck(const Guid& peer_id,bool check_frequent)
    {
        if (peer_id.is_empty())
        { 
            LOG4CPLUS_WARN(g_logger, "peer_id empty");
            return PEERID_CHECK_FAILED;
        }
        if (check_frequent && tracker_model_->IsTooFrequent(peer_id))
        {
            LOG4CPLUS_WARN(g_logger, "peer_id "<<peer_id<<" frequent check failed");
            return FREQUENT_CHECK_FAILED;
        }
        return 0;
    }
    int UdpTrackerModule::PeerIdRidCheck(const Guid& peer_id,bool check_frequent,const RID& rid)
    {
        if (rid.is_empty())
        {
            return RESOURCEID_CHECK_FAILED;
        }
        return PeerIdCheck(peer_id,check_frequent);
    }

    void UdpTrackerModule::OnListRequest(
        const boost::asio::ip::udp::endpoint& end_point, 
        protocol::ListPacket & list_request)
    {
         LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::OnListRequest: " << end_point);

        if( !is_running_ )
        { 
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnListRequest: Tracker Module is not running. Return.");
            return;
        }

        RID resource_id = list_request.request.resource_id_;
        Guid peer_guid = list_request.request.peer_guid_;          

        int check_ret = PeerIdRidCheck(peer_guid,false,resource_id);
        if (check_ret < 0)
        {
            LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret<<" version:"<<int(list_request.peer_version_)<<" endpoint:"<<end_point);
            tracker_statistic_->SubmitListRequestError(list_request.length());
            return;
        }       

        boost::uint16_t peer_request_count = list_request.request.request_peer_count_;

        LOG4CPLUS_DEBUG(g_logger," ResourceID: " << resource_id<<" Peer Request Count: " << peer_request_count<<" Peer Guid: " << peer_guid);

        boost::int16_t resource_count = tracker_model_->UpdatePeerKeepAliveTime(peer_guid);

        // list peers
        std::vector<protocol::CandidatePeerInfo> peerInfos;
        //std::vector<udptracker::PeerInfo> peerInfos;
        tracker_model_->ListPeers(resource_id, peer_request_count, peerInfos , end_point,list_request.request.peer_nat_type_);

        // packet
        boost::shared_ptr<protocol::ListPacket> response (
            new protocol::ListPacket( 
            list_request.transaction_id_, 
            0, // error code
            resource_id,list_request.end_point
            ) );   
        response->response.peer_infos_.swap(peerInfos);

        // statistic
        tracker_statistic_->SubmitListResponse(response->length());

        tracker_statistic_->AddRidTimes(resource_id);

        tracker_statistic_->AddIpTimes(end_point.address().to_string());
        
        //hit request times, added by youngkyyang 2012/02/22
        if (!response->response.peer_infos_.empty())
        {
            tracker_statistic_->AddHitCmdTimes(protocol::ListPacket::Action);
        }
           
        tracker_statistic_->InsertStatRidHashMap(resource_id, response->response.peer_infos_.size());

        // post
        //! 发包直接调用
        DoSendPacket(end_point, response , list_request.peer_version_ );

        LOG4CPLUS_DEBUG(g_logger,"Resource Count: " << tracker_model_->TotalResourcesCount() << " Peer Count: " << tracker_model_->TotalPeersCount());
    }

    void UdpTrackerModule::OnListTcpRequest(
        const boost::asio::ip::udp::endpoint& end_point, 
        protocol::ListTcpPacket & list_request)
    {
        TRACK_DEBUG("UdpTrackerModule::OnListRequest: " << end_point);

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnListRequest: Tracker Module is not running. Return.");
            tracker_statistic_->SubmitListRequestError(list_request.length());
            return;
        }

        RID resource_id = list_request.request.resource_id_;
        Guid peer_guid = list_request.request.peer_guid_;

        int check_ret = PeerIdRidCheck(peer_guid,false,resource_id);
        if (check_ret < 0)
        {
            LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret);
            return;
        }

        boost::uint16_t peer_request_count = list_request.request.request_peer_count_;

        LOG4CPLUS_DEBUG(g_logger," ResourceID: " << resource_id<<" Peer Request Count: " << peer_request_count<<" Peer Guid: " << peer_guid);

        boost::int16_t resource_count = tracker_model_->UpdatePeerKeepAliveTime(peer_guid);
        // list peers
        std::vector<protocol::CandidatePeerInfo> peerInfos;
        //查询tcp节点，是无需通过nat类型来设置优先级的，因此这里传了TYPE_ERROR
        tracker_model_->ListTcpPeers(resource_id, peer_request_count, peerInfos , end_point,TYPE_ERROR);

        // packet
        boost::shared_ptr<protocol::ListTcpPacket> response (
            new protocol::ListTcpPacket( 
            list_request.transaction_id_, 
            0, // error code
            resource_id, list_request.end_point
            ) );
        response->response.peer_infos_.swap(peerInfos);

        // statistic
        tracker_statistic_->SubmitListResponse(response->length());

        tracker_statistic_->AddRidTimes(resource_id);

        tracker_statistic_->AddIpTimes(end_point.address().to_string());

        //hit request times, added by youngkyyang 2012/02/22
        if (!response->response.peer_infos_.empty())
        {
            tracker_statistic_->AddHitCmdTimes(protocol::ListTcpPacket::Action);
        }
            
        tracker_statistic_->InsertStatRidHashMap(resource_id, response->response.peer_infos_.size());
      

        // post
        //! 发包直接调用
        DoSendPacket(end_point, response , list_request.peer_version_ );

        LOG4CPLUS_DEBUG(g_logger,"Resource Count: " << tracker_model_->TotalResourcesCount() << " Peer Count: " << tracker_model_->TotalPeersCount());
    }

    void UdpTrackerModule::OnListWithIpRequest(
        const boost::asio::ip::udp::endpoint& end_point, 
        protocol::ListWithIpPacket & list_request)
    {
        TRACK_DEBUG("UdpTrackerModule::OnListWithIpRequest: " << end_point);

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnListWithIpRequest: Tracker Module is not running. Return.");
            tracker_statistic_->SubmitListRequestError(list_request.length());
            return;
        }

        RID resource_id = list_request.request.resource_id_;
        Guid peer_guid = list_request.request.peer_guid_;

        int check_ret = PeerIdRidCheck(peer_guid,false,resource_id);
        if (check_ret < 0)
        {
            LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret);
            return;
        }

        boost::uint16_t peer_request_count = list_request.request.request_peer_count_;

        LOG4CPLUS_DEBUG(g_logger," ResourceID: " << resource_id<<" Peer Request Count: " << peer_request_count<<" Peer Guid: " << peer_guid);

        boost::int16_t resource_count = tracker_model_->UpdatePeerKeepAliveTime(peer_guid);
        // list peers
        std::vector<protocol::CandidatePeerInfo> peerInfos;
        tracker_model_->ListPeers(resource_id, peer_request_count, peerInfos , end_point,list_request.request.reqest_ip_);

        // packet
        boost::shared_ptr<protocol::ListWithIpPacket> response (
            new protocol::ListWithIpPacket( 
           list_request.transaction_id_, 
            0, // error code
            resource_id, list_request.end_point
            ) );
        response->response.peer_infos_.swap(peerInfos);
        // statistic
        tracker_statistic_->SubmitListResponse(response->length());

        tracker_statistic_->AddRidTimes(resource_id);

        tracker_statistic_->AddIpTimes(boost::asio::ip::address_v4(list_request.request.reqest_ip_).to_string());

        //hit request times, added by youngkyyang 2012/02/22
        if (!response->response.peer_infos_.empty())
        {
            tracker_statistic_->AddHitCmdTimes(protocol::ListWithIpPacket::Action);
        }
            
        tracker_statistic_->InsertStatRidHashMap(resource_id, response->response.peer_infos_.size());
        
        // post
        //! 发包直接调用
        DoSendPacket(end_point, response , list_request.peer_version_ );

        LOG4CPLUS_DEBUG(g_logger,"Resource Count: " << tracker_model_->TotalResourcesCount() << " Peer Count: " << tracker_model_->TotalPeersCount());
    }

    void UdpTrackerModule::OnListTcpWithIpRequest(
        const boost::asio::ip::udp::endpoint& end_point, 
        protocol::ListTcpWithIpPacket & list_request)
    {
         LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::OnListTcpWithIpRequest: " << end_point);

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnListTcpWithIpRequest: Tracker Module is not running. Return.");
            tracker_statistic_->SubmitListRequestError(list_request.length());
            return;
        }

        RID resource_id = list_request.request.resource_id_;
        Guid peer_guid = list_request.request.peer_guid_;

        int check_ret = PeerIdRidCheck(peer_guid,false,resource_id);
        if (check_ret < 0)
        {
            LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret);
            return;
        }

        boost::uint16_t peer_request_count = list_request.request.request_peer_count_;

        LOG4CPLUS_DEBUG(g_logger," ResourceID: " << resource_id<<" Peer Request Count: " << peer_request_count<<" Peer Guid: " << peer_guid);

        boost::int16_t resource_count = tracker_model_->UpdatePeerKeepAliveTime(peer_guid);
        // list peers
        std::vector<protocol::CandidatePeerInfo> peerInfos;
        tracker_model_->ListTcpPeers(resource_id, peer_request_count, peerInfos , end_point,list_request.request.reqest_ip_);

        // packet
        boost::shared_ptr<protocol::ListTcpWithIpPacket> response (
            new protocol::ListTcpWithIpPacket( 
            list_request.transaction_id_, 
            0, // error code
            resource_id,list_request.end_point
            ) );
        response->response.peer_infos_.swap(peerInfos);
        // statistic
        tracker_statistic_->SubmitListResponse(response->length());

        tracker_statistic_->AddRidTimes(resource_id);

        tracker_statistic_->AddIpTimes(boost::asio::ip::address_v4(list_request.request.reqest_ip_).to_string());

        //hit request times, added by youngkyyang 2012/02/22
        if (!response->response.peer_infos_.empty())
        {
            tracker_statistic_->AddHitCmdTimes(protocol::ListTcpWithIpPacket::Action);
        }
            
        tracker_statistic_->InsertStatRidHashMap(resource_id, response->response.peer_infos_.size());
        
        // post
        //! 发包直接调用
        DoSendPacket(end_point, response , list_request.peer_version_ );

        LOG4CPLUS_DEBUG(g_logger, "Resource Count: " << tracker_model_->TotalResourcesCount() << " Peer Count: " << tracker_model_->TotalPeersCount());
    }



    void UdpTrackerModule::OnReportRequest( const boost::asio::ip::udp::endpoint& end_point,
        protocol::ReportPacket & report_request)
    {
        LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::OnReportRequest: " << end_point);
        tracker_statistic_->AddIpTimes(end_point.address().to_string());

        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnReportRequest: Tracker Module is not running. Return.");
            return;
        }
        // get fields
        Guid peer_guid = report_request.request.peer_guid_;

        int check_ret = PeerIdCheck(peer_guid,true);
        if (check_ret < 0)
        {
            LOG4CPLUS_WARN(g_logger,"check failed,ret is:"<<check_ret<<" peerversion:"<<int(report_request.peer_version_)<<" endpoint"<<end_point);
            tracker_statistic_->SubmitReportRequestError(report_request.length());
            return;
        }        

        std::vector<boost::uint32_t> real_ips = report_request.request.real_ips_;
        if (real_ips.size() == 0)
        {
            tracker_statistic_->SubmitReportRequestError(report_request.length());
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnReportRequest: Real IPs number is zero. Ignore. (" << end_point << ", " << report_request.transaction_id_ << ")"
                <<" peerversion:"<<int(report_request.peer_version_));
            return ;
        }

        boost::uint16_t udp_port = report_request.request.udp_port_;// ->GetUdpPort();
        boost::uint32_t stun_peer_ip = report_request.request.stun_peer_ip_;// ->GetStunPeerIP();
        boost::uint16_t stun_peer_udp_port = report_request.request.stun_peer_udp_port_;// ->GetStunPeerUdpPort();
        boost::uint8_t rid_count = report_request.request.resource_ids_.size();// ->GetResourceCount();
        const std::vector<protocol::REPORT_RESOURCE_STRUCT>& resource_updates = report_request.request.resource_ids_;// ->GetResourceIDs();
        boost::uint16_t client_resource_count = report_request.request.local_resource_count_;// ->GetLocalResourceCount();
        boost::uint8_t peer_nat_type = report_request.request.peer_nat_type_;// ->GetPeerNatType();
        boost::uint8_t upload_priority = report_request.request.upload_priority_;// ->GetUploadPriority();
        boost::uint8_t idle_time_in_mins = report_request.request.idle_time_in_mins_;// ->GetIdleTimeInMins();

        LOG4CPLUS_DEBUG( g_logger, " Peer Guid: " << peer_guid<<" UDP Port: " << udp_port<<" TCP Port: " << report_request.request.upnp_tcp_port_<<" Local Resource Count: " 
            << client_resource_count<<" Upload Priority: " << (boost::uint32_t)upload_priority<<" Idle Time (min): " << (boost::uint32_t)idle_time_in_mins);

        // create peer info
        PeerInfo peerInfo;
        peerInfo.StunIP = stun_peer_ip;
        peerInfo.StunUdpPort = stun_peer_udp_port;
        peerInfo.PeerVersion = report_request.peer_version_;
        peerInfo.UdpPort = udp_port;
        //        peerInfo.DetectUdpPort = commit_request->GetStunDetectUdpPort();
        peerInfo.DetectIP = end_point.address().to_v4().to_ulong();

        //ck 20120626新加上的逻辑，如果tracker看到的ip和stun看到的ip不同的时候，进行特殊的处理。
        if(use_stun_detect_ip_ && report_request.request.stun_detected_ip_!=0)
        {
            LOG4CPLUS_DEBUG( g_logger, "stun_detected_ip:"<<report_request.request.stun_detected_ip_<<" DetectIP:"<<peerInfo.DetectIP
                <<" stun_detected_udp_port_:"<<report_request.request.stun_detected_udp_port_);
            if(report_request.request.stun_detected_ip_ != 0)
            {
                peerInfo.DetectIP = report_request.request.stun_detected_ip_;
            }
            peerInfo.UdpPort =  report_request.request.stun_detected_udp_port_;
        }

        peerInfo.PeerNatType = peer_nat_type;
        peerInfo.UploadPriority = upload_priority;
        peerInfo.IdleTimeInMins = idle_time_in_mins;

        peerInfo.upload_bandwidth_kbs = report_request.request.upload_bandwidth_kbs_;
        peerInfo.upload_limit_kbs = report_request.request.upload_limit_kbs_;
        peerInfo.upload_speed_kbs = report_request.request.upload_speed_kbs_;

        //从version9之后才有tcpport，PeerInfo的默认构造函数设置为0
        if (report_request.peer_version_ >= protocol::PEER_VERSION_V9)
        {
            peerInfo.upnp_tcp_port = report_request.request.upnp_tcp_port_;
            peerInfo.internal_tcp_port = report_request.request.internal_tcp_port_;
        }


        //        if (report_request->GetStunDetectUdpPort() == 0)
        if (report_request.request.stun_detected_udp_port_ == 0)
            peerInfo.DetectUdpPort = end_point.port();
        else
            peerInfo.DetectUdpPort = report_request.request.stun_detected_udp_port_;

        if (peerInfo.DetectUdpPort == 0)
        {
            tracker_statistic_->SubmitReportRequestError(report_request.length());
            LOG4CPLUS_WARN(g_logger,"Detected Udp Port is 0. Ignore. (" << end_point
                << ")");
            return ;
        }

        // choose an IP
        if (std::find(real_ips.begin(), real_ips.end(), peerInfo.DetectIP) != real_ips.end())
        {
            peerInfo.IP = peerInfo.DetectIP;
        }
        else
        {
            std::vector<boost::uint32_t>::iterator it = 
                std::find_if(real_ips.begin(), real_ips.end(), std::bind2nd(std::greater<boost::uint32_t>(), 0));
            if (it == real_ips.end())
            {
                tracker_statistic_->SubmitReportRequestError(report_request.length());
                LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnReportRequest, Can not find NonZero IP. Ignore. From " << end_point );
                return ;
            }
            peerInfo.IP = *it;
        }

        // check null guid
        STL_FOR_EACH_CONST(std::vector<protocol::REPORT_RESOURCE_STRUCT>, resource_updates, iter)
        {
            if (iter->ResourceID.is_empty())
            {
                tracker_statistic_->SubmitReportRequestError(report_request.length());
                LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnReportRequest, Report Null Resource ID. Ignore. From " << end_point);
                return ;
            }
        }

        //        if (report_request->GetResourceCount() == 0 && report_request->GetLocalResourceCount() != report_request->GetServerResourceCount())
        if (report_request.request.resource_ids_.size() == 0 && report_request.request.local_resource_count_ != report_request.request.server_resource_count_)
        {
            tracker_statistic_->SubmitReportRequestError(report_request.length());
            // TODO: fix this
            //            RELEASE_LOG("UdpTrackerModule::OnReportRequest, Invalid Report Request (report == 0 but local != server):" << report_request->GetBuffer().data_.get());            return ;
        }

        // report
       boost::uint16_t resource_count = tracker_model_->ReportPeerInfo(peer_guid, resource_updates, peerInfo, client_resource_count);

        // add random sample
        if (sample_resources_random_.size() < 500)
        {
            for (int i = 0; i < resource_updates.size(); ++i)
            {
                //if (resource_updates[i].Type == 1 && tracker_model_->GetResourcePeerCount(resource_updates[i].ResourceID) >= 300)
                if (resource_updates[i].Type == 1)
                {
                    sample_resources_random_.insert(resource_updates[i].ResourceID);
                }
            }
        }

        // non-match
        //        if (report_request->GetResourceCount() == 0 && report_request->GetLocalResourceCount() != resource_count)
        if (report_request.request.resource_ids_.size() == 0 && report_request.request.local_resource_count_ != resource_count)
        {
            tracker_statistic_->SubmitReportNonMatch(report_request.length());
        }

        // response
        boost::shared_ptr<protocol::ReportPacket> response (
            new protocol::ReportPacket(
            //            protocol::ReportResponsePacket::CreatePacket(
            report_request.transaction_id_,
            0,
            GetKeepAliveIntervalInSeconds(),
            peerInfo.DetectIP, peerInfo.DetectUdpPort, 
            resource_count , report_request.end_point
            ) );

        // statistic
        tracker_statistic_->SubmitReportResponse(response->length());

        DoSendPacket(end_point, response , report_request.peer_version_);

         LOG4CPLUS_DEBUG(g_logger, "Resource Count: " << tracker_model_->TotalResourcesCount() << " Peer Count: " << tracker_model_->TotalPeersCount());
    }

     
    void UdpTrackerModule::OnLeaveRequest( const boost::asio::ip::udp::endpoint& end_point, 
        protocol::LeavePacket & leave_request)
    {
        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnLeaveRequest: Tracker Module is not running. Return.");
            return;
        }

        tracker_model_->RemovePeer(leave_request.peer_guid_);
    }

    void UdpTrackerModule::OnInternalCommandRequest(const boost::asio::ip::udp::endpoint& end_point, 
        protocol::InternalCommandPacket & icommand_request)
    {
        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnInternalCommandRequest: Tracker Module is not running. Return.");
            return;
        }
        if (icommand_request.magic_number_ != GetInternalCommandMagicNumber() ) {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnInternalCommandRequest: Magic Number not matached " << icommand_request.magic_number_);
            return;
        }
        boost::uint16_t icommand = icommand_request.command_id_;
        boost::uint32_t pcurrent_sec = framework::timer::TickCounter::tick_count() / 1000;

        switch (icommand) {
        case TYPE_ICOMMAND_DUMPPEER:
            if ((pcurrent_sec - dump_peer_list_last_sec_) > INTERNAL_COMMAND_INTERVAL) { 
                dump_peer_list_ = true;
                dump_peer_list_last_sec_ = pcurrent_sec;
            }else{
                LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::OnInternalCommandRequest: DUMPPEER command too frequent!, ignored.");
            }
            break;
        case TYPE_ICOMMAND_DUMPRESOURCE:
            if ((pcurrent_sec - dump_resource_map_last_sec_) > INTERNAL_COMMAND_INTERVAL) { 
                dump_resource_map_ = true;
                dump_resource_map_last_sec_ = pcurrent_sec;
            }else{
                LOG4CPLUS_DEBUG(g_logger,"UdpTrackerModule::OnInternalCommandRequest: DUMPRESOURCE command too frequent!, ignored.");
            }
            break;
        case ENABLE_PRINT_RID_PEER_STATISTIC:
            {
                boost::uint32_t args = icommand_request.reserved_;
                boost::shared_ptr<protocol::InternalCommandPacket> response ( new protocol::InternalCommandPacket( 
                    icommand_request.transaction_id_, 
                    0,  // error_code
                    icommand_request.end_point
                    ) );

                if ((args>>16) > 0)
                {
                    tracker_statistic_->SetIsPrint(true);
                    tracker_statistic_->SetPrintTimes(args & 0xffff);
					peer_statistic_timer_->interval((args >> 16) * 60);
                    LOG4CPLUS_INFO(g_logger,"set interval:"<<((args >> 16) * 60)<<" interval:"<<peer_statistic_timer_->interval());
                    Timer::pointer tp = peer_statistic_timer_;
                    LOG4CPLUS_INFO(g_logger,"set interval:"<<((args >> 16) * 60)<<"tp interval:"<<tp->interval());
                }
                else
                {
                    response->error_code_ = 1;
                }

                DoSendPacket(end_point, response , icommand_request.peer_version_);
            }
            break;
        case DISABLE_PRINT_RID_PEER_STATISTIC:
            {
                tracker_statistic_->SetIsPrint(false);
                tracker_statistic_->SetPrintTimes(0);

                boost::shared_ptr<protocol::InternalCommandPacket> response ( new protocol::InternalCommandPacket( 
                    icommand_request.transaction_id_, 
                    0,  // error_code
                    icommand_request.end_point
                    ) );

                DoSendPacket(end_point, response , icommand_request.peer_version_);
            }
            break;
        default:
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnInternalCommandRequest: Unknown command " << icommand);
        }
    }

    void UdpTrackerModule::OnQueryPeerCountRequest( const boost::asio::ip::udp::endpoint& end_point,
        protocol::QueryPeerCountPacket & query_peer_count_request)
    {
        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnQueryPeerCountRequest: Tracker Moduel is not running. Return.");
            return ;
        }

        Guid resource_id = query_peer_count_request.request.resource_id_;

        LOG4CPLUS_DEBUG(g_logger,"Query Resource PeerCount: " << resource_id << ", from " << end_point);

        unsigned peer_count = tracker_model_->GetResourcePeerCount(resource_id);

        boost::shared_ptr<protocol::QueryPeerCountPacket> response ( new protocol::QueryPeerCountPacket( 
            query_peer_count_request.transaction_id_, 
            0,  // error_code
            resource_id,
            peer_count , query_peer_count_request.end_point
            ) );

        DoSendPacket(end_point, response , query_peer_count_request.peer_version_);
    }

    void UdpTrackerModule::OnQueryPeerResourcesRequest(const boost::asio::ip::udp::endpoint&  end_point, 
        protocol::QueryPeerResourcesPacket& query_peer_resources_request)
    {
        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::QueryPeerResourcesPacket: Tracker Moduel is not running. Return.");
            return ;
        }

        Guid peer_id = query_peer_resources_request.request.peer_guid_;

        LOG4CPLUS_INFO(g_logger,"Query Peer Resources: " << peer_id << ", from " << end_point);

        //unsigned peer_count = tracker_model_->GetResourcePeerCount(resource_id);
        vector<RID> resources;
        tracker_model_->ListPeerResources(peer_id,resources);

        boost::shared_ptr<protocol::QueryPeerResourcesPacket> response ( new protocol::QueryPeerResourcesPacket( 
            query_peer_resources_request.transaction_id_, 
            0,  // error_code
            peer_id,
            resources , query_peer_resources_request.end_point
            ) );

        DoSendPacket(end_point, response , query_peer_resources_request.peer_version_);
    }

    void UdpTrackerModule::OnQueryTrackerStatisticRequest(const boost::asio::ip::udp::endpoint&  end_point, 
        protocol::QueryTrackerStatisticPacket& query_tracker_statistic_request)
    {
        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnQueryTrackerStatisticRequest: Tracker Moduel is not running. Return.");
            return ;
        }

        LOG4CPLUS_INFO(g_logger,"Query trackerstaticstc , from " << end_point);

        std::string tracker_statistic = tracker_model_->GetTrackerStatistic();

        boost::shared_ptr<protocol::QueryTrackerStatisticPacket> response ( new protocol::QueryTrackerStatisticPacket( 
            query_tracker_statistic_request.transaction_id_, 
            0,  // error_code
            tracker_statistic , query_tracker_statistic_request.end_point
            ) );

        DoSendPacket(end_point, response , query_tracker_statistic_request.peer_version_);
    }
  
#ifdef WIN32
    size_t UdpTrackerModule::BackupFile(std::string sample_path)
    {
        std::string date_string = "_" + GetDateString();
        std::string backup_path = boost::algorithm::replace_last_copy(sample_path, ".log", date_string + ".log");
        // expand all '\' to '\\'
        boost::algorithm::replace_all(sample_path, "\\", "\\\\");
        boost::algorithm::replace_all(backup_path, "\\", "\\\\");
        BOOL ok = ::MoveFileExA(sample_path.c_str(), backup_path.c_str(), MOVEFILE_REPLACE_EXISTING);
        size_t e_code = GetLastError();
        return e_code;
    }

    size_t UdpTrackerModule::MoveFile(std::string source_file, std::string dest_file, size_t flags)
    {
        size_t e_code = ::MoveFileExA(source_file.c_str(), dest_file.c_str(), flags);
        return e_code;
    }
#else
#endif
    void UdpTrackerModule::OnTimerElapsed(const Timer::pointer * pointer, unsigned times)
    {
        if( !is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnTimerElapsed: Tracker Module is not running. Return.");
            cout.write("update",6);
            return;
        }
        LOG4CPLUS_INFO(g_logger,"interval_ms:"<<peer_statistic_timer_->interval());

  //      LOG4CPLUS_INFO(g_logger,"on timer elapsed,times:"<<times<<" "<<asio_timer_manager_->next_tick()
   //         <<" times:"<<asio_timer_manager_->times()<<" interval_ms"<<asio_timer_manager_->interval_ms());
//            <<" times:"<<(*pointer)->times() );

        if (*pointer == resource_dump_timer_)
        {
            LOG4CPLUS_DEBUG(g_logger,"resource_dump_timer_...");
            TRACK_DEBUG("UdpTrackerModule::OnTimerElapsed: From Resource Dump Timer. " << times << " times.");

            tracker_statistic_->SetPeerCount(tracker_model_->TotalPeersCount());
            tracker_statistic_->SetRidCount(tracker_model_->TotalResourcesCount());
            tracker_statistic_->OutputStatisticData();

            // check resources
            if (check_resources_on_)
            {
                tracker_model_->CheckResources("copy_resources", GetCopyResourceMinPeerCount());
            }

            // 检测是否需要DUMP
            if( true == dump_resource_map_ ) {
                //we first clear up the dump flag
                dump_resource_map_ = false;
                //dump the statistics to a file with name based on the dump timestamp
                char pfilename_local[1024];
                sprintf(pfilename_local, "resource_map_%010d.log", dump_resource_map_last_sec_);
                std::ofstream f( pfilename_local );
                f << "begin dump statistics: " << time(0) << "\r\n";
                // dump 信息
                std::string stat =  this->tracker_statistic_->QueryAllStatisticInfo().ToString();
                f << stat << "\r\n";
                f << "dumping maps: " << time(0) << "\r\n";
                this->tracker_model_->container_->DumpAllPeersDetailInfo(f);
                f << "end dump: " << time(0) << "\r\n";
                f.flush();
                f.close();
            }
            if ( true == dump_peer_list_ ) {
                dump_peer_list_ = false;
                LOG4CPLUS_WARN(g_logger,"UdpTrackerModule::OnTimerElapsed: From Resource Dump Timer: dump peer list not implemented yet.");
            }
        }
        else if (*pointer == check_alive_timer_)
        {
            LOG4CPLUS_DEBUG(g_logger,"Check Alive of Peers");
            tracker_model_->RemoveUnAlivePeers();
            TRACK_DEBUG(" Resource Count: " << tracker_model_->TotalResourcesCount() );
            TRACK_DEBUG(" Peer Count: " << tracker_model_->TotalPeersCount());
            /*
            if (times % 60 == 0)
            {
            Log::Inst().GetDefaultLogWriter()->ClearLog();
            }
            */
        }
        else if (*pointer == statistic_timer_) // 24 hours
        {
            LOG4CPLUS_DEBUG(g_logger, "statistic_timer_...");
            // 用来导出 RIDCount 和 PeerCount
            this->out_put_count_stats();
        }
		else if (*pointer == peer_statistic_timer_)
		{
			if (tracker_statistic_->IsPrintable() & !(tracker_statistic_->IsPrintCompelted()))
			{
				tracker_statistic_->OutputRidPeerHashMap();
				tracker_statistic_->DecPrintTimes();
			}
		}
        else if (*pointer == copy_resource_timer_)
        {
            LOG4CPLUS_DEBUG(g_logger, "copy_resource_timer_...");
            if (check_resources_on_)
            {

            }
        }
        else if (*pointer == sample_resource_dump_timer_)
        {
            LOG4CPLUS_DEBUG(g_logger, "sample_resource_dump_timer_");
            if (sample_resource_dump_on_)
            {
                tracker_model_->DumpSampleResources("sample_resource_log_writer", sample_resources_);
                tracker_model_->DumpSampleResources("random_sample_resource_log_writer", sample_resources_random_);
            }
        }
        else if (*pointer == delay_dump_timer_)
        {
            LOG4CPLUS_DEBUG(g_logger, "delay_dump_timer_");
        }
    }

    void UdpTrackerModule::WriteStatisticHeader()
    {

    }

    void UdpTrackerModule::DumpResources()
    {

    }

    void UdpTrackerModule::DumpPeers()
    {

    }

    void UdpTrackerModule::out_put_count_stats()
    {
        // 
        {
            FILE * fp = fopen( "submit_status_peer.txt" , "w" );
            if( 0 == fp ) {
                // 可能是被占用了
                return;
            }

            int pid_count = this->tracker_model_->container_->PeersCount();
            std::ostringstream oss;
            oss << pid_count << "\r\n";

            fwrite( oss.str().c_str() , oss.str().size() , 1 , fp );
            fclose( fp );
        }

        // 
        {
            FILE * fp = fopen( "submit_status_resource.txt" , "w" );
            if( 0 == fp ) {
                // 可能是被占用了
                return;
            }

            int rid_count = this->tracker_model_->container_->ResourcesCount();
            std::ostringstream oss;
            oss << rid_count << "\r\n";

            fwrite( oss.str().c_str() , oss.str().size() , 1 , fp );
            fclose( fp );
        }
    }

    void UdpTrackerModule::DumpSampleResources()
    {
        tracker_model_->DumpSampleResources("sample_resource_log_writer", sample_resources_);
    }

    void UdpTrackerModule::LoadSampleResources()
    {
        std::string sample_rid_string = Config::Inst().GetTString("tracker.sample_resources");
        vector<std::string> rids_;
        boost::algorithm::split(rids_, sample_rid_string, boost::algorithm::is_any_of(";,@ "));
        sample_resources_.clear();
        for (int i = 0; i < rids_.size(); ++i)
        {
            RID rid;
            if (rid.from_string(rids_[i]) && !rid.is_empty())
            {
                sample_resources_.insert(rid);
            }
        }
        // clear list count
        //tracker_model_->ClearSampleResourcesListCount(sample_resources_);
    }

    void UdpTrackerModule::DelayDump(unsigned time_in_seconds)
    {
        sample_resource_dump_on_ = false;
        if (delay_dump_timer_) delay_dump_timer_->cancel();
        delay_dump_timer_ = new framework::timer::OnceTimer(*this->asio_timer_manager_ , time_in_seconds * 1000, 
            boost::bind(&UdpTrackerModule::OnTimerElapsed, this , &this->delay_dump_timer_ , 0 ) );
        //shared_from_this());
        //tracker_model_->ClearResourcesListCount();
        delay_dump_timer_->start();
    }

    //////////////////////////////////////////////////////////////////////////
    // Keep Alive

    unsigned UdpTrackerModule::GetKeepAliveIntervalInSeconds()
    {
        return TrackerModel::GetKeepAliveIntervalInSeconds();
    }

    unsigned UdpTrackerModule::GetCheckAliveCheckIntervalInSeconds()  // tracker.check_alive_interval_in_seconds
    {
        return check_alive_interval_in_seconds_;
    }

    //////////////////////////////////////////////////////////////////////////
    // Dump

    unsigned UdpTrackerModule::GetDumpResourceIntervalInSeconds()  // tracker.dump_interval_in_seconds
    {
        return dump_interval_in_seconds_;
    }

    unsigned UdpTrackerModule::GetDumpResourceCount()  // tracker.dump_resource_count
    {
        return dump_resource_count_;
    }

    unsigned UdpTrackerModule::GetInternalCommandMagicNumber()
    {
        return internal_command_magic_number_;
    }

    //////////////////////////////////////////////////////////////////////////
    // udp

    unsigned UdpTrackerModule::GetUdpServerReceiveCount()  // tracker.udp_server_receive_count
    {
        return udp_server_receive_count_;
    }

    //////////////////////////////////////////////////////////////////////////
    // Statistic
    unsigned UdpTrackerModule::GetStatisticLogIntervalInHours()
    {
        return statistic_log_interval_in_hours_;
    }

    //////////////////////////////////////////////////////////////////////////
    // Resource Copy

    unsigned UdpTrackerModule::GetCopyResourceIntervalInSeconds()
    {
#ifdef DEBUG
        return Config::Inst().GetInteger("tracker.copy_resource_interval_in_seconds", 10);
#else
        return copy_resource_interval_in_seconds_;
#endif
    }

    unsigned UdpTrackerModule::GetCopyResourceMinPeerCount()
    {
        return copy_resource_min_peer_count_;
    }

    std::string UdpTrackerModule::GetCopyResourceDestination()
    {
        return copy_resource_destination_;
    }

    //////////////////////////////////////////////////////////////////////////
    // Sample

    unsigned UdpTrackerModule::GetSampleIntervalInSeconds()
    {
        return sample_interval_in_seconds_;
    }

    void UdpTrackerModule::InitConfigValues()
    {
        check_alive_interval_in_seconds_ = Config::Inst().GetInteger(
            "tracker.check_alive_interval_in_seconds", DEFAULT_CHECK_ALIVE_INTERVAL_IN_SECONDS);

        dump_interval_in_seconds_ = Config::Inst().GetInteger(
            "tracker.dump_interval_in_seconds", DEFAULT_RESOURCE_DUMP_INTERVAL_IN_SECONDS);
        dump_resource_count_ = Config::Inst().GetInteger(
            "tracker.dump_resource_count", DEFAULT_RESOURCE_DUMP_COUNT);
        udp_server_receive_count_ = Config::Inst().GetInteger(
            "tracker.udp_server_receive_count", RECEIVE_COUNT);

        internal_command_magic_number_ = Config::Inst().GetInteger(
            "tracker.internal_command_magic_number", INTERNAL_COMMAND_MAGIC_NUMBER);
        LIMIT_MIN_MAX(internal_command_magic_number_, 1, 65535);

        statistic_log_interval_in_hours_ = Config::Inst().GetInteger(
            "tracker.statistic_log_interval_in_hours", DEFAULT_STATISTIC_LOG_INTERVAL_IN_HOURS);
        LIMIT_MIN_MAX(statistic_log_interval_in_hours_, 1, 48);

        copy_resource_interval_in_seconds_ = Config::Inst().GetInteger(
            "tracker.copy_resource_interval_in_seconds", DEFAULT_COPY_RESOURCE_INTERVAL_IN_SECONDS);
        LIMIT_MIN(copy_resource_interval_in_seconds_, DEFAULT_COPY_RESOURCE_INTERVAL_IN_SECONDS);

        copy_resource_min_peer_count_ = Config::Inst().GetInteger(
            "tracker.copy_resource_min_peer_count", DEFAULT_COPY_RESOURCE_MIN_PEER_COUNT);
        LIMIT_MIN(copy_resource_min_peer_count_, DEFAULT_COPY_RESOURCE_MIN_PEER_COUNT);

        copy_resource_destination_ = Config::Inst().GetTString("tracker.copy_resource_destination", "");
        sample_interval_in_seconds_ = Config::Inst().GetInteger("tracker.sample_interval_in_seconds", 5 * 60);
    }

}
