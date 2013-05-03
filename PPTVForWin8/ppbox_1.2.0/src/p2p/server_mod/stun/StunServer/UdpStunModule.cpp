#include "Common.h"
#include "protocol/UdpServer.h"
#include "UdpStunModule.h"


FRAMEWORK_LOGGER_DECLARE_MODULE("StunServer");
namespace udpstun
{
    UdpStunModule::p UdpStunModule::inst_(new UdpStunModule());

    void UdpStunModule::Start(u_short local_udp_port)
    {
        if (is_running_ == true)return;
        is_running_ = true;
        udp_server_.reset(new ::protocol::UdpServer( ::MainThread::IOS() , shared_from_this() ));

        if( udp_server_->Listen(local_udp_port) == false )
        {
            CONSOLE_LOG("Failed to Listen to the UDP Port " << local_udp_port);
            CONSOLE_LOG("I Have Stopped to Work!!!!!");
            CONSOLE_LOG("Please Type \"exit\" to Quit");
            MainThread::IOS().post(
                boost::bind(&UdpStunModule::Stop, UdpStunModule::Inst())
                );
        }
        else
        {
            CONSOLE_LOG("UdpIndexModule::Start() Udp Listen To port " << local_udp_port);
        }

        //int udp_server_receive_count = Config::Inst().GetInteger("stun.udp_server_receive_count");
        u_int udp_server_receive_count = Config::Inst().GetInteger("stun.udp_server_receive_count", 1000);
        keep_alive_time_ = Config::Inst().GetInteger("stun.udp_keep_alive_time", 10);
        if (udp_server_receive_count == 0)
        {
            CONSOLE_LOG("Failed to Read the udp_server_receive_count from the Config File!!!!");
            CONSOLE_LOG("The udp_server_receive_count has been Set as 1000");
            udp_server_receive_count = 1000;
        }
        CONSOLE_LOG("The udp_server_receive_count is "<<udp_server_receive_count);
        if (keep_alive_time_ == 0)
        {
            CONSOLE_LOG("Failed to Read the keep_alive_time_ from the Config File!!!!");
            CONSOLE_LOG("The keep_alive_time_ has been Set as 10000");
            keep_alive_time_ = 10;
        }
        min_protocol_version_ = Config::Inst().GetInteger("stun.min_protocol_version", 0);
        udp_server_->set_minimal_protocol_verion(min_protocol_version_);

        RegisterAllPackets();
        CONSOLE_LOG("Start completed, begin to receive packages...");
        udp_server_->Recv(udp_server_receive_count);

        stun_statistic_ = StunStatistic::Create();
        stun_statistic_->Start();
        
        asio_timer_manager_ = new framework::timer::AsioTimerManager( ::MainThread::IOS() , boost::posix_time::seconds(1) );
        asio_timer_manager_->start();

        //print_net_info_timer_ = framework::timer::PeriodicTimer::create(24*60*60*1000, shared_from_this());

        print_net_info_timer_ = new framework::timer::PeriodicTimer( *this->asio_timer_manager_, 24*60*60*1000, 
            boost::bind(&UdpStunModule::OnTimerElapsed, this , &this->print_net_info_timer_, 0 ) );

        print_net_info_timer_->start();
        time_start_ = time(NULL);

        tick_count_.reset();
        peer_count_ = 0;
    }

    template<typename type>
    void UdpStunModule::DoSendPacket(type const & packet, boost::uint32_t dest_protocol_version)
    {
        if(false == is_running_) return;

        udp_server_->send_packet(packet, dest_protocol_version);
    }

    void UdpStunModule::Stop()
    {
        if (is_running_ == false)return;
        is_running_ = false;

        if (udp_server_)
        {
            udp_server_->Close();
        }

        stun_statistic_->Stop();
        print_net_info_timer_->stop();
    }

