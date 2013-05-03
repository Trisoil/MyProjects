#include <pre.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <framework/Framework.h>
#include <framework/configure/Profile.h>
#include <framework/string/Uuid.h>

#include <framework/timer/Timer.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/timer/TickCounter.h>

#include <util/serialization/Uuid.h>

#include <network/UdpServer.h>

#include <util/Util.h>


#include "tracker/TrackerManager.h"

#include <util/archive/BinaryIArchive.h>
#include <util/archive/BinaryOArchive.h>
//#include "io/ByteStream.h"
#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

framework::timer::AsioTimerManager * TrackerManager::asio_timer_manager_;


TrackerManager::p TrackerManager::inst_(new TrackerManager());

void TrackerManager::Start(const string& config_path, bool need_report)
{
    LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "TrackerManager::Start()");
    if( is_running_ == true ) 
    {
        ///LOG(__WARN, "tracker", "TrackerManager is running...");
        LOG4CPLUS_WARN(Loggers::Service(), "TrackerManager is already running...");
        return;
    }

    need_report_ = need_report;

    // config path
    LOG4CPLUS_INFO(Loggers::Service(), "tracker config_path = " << config_path);
    if (config_path.length() == 0) {
        string szPath;
#ifdef DISK_MODE
        if (storage::Storage::GetAppDataPath(szPath)) {
            tracker_list_save_path_ = szPath;
        }
#endif//#ifdef DISK_MODE

    }
    else {
        tracker_list_save_path_ = config_path;
    }
    boost::filesystem::path list_path(tracker_list_save_path_);
    list_path /= ("pptl");
    tracker_list_save_path_ = list_path.file_string();

    upnp_ip_ = 0;
    upnp_tcp_port_ = 0;
    upnp_udp_port_ = 0;

    group_count_ = 0;

    //MainThread::Post(boost::bind(&TrackerManager::StartAllGroups, shared_from_this()));
    //MainThread::Post(boost::bind(&TrackerManager::LoadTrackerList, shared_from_this()));
    ///LOG(__EVENT, "tracker", "Tracker Manager has started successfully.");
    LOG4CPLUS_INFO(Loggers::Service(), "Tracker Manager has started successfully.");
    LoadTrackerList();

    is_running_ = true;

    StartAllGroups();

}


bool TrackerManager::NeedReport() const
{
    return need_report_;
}

void TrackerManager::Stop()
{
    if( is_running_ == false ) return;

    StopAllGroups();

    is_running_ = false;
    inst_.reset();
}

void TrackerManager::LoadTrackerList()
{
    uint32_t save_group_count;
    std::vector<protocol::TRACKER_INFO> save_tracker_info;
    uint32_t info_size = 0;

    try
    {
        std::ifstream ifs(tracker_list_save_path_.c_str());
        if (!ifs)
        {
            LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager File Read Error");
            return;
        }
        util::archive::BinaryIArchive<>  ia(ifs);

        ia>>save_group_count;
        ia>>info_size;

        for (uint32_t i = 0; i != info_size; ++i)
        {
            protocol::TRACKER_INFO ti;
            ia>>ti.StationNo ;//= framework::io::BytesToUI16(p_buf); p_buf+=2;
            ia>>ti.Reserve;
            ia>>ti.ModNo ;//= framework::io::BytesToUI08(p_buf); p_buf+=1;
            ia>>ti.IP;// = framework::io::BytesToUI32(p_buf); p_buf+=4;
            ia>>ti.Port ;//= framework::io::BytesToUI16(p_buf); p_buf+=2;
            ia>>ti.Type;// = framework::io::BytesToUI08(p_buf); p_buf+=1;
            save_tracker_info.push_back(ti);
        }
        ifs.close();
    }
    catch (...)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager File Read Error");
        return;
    }

	SetTrackerList(save_group_count, save_tracker_info);
}

