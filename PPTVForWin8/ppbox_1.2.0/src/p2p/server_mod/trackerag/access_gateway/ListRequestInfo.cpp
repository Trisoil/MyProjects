#include "ListRequestInfo.h"


using namespace udptrackerag;
using namespace protocol;

//TODO:这里可能可以考虑，按地域较近来排序，而不是按优先级来排序
bool HighPrioPeer(const CandidatePeerInfo& peer_info1,const CandidatePeerInfo& peer_info2)
{
    return peer_info1.TrackerPriority > peer_info2.TrackerPriority;
}

ListRequestInfo::ListRequestInfo()
{
    LOG4CPLUS_TRACE(g_logger,"ListRequestInfo()");
    send_request_time = 0;
    request_peer_count = 0;
    tracker_response.clear();
    transaction_id=0;
    magic_num = 0;
    query_tracker_num = 0;
    action = 0;
    peer_version = 0;
}

void ListRequestInfo::UpdateMagicNum()
{
    magic_num = GetMagicNum(resource_id,transaction_id);
}

unsigned ListRequestInfo::GetLeftResponse()
{
    return query_tracker_num - tracker_response.size();
}

boost::uint64_t ListRequestInfo::GetMagicNum(const RID& resource_id,unsigned transaction_id)  
{
    boost::uint64_t  ret = ((boost::uint64_t)transaction_id)<<32 | resource_id.hash_value();
    LOG4CPLUS_TRACE(g_logger,"rid:"<<resource_id<<" tid:"<<transaction_id<<" magicnum:"<<ret<<" tid<<32"<<(((boost::uint64_t)transaction_id)<<32)<<"rid hash"<<resource_id.hash_value());
    return ret;
}

void ListRequestInfo::AddPeerInfo(const boost::asio::ip::udp::endpoint& end_point,vector<protocol::CandidatePeerInfo>& peer_infos)
{
    tracker_response[end_point].swap(peer_infos);
}


int ListRequestInfo::GetPeerInfos(vector<protocol::CandidatePeerInfo>& peer_infos,int location_percent,bool random_shuf) const
{
    assert(magic_num != 0);

    if (location_percent>100 || location_percent<0)
    {
        LOG4CPLUS_ERROR(g_logger,"location percent:"<<location_percent);
        return -1;
    }  

    peer_infos.clear();

    for(map<boost::asio::ip::udp::endpoint,vector<protocol::CandidatePeerInfo> >::const_iterator it = tracker_response.begin(); it != tracker_response.end();++it)
    {
        std::copy(it->second.begin(),it->second.end(),back_inserter(peer_infos));
    }

    if (peer_infos.size() <= request_peer_count)
    {
        //查询到的数目不够，全部返回了。       
        return 0;
    }

    if (random_shuf)
    {
        std::random_shuffle(peer_infos.begin(),peer_infos.end());
        peer_infos.resize(request_peer_count);
    }
    else
    {
        //将peer按优先级排序
        std::nth_element(peer_infos.begin(),peer_infos.begin() + request_peer_count,peer_infos.end(),HighPrioPeer);

        //取出前面的高优先级的peer，后面的不要了
        //这里resize的时候，多预留一些，因为可能会有重复，后面有去重的操作。
        peer_infos.resize(request_peer_count);

        //按优先级排序，然后移除重复的元素,uniq不会改变vector的size。
        std::sort(peer_infos.begin(),peer_infos.end(),HighPrioPeer);
        std::unique(peer_infos.begin(),peer_infos.end());   
        peer_infos.resize(min<unsigned>(request_peer_count * 0.01 * location_percent,peer_infos.size()));

        ComplementPeerInfos(peer_infos);
    }

    return 0;  
}

void ListRequestInfo::ComplementPeerInfos(vector<protocol::CandidatePeerInfo>& peer_info) const
{

    assert(GetResponsePeerCount() > request_peer_count );
    uint8_t base_priority = 255;
    if(!peer_info.empty())
    {
        base_priority = peer_info.rbegin()->TrackerPriority;
    }

    //目前采取的策略是从尾巴开始补足。
    int rindex = 1;
    while(rindex < request_peer_count)
    {
        for(map<boost::asio::ip::udp::endpoint,vector<protocol::CandidatePeerInfo> >::const_iterator it = tracker_response.begin(); it != tracker_response.end();++it)
        {
            if (it->second.size() >= rindex)
            {
                if (base_priority < it->second[it->second.size() - rindex].TrackerPriority)
                {
                    //已经是重复的数据了
                    LOG4CPLUS_DEBUG(g_logger,"candidate already pushed"<< it->second[it->second.size() - rindex]);                
                }
                else
                {
                    peer_info.push_back(it->second[it->second.size() - rindex]);
                }

                if (peer_info.size() >= request_peer_count)
                {
                    return;
                }
            }
        }
        ++rindex;
    }
}

unsigned ListRequestInfo::GetResponsePeerCount() const
{
    map<boost::asio::ip::udp::endpoint,vector<protocol::CandidatePeerInfo> >::const_iterator it = tracker_response.begin();
    unsigned count = 0;
    while(it != tracker_response.end())
    {
        count += it->second.size();
        ++it;
    }
    return count;
}
