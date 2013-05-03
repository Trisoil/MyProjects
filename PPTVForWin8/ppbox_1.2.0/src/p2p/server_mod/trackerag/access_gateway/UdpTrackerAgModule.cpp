#include "Common.h"
#include "UdpTrackerAgModule.h"
//#include "TrackerLogger.h"

#include "protocol/UdpServer.h"
//#include "protocol/Cryptography.h"

#include "MainThread.h"
#include <fstream>
extern log4cplus::Logger g_logger;
extern std::string g_config_file_name;

using namespace framework;
using namespace boost;



namespace udptrackerag
{    //size_t UdpTrackerAgModule::udp_server_receive_count_;

    UdpTrackerAgModule::p UdpTrackerAgModule::s_instance;//(new UdpTrackerAgModule());

    UdpTrackerAgModule::UdpTrackerAgModule()
        : is_running_(false)//,unexpected_response_(0),timeout_response_(0)//,use_cache_result_(0),total_result_(0)
    {
    }

    void UdpTrackerAgModule::Start(u_short local_udp_port)
    {
        LOG4CPLUS_INFO(g_logger, "Starting UDP Tracker Ag Module...");
        cout<<("正在启动TrackerAg模块...");

        if( true == is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"TrackerAg server is running");
            return;
        }

        InitConfigValues();

        //弄一个10ms的精度的定时管理
        asio_timer_manager_ = new AsioTimerManager( ::MainThread::IOS() , boost::posix_time::milliseconds(10));
        asio_timer_manager_->start();

        // 启动Udp服务器
        //udp_server_ = new ::protocol::UdpServer( ::MainThread::IOS() , this );
        udp_server_.reset(new protocol::UdpServer(::MainThread::IOS(), shared_from_this()));

        // 设置最小支持的版本号
        int mini_support_protocol_version = Config::Instance().GetInteger("trackerag.min_support_protocol_version", 0);
        udp_server_->set_minimal_protocol_verion( (boost::uint16_t) mini_support_protocol_version );

        TrackerRequestHandler::Instance()->SetUdpServer(udp_server_);

        protocol::register_tracker_packet(*this->udp_server_);
        LOG4CPLUS_INFO(g_logger, "UDP Server has been Created.");
        LOG4CPLUS_INFO(g_logger, "Try to listen on port " << local_udp_port);

		trackerag_statistic_ = TrackeragStatistic::Inst();
		trackerag_statistic_->Start(1000);

        if( false == udp_server_->Listen(local_udp_port) )
        {
            //! Release 打印不出来
            LOG4CPLUS_WARN(g_logger,"Udp Listen To port" << local_udp_port << ". Failed");
            CONSOLE_LOG("监听 " << local_udp_port << " 端口失败");
            udp_server_->Close();

            //  一旦监听失败，可以直接退出了
            exit( -1 );
            return;
        }

        LOG4CPLUS_INFO(g_logger, "Listen on port " << local_udp_port << " successfully.");
        CONSOLE_LOG("监听 " << local_udp_port << " 端口成功");

