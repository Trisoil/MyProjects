#include "Common.h"
#include "ManageInterfaceOld.h"
#include "CacheManager.h"

#include <MsgTrans/MsgServer3.h>
#include <MsgTrans/TransListener.h>
#include <MsgTrans/TransServer.h>
using namespace Util;

#include <Archive/TextIArchive.h>
#include <Archive/TextOArchive.h>

static const UInt32 _MT_COPY_ADD = 2;							//表示增加资源
static const UInt32 _MT_COPY_DELETE = 3;							//表示减少资源

struct packet_movie_add_old
    : packet<_MT_COPY_ADD>
{
    struct input_type
    {
        String rid;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            std::string head;
            int ver;
            ar >> head >> ver >> rid;
        }
    } input;

    struct output_type
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
        }
    } output;
};

struct packet_movie_delete_old
    : packet<_MT_COPY_DELETE>
{
    struct input_type
    {
        String rid;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            std::string head;
            int ver;
            ar >> head >> ver >> rid;
        }
    } input;

    struct output_type
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
        }
    } output;
};


class ImageManageServerOld
    : public MsgServer3<
    ImageManageServerOld, 
    Serialize::TextIArchive, 
    Serialize::TextOArchive, 
    Util::TransListener_Base<Util::TransServer_Base<read_packet, send_packet> > >
{
public:
    typedef MsgServer3<ImageManageServerOld, 
        Serialize::TextIArchive, 
        Serialize::TextOArchive, 
        Util::TransListener_Base<Util::TransServer_Base<read_packet, send_packet> > >
    SuperType;
    /** 以下三个函数是作为服务器所需的初始化、启动、停止接口
    */
    ImageManageServerOld()
    {
        // 设置默认配置
        addr.ip = "0.0.0.0";
        addr.port = 7100;
        log_level = 2;

        register_packet(packet_movie_add_old());
        register_packet(packet_movie_delete_old());
    }

public:
    void process_request(
        packet_movie_add_old & p, 
        Interface::response_type const & r)
    {
        packet_movie_add_old::input_type const & in = p.input;
        //packet_movie_add_old::output_type & out = p.output;

        logger.Log(Logger::kLevelDebug, log_level, "[ManageInterfaceOld::add_movie] (resId = %s)", 
            in.rid.c_str());

        ErrorCode ret = CacheManager::AddResource(in.rid, (size_t)-1);
        if (ret == ERROR_MOVIE_ALREADY_EXIST || ret == ERROR_COPY_ALREADY_EXIST)
            ret = ERROR_SUCCEED;
        r(ret, "22 Serialize::archive 3 ");
    }

    void process_request(
        packet_movie_delete_old & p, 
        Interface::response_type const & r)
    {
        packet_movie_delete_old::input_type const & in = p.input;
        //packet_movie_delete_old::output_type & out = p.output;

        logger.Log(Logger::kLevelDebug, log_level, "[ManageInterfaceOld::delete_movie] (resId = %s)", 
            in.rid.c_str());

        CacheManager::DeleteResource(in.rid, boost::bind(r, ERROR_SUCCEED, "22 Serialize::archive 3 "));
    }
};

ImageManageServerOld * ManageInterfaceOld::server_ = NULL;

ErrorCode ManageInterfaceOld::init(
                             Config & conf)
{
    server_ = new ImageManageServerOld;

    return server_->init(conf, "ManageInterfaceOld");
}

ErrorCode ManageInterfaceOld::start(
                              boost::asio::io_service & ios)
{
    return server_->start(ios);
}

ErrorCode ManageInterfaceOld::stop()
{
    if (server_) {
        server_->stop();
        delete server_;
    }
    return ERROR_SUCCEED;
}
