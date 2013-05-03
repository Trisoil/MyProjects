#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/udp.hpp>
#include "struct/UdpBuffer.h"
#include "protocol/Protocol.h"
#include "statistic/NatCheckStatistic.h"
#include "framework/timer/Timer.h"
#include "framework/timer/AsioTimerManager.h"
#include "MainThread.h"
using namespace std;


namespace udpnatcheck
{
    class UdpNatCheckModule
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpNatCheckModule>
        , public protocol::IUdpServerListener
    {
    public:
        typedef boost::shared_ptr<UdpNatCheckModule> p;
        static p Create() { return p(new UdpNatCheckModule); }
    public:
        //void Start(unsigned master_ip, unsigned short master_port,unsigned slave_ip,unsigned short slave_port);
        void Start();
        void Stop();
        bool IsRunning() const { return is_running_; }

        void OnUdpRecv( protocol::Packet const & packet);

        void OnNatCheckSameRoute(const protocol::NatCheckSameRoutePacket& packet);
        void OnNatNatCheckDiffIp(const protocol::NatCheckDiffIpPacket& packet);
        void OnNatNatCheckDiffPort(const protocol::NatCheckDiffPortPacket& packet);

        //void OnHandShake( const protocol::StunHandShakePacket & handshake_pakt );
        //void OnInvoke( const protocol::StunInvokePacket & invoke_pakt );

        void PrintNetInfo(bool is_timer);

        virtual void OnTimerElapsed(framework::timer::Timer::pointer * pointer, u_int times);

        framework::timer::AsioTimerManager * asio_timer_manager_;

        void RegisterAllPackets();

        template<typename type>
        void DoSendPacket(type const & packet, boost::uint32_t dest_protocol_version);

        //StunStatistic::p GetStatistic() { return stun_statistic_; }

        //要初始化的udpserver太多，就提取一个初始化函数出来吧。
        bool InitUdpServer(boost::shared_ptr<protocol::UdpServer>& udp_server,const string& ip,
            unsigned short port,unsigned short min_protocol_version,unsigned server_recv_count);


        template<typename type>
        void DoMasterSendPacket(type const & packet, boost::uint32_t dest_protocol_version)
        {
            if(false == is_running_) return;

            master_udp_server_->send_packet(packet, dest_protocol_version);
        }

        template<typename type>
        void DoIpDiffSlaveSendPacket(type const & packet, boost::uint32_t dest_protocol_version)
        {
            if(false == is_running_) return;

            slave_udp_server_diff_ip_->send_packet(packet, dest_protocol_version);
        }

        template<typename type>
        void DoPortDiffSlaveSendPacket(type const & packet, boost::uint32_t dest_protocol_version)
        {
            if(false == is_running_) return;

            slave_udp_server_diff_port_->send_packet(packet, dest_protocol_version);
        }

    private:
        void OutPutPeerCount();

    private:
       // pair<std::string, std::string> FormatOutputStr(StunStatisticInfo statistic_info);

    private:
        UdpNatCheckModule():is_running_(false){};

        bool is_running_;

        u_int keep_alive_time_;

        boost::uint16_t min_protocol_version_;

        NatCheckStatistic::p natcheck_statistic_;

        size_t time_start_;

        framework::timer::PeriodicTimer::pointer print_net_info_timer_;

        static UdpNatCheckModule::p inst_;

        boost::shared_ptr<protocol::UdpServer> master_udp_server_;
        boost::uint32_t master_ip_;
        boost::uint16_t master_port_;

        //和master_udp_server是不同的ip
        boost::shared_ptr<protocol::UdpServer> slave_udp_server_diff_ip_;
        boost::uint32_t slave_diff_ip_ip_;
        boost::uint16_t slave_diff_ip_port_;

        //和master_udp_server是相同ip，不同的端口
        boost::shared_ptr<protocol::UdpServer> slave_udp_server_diff_port_;
        boost::uint16_t slave_diff_port_port_;

        boost::uint32_t peer_count_;

        framework::timer::TickCounter tick_count_;
    public:
        static UdpNatCheckModule::p Inst() { return inst_; };
    };

// #define OutputInfo(sstr, is_timer) do\
//     {\
//         if (is_timer == true)\
//         {\
//             RELEASE_OUTPUT(sstr);\
//         }\
//         CONSOLE_OUTPUT(sstr);\
//     }while(false);
//#define OutputInfo(sstr, is_timer) do\
//    {\
//    if (is_timer == true)\
//    {\
//    }\
//    CONSOLE_OUTPUT(sstr);\
//    }while(false);
}