    void UdpStunModule::OnUdpRecv(protocol::Packet const & packet)
    {
        if (is_running_ == false) return;

        switch (packet.PacketAction)
        {
        case protocol::StunHandShakePacket::Action:
            {
                stun_statistic_->SubmitHandSharkRequest( packet.length() );
                // LOG(__EVENT, "Stun", "UdpStunModule::OnUdpRecv() Hand Shake");
                protocol::StunHandShakePacket const & handshake_packet = (protocol::StunHandShakePacket const &)packet;
                OnHandShake( handshake_packet );
            }
            break;
        case protocol::StunKPLPacket::Action:
            {
                stun_statistic_->SubmitKeepAliveRequest( packet.length() );
                // LOG(__EVENT, "Stun", "UdpStunModule::OnUdpRecv() Keep Alive");
                if (tick_count_.elapsed() >= keep_alive_time_ * 1000)
                {
                    // LOG(__DEBUG, "StunServer", peer_count_);
                    OutPutPeerCount();
                    peer_count_ = 0;
                    tick_count_.reset();
                }
                else
                {
                    ++peer_count_;
                }
            }
            break;
        case protocol::StunInvokePacket::Action:
            {
                stun_statistic_->SubmitInvokeRequest( packet.length() );
                // LOG(__EVENT, "Stun", "UdpStunModule::OnUdpRecv() Invoke");
                protocol::StunInvokePacket const & invoke_packet = (protocol::StunInvokePacket const &)packet;
                OnInvoke( invoke_packet );
            }
            break;
        }

    }

    void UdpStunModule::OnTimerElapsed(framework::timer::Timer::pointer * pointer, u_int times)
    {
        if (is_running_ == false)
        {
            return;
        }

        if (*pointer == print_net_info_timer_)
        {
            PrintNetInfo(true);
            stun_statistic_->Clear();
            time_start_ = time(NULL);
        }
    }

    void UdpStunModule::OnHandShake( const protocol::StunHandShakePacket & handshake_pakt )
    {
        if (is_running_ == false) return;

        if (handshake_pakt.IsRequest == false)
        {
            return;
        }

//         u_long ep_ip;
//         u_short ep_port;
//         framework::network::EndpointToIpPort(end_point, ep_ip, ep_port);

        //StunHandShakeResponsePacket::p response_packet = StunHandShakeResponsePacket::CreatePacket(request_packet->GetTransactionID(), 0, keep_alive_time_, ep_ip, ep_port);

        protocol::StunHandShakePacket response_packet(
            handshake_pakt.transaction_id_,
            0,
            keep_alive_time_,
            handshake_pakt.end_point.address().to_v4().to_ulong(),
            handshake_pakt.end_point.port(),
            handshake_pakt.end_point
            );
        
        //if (response_packet)
        //{
            stun_statistic_->SubmitHandSharkResponse(handshake_pakt.length());

            DoSendPacket(response_packet, handshake_pakt.peer_version_);
        //}
        //else
        //{
        //    assert(0);
        //}
    }

    void UdpStunModule::OnInvoke( const protocol::StunInvokePacket & invoke_pakt )
    {
        if (is_running_ == false) return;

        
        //if (invoke_pakt)
        //{
//             boost::asio::ip::udp::endpoint end_point_invoke = network::IpPortToUdpEndpoint(
//                 packet_invoke->GetCandidatePeerInfoHis().DetectIP,
//                 packet_invoke->GetCandidatePeerInfoHis().DetectUdpPort);

            boost::asio::ip::udp::endpoint end_point_invoke(
                boost::asio::ip::address_v4(invoke_pakt.candidate_peer_info_his_.DetectIP),
                invoke_pakt.candidate_peer_info_his_.DetectUdpPort);

            // LOG(__EVENT, "packet", "UdpStunModule::OnInvoke"<<end_point_invoke);

            if (invoke_pakt.candidate_peer_info_mine_ == invoke_pakt.candidate_peer_info_his_
                || invoke_pakt.candidate_peer_info_his_.DetectIP == 0
                || invoke_pakt.candidate_peer_info_his_.DetectIP == 4294967295 //255.255.255.255
                || invoke_pakt.candidate_peer_info_his_.DetectUdpPort == 0
                || invoke_pakt.candidate_peer_info_his_.DetectUdpPort == 65535)
                return;

            protocol::StunInvokePacket temp_invoke_pakt = invoke_pakt;
            if (invoke_pakt.candidate_peer_info_mine_.DetectIP == 0)
            {
                boost::uint32_t detected_ip = invoke_pakt.end_point.address().to_v4().to_ulong();
                boost::uint16_t detected_udp_port = invoke_pakt.end_point.port();
                temp_invoke_pakt.candidate_peer_info_mine_.DetectIP = detected_ip;
                temp_invoke_pakt.candidate_peer_info_mine_.DetectUdpPort = detected_udp_port;
            }
            
            temp_invoke_pakt.end_point = end_point_invoke;
            temp_invoke_pakt.candidate_peer_info_mine_.PeerVersion = temp_invoke_pakt.peer_version_;
            temp_invoke_pakt.peer_version_ = protocol::PEER_VERSION;

            stun_statistic_->SubmitInvokeResponse(temp_invoke_pakt.length());

            DoSendPacket(temp_invoke_pakt, temp_invoke_pakt.candidate_peer_info_his_.PeerVersion);
        //} 
        //else
        //{
        //    LOG(__ERROR, "packet", "packet parse error");
        //    assert(0);
        //}
    }

