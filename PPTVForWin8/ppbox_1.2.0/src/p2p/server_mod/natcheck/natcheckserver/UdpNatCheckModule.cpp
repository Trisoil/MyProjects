#include "Common.h"
#include "protocol/UdpServer.h"
#include "UdpNatCheckModule.h"

extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_statlog;

namespace udpnatcheck
{
    UdpNatCheckModule::p UdpNatCheckModule::inst_(new UdpNatCheckModule());

    void UdpNatCheckModule::Start()
    {
        if (is_running_ == true)return;
            is_running_ = true;        

      
        u_int udp_server_receive_count = Config::Inst().GetInteger("natcheck.udp_server_receive_count", 1000);
        min_protocol_version_ = Config::Inst().GetInteger("natcheck.min_protocol_version", 0);

        string udp_ip = Config::Inst().GetTString("natcheck.master_ip");

        in_addr add;
#if defined(_MSC_VER) 
        master_ip_ = ntohl(inet_addr(udp_ip.c_str()));
#else
        inet_aton(udp_ip.c_str(),&add);
        master_ip_ = ntohl(add.s_addr);
#endif
        master_port_ = Config::Inst().GetInteger("natcheck.master_port", 3478);
        if(!InitUdpServer(master_udp_server_,udp_ip,master_port_,min_protocol_version_,udp_server_receive_count))
        {
            return;
        }
        else
        {
            LOG4CPLUS_INFO(g_logger,"InitUdpServer master_udp_server,bind to:" << udp_ip << ":"<<master_port_
                <<" min_protocol_version_"<<min_protocol_version_<<" udp_server_receive_count:"<<udp_server_receive_count);
        }

        udp_ip = Config::Inst().GetTString("natcheck.master_ip");
        slave_diff_port_port_ = Config::Inst().GetInteger("natcheck.diff_port_slave_port", 3479);
        if(!InitUdpServer(slave_udp_server_diff_port_,udp_ip,slave_diff_port_port_,min_protocol_version_,udp_server_receive_count))
        {
            return;
        }
        else
        {
            LOG4CPLUS_INFO(g_logger,"InitUdpServer slave_udp_server_diff_port,bind to:" << udp_ip << ":"<<slave_diff_port_port_
                <<" min_protocol_version_"<<min_protocol_version_<<" udp_server_receive_count:"<<udp_server_receive_count);
        }

        udp_ip = Config::Inst().GetTString("natcheck.diff_ip_slave_ip");
#if defined(_MSC_VER) 
        slave_diff_ip_ip_ = ntohl(inet_addr(udp_ip.c_str()));
#else
        inet_aton(udp_ip.c_str(),&add);
        slave_diff_ip_ip_ = ntohl(add.s_addr);
#endif
        slave_diff_ip_port_ = Config::Inst().GetInteger("natcheck.diff_ip_slave_port", 3480);
        if(!InitUdpServer(slave_udp_server_diff_ip_,udp_ip,slave_diff_ip_port_,min_protocol_version_,udp_server_receive_count))
        {
            return;
        }    
        else
        {
            LOG4CPLUS_INFO(g_logger,"InitUdpServer slave_udp_server_diff_ip,bind to:" << udp_ip << ":"<<slave_diff_ip_port_
                <<" min_protocol_version_"<<min_protocol_version_<<" udp_server_receive_count:"<<udp_server_receive_count);
        }
        
        asio_timer_manager_ = new framework::timer::AsioTimerManager( ::MainThread::IOS() , boost::posix_time::seconds(1) );
        asio_timer_manager_->start();

        print_net_info_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_, 5*1000, 
            boost::bind(&UdpNatCheckModule::OnTimerElapsed, this , &this->print_net_info_timer_, 0 ) );

        print_net_info_timer_->start();
        time_start_ = time(NULL);

        tick_count_.reset();
        peer_count_ = 0;

