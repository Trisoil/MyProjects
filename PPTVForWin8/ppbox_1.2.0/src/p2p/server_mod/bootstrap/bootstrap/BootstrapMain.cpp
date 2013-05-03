//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Bootstrap.h"
#include "MainThread.h"
#ifdef WIN32
#include "MiniDump.h"
#endif
#include <string>

#include <cstdio>

#include <log4cplus/configurator.h>

FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");

void PrintSrvList(const bootstrap::SrvList& sl)
{
    std::cout<<"[Type]: "<<sl.type<<std::endl;
    std::cout<<"[Level]: "<<sl.level<<std::endl;
    for (std::vector<bootstrap::SrvInfo>::const_iterator vi = sl.sevs.begin();
        vi != sl.sevs.end(); ++vi)
    {
        printf("\tmod: %u conprtcl: %u ip: %s port: %u\n", 
            vi->mod, vi->prtcl, 
            boost::asio::ip::address_v4(vi->ip).to_string().c_str(), vi->port);
    }
    for (unsigned int i = 0; i < sl.appds.size(); ++i)
    {
        printf("%u %u\n", i, sl.appds[i]);
    }
}

void PrintSrvMap(const bootstrap::ServerMap_T& srv_map)
{
    for (bootstrap::ServerMap_T::const_iterator it = srv_map.begin();
        it != srv_map.end(); ++it)
    {
        PrintSrvList(it->second);
    }
}

void PrintAcServer(bootstrap::ServerPacker& srv_packer, boost::uint32_t ac)
{
    const bootstrap::AcServer& ac_srv = srv_packer.GetAcServer(ac);
    std::cout<<"地域码："<<ac<<std::endl;
    std::cout<<"-------------------- Commit Tracker --------------------"<<std::endl;
    PrintSrvMap(ac_srv.tracker_commit);
    std::cout<<"-------------------- List Tracker --------------------"<<std::endl;
    PrintSrvMap(ac_srv.tracker_list);
    std::cout<<"-------------------- Cache --------------------"<<std::endl;
    PrintSrvMap(ac_srv.cache);
    std::cout<<"-------------------- Collection --------------------"<<std::endl;
    PrintSrvMap(ac_srv.collection);
    std::cout<<"-------------------- Config --------------------"<<std::endl;
    std::cout<<"collect_pb: "<<ac_srv.config.collect_pb<<std::endl;
    std::cout<<"up_pic_pb: "<<ac_srv.config.up_pic_pb<<std::endl;
}

void TestServerPacker(bootstrap::ServerPacker& srv_packer)
{
    // ----------------------------------------------------------------------
    // 测试ServerPacker

    std::cout<<
        "**********************************************************************\n"
        "                   ServerPacker测试结果\n"
        "**********************************************************************\n";

    std::cout<<"-------------------- Index Server --------------------"<<std::endl;
    PrintSrvList(srv_packer.GetIndexSrv());
    std::cout<<"-------------------- Stun Server --------------------"<<std::endl;
    PrintSrvList(srv_packer.GetStunSrv());

    PrintAcServer(srv_packer, 0);
    PrintAcServer(srv_packer, 10002);
}

void TestIPLib(bootstrap::IPLib& ip_lib)
{
    // ----------------------------------------------------------------------
    // 测试IPLib

    std::cout<<
        "**********************************************************************\n"
        "                   IPLib测试结果\n"
        "**********************************************************************\n";

    std::cout<<"-------------------- IPLib当前数据 --------------------"<<std::endl;

    printf("IPLibVersion: %u\n", ip_lib.Version());
    printf("PublicID: %u\n", ip_lib.PublicAc());
    printf("AreaCode: \n");
    std::set<boost::uint32_t> ac_array = ip_lib.GetAcList();
    for (std::set<boost::uint32_t>::iterator it = ac_array.begin();
        it != ac_array.end(); ++it)
    {
        printf("\t%u\n", *it);
    }

    std::cout<<"-------------------- 查询 --------------------"<<std::endl;
    boost::uint32_t ac;
    std::vector<std::string> ip_s;
    printf("%-20s%-17s\n", "IP Address", "Area Code");
    printf("---------------     ----------\n");
    ip_s.push_back("192.168.1.3");
    ip_s.push_back("192.168.31.3");
    ip_s.push_back("220.113.0.2");
    ip_s.push_back("220.113.14.254");
    ip_s.push_back("211.161.192.123");
    ip_s.push_back("172.16.0.1");
    for (std::vector<std::string>::iterator it = ip_s.begin();
        it != ip_s.end(); ++it)
    {
        ac = ip_lib.FindAreaCode(*it);
        printf("%-20s%-17u\n", it->c_str(), ac);
    }
}

