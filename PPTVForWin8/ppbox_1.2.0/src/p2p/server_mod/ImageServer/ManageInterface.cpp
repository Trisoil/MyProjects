#include "Common.h"
#include "ManageInterface.h"
#include "CacheManager.h"

#include <PacketImageServerManage.h>
using namespace PacketImageServerManage;

#include <MsgTrans/MsgServer3.h>
#include <MsgTrans/TransListener.h>
#include <MsgTrans/TransServer.h>
using namespace Util;

class ImageManageServer
    : public MsgServer3<ImageManageServer, iarchive, oarchive, Util::TransListener_Base<Util::TransServer_Base<read_packet, send_packet> > >
{
public:
    typedef MsgServer3<ImageManageServer, iarchive, oarchive, Util::TransListener_Base<Util::TransServer_Base<read_packet, send_packet> > > SuperType;
    /** 以下三个函数是作为服务器所需的初始化、启动、停止接口
    */
    ImageManageServer()
    {
        // 设置默认配置
        addr.ip = "0.0.0.0";
        addr.port = 7200;
        log_level = 2;

        register_packet(packet_copy_add());
        register_packet(packet_copy_delete());
        register_packet(packet_copy_modify_status());
    }

public:
    void process_request(
        packet_copy_add & p, 
        Interface::response_type const & r)
    {
        packet_copy_add::input_type const & in = p.input;
        //packet_copy_add::output_type & out = p.output;

        logger.Log(Logger::kLevelDebug, log_level, "[ManageInterface::add_copy] (resId = %s)", 
            in.resId.c_str());

        ErrorCode ret = CacheManager::AddResource(in.resId, in.disk);
        r(ret, "");
    }

    void process_request(
        packet_copy_delete & p, 
        Interface::response_type const & r)
    {
        packet_copy_delete::input_type const & in = p.input;
        //packet_copy_delete::output_type & out = p.output;

        logger.Log(Logger::kLevelDebug, log_level, "[ManageInterface::delete_copy] (resId = %s)", 
            in.resId.c_str());

        CacheManager::DeleteResource(in.resId, r);
    }

    void process_request(
        packet_copy_modify_status & p, 
        Interface::response_type const & r)
    {
        packet_copy_modify_status::input_type const & in = p.input;
        //packet_copy_modify_status::output_type & out = p.output;

        logger.Log(Logger::kLevelDebug, log_level, "[ManageInterface::modify_copy_status] (resId = %s, status = %s)", 
            in.resId.c_str(), in.status == in.online ? "online" : "offline");

        ErrorCode ret = in.status == in.online ? 
            CacheManager::ResumeResource(in.resId) : 
            CacheManager::PauseResource(in.resId);
        r(ret, "");
    }
};

ImageManageServer * ManageInterface::server_ = NULL;

ErrorCode ManageInterface::init(
                          Config & conf)
{
    server_ = new ImageManageServer;
    return server_->init(conf, "ManageInterface");
}

ErrorCode ManageInterface::start(
                           boost::asio::io_service & ios)
{
    return server_->start(ios);
}

ErrorCode ManageInterface::stop()
{
    if (server_) {
        server_->stop();
        delete server_;
    }
    return ERROR_SUCCEED;
}