        natcheck_statistic_=NatCheckStatistic::Create();
        natcheck_statistic_->Start();
    }

    bool UdpNatCheckModule::InitUdpServer(boost::shared_ptr<protocol::UdpServer>& udp_server,const string& ip,unsigned short port,unsigned short min_protocol_version,unsigned server_recv_count)
    {
        udp_server.reset(new ::protocol::UdpServer( ::MainThread::IOS() , shared_from_this() ));

        if( !udp_server->Listen(ip,port))
        {
            CONSOLE_LOG("Failed to Listen to the UDP IP:Port " << ip<<":"<<port);
            CONSOLE_LOG("I Have Stopped to Work!!!!!");
            CONSOLE_LOG("Please Type \"exit\" to Quit"); 
            MainThread::IOS().post(boost::bind(&UdpNatCheckModule::Stop, UdpNatCheckModule::Inst()));
            return false;
        }
        else
        {
            CONSOLE_LOG("UdpNatCheckModule::Start() Udp Listen To IP:Port " << ip<<":"<<port);
        }
        udp_server->set_minimal_protocol_verion(min_protocol_version);
        udp_server->Recv(server_recv_count);
        protocol::register_natcheck_packet(*udp_server);  
        return true;
    }

    void UdpNatCheckModule::Stop()
    {
        if (is_running_ == false)return;
        is_running_ = false;

        if (master_udp_server_)
        {
            master_udp_server_->Close();
        }
        if (slave_udp_server_diff_ip_)
        {
            slave_udp_server_diff_ip_->Close();
        }
        if (slave_udp_server_diff_port_)
        {
            slave_udp_server_diff_port_->Close();
        }

        natcheck_statistic_->Stop();
    }

    void UdpNatCheckModule::OnUdpRecv(protocol::Packet const & packet)
    {
        if (is_running_ == false) return;

        switch (packet.PacketAction)
        {
        case protocol::NatCheckSameRoutePacket::Action:
            {
                protocol::NatCheckSameRoutePacket const & natcheck_packet = (protocol::NatCheckSameRoutePacket const &)packet;
                
                natcheck_statistic_->SubmitSameRouteRequest(natcheck_packet.length());
                LOG4CPLUS_DEBUG(g_logger,"recv "<<packet.PacketAction<<" cmd from"<<packet.end_point
                    <<" pack length"<<natcheck_packet.length());
                OnNatCheckSameRoute(natcheck_packet);
            }
            break;
        case protocol::NatCheckDiffIpPacket::Action:
            {
                protocol::NatCheckDiffIpPacket const & natcheck_packet = (protocol::NatCheckDiffIpPacket const &)packet;
                natcheck_statistic_->SubmitDiffIpRequest(natcheck_packet.length());
                LOG4CPLUS_DEBUG(g_logger,"recv "<<packet.PacketAction<<" cmd from"<<packet.end_point
                    <<" pack length"<<natcheck_packet.length());
                OnNatNatCheckDiffIp(natcheck_packet);               
            }
            break;
        case protocol::NatCheckDiffPortPacket::Action:
            {
                protocol::NatCheckDiffPortPacket const & natcheck_packet = (protocol::NatCheckDiffPortPacket const &)packet;
                natcheck_statistic_->SubmitDiffPortRequest(natcheck_packet.length());
                LOG4CPLUS_DEBUG(g_logger,"recv "<<packet.PacketAction<<" cmd from"<<packet.end_point
                    <<" pack length"<<natcheck_packet.length());
                OnNatNatCheckDiffPort(natcheck_packet);                
            }
            break;
        }

    }

    void UdpNatCheckModule::OnTimerElapsed(framework::timer::Timer::pointer * pointer, u_int times)
    {
        if (is_running_ == false)
        {
            return;
        }

        if (*pointer == print_net_info_timer_)
        {
            PrintNetInfo(true);
            //stun_statistic_->Clear();
            time_start_ = time(NULL);
        }
    }

    //通过部署，保证所有的包的接收都是master接收的，发送可能是slave或者master
    void UdpNatCheckModule::OnNatCheckSameRoute(const protocol::NatCheckSameRoutePacket& packet)
    {
        protocol::NatCheckSameRoutePacket rsp(packet.query_times_,packet.end_point.address().to_v4().to_ulong(),packet.end_point.port(),
            master_ip_,master_port_,packet.transaction_id_,0,packet.end_point);

        natcheck_statistic_->SubmitSameRouteResponse(rsp.length());
       
        natcheck_statistic_->SubmitQueryTimes(protocol::NatCheckSameRoutePacket::Action,packet.query_times_);

        DoMasterSendPacket(rsp,packet.peer_version_);

    }
    void UdpNatCheckModule::OnNatNatCheckDiffIp(const protocol::NatCheckDiffIpPacket& packet)
    {
        protocol::NatCheckDiffIpPacket rsp(packet.query_times_,packet.end_point.address().to_v4().to_ulong(),packet.end_point.port(),
            slave_diff_ip_ip_,slave_diff_ip_port_,master_ip_,master_port_,packet.transaction_id_,0,packet.end_point);

        natcheck_statistic_->SubmitDiffIpResponse(rsp.length());
        natcheck_statistic_->SubmitQueryTimes(protocol::NatCheckDiffIpPacket::Action,packet.query_times_);

        DoIpDiffSlaveSendPacket(rsp,packet.peer_version_);
    }
    void UdpNatCheckModule::OnNatNatCheckDiffPort(const protocol::NatCheckDiffPortPacket& packet)
    {
        protocol::NatCheckDiffPortPacket rsp(packet.query_times_,packet.end_point.address().to_v4().to_ulong(),packet.end_point.port(),
            slave_diff_ip_ip_,slave_diff_ip_port_,master_port_,packet.transaction_id_,0,packet.end_point);

        natcheck_statistic_->SubmitDiffPortResponse(rsp.length());
        natcheck_statistic_->SubmitQueryTimes(protocol::NatCheckDiffPortPacket::Action,packet.query_times_);

        DoPortDiffSlaveSendPacket(rsp,packet.peer_version_);
    }

    void UdpNatCheckModule::PrintNetInfo(bool is_timer)
    {
        if(is_timer)
        {
            LOG4CPLUS_INFO(g_statlog,"dumpinfo:"<<natcheck_statistic_->DumpInfo());
        }
        else
        {
            CONSOLE_LOG("dumpinfo:"<<natcheck_statistic_->DumpInfo());
        }
    }

}