void TrackerManager::SaveTrackerList()
{
    uint32_t save_group_count = group_count_;
    std::vector<protocol::TRACKER_INFO> save_tracker_info;

    for (ModIndexer::iterator it = mod_indexer.begin();
        it != mod_indexer.end(); ++it)
    {
        std::vector<protocol::TRACKER_INFO> group_tracker_info = it->second->GetTrackers();
        save_tracker_info.insert(save_tracker_info.end(), group_tracker_info.begin(), group_tracker_info.end());
    }

    try
    {
        std::ofstream ifs(tracker_list_save_path_.c_str());
        util::archive::BinaryOArchive<> ia(ifs);
        ia<<save_group_count;
        ia<<save_tracker_info;
        ifs.close();

    }
    catch (...)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManger File Write Error");
    }
}

void TrackerManager::SetTrackerList(uint32_t group_count, std::vector<protocol::TRACKER_INFO> tracker_s)
{
    if( is_running_ == true ) 
    {
        StopAllGroups();
        LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::SetTrackerList    Stop All Groups");
    }

    //LOG(__DEBUG, "tracker", "TrackerManager::SetTrackerList: group_count = " << group_count << ", Tracker List = " << tracker_s);

    if (group_count == 0 && tracker_s.empty()) 
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "Group Count is zero and Tracker Infos is empty.");
        return ;
    }
    else
    {
        uint32_t max_count = 0;
        for (uint32_t i = 0; i < tracker_s.size(); ++i)
        {
            uint32_t mod_num = static_cast<uint32_t>(tracker_s[i].ModNo);
            if (mod_num > max_count)
                max_count = mod_num;
        }
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager::SetTrackerList max_count = " << max_count);
        // check
        if (max_count + 1 != group_count)
        {
            LOG4CPLUS_WARN(Loggers::Service(), "TrackerManager::SetTrackerList()" << __FUNCTION__ << " Invalid tracker std::list");
            return ;
        }
    }

    LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager local_group_count = " << group_count_ << ", set_group_count = " << group_count);
    // 发现策略是否变化
    // 
    // 第一步 添加 新的 TrackerGroup
    //        group_count > group_count_
    //
    // 第二步 删除 多余的 TrackerGroup
    //         group_count < group_count_
    // 
    // 第三步 每个Group设置Trackers
    //      同时 更新 endpoint_indexer
    //  
    // 第四步 group_count_ = group_count
    //

    // 维护mod_indexer里面有 [0,group_count) 所有的key
    if (group_count < group_count_) // 减少了
    {
        for (ModIndexer::iterator it = mod_indexer.begin(), eit = mod_indexer.end(); it != eit; ) 
        {
            if ((uint32_t)it->first >= group_count) {
                mod_indexer.erase(it++);
            }
            else ++it;
        }
    }
    else if (group_count > group_count_)
    {
        for (uint32_t group_key = group_count_; group_key < group_count; group_key++)
        {
            // 必须是create的
            TrackerGroup::p group = TrackerGroup::Create( this->asio_timer_manager_ );
            mod_indexer[group_key] = group;
        }
    }

    // sort the trackers by their ModNo
    std::sort(tracker_s.begin(), tracker_s.end(), TrackerInfoSorterByMod());

    uint32_t i, j;
    for (i = 0; i < tracker_s.size(); i = j)
    {
        std::set<protocol::TRACKER_INFO> trackers_in_group;

        int group_key = tracker_s[i].ModNo;
        for (j = i; j < tracker_s.size() && tracker_s[j].ModNo == group_key; j++)
            trackers_in_group.insert(tracker_s[j]);

        // found a group range from [i, j)
        ModIndexer::iterator it = mod_indexer.find(group_key);

        if (it != mod_indexer.end())
        {
            TrackerGroup::p group = it->second;
            // std::set trackers
            group->SetTrackers(group_count, trackers_in_group);
        }
        else
        {
            // ERROR!! 不可能出现的情况，状态不一致
            LOG4CPLUS_ERROR(Loggers::Service(), "TrackerManager Mod Indexer Inconsistence.");
            assert(0);
        }
    }

    // endpoint
    endpoint_indexer.clear();
    for (i = 0; i < tracker_s.size(); i++)
    {
        boost::asio::ip::udp::endpoint end_point = 
            framework::network::Endpoint(tracker_s[i].IP, tracker_s[i].Port);
        endpoint_indexer[end_point] = mod_indexer[tracker_s[i].ModNo];
    }

    // 在这里创建扩展的 队列
    {
        // 清除原来的列表
        for ( std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p>::iterator i = this->cached_groups_.begin(); 
            i != this->cached_groups_.end()  ; ++i ) {
                // 
                (i->second)->Stop();
        }
        this->cached_groups_.clear();

        for (size_t i = 0; i < tracker_s.size(); i++) {
            // 查看是否存在
            boost::asio::ip::udp::endpoint end_point = framework::network::Endpoint(tracker_s[i].IP, tracker_s[i].Port);

            // 
            // 创建一个
            TrackerGroup::p group = TrackerGroup::Create( this->asio_timer_manager_ );
            cached_groups_[end_point] = group;
            std::set<protocol::TRACKER_INFO> trackers;
            trackers.insert( tracker_s[i] );
            group->SetTrackers( group_count , trackers );
        }
    }

    group_count_ = group_count;

    if( is_running_ == true ) 
    {
        StartAllGroups();
        LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::SetTrackerList  Start All Groups");

        SaveTrackerList();
    }
}

