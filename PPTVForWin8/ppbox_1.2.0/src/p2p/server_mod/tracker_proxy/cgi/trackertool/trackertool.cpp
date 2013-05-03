#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <string.h>
#include <vector>
//#include "PPlivePubApi.h"
#include "TrackerApi.h"
#include "CgiEx.h"
#include "framework/Framework.h"
#include "framework/configure/Config.h"
#include "framework/string/ParseStl.h"
//#include "framework/logger/Logger.h"
//#include <framework/logger/LoggerListRecord.h>
//#include <framework/logger/LoggerFormatRecord.h>
//#include <framework/logger/LoggerStreamRecord.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/bind.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

//#include <fstream>

#ifndef BOOST_WINDOWS_API
#include <fcgi_stdio.h>
#endif

using namespace std;
using namespace framework;
using namespace ns_pplive::ns_tracker;

boost::asio::io_service g_ios;
double g_timeout = 1.0;
//const std::string cgiconf_file_name = "/usr/local/nginx/conf/trackercgi.conf";
const std::string log_conf = "/usr/local/nginx/conf/cgilogconf.conf";

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_tracker");

FRAMEWORK_LOGGER_DECLARE_MODULE("trackertool");


class TrackerTool:public CgiEx
{
public:	
    TrackerTool():CgiEx(true){}
    ~TrackerTool(){}
    bool DealInput(map<std::string, std::string> &Params);
    bool DealList(map<std::string, std::string> &Params);
    bool DealGetPeerCount(map<std::string, std::string> &Params);
    bool DealGetPeerCountAll(map<std::string, std::string> &Params);
    bool DealGetPeerRids(map<std::string, std::string> &Params);
    bool DealTrackerStatistic(map<std::string, std::string> &Params); 
    bool DealShowUsage(map<std::string, std::string> &Params);

    void OutputXml(int iRetCode);
    void OutputXml(const std::string& rid,const vector<CandidatePeerInfo>& candidate_peer_info,int iRetCode=0);
    //void OutputData(const std::string& rid,const vector<CandidatePeerInfo>& candidate_peer_info);
    void OutputBegin(int iRetCode);
    void OutputEnd();
    void OutputXml(const std::string& rid,int peer_count);
    void OutputXml(const std::string& pid,vector<RID>& resources);
    void OutputXml(const std::string& tracker_statistic);
    void OutputXml(map<boost::asio::ip::udp::endpoint,int>&  tracker_peercoun,const std::string& rid);
    void GetEndPoint(map<std::string, std::string> &Params);
    static void GetPeerCountProc(const void* para);
   
    //ns_pplive::ns_tracker::TrackerApi trackerapi_;
    ns_pplive::ns_tracker::TrackerClient track_client_;
    boost::asio::ip::udp::endpoint endpoint_;
    std::string query_time_;
};

void TrackerTool::OutputXml(int iRetCode)
{
    std::string tmprid;
    vector<CandidatePeerInfo> tmpinfo;
    OutputXml(tmprid,tmpinfo,iRetCode);
}

void TrackerTool::OutputBegin(int iRetCode)
{
    printf("%s",OutputHttpHeader(iRetCode).c_str());
    printf("<?xml version=\"1.0\" encoding=\"GBK\"?>\r\n");	
    printf("<pplive time=\"%s\">\r\n",query_time_.c_str());	
    printf("	<result type=\"%d\">\r\n",iRetCode);
    printf("		<message>\r\n");
    printf("		</message>\r\n");
}

void TrackerTool::OutputEnd()
{
    printf("		</data>\r\n");
    printf("	</result>\r\n");
    printf("</pplive>\r\n");
}

void TrackerTool::OutputXml(const std::string& rid,int peer_count)
{
    OutputBegin(0);
    printf("		<data sr=\"%s\" c=\"%d\">\r\n",rid.c_str(),peer_count);
    OutputEnd();
}

void TrackerTool::OutputXml(map<boost::asio::ip::udp::endpoint,int>&  tracker_peercount,const std::string& rid)
{
    OutputBegin(0);
    int total = 0;
    for(map<boost::asio::ip::udp::endpoint,int>::iterator it = tracker_peercount.begin();it != tracker_peercount.end();++it)
    {
        total += it->second;
    }
    printf("		<data sr=\"%s\" trackercount=\"%d\" peercount=\"%d\">\r\n",rid.c_str(),tracker_peercount.size(),total);
    for(map<boost::asio::ip::udp::endpoint,int>::iterator it = tracker_peercount.begin();it != tracker_peercount.end();++it)
    {
        stringstream ss;
        ss<<(it->first);
        printf("            <tracker endpoint=\"%s\" c=\"%d\" />\r\n",ss.str().c_str(),it->second);
    }
    OutputEnd();
}

