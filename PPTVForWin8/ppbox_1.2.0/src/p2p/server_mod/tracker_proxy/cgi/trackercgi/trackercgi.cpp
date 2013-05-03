#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <hash_map>
#include <set>
#include <string>
#include <string.h>
#include <vector>
#include "TrackerApi.h"
#include "BootstrapClient.h"
#include "PPlivePubApi.h"
#include "CgiEx.h"
#include "framework/Framework.h"
#include "framework/configure/Config.h"
#include "framework/string/ParseStl.h"
#include "framework/string/Uuid.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include "LoadBalance.h"
#include "TrackerPacket.h"

#ifndef BOOST_WINDOWS_API
#include <fcgi_stdio.h>
#endif

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <netdb.h>
#include <sys/socket.h>

using namespace std;
using namespace framework;
using boost::asio::ip::udp;
using namespace ns_pplive;
using namespace protocol;
using namespace ns_pplive::ns_loadbalance;

//extern 
boost::asio::io_service g_ioservice;
const std::string log_conf = "/usr/local/nginx/conf/cgilogconf.conf";

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_tracker");
#define TRACKER_TAG "tracker"

//FRAMEWORK_LOGGER_DECLARE_MODULE("trackercgi");

LoadBalance tracker_mod_loadbalance_;


class TrackerCgi:public CgiEx
{
public:	
    TrackerCgi():CgiEx(true)
	{
		all_timeout_ = atof(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"alltimeout").c_str());
		tracker_timeout_ = atof(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"trackertimeout").c_str());
		max_req_count_ = atoi(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"maxlistcount").c_str());
		if(max_req_count_>200 || max_req_count_< 0)
		{
			max_req_count_ = 100;
		}
		//latest_time_ = 0.0;
	}
    ~TrackerCgi(){}
    bool DealInput(map<std::string, std::string> &Params);
    void OutputXml(int iRetCode);
    void OutputXml(const std::string& rid,const vector<CandidatePeerInfo>& candidate_peer_info,int iRetCode=0);
	void FlushTrackerInfo();
	int ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,
		boost::uint16_t request_peer_count,unsigned request_ip,boost::asio::io_service& ios,double timeout = 0.0);
private:
	ns_pplive::ns_tracker::TrackerApi trackerapi_;
	ns_pplive::ns_tracker::TrackerClient trackerclient_;
	
	double all_timeout_;
	double tracker_timeout_;
	int max_req_count_;	
	static double latest_time_;
	
};


int TrackerCgi::ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid, 
								  boost::uint16_t request_peer_count,unsigned request_ip,boost::asio::io_service& ios,double timeout)
{
	boost::asio::ip::udp::endpoint _endpoint;
	double start_time = ns_pplive::PubApi::GetExactTimeNow();
	set<boost::asio::ip::udp::endpoint> _exlude_endpoint;
	while(candidate_peer_info.size()<request_peer_count && ns_pplive::PubApi::GetExactTimeNow()-start_time < all_timeout_)
	{
		double beforegetresulttime = ns_pplive::PubApi::GetExactTimeNow();

		int ret = tracker_mod_loadbalance_.GetResult(_endpoint, _exlude_endpoint);
		if ( ret == LOADBALANCE_FAIL_CAN_RETRY )
		{
			LOG4CPLUS_DEBUG(g_logger, "GetRandomEnd failed,group size:"<<tracker_mod_loadbalance_.Size()<<" exclude size:"<<_exlude_endpoint.size());	
			continue;
			//return -1;  //此处不用return
		}
		else if ( ret == LOADBALANCE_FAIL_NO_RETRY )
		{
			return 0;
		}


		_exlude_endpoint.insert(_endpoint);
		vector<CandidatePeerInfo> candidatepeerinfo;	
		double beforelisttime = ns_pplive::PubApi::GetExactTimeNow();
        ret = trackerclient_.ListPeerTcpWithIp(candidatepeerinfo,boost::uint32_t(ns_pplive::PubApi::GenRandom()),resource_id,peer_guid,boost::uint16_t(50),request_ip,_endpoint,ios,timeout); 
		if (0 != ret)
		{
			tracker_mod_loadbalance_.UpdatePara(_endpoint, 1, 1.0);
			LOG4CPLUS_DEBUG(g_logger, "listpeer failed,msg:" <<trackerclient_.GetErrMsg()<<"timenow:"<<time(NULL));
		}
		else
		{
			double afterlisttime = ns_pplive::PubApi::GetExactTimeNow();
			double spenttime = afterlisttime - beforelisttime;
			tracker_mod_loadbalance_.UpdatePara(_endpoint, 0,  spenttime);
			if (spenttime < 0)
			{
				LOG4CPLUS_DEBUG(g_logger, "listpeer sucess,msg: spent time :"<<spenttime);
			}
			LOG4CPLUS_DEBUG(g_logger, "listpeer sucess,msg: spent time:"<<ns_pplive::PubApi::GetExactTimeNow()-start_time);
			std::copy(candidatepeerinfo.begin(),candidatepeerinfo.end(),std::back_inserter(candidate_peer_info));	

		}
	}
    LOG4CPLUS_DEBUG(g_logger, "listpeer finished,msg:" <<"all spent time:"<<ns_pplive::PubApi::GetExactTimeNow()-start_time);
	return 0;
}

