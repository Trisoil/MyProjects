#include "TrackerRequestHandler.h"
#include "MainThread.h"
#include "RidListCache.h"
#include <math.h>


using namespace protocol;
using namespace udptrackerag;

TrackerRequestHandler* TrackerRequestHandler::s_instance = NULL;

TrackerRequestHandler* TrackerRequestHandler::Instance()
{		
    if( NULL == s_instance )
    {
        LOG4CPLUS_INFO(g_logger,"init TrackerRequestHandler");
        s_instance = new TrackerRequestHandler();
    }
    return s_instance;
}

TrackerRequestHandler::TrackerRequestHandler()
{
    //读入配置
    InitConfigValue();
    //udp_server_ = NULL;
}

//void TrackerRequestHandler::SetUdpServer(protocol::UdpServer::pointer udp_server)
void TrackerRequestHandler::SetUdpServer(boost::shared_ptr<protocol::UdpServer> udp_server)
{
    LOG4CPLUS_TRACE(g_logger,"SetUdpServer");
    udp_server_ = udp_server;
}

void TrackerRequestHandler::InitConfigValue()
{
    //格式：
    //[tracker]
    //groupnum=4
    //groupnum_0 = 1
    //ipport_0_0 =118.123.201.105:18000
    //groupnum_0 = 2
    //ipport_1_0 =118.123.201.105:18001
    //ipport_1_1 =118.123.201.105:18001
    //groupnum_0 = 1
    //ipport_2_0 =118.123.201.105:18002
    //groupnum_0 = 3
    //ipport_3_0 =118.123.201.105:18003
    //ipport_3_1 =118.123.201.105:18003
    //ipport_3_2 =118.123.201.105:18003

    //tracker一共分了几个组
    int groupnum = Config::Instance().GetInteger("tracker.groupnum", 0);
    if (0 >= groupnum)
    {
        LOG4CPLUS_ERROR(g_logger, "groupnum error:" <<groupnum);	
        return;
    }
    LOG4CPLUS_DEBUG(g_logger, "groupnum " <<groupnum);

    boost::asio::ip::udp::endpoint _endpoint;
    {
        tracker_group_.clear();
        for(int i=0;i<groupnum;++i)
        {
            char tmpi[50];
            sprintf(tmpi,"tracker.groupnum_%d",i);
            //每个组里有多少元素
            int tracker_numi =  Config::Instance().GetInteger(string(tmpi), 0); 
            if (0 >= tracker_numi)
            {
                LOG4CPLUS_ERROR(g_logger, "tracker_numi error:" <<tracker_numi);
                return;
            }
            for(int j=0;j<tracker_numi;++j)
            {
                char tmpij[50];
                sprintf(tmpij,"tracker.ipport_%d_%d",i,j);
                string ipport = Config::Instance().GetTString(string(tmpij),"");
                string::size_type pos = ipport.find(':');
                if (pos == string::npos)
                {
                    LOG4CPLUS_ERROR(g_logger, "ipport string error:" <<ipport);
                    return;
                }
                string ip = ipport.substr(0,pos);
                unsigned short port = atoi(ipport.substr(pos+1).c_str());
                boost::asio::ip::address_v4 av4;
                av4 = boost::asio::ip::address_v4::from_string(ip);
                _endpoint.address(av4);
                _endpoint.port(port);
                tracker_group_[i].insert(_endpoint);
                LOG4CPLUS_DEBUG(g_logger,"group "<<i<<" insert:"<<_endpoint);
            }
        }
    }

    max_req_num_ = Config::Instance().GetInteger("tracker.max_req_num", 8);
    if (max_req_num_>12 || max_req_num_<=0)
    {
        max_req_num_ = 8;
    }
    LOG4CPLUS_DEBUG(g_logger,"max_req_num:"<<max_req_num_);

	min_req_num_ = Config::Instance().GetInteger("tracker.min_req_num", 1);
	if (min_req_num_>4 || min_req_num_<=0)
	{
		min_req_num_ = 2;
	}
	LOG4CPLUS_DEBUG(g_logger,"min_req_num:"<<min_req_num_);


    max_list_num_= Config::Instance().GetInteger("tracker.max_list_num_", 50);
    if (max_list_num_>200 || max_list_num_<=0)
    {
        max_list_num_ = 50;
    }
}