void TrackerTool::OutputXml(const std::string& rid,const vector<CandidatePeerInfo>& candidate_peer_info,int iRetCode)
{
    OutputBegin(iRetCode);
    if (iRetCode != 0)
    {
        printf("		<data>\r\n");
    }
    else
    {
        printf("		<data sr=\"%s\" c=\"%d\">\r\n",rid.c_str(),candidate_peer_info.size());
        for (vector<CandidatePeerInfo>::const_iterator it=candidate_peer_info.begin(); it!=candidate_peer_info.end(); ++it)
        {
            printf("<p v=\"%d\" i1=\"%s\" p1=\"%u\" i2=\"%s\" p2=\"%u\" up=\"%u\" tp=\"%u\" />\r\n",
                it->PeerVersion,boost::asio::ip::address_v4((it->IP)).to_string().c_str(),it->UdpPort,
                boost::asio::ip::address_v4((it->DetectIP)).to_string().c_str(),it->DetectUdpPort,it->UploadPriority,it->TrackerPriority);
        }
    }
    OutputEnd();
}

void TrackerTool::OutputXml(const std::string& pid,vector<RID>& resources)
{
    OutputBegin(0);
    printf("		<data sr=\"%s\" c=\"%d\">\r\n",pid.c_str(),resources.size());
    for (unsigned i=0;i<resources.size();++i)
    {
        printf("			<rid value%d=\"%s\" />\r\n",i,resources[i].to_string().c_str());
    }
    OutputEnd();
}

void TrackerTool::OutputXml(const std::string& tracker_statistic)
{
    OutputBegin(0);
    printf("		<data statistic=\"%s\">\r\n",tracker_statistic.c_str());	
    OutputEnd();
}

bool TrackerTool::DealList(map<std::string, std::string> &Params)
{
    //http://172.16.200.11:1982/trackertool?ip=220.170.193.111&port=18000&action=list&rid=275781FED82F1AD9B45637CAD3DB809F
    GetEndPoint(Params);
    if (Params["rid"].size() != 32)
    {	
        LOG_S(::framework::logger::Logger::kLevelError, "rid:" <<Params["rid"]<<"error");
        //参数有问题,返回-3		
        OutputXml(-3);
        return false;		
        //Params["rid"]="275781FED82F1AD9B45637CAD3DB809F";		
    }

    if (Params["pid"].size() != 32)
    {
        Params["pid"]="A5415160B93147369F345713BE501A5C";
    }

    boost::int32_t reqcount = atoi(Params["count"].c_str());
    if (0 == reqcount)
    {
        reqcount=50;
    }
    LOG_S(::framework::logger::Logger::kLevelDebug, "rid:" <<Params["rid"]<<" pid:"<<Params["pid"]);

    vector<CandidatePeerInfo> all_candidate_peer_info;	
    if (0 != track_client_.ListPeer(all_candidate_peer_info,rand(),RID(Params["rid"]),Guid(Params["pid"]),reqcount,endpoint_,g_ios,g_timeout))
    {
        OutputXml(-2);
        return false;
    }

    OutputXml(Params["rid"],all_candidate_peer_info,0);
    return true;
}

bool TrackerTool::DealGetPeerCount(map<std::string, std::string> &Params)
{
    //http://172.16.200.11:1982/trackertool?ip=220.170.193.111&port=18000&action=getpeercount&rid=275781FED82F1AD9B45637CAD3DB809F
     GetEndPoint(Params);
    std::string rid = Params["rid"];
    boost::uint32_t peer_count = 0;
    if(0 != track_client_.QueryPeerCount(peer_count,rand(),RID(rid),endpoint_,g_ios,g_timeout))
    {
        OutputXml(-2);
        return false;
    }
    OutputXml(rid,peer_count);
    return true;
}

std::string g_getpeercountrid;
map<boost::asio::ip::udp::endpoint,int >  g_tracker_peercount;

void TrackerTool::GetPeerCountProc(const void* para)
{

    boost::asio::ip::udp::endpoint end_point =  *((boost::asio::ip::udp::endpoint*)para);
    LOG4CPLUS_TRACE(g_logger,"get peer count:"<<end_point<<"rid:"<<g_getpeercountrid);
    TrackerClient tracker_client;
    boost::uint32_t peer_count = 0;
    if(0 != tracker_client.QueryPeerCount(peer_count,rand(),RID(g_getpeercountrid),end_point,g_ios,1.0))
    {
        if(0 != tracker_client.QueryPeerCount(peer_count,rand(),RID(g_getpeercountrid),end_point,g_ios,1.0))
        {
            LOG4CPLUS_INFO(g_logger,"query peer count failed:"<<tracker_client.GetErrMsg()<<" rid:"<<g_getpeercountrid);
            return;
        }
    }
    LOG4CPLUS_TRACE(g_logger,"get peer count:"<<end_point<<"rid:"<<g_getpeercountrid<<" get count:"<<peer_count);
    g_tracker_peercount[end_point] = peer_count;
}