// void TestIOUtil()
// {
//     // ----------------------------------------------------------------------
//     // 测试IOUtil
// 
//     std::cout<<
//         "**********************************************************************\n"
//         "                   IOUtil测试结果\n"
//         "**********************************************************************\n";
// 
//     std::cout<<"-------------------- 判断文件是否存在 --------------------"<<std::endl;
// 
//     std::cout<<"[E:\\test\\emacs\\byte_stream\\ByteStream.h, 已存在] : "
//         <<file_io::AccessFile(TEXT("E:\\test\\emacs\\byte_stream\\ByteStream.h"))
//         <<std::endl;
//     std::cout<<"[E:\\test\\emacs\\byte_stream\\ByteStream_no.h, 不存在] : "
//         <<file_io::AccessFile(TEXT("E:\\test\\emacs\\byte_stream\\ByteStream_no.h"))
//         <<std::endl;
//     std::cout<<"[E:\\test\\emacs_no\\byte_stream\\ByteStream.h, 不存在] : "
//         <<file_io::AccessFile(TEXT("E:\\test\\emacs_no\\byte_stream\\ByteStream.h"))
//         <<std::endl;
// 
//     std::cout<<"-------------------- 判断目录是否存在 --------------------"<<std::endl;
//     std::cout<<"[E:\\test\\emacs\\byte_stream, 已存在] : "
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\byte_stream"))
//         <<std::endl;
//     std::cout<<"[E:\\test\\emacs\\byte_stream_not, 不存在] : "
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\byte_stream_not"))
//         <<std::endl;
//     std::cout<<"[E:\\test\\emacs_no\\byte_stream, 不存在] : "
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs_no\\byte_stream"))
//         <<std::endl;
// 
//     std::cout<<"-------------------- 遍历目录 --------------------"<<std::endl;
//     std::cout<<"[E:\\test\\emacs, 已存在] :"<<std::endl;
//     list<tstring> file_list, dir_list;
//     file_io::GetDirFileList(TEXT("E:\\test\\emacs"), file_list, dir_list);
//     std::cout<<"==> 文件："<<std::endl;
//     for (list<tstring>::iterator it = file_list.begin();
//         it != file_list.end(); ++it)
//     {
//         std::cout<<"\t"<<w2b(*it)<<std::endl;
//     }
//     std::cout<<"==> 目录："<<std::endl;
//     for (list<tstring>::iterator it = dir_list.begin();
//         it != dir_list.end(); ++it)
//     {
//         std::cout<<"\t"<<w2b(*it)<<std::endl;
//     }
// 
//     std::cout<<"-------------------- 创建目录 --------------------"<<std::endl;
//     std::cout<<"[E:\\test\\emacs\\test_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\test_dir"))
//         <<std::endl;
//     file_io::MakeDir(TEXT("E:\\test\\emacs\\test_dir"));
//     std::cout<<"[E:\\test\\emacs\\test_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\test_dir"))
//         <<std::endl;
// 
//     std::cout<<"-------------------- 删除目录 --------------------"<<std::endl;
//     std::cout<<"[E:\\test\\emacs\\test_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\test_dir"))
//         <<std::endl;
//     file_io::DelDir(TEXT("E:\\test\\emacs\\test_dir"));
//     std::cout<<"[E:\\test\\emacs\\test_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\test_dir"))
//         <<std::endl;
// 
//     std::cout<<"-------------------- 拷贝目录 --------------------"<<std::endl;
//     std::cout<<"[E:\\test\\emacs\\copy_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\copy_dir"))
//         <<std::endl;
//     file_io::CopyDir(TEXT("E:\\test\\emacs"), TEXT("E:\\test\\emacs\\copy_dir"), false);
//     std::cout<<"[E:\\test\\emacs\\copy_dir] :"
//         <<file_io::AccessDir(TEXT("E:\\test\\emacs\\copy_dir"))
//         <<std::endl;
// }

