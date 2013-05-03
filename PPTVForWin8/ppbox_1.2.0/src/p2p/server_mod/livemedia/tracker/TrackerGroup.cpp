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

    // �������е�client
    StartAllClients();
    // 
    // ������ʱ�� timer_, ��ʼ��ʱʱ��Ϊ 5�룬���ҰѸö�ʱ����������

    OnTimerElapsed(&timer_);
    timer_.start();
}

void TrackerGroup::Stop()
{
    error_times_ = 0;
    // �ɵ���ʱ��

    timer_.stop();

    // ֹͣ���е�Client
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
    // �������е� TrackerClient�� ��ÿ��TrackerClient DoList
    for(TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->DoList(rid);
    }
}

void TrackerGroup::DoQueriedPeerCount(const RID& rid)
{
    // �������е� TrackerClient�� ��ÿ��TrackerClient DoList
    for(TrackerClientList::iterator it = tracker_list_.begin(), 
        eit = tracker_list_.end(); it != eit; it++)
    {
        (*it)->DoQueriedPeerCount(rid);
    }
}
void TrackerGroup::SetTrackers( boost::uint32_t group_count, const std::set<protocol::TRACKER_INFO>& trackers )
{
    // ��һ�� ����µ�Tracker
    // �ڶ��� ɾ���ɵ�Tracker
    //   ����� �ɵ�Tracker �� ��ǰTracker  
    //   ��ô�� current_tracker_ ��Ϊ��
    // ������
    //   ���� tracker_list_

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
    // ȷ���� Commit/KeepAlive ��ʱ��
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

    // ��� is_responsed_ == false �ϴη���Commit/KeepAlive û���յ��ذ����߻ذ�������
    //     error_time ++
    //   ��� error_times >= 3 ��ô�� 
    //        current_tacker_ ��ֵΪ0
    //        timer_->SetInterval(  10�� );  ��ʱ��ʱ�����
    if (!is_responsed_)
    {
        error_times_++;
        LOG4CPLUS_DEBUG(Loggers::Service(), "tracker:: No Response, Times: " << error_times_);
        if (error_times_ >= 3)
        {
            error_times_ = 0;
            // ͳ����Ϣ
            if (current_tracker_)
            {
                // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
                //				statistic::StatisticModule::Inst()->SetIsSubmitTracker(current_tracker_->GetTrackerInfo(), false);
            }
            // ��Ϊ��
            current_tracker_ = TrackerClient::p();
            timer_.interval( DEFAULT_INTERVAL_IN_SECONDS_ *1000);
            LOG4CPLUS_DEBUG(Loggers::Service(), "tracker  No Response, Set Interval to default: ");
        }
    }

    // ��� current_tacker_ ����
    //   current_tacker_->DoSubmit()     ����������Դ���������KeepAlive����Commit
    //   DoSubmit �� ���� TransactionID ��ֵ��is_responsed_ = false
    // ����
    //   ��� trying_tracker Ϊ�գ� trying_tracker ��Ϊ tracker_list_ �ĵ�һ����
    //   ���� Ϊ tracker_list_ ����һ��
    //   last_response_rid_count = 0;
    //   Ȼ�� DoSubmit()  �� ���� TransactionID ��ֵ��is_responsed_ = false
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
        trying_tracker_->SetRidCount(0); //! trick��ʹSubmitʱ���ͬ����Դ��������ȫ��Report
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

    // �� trackers_ �и��� end_point �ҵ���Ӧ�� TrackerClient, ����Ҳ�����ֱ�ӷ���
    // �� TrackerClient->OnCommitResponsePacket(packet); 
    if (trackers_.count(end_point) == 0)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket No such end_point: " << end_point);
        return ;
    }
    trackers_[end_point]->OnReportResponsePacket(packet);

    // ��� packet::GetTranscationID() != trans_id_
    //      ֱ�ӷ���
    if (packet.transaction_id_ != last_transcation_id_)
    {
        LOG4CPLUS_DEBUG(Loggers::Service(), "TrackerGroup::OnReportResponsePacket() TrasactionID is " << packet.transaction_id_ << ", Expect: " << last_transcation_id_ );
        return;
    }

    // ��� �ɹ� packet->ErrorCode() == 0
    //        error_time = 0    is_responsed_ = true;
    //        timer_->SetInterval(  packet->GetInterval() );        ��ʱ��ʱ���賤 ����ΪTracker�ķ���ֵ��
    //        ��� current_tracker_ Ϊ��
    //            ��ô current_tracker_ = ��ǰend_point��Ӧ �� TrackerClient
    //        current_tracker->Set_Rid_Count(packet->GetRidCount());
    // ��� ʧ�� packet->ErrorCode() != 0
    //      ֱ�ӷ���

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

            // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
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
    // �� trackers_ �и��� end_point �ҵ���Ӧ�� TrackerClient, ����Ҳ�����ֱ�ӷ���
    // �� TrackerClient->OnListResponsePacket(packet);
    if (trackers_.count(end_point) != 0)
    {
        trackers_[end_point]->OnListResponsePacket(packet);
    }
}

void TrackerGroup::OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet)
{
    boost::asio::ip::udp::endpoint end_point = packet.end_point;
    // �� trackers_ �и��� end_point �ҵ���Ӧ�� TrackerClient, ����Ҳ�����ֱ�ӷ���
    // �� TrackerClient->OnListResponsePacket(packet);
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

