#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/udp.hpp>
#include "struct/UdpBuffer.h"
#include "protocol/Protocol.h"
//#include "protocol/Packet.h"
//#include "protocal/StunServerPacket.h"
#include "statistic/StunStatistic.h"
#include "framework/timer/Timer.h"
#include "framework/timer/AsioTimerManager.h"
#include "MainThread.h"
using namespace std;
//using namespace framework;
//using namespace network;

namespace udpstun
{
    class UdpStunModule
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpStunModule>
        , public protocol::IUdpServerListener
    {
    public:
        typedef boost::shared_ptr<UdpStunModule> p;
        static p Create() { return p(new UdpStunModule); }
    public:
        void Start(u_short local_udp_port);
        void Stop();
        bool IsRunning() const { return is_running_; }

        void OnUdpRecv( protocol::Packet const & packet);

        void OnHandShake( const protocol::StunHandShakePacket & handshake_pakt );
        void OnInvoke( const protocol::StunInvokePacket & invoke_pakt );

        void PrintNetInfo(bool is_timer);

        virtual void OnTimerElapsed(framework::timer::Timer::pointer * pointer, u_int times);

        framework::timer::AsioTimerManager * asio_timer_manager_;

        void RegisterAllPackets();

        template<typename type>
        void DoSendPacket(type const & packet, boost::uint32_t dest_protocol_version);

        StunStatistic::p GetStatistic() { return stun_statistic_; }

    private:
        void OutPutPeerCount();

    private:
        pair<std::string, std::string> FormatOutputStr(StunStatisticInfo statistic_info);

    private:
        UdpStunModule():is_running_(false){};

        bool is_running_;

        u_int keep_alive_time_;

        boost::uint16_t min_protocol_version_;

        StunStatistic::p stun_statistic_;

        size_t time_start_;

        framework::timer::PeriodicTimer::pointer print_net_info_timer_;

        static UdpStunModule::p inst_;
        boost::shared_ptr<protocol::UdpServer> udp_server_;

        boost::uint32_t peer_count_;

        framework::timer::TickCounter tick_count_;
    public:
        static UdpStunModule::p Inst() { return inst_; };
    };

// #define OutputInfo(sstr, is_timer) do\
//     {\
//         if (is_timer == true)\
//         {\
//             RELEASE_OUTPUT(sstr);\
//         }\
//         CONSOLE_OUTPUT(sstr);\
//     }while(false);
#define OutputInfo(sstr, is_timer) do\
    {\
    if (is_timer == true)\
    {\
    }\
    CONSOLE_OUTPUT(sstr);\
    }while(false);
}