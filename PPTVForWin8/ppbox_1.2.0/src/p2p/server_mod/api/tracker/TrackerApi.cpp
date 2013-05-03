#include "TrackerApi.h"
#include "Memconfig.h"
#include "PPlivePubApi.h"
#define TRACKER_TAG "tracker"

//#include <log4cplus/logger.h>
//#include <log4cplus/configurator.h>

//extern log4cplus::Logger g_logger;

namespace ns_pplive
{
    namespace ns_tracker
    {
        //FRAMEWORK_LOGGER_DECLARE_MODULE("trackapi");

        //这个函数在set中随机选取一个，并删除掉原有的。
        bool GetRandomEnd(set<boost::asio::ip::udp::endpoint>& ends,boost::asio::ip::udp::endpoint& endpoint_)
        {
            if (ends.empty())
            {
                return false;
            }
            boost::uint32_t i = ns_pplive::PubApi::GenRandom()%ends.size();
            set<boost::asio::ip::udp::endpoint>::iterator it = ends.begin();
            advance(it,i);
            endpoint_ = *it;
            ends.erase(it);
            return true;
        }

        uint32_t GuidMod(const Guid& guid, uint32_t mod)
        {
            boost::uint64_t buf[2];
            memcpy(&buf, &guid.data(), sizeof(guid.data()));
            buf[1] = framework::system::BytesOrder::little_endian_to_host_longlong(buf[1]);
            return static_cast<uint32_t> (buf[1] % mod);
        }

        TrackerApi::TrackerApi()
        {

        }
        TrackerApi::~TrackerApi()
        {

        }

        int TrackerApi::ReadTrackerConfig(map<int,set<boost::asio::ip::udp::endpoint> >&  tracker_group)
        { //tracker一共分了几个组
            int groupnum = atoi(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"groupnum").c_str());
            if (0 >= groupnum)
            {
                //LOG4CPLUS_ERROR(g_logger, "groupnum error:" <<groupnum);
                return -1;	
            }

            boost::asio::ip::udp::endpoint _endpoint;

            if (tracker_group.size() != groupnum)
            {
                tracker_group.clear();
                for(int i=0;i<groupnum;++i)
                {
                    char tmpi[50];
                    sprintf(tmpi,"groupnum_%d",i);
                    //每个组里有多少元素
                    int groupinum = atoi(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,tmpi).c_str());
                    if (0 >= groupinum)
                    {
                        //LOG4CPLUS_ERROR(g_logger, "groupinum error:" <<groupinum);
                        return -1;	
                    }
                    for(int j=0;j<groupinum;++j)
                    {
                        char tmpij[50];
                        sprintf(tmpij,"ipport_%d_%d",i,j);
						std::string ipport =  ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,tmpij);
						std::string::size_type pos = ipport.find(':');
                        if (pos == string::npos)
                        {
                           //LOG4CPLUS_ERROR(g_logger, "ipport string error:" <<ipport);
                            return -1;
                        }
                        string ip = ipport.substr(0,pos);
                        unsigned short port = atoi(ipport.substr(pos+1).c_str());
                        boost::asio::ip::address_v4 av4;
                        av4 = boost::asio::ip::address_v4::from_string(ip);
                        _endpoint.address(av4);
                        _endpoint.port(port);
                        tracker_group[i].insert(_endpoint);
                    }

                }
            }
            return 0;
        }

        int TrackerApi::ListPeerCom(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
            boost::asio::io_service& ios,boost::uint8_t action,boost::uint32_t request_ip,double timeout)
        {
            ReadTrackerConfig(tracker_group_);
            double trackertimeout = atof(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"trackertimeout").c_str());
            double alltimeout = atof(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"alltimeout").c_str());
            //如果调用者传入了超时值，就按调用者传入的为准。
            if (timeout > 0)
            {
                alltimeout = timeout;
            }

            int maxreqcount = atoi(ns_pplive::PubApiTag::ConfigItem(TRACKER_TAG,"maxlistcount").c_str());
            if (request_peer_count > maxreqcount)
            {
                request_peer_count = maxreqcount;
            }			
            double starttime = ns_pplive::PubApi::GetExactTimeNow();
            unsigned groupindex = GuidMod(resource_id,tracker_group_.size());
            //由于GetRandomEnd会修改第一个参数，因此这里做了一个拷贝。
            set<boost::asio::ip::udp::endpoint> group_end = tracker_group_[groupindex];
            boost::asio::ip::udp::endpoint _endpoint;
            while(candidate_peer_info.size()<request_peer_count && group_end.size() > 0 && ns_pplive::PubApi::GetExactTimeNow()-starttime<alltimeout)			
            {					
                if ( !GetRandomEnd(group_end,_endpoint))
                {
                   //LOG4CPLUS_ERROR(g_logger, "GetRandomEnd failed,group size:"<<group_end.size());			
                    return -1;
                }
                vector<CandidatePeerInfo> candidatepeerinfo;	
                int ret = 0;
                if (protocol::ListTcpPacket::Action == action)
                {
                    ret = client_.ListPeerTcp(candidatepeerinfo,boost::uint32_t(ns_pplive::PubApi::GenRandom()),resource_id,peer_guid,boost::uint16_t(50),_endpoint,ios,trackertimeout);
                }
                else if (protocol::ListPacket::Action == action)
                {
                    ret = client_.ListPeer(candidatepeerinfo,boost::uint32_t(ns_pplive::PubApi::GenRandom()),resource_id,peer_guid,boost::uint16_t(50),_endpoint,ios,trackertimeout);
                }
                else if (protocol::ListTcpWithIpPacket::Action == action)
                {
                    ret = client_.ListPeerTcpWithIp(candidatepeerinfo,boost::uint32_t(ns_pplive::PubApi::GenRandom()),resource_id,peer_guid,boost::uint16_t(50),request_ip,_endpoint,ios,trackertimeout); 
                }
                else
                {
                    //LOG4CPLUS_ERROR(g_logger, "unknown action:"<<int(action));			
                    return -1;
                }

                if (0 != ret)
                {
                    //LOG4CPLUS_INFO(g_logger, "listpeer failed,msg:" <<client_.GetErrMsg()<<"timenow:"<<time(NULL));
                }
                else
                {
                    std::copy(candidatepeerinfo.begin(),candidatepeerinfo.end(),std::back_inserter(candidate_peer_info));					
                }
            }

            //LOG4CPLUS_DEBUG(g_logger, "time use:" <<ns_pplive::PubApi::GetExactTimeNow()-starttime);			

            return 0;			
        }


        int TrackerApi::ListPeer(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,
            boost::uint16_t request_peer_count,boost::asio::io_service& ios,double timeout)
        {	
            return ListPeerCom(candidate_peer_info,resource_id,peer_guid,request_peer_count,ios,protocol::ListPacket::Action,0,timeout);
        }

        int TrackerApi::ListPeerTcp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,
            boost::uint16_t request_peer_count,boost::asio::io_service& ios,double timeout)
        {
            return ListPeerCom(candidate_peer_info,resource_id,peer_guid,request_peer_count,ios,protocol::ListTcpPacket::Action,0,timeout);
        }

        int TrackerApi::ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,
            boost::uint16_t request_peer_count,unsigned request_ip,boost::asio::io_service& ios,double timeout)
        {
            return ListPeerCom(candidate_peer_info,resource_id,peer_guid,request_peer_count,ios,protocol::ListTcpWithIpPacket::Action,request_ip,timeout);
        }
    };

};