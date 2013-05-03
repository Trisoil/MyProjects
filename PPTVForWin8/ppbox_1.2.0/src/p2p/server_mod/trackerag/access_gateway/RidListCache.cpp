#include "RidListCache.h"

using namespace udptrackerag;

extern log4cplus::Logger g_logger;

RidListCache* RidListCache::s_instance = NULL;

RidListCache* RidListCache::Instance()
{		
    if( NULL == s_instance )
    {
        LOG4CPLUS_INFO(g_logger,"init RidListCache");
        s_instance = new RidListCache();
    }
    return s_instance;
}

RidListCache::RidListCache()
:rid_hash_index_((list_result_.get<0>())),set_tick_index_((list_result_.get<1>()))
{

}


void RidListCache::SetListResult(const RID& resource_id,int count)
{
    vector<protocol::CandidatePeerInfo> peer_infos;
    SetListResult(resource_id,count,peer_infos);
}

void RidListCache::SetListResult(const RID& resource_id,int count,const vector<protocol::CandidatePeerInfo>& peer_infos)
{
    ListResultRidHashIndex::iterator it = rid_hash_index_.find(resource_id);
    if( it != rid_hash_index_.end() )
    {
        //注意，这里的count和peers是分开赋值的。
        if(it->count < count)
        {
            //it->count = count; 这样写是编译不过的
            ListResult& lr= (ListResult&)*it;
            lr.count = count;
        }

        if(it->peers.size() < peer_infos.size())
        {
            ListResult& lr= (ListResult&)*it;
            lr.peers = peer_infos;
            //需要重设优先级
            for(unsigned i=0;i<lr.peers.size();++i)
            {
                lr.peers[i].TrackerPriority = 10;
            }
        }
    }
    else
    {
        ListResult lr;
        lr.resource_id = resource_id;
        lr.set_tick = framework::timer::TickCounter::tick_count();
        lr.count = count;
        rid_hash_index_.insert(lr);
    }
}

int RidListCache::GetListResult(const RID& resource_id)
{
    ListResultRidHashIndex::iterator it = rid_hash_index_.find(resource_id);
    if( it != rid_hash_index_.end() )
    {
        return it->count;
    }
    return 0;
}

int RidListCache::GetListResult(const RID& resource_id,vector<protocol::CandidatePeerInfo>& peer_infos)
{
    peer_infos.clear();
    ListResultRidHashIndex::iterator it = rid_hash_index_.find(resource_id);
    if( it != rid_hash_index_.end() )
    {
        peer_infos = it->peers;
        return it->count;
    }
    return 0;
}

void RidListCache::RemoveTimeout(unsigned timeout_ms)
{
    LOG4CPLUS_INFO(g_logger,"timeout_ms:"<<timeout_ms<<" before remove size:"<<set_tick_index_.size());
    //这里必须基于framework::timer::TickCounter::tick_count() 获取的时间不会溢出      
    boost::uint64_t now_tick = framework::timer::TickCounter::tick_count();
    ListResultSetTickIndex::iterator it = set_tick_index_.begin();
    for(;it!= set_tick_index_.end();++it)
    {       
        if (it->set_tick + timeout_ms > now_tick)
        {  
            // 因为已经按时间排序了，所以无需再往后走了           
            break;        
        } 
    }
    set_tick_index_.erase(set_tick_index_.begin(),it);
    LOG4CPLUS_INFO(g_logger,"timeout_ms:"<<timeout_ms<<" end remove size:"<<set_tick_index_.size());
}

