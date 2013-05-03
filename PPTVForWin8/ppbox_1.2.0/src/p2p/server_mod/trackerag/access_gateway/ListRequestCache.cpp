#include "ListRequestCache.h"
#include "MainThread.h"

using namespace protocol;
using namespace udptrackerag;

ListRequestCache* ListRequestCache::s_instance = NULL;

ListRequestCache* ListRequestCache::Instance()
{		
    if( NULL == s_instance )
    {
        LOG4CPLUS_INFO(g_logger,"init ListRequestCache");
        s_instance = new ListRequestCache();
    }
    return s_instance;
}

ListRequestCache::ListRequestCache()
    :request_info_magic_index_((request_infos_.get<0>())),request_info_time_index_((request_infos_.get<1>()))
{
    
}

void ListRequestCache::AddRequest(const RID& resource_id,const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t PacketAction,
                                  uint32_t transaction_id,uint16_t peer_version,unsigned short request_peer_count,unsigned query_tracker_num)
{
    //上界先随便设置一个，免得传得很离谱
    if(0 == query_tracker_num || query_tracker_num >= 1000)
    {
        return;
    }
    ListRequestInfo info;
    info.resource_id = resource_id;   
    info.end_point = end_point; 
    info.action = PacketAction;
    info.transaction_id = transaction_id;
    info.peer_version = peer_version;
    info.request_peer_count = request_peer_count;    
    info.query_tracker_num = query_tracker_num;
    info.send_request_time = framework::timer::TickCounter::tick_count();
    info.UpdateMagicNum();
    if( request_info_magic_index_.find(info.magic_num) != request_info_magic_index_.end())
    {
        LOG4CPLUS_ERROR(g_logger,"request_info_magic_index_.find(info.magic_num) != request_info_magic_index_.end(),rid:"<<resource_id<<" tid:"<<transaction_id);
    }
    request_info_magic_index_.insert(info);     
    LOG4CPLUS_TRACE(g_logger,"after addrequest request_info_time_index_ size:"<<request_info_time_index_.size());
}

int ListRequestCache::AddResponse(const RID& resource_id,uint32_t transaction_id,const boost::asio::ip::udp::endpoint& end_point,vector<CandidatePeerInfo>& peer_infos)
{
    LOG4CPLUS_DEBUG(g_logger,"AddResponse:rid:"<<resource_id<<" endpoint:"<<end_point<<" transaction_id:"<<transaction_id<<" request count:"<<request_info_magic_index_.size());
    boost::uint64_t magic_num = ListRequestInfo::GetMagicNum(resource_id,transaction_id);
    ListRequestInfoMagicNumHashIndex::iterator it = request_info_magic_index_.find(magic_num);
    if (it == request_info_magic_index_.end())
    {
        LOG4CPLUS_INFO(g_logger,"AddResponse not find key,rid:"<<resource_id<<" tid:"<<transaction_id<<" requset info size:"<<request_info_magic_index_.size());
        return -1;
    }

    //这个强制转换，去掉const属性
    ListRequestInfo& info = const_cast<ListRequestInfo&>(*it);

    //注意，这里为了效率，把传入的参数改变了。
    info.AddPeerInfo(end_point,peer_infos);

    assert(info.query_tracker_num >= info.tracker_response.size());

    return int(info.query_tracker_num) - it->tracker_response.size();
}

int ListRequestCache::GetUnResponseCount(const RID& resource_id,uint32_t transaction_id)
{
	LOG4CPLUS_DEBUG(g_logger,"AddResponse:rid:"<<resource_id<<" transaction_id:"<<transaction_id<<" request count:"<<request_info_magic_index_.size());
	boost::uint64_t magic_num = ListRequestInfo::GetMagicNum(resource_id,transaction_id);
	ListRequestInfoMagicNumHashIndex::iterator it = request_info_magic_index_.find(magic_num);
	if (it == request_info_magic_index_.end())
	{
		LOG4CPLUS_INFO(g_logger,"AddResponse not find key,rid:"<<resource_id<<" tid:"<<transaction_id<<" requset info size:"<<request_info_magic_index_.size());
		return -1;
	}

	//这个强制转换，去掉const属性
	ListRequestInfo& info = const_cast<ListRequestInfo&>(*it);

	assert(info.query_tracker_num >= info.tracker_response.size());

    LOG4CPLUS_TRACE(g_logger,"info.query_tracker_num:"<<int(info.query_tracker_num));
    LOG4CPLUS_TRACE(g_logger,"tracker_response:"<<int(it->tracker_response.size()));
	return int(info.query_tracker_num) - it->tracker_response.size();
}