    void UdpStunModule::PrintNetInfo(bool is_timer)
    {
        pair<string, string> str_fmt;
        string str_hd = boost::str(boost::format("%9s%10s%10s%10s%10s%10s") 
            %"Times" %"Bytes" %"Freq" %"Speed" %"I Freq" %"I Speed");

        OutputInfo("---------------------------- Net Info ----------------------------", is_timer);
        CONSOLE_LOG("");
        if (is_timer)
        {
            RELEASE_LOG("");
        }
        size_t time_sec = time(NULL) - time_start_;
        OutputInfo("During Time: " << time_sec/3600 << ":" << time_sec%3600/60 << ":" << time_sec%60, is_timer);
        OutputInfo("Operation       " << str_hd ,is_timer);

        StunStatisticInfo statistic_info = stun_statistic_->TotalInfo();                    
        str_fmt = FormatOutputStr(statistic_info);
        OutputInfo("AllRequest      " << str_fmt.first ,is_timer);
        OutputInfo("AllResponse     " << str_fmt.second ,is_timer);

        statistic_info = stun_statistic_->HandSharkInfo();
        str_fmt = FormatOutputStr(statistic_info);
        OutputInfo("HandSharkRqst   " << str_fmt.first ,is_timer);
        OutputInfo("HandSharkRsps   " << str_fmt.second ,is_timer);

        statistic_info = stun_statistic_->KeepAliveInfo();
        str_fmt = FormatOutputStr(statistic_info);
        OutputInfo("KeepAliveRqst   " << str_fmt.first ,is_timer);
        OutputInfo("KeepAliveRsps   " << str_fmt.second ,is_timer);

        statistic_info = stun_statistic_->InvokeInfo();
        str_fmt = FormatOutputStr(statistic_info);
        OutputInfo("InvokeRequest   " << str_fmt.first ,is_timer);
        OutputInfo("InvokeResponse  " << str_fmt.second ,is_timer);
        OutputInfo("-------------------------------------------------------------------", is_timer);
    }

    pair<std::string, std::string> UdpStunModule::FormatOutputStr(StunStatisticInfo statistic_info)
    {
        string str_rq = boost::str(boost::format("%9u %9u %9f %9f %9f %9f")
            %statistic_info.request_count_
            %statistic_info.request_bytes_
            %statistic_info.request_frequency_
            %statistic_info.request_speed_
            %statistic_info.recent_minute_request_frequency_
            %statistic_info.recent_minute_request_speed_);
        string str_rp = boost::str(boost::format("%9u %9u %9f %9f %9f %9f")
            %statistic_info.response_count_
            %statistic_info.response_bytes_
            %statistic_info.response_frequency_
            %statistic_info.response_speed_
            %statistic_info.recent_minute_response_frequency_
            %statistic_info.recent_minute_response_speed_);
        return make_pair(str_rq, str_rp);
    }

    void UdpStunModule::RegisterAllPackets()
    {
        if (false == is_running_)
            return ;
        protocol::register_stun_packet(*udp_server_);
    }

    void UdpStunModule::OutPutPeerCount()
    {
        FILE * fp = fopen( "peer_count.txt" , "w" );
        if( 0 == fp ) {
            // 可能是被占用了
            return;
        }
       
        std::ostringstream oss;
        oss << peer_count_ << "\r\n";

        fwrite( oss.str().c_str() , oss.str().size() , 1 , fp );
        fclose( fp );
    }
}