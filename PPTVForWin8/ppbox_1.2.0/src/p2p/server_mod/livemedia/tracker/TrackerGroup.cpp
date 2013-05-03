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

#include "tracker/TrackerGroup.h"
#include "tracker/TrackerManager.h"




void TrackerGroup::Start()
{
    is_responsed_ = true;
    error_times_ = 0;

    // 启动所有的client
    StartAllClients();
    // 
    // 创建定时器 timer_, 初始定时时间为 5秒，并且把该定时器启动起来

    OnTimerElapsed(&timer_);
    timer_.start();
}

void TrackerGroup::Stop()
{
    error_times_ = 0;
    // 干掉定时器

    timer_.stop();

    // 停止所有的Client
    ClearAllClients();
}

void TrackerGroup::StartAllClients()
{
    for (TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->Start();
    }
}

void TrackerGroup::StopAllClients()
{
    for (TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->Stop();
    }
}

void TrackerGroup::ClearAllClients()
{
    StopAllClients();

    trackers_.clear();
    tracker_list_.clear();
    current_tracker_ = TrackerClient::p();
    trying_tracker_iterator_ = tracker_list_.end();
    trying_tracker_ = TrackerClient::p();
}

void TrackerGroup::DoList(const RID& rid)
{
    // 遍历所有的 TrackerClient， 对每个TrackerClient DoList
    for(TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->DoList(rid);
    }
}

void TrackerGroup::DoQueriedPeerCount(const RID& rid)
{
    // 遍历所有的 TrackerClient， 对每个TrackerClient DoList
    for(TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->DoQueriedPeerCount(rid);
    }
}
void TrackerGroup::SetTrackers( boost::uint32_t group_count, const std::set<protocol::TRACKER_INFO>& trackers )
{
    // 第一步 添加新的Tracker
    // 第二步 删除旧的Tracker
    //   如果将 旧的Tracker 是 当前Tracker  
    //   那么将 current_tracker_ 置为空
    // 第三步
    //   重组 tracker_list_

    ClearAllClients();

    SetGroupCount(group_count);

    if (trackers.empty()) return ;

    std::vector<protocol::TRACKER_INFO> random_shuffled_trackers(trackers.begin(), trackers.end());

    // random shuffle
    std::random_shuffle(random_shuffled_trackers.begin(), random_shuffled_trackers.end());

    // make sure all the mod no of these trackers are the same
    for (std::vector<protocol::TRACKER_INFO>::iterator it = random_shuffled_trackers.begin(); 
        it != random_shuffled_trackers.end(); it++)
    {
        protocol::TRACKER_INFO& info = *it;
        if (info.Type) // udp
        {
            boost::asio::ip::udp::endpoint end_point = 
                framework::network::Endpoint(info.IP, info.Port);

            TrackerClient::p tracker_client = TrackerClient::Create(end_point);
            // tracker info
            tracker_client->SetTrackerInfo(info);
            tracker_client->SetGroupCount(group_count);

            tracker_list_.push_back(tracker_client);
            trackers_[end_point] = tracker_client;
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::Service(), "Tracker Type not supported, Type: " << info.Type);
            assert(0); // not supported
        }
    }

    // start all clients
    StartAllClients();
}

const std::vector<protocol::TRACKER_INFO> TrackerGroup::GetTrackers()
{
    std::vector<protocol::TRACKER_INFO> ret_tracker_infos;
    for (TrackerClientList::iterator it = tracker_list_.begin();
        it != tracker_list_.end(); ++it)
    {
        ret_tracker_infos.push_back((*it)->GetTrackerInfo());
    }
    return ret_tracker_infos;
}

boost::uint32_t TrackerGroup::GetTrackersNum(const RID &rid)
{
    return tracker_list_.size();
}
void TrackerGroup::OnTimerElapsed(framework::timer::Timer * pointer)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerGroup::OnTimerElapsed times = " << pointer->times());
    // 确认是 Commit/KeepAlive 定时器
    if( pointer != &timer_)
    {
        assert(0);
        return;
    }

    if (tracker_list_.empty())
    {
        LOG4CPLUS_WARN(Loggers::Service(), "Tracker List is empty.");
        return ;
    }

    // 如果 is_responsed_ == false 上次发的Commit/KeepAlive 没有收到回包或者回包有问题
    //     error_time ++
    //   如果 error_times >= 3 那么就 
    //        current_tacker_ 赋值为0
    //        timer_->SetInterval(  10秒 );  定时器时间设短
    if (!is_responsed_)
    {
        error_times_++;
        LOG4CPLUS_DEBUG(Loggers::Service(), "tracker:: No Response, Times: " << error_times_);
        if (error_times_ >= 3)
        {
            error_times_ = 0;
            // 统计信息
            if (current_tracker_)
            {
                // TODO: 这里暂时去掉跟 static 的关联,如果以后需要可以再补上
                //				statistic::StatisticModule::Inst()->SetIsSubmitTracker(current_tracker_->GetTrackerInfo(), false);
            }
            // 设为空
            current_tracker_ = TrackerClient::p();
            timer_.interval( DEFAULT_INTERVAL_IN_SECONDS_ *1000);
            LOG4CPLUS_DEBUG(Loggers::Service(), "tracker  No Response, Set Interval to default: ");
        }
    }

    // 如果 current_tacker_ 存在
    //   current_tacker_->DoSubmit()     里面会根据资源情况决定是KeepAlive还是Commit
    //   DoSubmit 后 记下 TransactionID 的值，is_responsed_ = false
    // 否则
    //   如果 trying_tracker 为空， trying_tracker 就为 tracker_list_ 的第一个。
    //   否则 为 tracker_list_ 的下一个
    //   last_response_rid_count = 0;
    //   然后 DoSubmit()  后 记下 TransactionID 的值，is_responsed_ = false
    if (current_tracker_)
    {
        is_responsed_ = false;
        last_transcation_id_ = current_tracker_->DoSubmit();
    }
    else if (!tracker_list_.empty()) // tracker_list could not be empty
    {
        if (trying_tracker_iterator_ == tracker_list_.end())
            trying_tracker_iterator_ = tracker_list_.begin();
        else 
        {
            (*trying_tracker_iterator_)->PPLeave();
            ++trying_tracker_iterator_;
        }

        if (trying_tracker_iterator_ != tracker_list_.end())
            trying_tracker_ = *trying_tracker_iterator_;

        is_responsed_ = false;
        trying_tracker_->SetRidCount(0); //! trick，使Submit时清空同步资源，并进行全新Report
        last_transcation_id_ = trying_tracker_->DoSubmit();
    }
}