void TrackerManager::DoList(RID rid)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::DoList, RID: " << rid);

    if( is_running_ == false ) 
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker Manager is not running. Return.");
        return;
    }

    if (group_count_ == 0)
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker List is not std::set. Return.");
        return ;
    }

    // 根据 rid % group_count_ 从 mod_indexer 中定位出 TrackerGroup,
    // 然后 该 group 做 DoList
    uint32_t group_key = GuidMod(rid, group_count_);
    TrackerGroup::p tracker_group = mod_indexer[group_key];
    if (tracker_group)
    {
        tracker_group->DoList(rid);
    }
    else
    {
        LOG4CPLUS_ERROR(Loggers::Service(), "TrackerManager mod indexer data inconsistence");
        assert(0);
    }
}

void TrackerManager::DoQueriedPeerCount(const RID& rid)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::DoQuriedPeerCount, RID: " << rid);

    if( is_running_ == false ) 
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker Manager is not running. Return.");
        return;
    }

    if (group_count_ == 0)
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker List is not set. Return.");
        return ;
    }

    // 根据 rid % group_count_ 从 mod_indexer 中定位出 TrackerGroup,
    // 然后 该 group 做 DoList
    boost::uint32_t group_key = GuidMod(rid, group_count_);
    TrackerGroup::p tracker_group = mod_indexer[group_key];
    if (tracker_group)
    {
        tracker_group->DoQueriedPeerCount(rid);
    }
    else
    {
        LOG4CPLUS_ERROR(Loggers::Service(), "TrackerManager::mod indexer data inconsistence");
        assert(0);
    }
}
boost::uint32_t TrackerManager::GetTrackersNum(const RID& rid)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::GetTrackersNum, RID: " << rid);

    if( is_running_ == false) 
    {
        LOG4CPLUS_WARN(Loggers::Service(),"Tracker Manager is not running. Return.");
        return 0;
    }

    if (group_count_ == 0)
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker List is not set. Return.");
        return 0;
    }

    // 根据 rid % group_count_ 从 mod_indexer 中定位出 TrackerGroup,
    // 然后 该 group 做 DoList
    boost::uint32_t group_key = GuidMod(rid, group_count_);
    TrackerGroup::p tracker_group = mod_indexer[group_key];
    if (tracker_group)
    {
        return tracker_group->GetTrackersNum(rid);
    }
    else
    {
        LOG4CPLUS_ERROR(Loggers::Service(), "TrackerManager::mod indexer data inconsistence");
        assert(0);
    }
    return 0;
}
void TrackerManager::OnUdpRecv(protocol::ServerPacket const &packet)
{
    if( is_running_ == false ) return;

    switch(packet.PacketAction)
    {
    case protocol::ListPacket::Action:
        OnListResponsePacket((protocol::ListPacket const &)packet);
        break;
    case protocol::ReportPacket::Action:
        OnReportResponsePacket((protocol::ReportPacket const &)packet);
        break;
    case protocol::QueryPeerCountPacket::Action:
        OnQueryResponsePacket((protocol::QueryPeerCountPacket const &)packet);
        break;
    default:
        {
            //LOG(__WARN, "tracker", "Ignoring unknown action: 0x" << std::hex << packet_header->GetAction());
        }
        break;
    }
}