double TrackerCgi::latest_time_ = 0;

void TrackerCgi::OutputXml(int iRetCode)
{
    std::string tmprid;
    vector<CandidatePeerInfo> tmpinfo;
    OutputXml(tmprid,tmpinfo,iRetCode);
}

void TrackerCgi::OutputXml(const std::string& rid,const vector<CandidatePeerInfo>& candidate_peer_info,int iRetCode)
{
    printf("%s",OutputHttpHeader(iRetCode).c_str());
    printf("<?xml version=\"1.0\" encoding=\"GBK\"?>\r\n");	
    printf("<pplive>\r\n");	
    printf("	<result type=\"%d\">\r\n",iRetCode);
    printf("		<message>\r\n");
    printf("		</message>\r\n");
    if (iRetCode != 0)
    {
        printf("		<data>\r\n");
    }
    else
    {
        printf("		<data sr=\"%s\" c=\"%d\">\r\n",rid.c_str(),candidate_peer_info.size());
        for (vector<CandidatePeerInfo>::const_iterator it=candidate_peer_info.begin(); it!=candidate_peer_info.end(); ++it)
        {
			            /*
            <peer version=206 ip="192.168.21.1" port=9000 detectedIP=222.73.171.26 detectedPort=10001 uploadPriority=60 trackerPriority=60/>
            */
            printf("<p v=\"%d\" i1=\"%s\" p1=\"%u\" i2=\"%s\" p2=\"%u\" up=\"%u\" tp=\"%u\" />\r\n",
                it->PeerVersion,boost::asio::ip::address_v4((it->IP)).to_string().c_str(),it->UdpPort,
                boost::asio::ip::address_v4((it->DetectIP)).to_string().c_str(),it->DetectUdpPort,it->UploadPriority,it->TrackerPriority);
        }
    }

    printf("		</data>\r\n");
    printf("	</result>\r\n");
    printf("</pplive>\r\n");
}

