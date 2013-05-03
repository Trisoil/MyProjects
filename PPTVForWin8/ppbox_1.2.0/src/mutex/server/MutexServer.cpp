// MutexServer.cpp

#include "mutex/server/Common.h"
#include "mutex/server/MutexServer.h"
#include "mutex/server/HttpServer.h"
#include "mutex/server/HttpSessionManager.h"
#include "mutex/server/UdpServer.h"
using namespace mutexserver;

#include <util/protocol/http/HttpProxyManager.h>

#include <framework/filesystem/Path.h>
#include <framework/configure/Config.h>
#include <framework/network/NetName.h>
using namespace framework::configure;
using namespace framework::logger;

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

UdpServer * pUdpServer;

std::string version = "0.1.4.3";

    FRAMEWORK_LOGGER_DECLARE_MODULE("main");

    void timer_tick(const boost::system::error_code& ,boost::asio::deadline_timer* t);

    int main(int argc, char* argv[])
    {
        std::cout<<"version:" << version <<"\n";
        //std::cout<<"check=" <<MutexSession::check_md5(1281923768828,"pid&aid&&mid","b5df770b35435453117afe3239fff68e");

        update_time();
        boost::asio::io_service ios;
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(1));
        t.async_wait(boost::bind(timer_tick,
            boost::asio::placeholders::error, &t));

        framework::configure::Config conf("mutex.ini");
        framework::logger::glog.load_config(conf);
        LOG_F(Logger::kLevelInfor, ("server start."));
        int err = 0;
        boost::uint16_t udp_port=4040;
        boost::uint16_t tcp_port=0;
        boost::uint16_t interval=0;

        conf.register_module("mutex")
            << CONFIG_PARAM_RDWR(interval)
            << CONFIG_PARAM_RDWR(tcp_port)
            << CONFIG_PARAM_RDWR(udp_port);
        if (interval < 20) {
            interval = 20;
        }
        set_interval(interval);

        LOG_F(Logger::kLevelInfor, ("udp_port=%d,tcp_port=%d\n", udp_port, tcp_port));

        UdpServer udpServer(ios);
        pUdpServer = &udpServer;

        bool re = udpServer.Listen(udp_port);
        if (re) {
            LOG_F(Logger::kLevelInfor, ("udp listen ok."));
        } else {
            LOG_F(Logger::kLevelError, ("udp listen failed."));
            return 1;
        }
        udpServer.StartRecv();

        //http
        framework::network::NetName addr("0.0.0.0", tcp_port);
        util::protocol::HttpProxyManager<HttpServer> mgr( ios, addr);
        mgr.start();
        ios.run();
        return 0;
    }

    void timer_tick(const boost::system::error_code & ,
        boost::asio::deadline_timer* t)
    {
        update_time();
        MutexSession::Inst()->ClearExpiredSession();
        HttpSessionManager::Inst()->timeOutSessionReponse();
        t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
        t->async_wait(boost::bind(timer_tick,
            boost::asio::placeholders::error, t));
    }

    namespace mutexserver
    {
        UdpServer * getUdpServer() {
            return pUdpServer;
        }
    }

/*
0.1.0.0
	去掉MEV校验
0.1.1.0
	加上MEV校验和收包信息打印
0.1.2.0
	加上HTTP查询接口
0.1.2.1
	打印返回结果
	调整适应linux下的编译
0.1.2.2
	修正第一次HTTP KeepAlive 为立即返回
0.1.3.0
	修正协议Action与文档一致
0.1.3.1
	修正http第一次认证 返回失败的BUG
	调整协议打印方式 方便测试
0.1.4.0
	checksum功能加上
	本版本 禁用了checksum mev 及日志，提供测试使用
0.1.4.1
	功能与0.1.4.0相同，只是开了checksum 和日志
0.1.4.2
	修正KickOut报文没加checksum的BUG
0.1.4.3
	修正checksum 失败后，没有继续收包的BUG
*/