bool TrackerTool::DealGetPeerCountAll(map<std::string, std::string> &Params)
{
    //http://172.16.200.11:1982/trackertool?action=getpeercountall&rid=275781FED82F1AD9B45637CAD3DB809F

    //生成多个线程，每个线程去查询一个tracker

    //读取tracker的信息
    map<int,set<boost::asio::ip::udp::endpoint> >  tracker_group;
    TrackerApi::ReadTrackerConfig(tracker_group);
    if (0 == tracker_group.size())
    {
       // return false;
        set<boost::asio::ip::udp::endpoint> set_tracker;
        boost::asio::ip::address_v4 av4;
        av4 = boost::asio::ip::address_v4::from_string("118.123.201.105");
        boost::asio::ip::udp::endpoint end_point = boost::asio::ip::udp::endpoint(boost::asio::ip::address(av4),18000);
        set_tracker.insert(end_point);
        tracker_group[0] = set_tracker;
    }

    //先获取这个rid对应的组
    g_getpeercountrid = Params["rid"];
//    double start_time = ns_pplive::PubApi::GetExactTimeNow();
    int group_index = protocol::GetGuidMod(Guid(g_getpeercountrid),tracker_group.size());
    set<boost::asio::ip::udp::endpoint>& set_tracker = tracker_group[group_index];
    g_tracker_peercount.clear();
    
    std::vector<boost::thread*> thread_ids;
    for(set<boost::asio::ip::udp::endpoint>::iterator it = set_tracker.begin(); it != set_tracker.end(); ++it)
    {
        boost::thread* pThread = new boost::thread(boost::bind(&TrackerTool::GetPeerCountProc,&(*it)));

        if(NULL == pThread)
        {
            return false;
        }
        else
        {
            thread_ids.push_back(pThread);
        }
    }
    for(uint32_t i=0;i<thread_ids.size();++i)
    {
        thread_ids[i]->join();
    }
  //  LOG4CPLUS_INFO(g_logger,"query peer count time spend:"<<(ns_pplive::PubApi::GetExactTimeNow()-start_time)<<" query tracker num:"<<thread_ids.size()<<" response size:"<<g_tracker_peercount.size());
    OutputXml(g_tracker_peercount,g_getpeercountrid);
    return true;  

}


bool TrackerTool::DealGetPeerRids(map<std::string, std::string> &Params)
{
    //http://172.16.200.11:1982/trackertool?ip=220.170.193.111&port=18000&action=getrids&pid=275781FED82F1AD9B45637CAD3DB809F
     GetEndPoint(Params);
    std::string pid = Params["pid"];
    unsigned peer_count = 0;
    vector<RID> resources;
    if(0 != track_client_.QueryPeerResources(resources,rand(),Guid(pid),endpoint_,g_ios,g_timeout))
    {
        OutputXml(-2);
        return false;
    }
    OutputXml(pid,resources);
    return true;
}

bool TrackerTool::DealTrackerStatistic(map<std::string, std::string> &Params)
{
    //http://172.16.200.11:1982/trackertool?ip=220.170.193.111&port=18000&action=trackerstatistic
    GetEndPoint(Params);
    std::string tracker_statistic;
    if( 0 != track_client_.QueryTrackerStatistic(tracker_statistic,rand(),endpoint_,g_ios,g_timeout))
    {
        OutputXml(-2);
        return false;
    }
    OutputXml(tracker_statistic);
    return true;
}