void TrackerManager::OnReportResponsePacket(protocol::ReportPacket const & packet)
{
    if( is_running_ == false ) return;

    if (cached_groups_.count(packet.end_point) != 0)
    {
        const TrackerGroup::p group = cached_groups_[packet.end_point];//! shared_ptr
        group->OnReportResponsePacket( packet);
    }
}

void TrackerManager::OnListResponsePacket(protocol::ListPacket const & packet)
{
    if( is_running_ == false ) 
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "Tracker Manager is not running...");
        return;
    }

    // 根据 endpoint_indexer 索引 找到对应的 TrackerGroup
    // 然后对这个Group调用 OnCommitResponsePacket(end_point, p packet)
    // 如果找不到Group不管
    boost::asio::ip::udp::endpoint end_point = packet.end_point;
    if (cached_groups_.count(packet.end_point) != 0)
    {
        const TrackerGroup::p group = cached_groups_[packet.end_point];
        group->OnListResponsePacket( packet);
    }
    else
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager No such end point");
    }
}

void TrackerManager::OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet)
{
    if( is_running_ == false ) 
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "Tracker Manager is not running...");
        return;
    }
    //LOG(__DEBUG, "tracker", "TrackerManager::OnQueryResponsePacket: " << end_point.address().to_string());

    boost::asio::ip::udp::endpoint end_point = packet.end_point;
    // 根据 endpoint_indexer 索引 找到对应的 TrackerGroup
    // 然后对这个Group调用 OnCommitResponsePacket(end_point, p packet)
    // 如果找不到Group不管
    if (cached_groups_.count(end_point) != 0)
    {
        const TrackerGroup::p group = cached_groups_[end_point];
        group->OnQueryResponsePacket(packet);
    }
    else
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager No such end point");
    }
}

void TrackerManager::StartAllGroups()
{
    ///LOG(__INFO, "tracker", "Starting all tracker groups.");

    /*
    for (ModIndexer::iterator it = mod_indexer.begin(), eit = mod_indexer.end(); it != eit; it++)
    {
    it->second->Start();
    }
    */

    for ( std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p>::iterator i = this->cached_groups_.begin() ; i != this->cached_groups_.end() ; ++i ) {
        // 
        i->second->Start();
    }

    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager All tracker groups are started.");
}

void TrackerManager::StopAllGroups()
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager Stopping all tracker groups.");
    /*
    for (ModIndexer::iterator it = mod_indexer.begin(), eit = mod_indexer.end(); it != eit; it++)
    {
    it->second->Stop();
    }
    */

    for ( std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p>::iterator i = this->cached_groups_.begin() ; i != this->cached_groups_.end() ; ++i ) {
        // 
        i->second->Stop();
    }

    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::All tracker groups has been stopped.");
}

void TrackerManager::ClearAllGroups()
{
    StopAllGroups();
    mod_indexer.clear();
    endpoint_indexer.clear();
    group_count_ = 0;
}