int ListRequestCache::GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
                                  const RID& resource_id,uint32_t transaction_id,int location_percent,bool random_shuf)
{
    boost::uint64_t magic_num = ListRequestInfo::GetMagicNum(resource_id,transaction_id);
    return GetResponse(peer_infos,end_point,action,peer_version,magic_num,location_percent,random_shuf);
}

int ListRequestCache::GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
                                  boost::uint64_t magic_num,int location_percent,bool random_shuf)
{
    RID resouce_id;
    uint32_t transaction_id;
    return GetResponse(peer_infos,end_point,action,peer_version,resouce_id,transaction_id,magic_num,location_percent,random_shuf);
}

int ListRequestCache::GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
                RID& resouce_id,uint32_t& transaction_id,boost::uint64_t magic_num,int location_percent,bool random_shuf)
{
    ListRequestInfoMagicNumHashIndex::iterator it = request_info_magic_index_.find(magic_num);
    if (it == request_info_magic_index_.end())
    {
        LOG4CPLUS_INFO(g_logger,"GetResponse not find magic_num"<< magic_num<<" requset info size:"<<request_info_magic_index_.size());
        return -1;
    }
    //const ListRequestInfo& info = *it;  
    int ret = it->GetPeerInfos(peer_infos,location_percent,random_shuf);
    if (ret != 0)
    {
        return ret;
    }
    end_point = it->end_point;
    action = it->action;
    peer_version = it->peer_version;
    resouce_id = it->resource_id;
    transaction_id = it->transaction_id;
    return 0;
}

int ListRequestCache::GetResponseCount(const RID& resource_id,uint32_t transaction_id,int &avg_count,int& response_count)
{
    boost::uint64_t magic_num = ListRequestInfo::GetMagicNum(resource_id,transaction_id);
    ListRequestInfoMagicNumHashIndex::iterator it = request_info_magic_index_.find(magic_num);
    if (it == request_info_magic_index_.end())
    {
        LOG4CPLUS_INFO(g_logger,"GetAvgResponseCount not find key,rid:"<<resource_id<<" tid:"<<transaction_id<<" requset info size:"<<request_info_magic_index_.size());
        return -1;
    }

    response_count = it->tracker_response.size();
    if( response_count != 0)
    {
        avg_count = it->GetResponsePeerCount() / response_count;
    }
    else
    {
        avg_count = 0;
    }

    return 0;
}


int ListRequestCache::RemoveRequestInfo(boost::uint64_t magic_num)
{
    LOG4CPLUS_DEBUG(g_logger,"remove magic num:"<<magic_num<<" orginal size:"<<request_info_magic_index_.size());
    ListRequestInfoMagicNumHashIndex::iterator it = request_info_magic_index_.find(magic_num);
    if (it == request_info_magic_index_.end())
    {
        LOG4CPLUS_INFO(g_logger,"RemoveRequestInfo not find magic_num"<< magic_num<<" requset info size:"<<request_info_magic_index_.size());
        return -1;
    }
    request_info_magic_index_.erase(it);
    return 0;
}
int ListRequestCache::RemoveRequestInfo(const RID& resource_id,uint32_t transaction_id)
{
    boost::uint64_t magic_num = ListRequestInfo::GetMagicNum(resource_id,transaction_id);
    return RemoveRequestInfo(magic_num);
}

void ListRequestCache::GetDeadlineRequest(unsigned keep_alive_ms,map<RID,uint32_t>& dead_requests)
//void ListRequestCache::GetDeadlineRequest(unsigned keep_alive_ms,set<boost::uint64_t>& dead_requests)
{  
    dead_requests.clear();
    //这里必须基于framework::timer::TickCounter::tick_count() 获取的时间不会溢出      
    boost::uint64_t now_tick = framework::timer::TickCounter::tick_count();
    for(ListRequestInfoRequestTimeIndex::iterator it = request_info_time_index_.begin();it!= request_info_time_index_.end();++it)
    {
        if (it->send_request_time + keep_alive_ms < now_tick)
        {
          dead_requests[it->resource_id] =it->transaction_id;
        }
        else
        {
            // 因为已经按时间排序了，所以无需再往后走了
            break;
        }
    }
}