void TestBootstrap()
{
    MainThread::Inst().Start();
    bootstrap::Bootstrap::Inst()->Start("bs_config");
    bootstrap::Bootstrap::Inst()->ShowStatus();

    TestIPLib(bootstrap::Bootstrap::Inst()->GetIPLib());
    TestServerPacker(bootstrap::Bootstrap::Inst()->GetSrvPacker());

    MainThread::Inst().Stop();
}

void StartServer()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "StartServer()");

    using namespace bootstrap;
    printf(
        "*******************************************************************************\n"
        "                          PPVA Bootstrap Server 3.1.0.4\n"
        "*******************************************************************************\n"
        );

//     framework::configure::Config conf("log.conf");
//     framework::logger::glog.load_config(conf);

    MainThread::Inst().Start();

    LOG4CPLUS_INFO(bootstrap::Loggers::BootStrap(), "StartServer");

    MainThread::IOS().post(boost::bind(&Bootstrap::Start, Bootstrap::Inst(), "bs_conf"));

    const string version_str = "3.1.0.4";

    const string cmd_list_str = 
        "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        "\texit/quit                            退出\n"
        "\thelp/?                               输出命令列表\n"
        "\tloadconfig                           加载配置文件\n"
        "\tloaddata                             加载IP库和服务器信息\n"
        "\tstatus                               显示服务器状态\n"
        "\tpacket                               显示报文统计信息\n"
        "\tversion                              版本号\n"
        "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
        ;

    string cmd;
    while (true)
    {
        cin>>cmd;
        boost::algorithm::to_lower(cmd);
        if ("exit" == cmd || "quit" == cmd)
        {
            break;
        }
        else if ("?" == cmd || "help" == cmd)
        {
            printf("%s\n", cmd_list_str.c_str());
        }
        else if ("version" == cmd || "ver" == cmd)
        {
            printf("Bootstrap Version: %s\nCopyright (c) 2010 PPLive\n",
                version_str.c_str());
        }
        else if ("load_config" == cmd || "loadconfig" == cmd)
        {
            MainThread::IOS().post(boost::bind(&Bootstrap::LoadConfig, 
                Bootstrap::Inst()));
        }
        else if ("load_data" == cmd || "loaddata" == cmd)
        {
            MainThread::IOS().post(boost::bind(&Bootstrap::LoadDataInfo, 
                Bootstrap::Inst()));
        }
        else if ("status" == cmd)
        {
            MainThread::IOS().post(boost::bind(&Bootstrap::ShowStatus, 
                Bootstrap::Inst()));
        }
        else if ("packet" == cmd || "pakt" == cmd)
        {
            MainThread::IOS().post(boost::bind(&Bootstrap::ShowPaktStat,
                Bootstrap::Inst()));
        }
        else
        {
            printf("无效命令！请参考以下命令：\n");
            printf("%s", cmd_list_str.c_str());
        }
    }

    LOG4CPLUS_INFO(bootstrap::Loggers::BootStrap(), "Stop");
    MainThread::Inst().Stop();
}

int main(int argc, char* argv[])
{
//     freopen("out", "w", stdout);
//     TestServerPacker();
//     TestIPLib();
//  TestIOUtil();
//     TestBootstrap();
    log4cplus::PropertyConfigurator::doConfigure("Bootstrap-logging.conf");
#ifdef WIN32
    SetUnhandledExceptionFilter(CreateMiniDump);
#endif
    StartServer();
    return 0;
}