map<boost::asio::ip::udp::endpoint,int> TrackerRequestHandler::ClearListEndRequest()
{
    map<boost::asio::ip::udp::endpoint,int> tmp;
    tmp.swap(list_endpoint_request_);
    return tmp;
}


int TrackerRequestHandler::TrackerListRequest(const RID& resource_id,const Guid& peer_id,unsigned short request_peer_count,const protocol::ServerPacket &packet,unsigned request_ip)
{
    //发送包有两种可能，普通查询和tcp查询。但是一定都是带ip的。
    switch(packet.PacketAction)
    {
        case protocol::ListPacket::Action: 
        case protocol::ListWithIpPacket::Action:
            {
                //随便设置一个endpoint，因为TrackerListRequest(list_request);会继续设置的
                boost::asio::ip::udp::endpoint tmp_endpoint;
                ListWithIpPacket list_request(packet.transaction_id_,packet.peer_version_,resource_id,peer_id,request_peer_count,request_ip,tmp_endpoint); 
                return TrackerListRequest(list_request);
            }
            break;
        case protocol::ListTcpPacket::Action:
        case protocol::ListTcpWithIpPacket::Action:
            {
                boost::asio::ip::udp::endpoint tmp_endpoint;
                ListTcpWithIpPacket list_request(packet.transaction_id_,packet.peer_version_,resource_id,peer_id,request_peer_count,request_ip,tmp_endpoint);                
                return TrackerListRequest(list_request);
            }
            break;
        default:
            return -1;            
    }
    //不会走到这里
    return 0;
    
}

void TrackerRequestHandler::SetRandomSelect(set<boost::asio::ip::udp::endpoint>& selectset,int require_num,unsigned seed)
{
    //这里做了一个随机选择的算法，选出max_req_num_个。
    if(selectset.size() > require_num)
    {
        //个数足够，需要随机选取了。   
        srand(seed + framework::timer::TickCounter::tick_count());   

        set<boost::asio::ip::udp::endpoint> tmpset; 
        unsigned leftsize = selectset.size();
        for(set<boost::asio::ip::udp::endpoint>::iterator it = selectset.begin();it != selectset.end();++it,--leftsize)
        {
            int tmpset_size = tmpset.size();
            if(tmpset_size == require_num)
            {               
                break;
            }
            if(rand() % leftsize < (require_num-tmpset_size))
            {
                tmpset.insert(*it);
            }
        } 
        tmpset.swap(selectset);
    }
}

void TrackerRequestHandler::GetListTrackers(set<boost::asio::ip::udp::endpoint>& trackers,const RID& resource_id)
{
    trackers.clear();    
    trackers = tracker_group_[GetGuidMod(resource_id,tracker_group_.size())];

    int request_count = max_req_num_;
    int response_count = RidListCache::Instance()->GetListResult(resource_id);
    if(response_count > 0)
    {
        //ceil表示向上取整
        request_count = min<int>(max_req_num_,ceil(1.0*max_list_num_/response_count));
		response_count = max<int>(request_count, min_req_num_);
    }
    LOG4CPLUS_DEBUG(g_logger,"response count:"<<response_count<<" request count:"<<request_count);

    SetRandomSelect(trackers,request_count,resource_id.hash_value());

    for(set<boost::asio::ip::udp::endpoint>::iterator it = trackers.begin();it != trackers.end();++it)
    {
        LOG4CPLUS_TRACE(g_logger,"get a tracker:"<<*it);
    }
}