bool TrackerTool::DealShowUsage(map<std::string, std::string> &Params)
{
    OutputBegin(0);
    std::string usage[10];
    usage[0] = "ip=61.155.162.33&port=18000&action=list&rid=275781FED82F1AD9B45637CAD3DB809F";
    usage[1] = "ip=61.155.162.33&port=18000&action=getpeercount&rid=275781FED82F1AD9B45637CAD3DB809F";
    usage[2] = "action=getpeercountall&rid=275781FED82F1AD9B45637CAD3DB809F";
    usage[3] = "ip=61.155.162.33&port=18000&action=getrids&pid=275781FED82F1AD9B45637CAD3DB809F";
    usage[4] = "ip=61.155.162.33&port=18000&action=trackerstatistic";
    LOG4CPLUS_INFO(g_logger,"SERVER_NAME"<<getenv("SERVER_NAME")==NULL?"empty":std::string(getenv("SERVER_NAME")));
    LOG4CPLUS_INFO(g_logger,"SERVER_PORT"<<getenv("SERVER_PORT")==NULL?"empty":std::string(getenv("SERVER_PORT")));
    LOG4CPLUS_INFO(g_logger,"SCRIPT_NAME"<<getenv("SCRIPT_NAME")==NULL?"empty":std::string(getenv("SCRIPT_NAME")));
    LOG4CPLUS_INFO(g_logger,"REMOTE_HOST"<<getenv("REMOTE_HOST")==NULL?"empty":std::string(getenv("REMOTE_HOST")));
    LOG4CPLUS_INFO(g_logger,"REMOTE_ADDR"<<getenv("REMOTE_ADDR")==NULL?"empty":std::string(getenv("REMOTE_ADDR")));
    LOG4CPLUS_INFO(g_logger,"PATH_INFO"<<getenv("PATH_INFO")==NULL?"empty":std::string(getenv("PATH_INFO")));
    LOG4CPLUS_INFO(g_logger,"HTTP_REFERER"<<getenv("HTTP_REFERER")==NULL?"empty":std::string(getenv("HTTP_REFERER")));
    
    //http://172.16.200.11:1982/trackertool?ip=220.170.193.111&port=18000&action=trackerstatistic
     // printf("			<rid value%d=\"%s\" />\r\n",i,resources[i].to_string().c_str());
    printf("		    <data>\r\n");
    for(int i=0;i<10;++i)
    {
        if(!usage[i].empty())
        {
            printf("                    <usage url=\"http://%s:%s%s?%s\" />\r\n",
            getenv("SERVER_NAME")==NULL?"empty":getenv("SERVER_NAME"),getenv("SERVER_PORT")==NULL?"empty":getenv("SERVER_PORT"),
            getenv("SCRIPT_NAME")==NULL?"empty":getenv("SCRIPT_NAME"),usage[i].c_str());
        }
    }
    OutputEnd();
    return true;
}

bool TrackerTool::DealInput(map<std::string, std::string> &Params)
{
    //下面是测试用的东西
#if 0
    Params["ip"] = "220.170.193.111";
    Params["port"] = "18001";
#endif

#if 0
    Params["action"] = "list";
    Params["rid"] = "BB9D1008FC1DA6BCA50AD810F98EAB2B";	
#endif

#if 0
    Params["action"] = "getpeercount";
    Params["rid"] = "BB9D1008FC1DA6BCA50AD810F98EAB2B";
#endif

#if 0
    Params["action"] = "trackerstatistic";
#endif

#if 0
    if(Params.find("action") == Params.end())
    {
        Params["action"]="getrids";
        Params["pid"]="FD7472440B267E2ED897E19F02B20AFE";
    }
#endif

#if 0
    if(Params.find("action") == Params.end())
    {
        Params["action"]="getpeercountall";
        Params["rid"]="60407BB086E993994CFB4FF7B8C1F260";
        //这对ipport没用，只是为了下面统一做的事情。
        Params["ip"] = "220.170.193.111";
        Params["port"] = "18001";
    }
#endif    
    struct tm *newtime;

    time_t long_time;

    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */

    query_time_ = asctime(newtime);
    query_time_ = query_time_.substr(0,query_time_.size()-1);

    std::string action = Params["action"];
    LOG4CPLUS_DEBUG(g_logger,"action:"<<action.c_str());
    bool ret = false;
    if("list"==action)
    {
        ret = DealList(Params);
    }
    else if("getpeercount"==action)
    {
        ret = DealGetPeerCount(Params);
    }
    else if("trackerstatistic" == action)
    {
        ret = DealTrackerStatistic(Params);
    }
    else if("getrids" == action)
    {
        ret = DealGetPeerRids(Params);
    }
    else if("getpeercountall" == action)
    {
        ret = DealGetPeerCountAll(Params);
    }
    else
    {
        ret = DealShowUsage(Params);
    }
    return ret;
}
void TrackerTool::GetEndPoint(map<std::string, std::string> &Params)
{

    //每个请求里都有ip,port的参数，所以先把这些参数提取出来
    std::string ip = Params["ip"];
    unsigned short port = atoi(Params["port"].c_str());

    boost::asio::ip::address_v4 av4;
    av4 = boost::asio::ip::address_v4::from_string(ip);
    endpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address(av4),port);
}

int main( int argc, char *argv[] )
{	

    //framework::configure::Config conf(cgiconf_file_name);
    //framework::logger::glog.load_config(conf);
    log4cplus::PropertyConfigurator::doConfigure(log_conf); 

#ifndef BOOST_WINDOWS_API
    while( FCGI_Accept() >= 0 ) 
#endif
    {
        TrackerTool trackcgi;
        trackcgi.Run();		
    }
    return 0;
}
