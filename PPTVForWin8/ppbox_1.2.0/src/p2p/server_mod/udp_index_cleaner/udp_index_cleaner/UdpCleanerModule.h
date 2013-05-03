#pragma once

#include "framework/network/UdpServer.h"
#include "framework/timer/Timer.h"
#include "protocal/StatisticPacket.h"
#include "udp_index_cleaner/Config.h"

using namespace std;
using namespace stdext;
using namespace protocal;
using namespace framework::timer;

namespace udplog
{

    struct MemRecord
        : public boost::noncopyable
        , public boost::enable_shared_from_this<MemRecord>
    {
        typedef boost::shared_ptr<MemRecord> p;
        typedef std::map<std::string, time_t> rcd;
        rcd record_;

        static p Create()
        {
            return p(new MemRecord);
        }

    private:
        MemRecord(){};
    };

    //////////////////////////////////////////////////////////////////////////
    // UdpCleanerModule
    class UdpCleanerModule
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpCleanerModule>
        , public framework::network::IUdpServerListener
        , public framework::timer::ITimerListener
    {
    public:

        typedef boost::shared_ptr<UdpCleanerModule> p;

        static UdpCleanerModule::p Inst() { return inst_; }

    public:

		// ±¾µØUdpServer¼àÌý¶Ë¿Ú
        void Start(u_short local_udp_port);

        void Stop();

        bool IsRunning() const { return is_running_; }

    public:

        void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf);

        void DoSendPacket(boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet);

        void OnPrintMem();

        void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

    private:

        void OnVisit(boost::asio::ip::udp::endpoint& end_point, StatisticIndexUrlPacket::p packet);
        void CheckLog();

        inline pair<string,string> DateNowStr()
        {
            SYSTEMTIME sys_time;
            GetSystemTime(&sys_time);
            return make_pair(boost::str(boost::format("%04d-%02d-%02d") 
                %sys_time.wYear %sys_time.wMonth %sys_time.wDay),
                boost::str(boost::format("%02d") %sys_time.wHour) );
        }

    private:
        UdpCleanerModule():is_running_(false){};
        static UdpCleanerModule::p inst_;

    private:
        bool is_running_;

        framework::network::UdpServer::p udp_server_;
		framework::timer::PeriodicTimer::p check_time_timer_;

        pair<string/*date*/,string/*hour*/> last_clean_time_;
        string lastwritetime_;

        MemRecord::p mem_record_;
    };
}