void TrackerGroup::DoCommit()
{
    LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::DoCommit:");
    if (current_tracker_)
    {
        last_transcation_id_ = current_tracker_->DoSubmit();
    }
}

void TrackerGroup::OnReportResponsePacket( protocol::ReportPacket const & packet)
{
    boost::asio::ip::udp::endpoint  end_point = packet.end_point;

    // 在 trackers_ 中根据 end_point 找到对应的 TrackerClient, 如果找不到，直接返回
    // 该 TrackerClient->OnCommitResponsePacket(packet); 
    if (trackers_.count(end_point) == 0)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket No such end_point: " << end_point);
        return ;
    }
    trackers_[end_point]->OnReportResponsePacket(packet);

    // 如果 packet::GetTranscationID() != trans_id_
    //      直接返回
    if (packet.transaction_id_ != last_transcation_id_)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket() TrasactionID is " << packet.transaction_id_ << ", Expect: " << last_transcation_id_ );
        return;
    }

    // 如果 成功 packet->ErrorCode() == 0
    //        error_time = 0    is_responsed_ = true;
    //        timer_->SetInterval(  packet->GetInterval() );        定时器时间设长 （设为Tracker的返回值）
    //        如果 current_tracker_ 为空
    //            那么 current_tracker_ = 当前end_point对应 的 TrackerClient
    //        current_tracker->Set_Rid_Count(packet->GetRidCount());
    // 如果 失败 packet->ErrorCode() != 0
    //      直接返回

    if (packet.error_code_ == 0) // success
    {
        error_times_ = 0;
        is_responsed_ = true;
        if (trackers_[end_point]->IsSync()) {
            LOG4CPLUS_DEBUG(Loggers::Service(),"TrackerGroup::OnReportResponsePacket  Interval:" << packet.response.keep_alive_interval_ << " ResourceCount: " << (uint32_t) packet.response.resource_count_ );
            timer_.interval( packet.response.keep_alive_interval_*1000 );
        }
        else {
            // do not set timer, use default
            LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket() Tracker Not Synchronized. Do Not Use TrackerTime");
        }

        if (!current_tracker_)
        {
            current_tracker_ = trackers_[end_point];

            // TODO: 这里暂时去掉跟 static 的关联,如果以后需要可以再补上
            //			statistic::StatisticModule::Inst()->SetIsSubmitTracker(current_tracker_->GetTrackerInfo(), true);
        }
    }
    else
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket() packet.Error Code is: " << packet.error_code_);
    }
}

void TrackerGroup::OnListResponsePacket( protocol::ListPacket const & packet)
{
    boost::asio::ip::udp::endpoint end_point = packet.end_point;
    // 在 trackers_ 中根据 end_point 找到对应的 TrackerClient, 如果找不到，直接返回
    // 该 TrackerClient->OnListResponsePacket(packet);
    if (trackers_.count(end_point) != 0)
    {
        trackers_[end_point]->OnListResponsePacket(packet);
    }
}

void TrackerGroup::OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet)
{
    boost::asio::ip::udp::endpoint end_point = packet.end_point;
    // 在 trackers_ 中根据 end_point 找到对应的 TrackerClient, 如果找不到，直接返回
    // 该 TrackerClient->OnListResponsePacket(packet);
    if (trackers_.count(end_point) != 0)
    {
        trackers_[end_point]->OnQueryResponsePacket(packet);
    }
}
void TrackerGroup::SetGroupCount(boost::uint32_t group_count)
{
    group_count_ = group_count;
}

uint32_t TrackerGroup::GetGroupCount() const
{
    return group_count_;
}

void TrackerGroup::PPLeave()
{
    if (current_tracker_)
    {
        current_tracker_->PPLeave();
    }
}