bool TrackerCgi::DealInput(map<std::string, std::string> &Params)
{
	if (tracker_mod_loadbalance_.Size() <= 0)
	{
        LOG4CPLUS_ERROR(g_logger,"tracker_mod_loadbalance_.size:" <<tracker_mod_loadbalance_.Size());
        OutputXml(-4);
		return false;
	}
    if (Params["rid"].size() != 32)
    {	
        LOG4CPLUS_ERROR(g_logger,"rid:" <<Params["rid"]<<"error");
		OutputXml(-3);
		return false;	
    }

    if (Params["pid"].size() != 32)
    {
		Params["pid"]="A5415160B93147369F345713BE501A5C";
    }

    boost::int32_t reqcount = atoi(Params["count"].c_str());
    if (0 == reqcount || reqcount > max_req_count_)
    {
        reqcount=max_req_count_;
    }   

   unsigned request_ip;
   char *remote_ip = getenv("REMOTE_ADDR");
   if (NULL == remote_ip)
   {
       request_ip = 0;
   }
   else
   {
       request_ip = htonl(inet_addr(remote_ip));
   }
    
   LOG4CPLUS_DEBUG(g_logger,  "rid:" <<Params["rid"]<<" pid:"<<Params["pid"]<<" reqip:"<<request_ip);   

    vector<CandidatePeerInfo> all_candidate_peer_info;	

    if (0 != ListPeerTcpWithIp(all_candidate_peer_info,RID(Params["rid"]),Guid(Params["pid"]),reqcount,request_ip,g_ioservice,tracker_timeout_))
    {
        OutputXml(-2);
        return false;
    }

    OutputXml(Params["rid"],all_candidate_peer_info,0);
    return 0;
}
void TrackerCgi::FlushTrackerInfo()
{
	double flush_time = 30*60; //half an hour(s),3 minutes for test
	double now = ns_pplive::PubApi::GetExactTimeNow();
	const static unsigned interval  = ns_pplive::PubApi::GenRandom() % 100;
      
    if (now - latest_time_ > flush_time + interval)
    {
        LoadBalance tmp_tracker_mod_loadbalance;
        tmp_tracker_mod_loadbalance = tracker_mod_loadbalance_;
        do
        {
            LOG4CPLUS_INFO(g_logger,"flush time is:"<<flush_time + interval);				
		    //ppvabs.pplive.com
		    struct hostent *hptr = gethostbyname("ppvabs.pplive.com");
		    if(hptr == NULL)
		    {
			    LOG4CPLUS_INFO(g_logger,"gethostbyname error:"<<h_errno);
			    break;
		    }
		    char ip_string[32]={0};
		    if(NULL == inet_ntop(hptr->h_addrtype, hptr->h_addr, ip_string, sizeof(ip_string)))
		    {
			    LOG4CPLUS_INFO(g_logger,"inet_ntop error:");
			    break;
		    }
		    else
		    {
			    LOG4CPLUS_INFO(g_logger,"ipstring:"<<std::string(ip_string));
		    }

		    boost::asio::ip::udp::endpoint endpoint_( boost::asio::ip::address::from_string(ip_string), 6400);
		    Guid pid;

		    vector<TRACKER_INFO> tracker_info;
		    LOG4CPLUS_INFO(g_logger,"tracker_mod_loadbalance_.Size: "<<tracker_mod_loadbalance_.Size());
		    if (tracker_mod_loadbalance_.Size() > 0)
		    {
			    LOG4CPLUS_INFO(g_logger,"sort and print:"<<tracker_mod_loadbalance_.GetLoadBalanceInfo());
		    }

		    tracker_mod_loadbalance_.ClearEndpoint();
		    set<int> selected_station_number;
		    {
			    pid.generate();
			    tracker_info.clear();
			    ns_pplive::ns_bootstrap::BootstrapClient bootstrap_client;
			    int ret = bootstrap_client.QueryTrackerForListing(tracker_info,boost::uint32_t(rand()),Guid(pid),endpoint_,g_ioservice,2.0);
			    if (ret == 0)
			    {
				    latest_time_ = now;
				    for(unsigned i=0;i<tracker_info.size();++i)
				    {
					    boost::asio::ip::address_v4 av4 = boost::asio::ip::address_v4(tracker_info[i].IP);
					    unsigned short port = tracker_info[i].Port;
					    boost::asio::ip::udp::endpoint endpoint_2(boost::asio::ip::address(av4),port);
					    int tracker_station_num = tracker_info[i].StationNo;
					    if (selected_station_number.find(tracker_station_num) == selected_station_number.end())
					    {
						    tracker_mod_loadbalance_.AddEndpoint(endpoint_2);
						    selected_station_number.insert(tracker_station_num);
					    }
				    }
			    }
		    }
        } while (0);

	    if (tracker_mod_loadbalance_.Size() <= 0)
	    {
		    tracker_mod_loadbalance_ = tmp_tracker_mod_loadbalance;
	    }
    }
    
}


int main( int argc, char *argv[] )
{	
    log4cplus::PropertyConfigurator::doConfigure(log_conf); 

#ifndef BOOST_WINDOWS_API
    while( FCGI_Accept() >= 0 ) 
#endif	
    {
        TrackerCgi trackcgi;
		trackcgi.FlushTrackerInfo();
        trackcgi.Run();		
    }
    return 0;
}