void TrackerManager::PPLeave()
{
    if (false == is_running_) {
        return ;
    }
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerManager::PPLeave()");
    /*
    for (std::map<int, TrackerGroup::p> ::iterator iter = mod_indexer.begin(); iter != mod_indexer.end(); iter++)
    {
    iter->second->PPLeave();
    }
    */
    for ( std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p>::iterator i = this->cached_groups_.begin() ; i != this->cached_groups_.end() ; ++i ) {
        // 
        i->second->PPLeave();
    }
}    

//UPNP开启成功
// nightsuns: 服务器不需要UPNP,暂时不做处理
/*
void TrackerManager::OpenUPNPSucced(uint32_t ip, boost::uint16_t tcp_port, boost::uint16_t udp_port)
{
// 不用
return ;

LOG(__INFO, "upnp", "OpenUPNPSucced ip:"<<ip<<" tcp port:"<<tcp_port<<" udp port:"<<udp_port);

if (false == is_running_) {
LOG(__WARN, "upnp", "TrackerManager is not runnning");
return ;
}

if (udp_port == 0 && tcp_port == 0) {
LOG(__WARN, "upnp", "Invalid Ip/Port");
return ;
}

if (upnp_udp_port_ == udp_port && upnp_tcp_port_ != tcp_port) {
LOG(__WARN, "upnp", "upnp port need not change");
return ;
}

// check
protocol::SocketAddr detected = statistic::StatisticModule::Inst()->GetLocalDetectSocketAddress();
if (detected.Port == udp_port || (detected.IP == 0 && detected.Port == 0)) {
LOG(__WARN, "upnp", "upnp success! detected: " << detected << " upnp_udp_port="<<upnp_udp_port_);
// store
upnp_ip_ = detected.IP;
upnp_tcp_port_ = tcp_port;
upnp_udp_port_ = udp_port;

for (ModIndexer::iterator it = mod_indexer.begin(), eit = mod_indexer.end(); it != eit; it++) {
it->second->DoCommit();
}

// stop stun keep alive
// StunModule::Inst()->SetIsNeededStun(false);
}
else {
LOG(__WARN, "upnp", "upnp failed. udp_port=" << upnp_udp_port_ << " deteced=" << detected.Port);
}
}
*/

void TrackerManager::GetUPNPIPPort(uint32_t &ip, boost::uint16_t &tcp_port, boost::uint16_t &udp_port)
{
    ip = upnp_ip_;
    tcp_port = upnp_tcp_port_;
    udp_port = upnp_udp_port_;
}

void TrackerManager::OnDiskFileChange(const Guid& rid)
{
    if (false == is_running_) {
        return ;
    }

    LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerManager::OnDiskFileChange() Rid: " << rid);

    if (false == rid.is_empty() && group_count_ > 0)
    {
        int mod_no = GuidMod(rid, group_count_);

        ModIndexer::iterator it = mod_indexer.find(mod_no);
        assert(it != mod_indexer.end());
        if (it != mod_indexer.end() && it->second) {
            it->second->DoCommit();
        }
    }
}

void TrackerManager::list_trackers()
{
    std::cout << "listing trackers: " << std::endl;
    for ( ModIndexer::iterator i = mod_indexer.begin() ; i != mod_indexer.end() ; ++i ) {
        // group
        TrackerGroup::p group = i->second;
        std::cout << "group begin... " << std::endl ;
        for ( TrackerGroup::TrackerClientList::iterator j = group->tracker_list_.begin() ; j != group->tracker_list_.end() ; ++j ) {
            // 
            std::cout << "\t\t" << (*j)->end_point_ << std::endl;
        }
        std::cout << "group end " << std::endl ;
    }
}

void TrackerManager::UpdateTrackerRidList( const std::set<RID> & rids )
{
    // 在这里更新
    TrackerClient::client_resource_ids_.clear();
    TrackerClient::client_resource_ids_ = rids;

    // 由 tracker_client 自己维护列表,确定是否需要通知 tracker
}