        check_alive_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , check_alive_ms_ , 
            boost::bind(&UdpTrackerAgModule::OnTimerElapsed, this , &this->check_alive_timer_) );  

        reload_config_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 10*1000 , 
            boost::bind(&UdpTrackerAgModule::OnTimerElapsed, this , &this->reload_config_timer_) );  

        dump_info_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 5*1000 , 
            boost::bind(&UdpTrackerAgModule::OnTimerElapsed, this , &this->dump_info_timer_) ); 

        rid_list_cache_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 5*1000 , 
            boost::bind(&UdpTrackerAgModule::OnTimerElapsed, this , &this->rid_list_cache_timer_) );  


		//resource_dump_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_ , 5*1000 , 
			//boost::bind(&UdpTrackerAgModule::OnTimerElapsed, this , &this->resource_dump_timer_) );


        // Start to receive packages
        LOG4CPLUS_INFO(g_logger, "Start to Receive packets: " << udp_server_receive_count_);

        CONSOLE_LOG("开始收包,队列长度:" << udp_server_receive_count_);
        udp_server_->Recv( udp_server_receive_count_ );

       // 定时器
        check_alive_timer_->start();
        reload_config_timer_->start();
        dump_info_timer_->start();
        rid_list_cache_timer_->start();

		//resource_dump_timer_->start();

        // Running状态
        is_running_ = true;

        LOG4CPLUS_INFO(g_logger, "UDP Tracker Ag Module is started successful." );
        CONSOLE_LOG("UDP Tracker Access Gateway  启动完毕");
    }

    void UdpTrackerAgModule::Stop()
    {
        LOG4CPLUS_DEBUG(g_logger,"UdpTrackerAgModule::Stop");
        CONSOLE_LOG("UDP Tracker模块正在停止...");

        if( false == is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"Tracker模块没有运行");
            return;
        }
		trackerag_statistic_->Stop();

        // 定时器
        if(check_alive_timer_) check_alive_timer_->cancel();

        udp_server_->Close();

        //tracker_model_->Stop();

        is_running_ = false;

        LOG4CPLUS_INFO(g_logger,"Tracker Module stops successfully.");
        CONSOLE_LOG("Tracker模块成功停止...");
    }

    int UdpTrackerAgModule::PeerIdCheck(const Guid& peer_id)
    {
        if (peer_id.is_empty())
        { 
            LOG4CPLUS_WARN(g_logger, "peer_id empty");
            return PEERID_CHECK_FAILED;
        }
        return 0;
    }
    int UdpTrackerAgModule::RidCheck(const RID& rid)
    {
        if (rid.is_empty())
        { 
            LOG4CPLUS_WARN(g_logger, "rid empty");
            return RESOURCEID_CHECK_FAILED;
        }
        return 0;
    }

    void UdpTrackerAgModule::OnUdpRecv(protocol::Packet const & packet)
    {
        LOG4CPLUS_DEBUG(g_logger,"UdpTrackerAgModule::OnUdpRecv: " << packet.end_point);

        if( false == is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerAgModule::OnUdpRecv: Tracker is not running. Return.");
            return;
        }

        LOG4CPLUS_DEBUG(g_logger, "UDP Received, TID: " << packet.transaction_id_);

        // Tracker 相关协议
        if( packet.PacketAction >= 0x30 && packet.PacketAction < 0x50 )
        {
            LOG4CPLUS_DEBUG(g_logger, "Dispatching action: 0x" << std::hex << int(packet.PacketAction));
            DispatchAction(packet.end_point, packet.PacketAction, (protocol::ServerPacket &)packet);
        }
        else
        {
            LOG4CPLUS_WARN(g_logger,"Unknown Action：0x" << std::hex << int(packet.PacketAction) << ", from " << packet.end_point);
        }
    }

    void UdpTrackerAgModule::DispatchAction(const boost::asio::ip::udp::endpoint& end_point, boost::uint8_t action, ::protocol::ServerPacket const & packet)
    {
        LOG4CPLUS_TRACE(g_logger,"UdpTrackerAgModule::DispatchAction: " << end_point << ", action: 0x" << std::hex << int(action));
        if( false == is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"UdpTrackerAgModule::DispatchAction: Tracker Module is not running. Return.");
            return;
        }
        if (!( action >= 0x30 && action < 0x50 ))
        {
            LOG4CPLUS_WARN(g_logger, "UdpTrackerAgModule::DispatchAction: Action 0x" << std::hex << action << " is not accepted by UdpTrackerServer.");
            return ;
        }
        // 解析 Tracker 协议
        switch(action)
        {
            case protocol::ListPacket::Action:            
                {
                    if (packet.IsRequest)
                    {
						trackerag_statistic_->SubmitClientRequest( packet.length() );
						LOG4CPLUS_DEBUG(g_logger, "SubmitClientRequest packet.length: "<<packet.length());
                        OnRequestComm(end_point,(protocol::ListPacket&)packet,end_point.address().to_v4().to_ulong());
                    }
                    else
                    {
						trackerag_statistic_->SubmitListResponse(packet.length());
                        OnReponseComm(end_point,(protocol::ListPacket&)packet);
                    }
                }
                break;         
            case protocol::ListTcpPacket::Action:
                {
                    if (packet.IsRequest)
                    {
						trackerag_statistic_->SubmitClientRequest( packet.length() );
						LOG4CPLUS_DEBUG(g_logger, "SubmitClientRequest packet.length: "<<packet.length());
                        OnRequestComm(end_point,(protocol::ListTcpPacket&)packet,end_point.address().to_v4().to_ulong());
                    }
                    else
                    {
						trackerag_statistic_->SubmitListResponse(packet.length());
                        OnReponseComm(end_point,(protocol::ListTcpPacket&)packet);
                    }
                }
                break;     
            case protocol::ListWithIpPacket::Action:
                {
                    if (packet.IsRequest)
                    {
						trackerag_statistic_->SubmitClientRequest( packet.length() );
						LOG4CPLUS_DEBUG(g_logger, "SubmitClientRequest packet.length: "<<packet.length());
                        OnRequestComm(end_point,(protocol::ListWithIpPacket&)packet,((protocol::ListWithIpPacket&)packet).request.reqest_ip_);
                    }
                    else
                    {
						trackerag_statistic_->SubmitListResponse(packet.length());
                        OnReponseComm(end_point,(protocol::ListWithIpPacket&)packet);
                    }
                }
                break;     
            case protocol::ListTcpWithIpPacket::Action:
                {
                    if (packet.IsRequest)
                    {
						trackerag_statistic_->SubmitClientRequest( packet.length() );
						LOG4CPLUS_DEBUG(g_logger, "SubmitClientRequest packet.length: "<<packet.length());
                        OnRequestComm(end_point,(protocol::ListTcpWithIpPacket&)packet,((protocol::ListTcpWithIpPacket&)packet).request.reqest_ip_);
                    }
                    else
                    {
						trackerag_statistic_->SubmitListResponse(packet.length());
                        OnReponseComm(end_point,(protocol::ListTcpWithIpPacket&)packet);
                    }
                }
                break;     
            default:
            {
                LOG4CPLUS_WARN(g_logger, "Ignoring unknown action: 0x" << std::hex << int(action));               
                break;
            }
        }
    }   

    void UdpTrackerAgModule::SendResponse(const RID& resource_id,vector<protocol::CandidatePeerInfo>& peer_infos,
        boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,uint32_t transaction_id,uint16_t peer_version)
    {
        switch (action)
        {
            case protocol::ListPacket::Action: 
                {
                    boost::shared_ptr<protocol::ListPacket> response(new protocol::ListPacket(transaction_id,0,resource_id,peer_infos,end_point));
                    DoSendPacket(response,peer_version);
					trackerag_statistic_->SubmitCommitResponse(response->length());
                    return;
                }
            case protocol::ListTcpPacket::Action:
                {
                    boost::shared_ptr<protocol::ListTcpPacket> response(new protocol::ListTcpPacket(transaction_id,0,resource_id,peer_infos,end_point));
                    DoSendPacket(response,peer_version);
					trackerag_statistic_->SubmitCommitResponse(response->length());
                    return;
                }
            case protocol::ListWithIpPacket::Action:
                {
                    boost::shared_ptr<protocol::ListWithIpPacket> response(new protocol::ListWithIpPacket(transaction_id,0,resource_id,peer_infos,end_point));
                    DoSendPacket(response,peer_version);
					trackerag_statistic_->SubmitCommitResponse(response->length());
                    return;
                }
            case protocol::ListTcpWithIpPacket::Action:
                {
                    boost::shared_ptr<protocol::ListTcpWithIpPacket> response(new protocol::ListTcpWithIpPacket(transaction_id,0,resource_id,peer_infos,end_point));
                    DoSendPacket(response,peer_version);
					trackerag_statistic_->SubmitCommitResponse(response->length());
                    return;
                }
            default:
            {
                LOG4CPLUS_WARN(g_logger, "Ignoring unknown action: 0x" << std::hex << int(action));               
                return;
            }
        }        
	}
	void UdpTrackerAgModule::OnCheckListAlive()
    {
        map<RID,uint32_t> dead_requests;
        ListRequestCache::Instance()->GetDeadlineRequest(keep_alive_ms_,dead_requests);
        for( map<RID,uint32_t>::iterator it = dead_requests.begin();it != dead_requests.end(); ++it)
        {
            vector<protocol::CandidatePeerInfo> peer_infos;
            boost::asio::ip::udp::endpoint end_point;
            boost::uint8_t action;
            boost::uint16_t peer_version;
            const RID& resource_id = it->first;
            uint32_t transaction_id = it->second;
            ListRequestCache::Instance()->GetResponse(peer_infos,end_point,action,peer_version,resource_id,transaction_id,location_peer_percent_,random_shuffle_);
			int temp = ListRequestCache::Instance()->GetUnResponseCount(resource_id, transaction_id);
            ListRequestCache::Instance()->RemoveRequestInfo(resource_id,transaction_id);

			trackerag_statistic_->AddTotalFailResult(temp);
            LOG4CPLUS_INFO(g_logger,"temp:"<<temp);
            if(0 == peer_infos.size())
            {               
                int req_count = RidListCache::Instance()->GetListResult(resource_id,peer_infos);  
                if(peer_infos.size() != 0)
                {
                    LOG4CPLUS_INFO(g_logger,"use cache1,rid:"<<resource_id<<" peers size:"<<peer_infos.size()<<" reqcount:"<<req_count); 
                    //++use_cache_result_;
					trackerag_statistic_->AddUseCacheResult();
                }
               
            }
			//++total_result_;
			trackerag_statistic_->AddTotalResult();
            SendResponse(resource_id,peer_infos,end_point,action,transaction_id,peer_version); 
             //++list_count_times_[peer_infos.size()*(-1)];
			trackerag_statistic_->AddListCountTimes(peer_infos.size()*(-1));
        }
    }

    void UdpTrackerAgModule::OnReloadConfig()
    {
        LOG4CPLUS_DEBUG(g_logger,"OnReloadConfig");
        if ( Config::Instance().LoadConfig(g_config_file_name) == false )
        {
            LOG4CPLUS_WARN(g_logger,"Failed to Load Config file "<<g_config_file_name);               
            return;
        }

        this->InitConfigValues();
        TrackerRequestHandler::Instance()->InitConfigValue();     
    }

    void UdpTrackerAgModule::OnRidListCache()
    {
        LOG4CPLUS_DEBUG(g_logger,"OnRidListCache");
        RidListCache::Instance()->RemoveTimeout(list_result_timeout_ms_);
    }

    void UdpTrackerAgModule::OnTimerElapsed(const Timer::pointer * pointer)
    {
        //获取超时
        if (*pointer == check_alive_timer_)
        {
            OnCheckListAlive();           
        }
        if (*pointer == dump_info_timer_)
        {
            trackerag_statistic_->OutputSata();           
        } 
        if (*pointer == reload_config_timer_)
        {
            OnReloadConfig();           
        }
        if (*pointer == rid_list_cache_timer_)
        {
            OnRidListCache();
        }
		//if (*pointer == resource_dump_timer_)
		//{
		//	trackerag_statistic_->OutputStatisticData();

		//}
    }
   

    void UdpTrackerAgModule::InitConfigValues()
    {
        udp_server_receive_count_ = Config::Instance().GetInteger("trackerag.udp_server_receive_count", 1000);
        location_peer_percent_ = Config::Instance().GetInteger("trackerag.location_peer_percent", 70);
        keep_alive_ms_ = Config::Instance().GetInteger("trackerag.keep_alive_ms", 100);
        check_alive_ms_ = Config::Instance().GetInteger("trackerag.check_alive_ms", 30);
        random_shuffle_ = Config::Instance().GetBoolean("trackerag.random_shuffle",false);
        list_result_timeout_ms_ = Config::Instance().GetInteger("trackerag.list_result_timeout_ms", 60*1000);

        LOG4CPLUS_INFO(g_logger,"InitConfigValues  location_peer_percent_:"<<location_peer_percent_<<" keep_alive_ms_:"<<keep_alive_ms_<<" check_alive_ms_:"<<check_alive_ms_
            <<" random_shuffle_:"<<int(random_shuffle_));
    }

}
