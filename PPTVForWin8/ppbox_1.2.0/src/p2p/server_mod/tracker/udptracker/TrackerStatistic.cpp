#include "Common.h"
#include "udptracker/TrackerStatistic.h"
#include <sstream>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include "server_mod/api/utility/util.h"

using namespace std;

extern log4cplus::Logger g_stat_logger;
extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_rid_peer_logger;

namespace udptracker
{
    //////////////////////////////////////////////////////////////////////////
    // Statistic Info

    std::string StatisticInfo::ToString() const
    {
        static char buffer[1024];
        sprintf(buffer, "%20lld%12.2lf%12.2lf%12.2lf", TotalCount, AverageSpeed, CurrentSpeed, RecentMinuteSpeed);
        return buffer;
    }
    std::string NatStatisticInfo::ToString() const
    {
        static char buffer[1024];
        ostringstream oss;
        oss << "---------- NAT ---------" << endl;
        for (unsigned i = 0; i < StatisticType::NAT_ENUM_SIZE; i++)
        {
            sprintf(buffer, " %s %10u", StatisticType::GetNatTypeName((StatisticType::NatStatisticType)i), NatTypePeerCount[i]);
            oss << buffer << endl;
        }
        return oss.str();
    }
    std::string AllStatisticInfo::ToString() const
    {
        static char buffer[1024];
        static char spliter[] = "------------------------------------------------------------------------------------";
        ostringstream oss;
        // header
        sprintf(buffer, " %s %20s%12s%12s%12s", StatisticType::GetName(), 
            "总数",
            "平均数 ",
            "最近5秒 ",
            "最近1分钟");

        oss << buffer << endl;
        // all
        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            if (i % 6 == 0) oss << spliter << endl;
            sprintf(buffer, " %s %s", StatisticType::GetName((StatisticType::TrackerStatisticType)i), Info[i].ToString().c_str());
            oss << buffer << endl;
        }
        // nat
        oss << endl;
        oss << NatTypeInfo.ToString();
        // idle
        oss << spliter << endl;
        oss << IdleInfo.ToString();
        return oss.str();
    }
    std::string IdleTimeInfo::ToString() const
    {
        static char buffer[1024];
        ostringstream oss;
        unsigned total_idle_count = 0;
        for (unsigned i = 0; i < 256; ++i)
        {
            if (i > 0) {
                total_idle_count += IdleTimeCount[i];
            }
            sprintf(buffer, "%3d %10d", i, IdleTimeCount[i]);
            oss << buffer << endl;
        }
        oss << "----------------------------------------\n";
        oss << "IdleCount: " << total_idle_count << endl;
        oss << "BusyCount: " << IdleTimeCount[0] << endl;
        oss << endl;
        return oss.str();
    }

    //////////////////////////////////////////////////////////////////////////
    // TrackerStatistic

    TrackerStatistic::p TrackerStatistic::inst_(new TrackerStatistic());

    //TrackerStatistic::p TrackerStatistic::Create()
    //{
    //    return TrackerStatistic::p(new TrackerStatistic());
    //}

    TrackerStatistic::p TrackerStatistic::Inst()
    {
        return inst_;
    }

    TrackerStatistic::TrackerStatistic()
        : is_running_(false), peer_count_(0), rid_count_(0), is_print_(false), print_times_(1000)
    {
        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
			meters_[i] = measure::ByteSpeedMeter::Create();
        }
    }

    void TrackerStatistic::Start(size_t res_hash_map_size)
    {
        if (is_running_)
            return ;

        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            meters_[i]->Start();
        }

#ifndef _MSC_VER
        p_rid_peer_hash_map_.reset(new hash_map<RID, RidStat, RidHash>(res_hash_map_size));
#endif

        ClearNatStatistic();

        is_running_ = true;
    }

    void TrackerStatistic::Stop()
    {
        if (!is_running_)
            return ;

        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            meters_[i]->Stop();
        }

        is_running_ = false;
    }

    void TrackerStatistic::ClearNatStatistic()
    {
        memset(&nat_type_info_, 0, sizeof(nat_type_info_));
    }

    void TrackerStatistic::RestartStatistic()
    {
        if (!is_running_) return ;
        // stop all
        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            meters_[i]->Stop();
        }
        // start all
        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            meters_[i]->Start();
        }
    }

    unsigned TrackerStatistic::GetElapsedTimeInMilliSeconds()
    {
        if (is_running_)
        {
            return meters_[0]->GetElapsedTimeInMilliSeconds();
        }
        return 0;
    }

    string TrackerStatistic::ReportStatistic()
    {
        static char buffer[1024];

        unsigned millisec = GetElapsedTimeInMilliSeconds();
        sprintf(buffer, "%03d:%02d:%02d", millisec / 3600000, (millisec % 3600000) / 60000, (millisec % 3600000 % 60000) / 1000);

        ostringstream oss;

        oss << "统计时长:" 
            << buffer << endl;

        oss << QueryAllStatisticInfo().ToString() << endl;

        return oss.str();
    }

    void TrackerStatistic::Submit(StatisticType::TrackerStatisticType type, unsigned packet_size)
    {
        if (is_running_ && type >= 0 && type < StatisticType::ENUM_SIZE)
        {
            meters_[type]->SubmitBytes(packet_size);
        }
    }

    // 请求计数
    void TrackerStatistic::SubmitListRequest(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST, 1);
        Submit(StatisticType::LIST_REQUEST, 1);
        // bytes
        Submit(StatisticType::ALL_REQUEST_BYTES, packet_size);
        Submit(StatisticType::LIST_REQUEST_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitReportRequest(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST, 1);
        Submit(StatisticType::REPORT_REQUEST, 1);
        // bytes
        Submit(StatisticType::ALL_REQUEST_BYTES, packet_size);
        Submit(StatisticType::REPORT_REQUEST_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitLeaveRequest(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST, 1);
        Submit(StatisticType::LEAVE_REQUEST, 1);
        // bytes
        Submit(StatisticType::ALL_REQUEST_BYTES, packet_size);
        Submit(StatisticType::LEAVE_REQUEST_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitCommitRequest(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST, 1);
        Submit(StatisticType::COMMIT_REQUEST, 1);
        // bytes
        Submit(StatisticType::ALL_REQUEST_BYTES, packet_size);
        Submit(StatisticType::COMMIT_REQUEST_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitKeepAliveRequest(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST, 1);
        Submit(StatisticType::KEEP_ALIVE_REQUEST, 1);
        // bytes
        Submit(StatisticType::ALL_REQUEST_BYTES, packet_size);
        Submit(StatisticType::KEEP_ALIVE_REQUEST_BYTES, packet_size);
    }

    // 响应计数
    void TrackerStatistic::SubmitListResponse(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_RESPONSE, 1);
        Submit(StatisticType::LIST_RESPONSE, 1);
        // bytes
        Submit(StatisticType::ALL_RESPONSE_BYTES, packet_size);
        Submit(StatisticType::LIST_RESPONSE_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitReportResponse(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_RESPONSE, 1);
        Submit(StatisticType::REPORT_RESPONSE, 1);
        // bytes
        Submit(StatisticType::ALL_RESPONSE_BYTES, packet_size);
        Submit(StatisticType::REPORT_RESPONSE_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitLeaveResponse(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_RESPONSE, 1);
        Submit(StatisticType::LEAVE_RESPONSE, 1);
        // bytes
        Submit(StatisticType::ALL_RESPONSE_BYTES, packet_size);
        Submit(StatisticType::LEAVE_RESPONSE_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitCommitResponse(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_RESPONSE, 1);
        Submit(StatisticType::COMMIT_RESPONSE, 1);
        // bytes
        Submit(StatisticType::ALL_RESPONSE_BYTES, packet_size);
        Submit(StatisticType::COMMIT_RESPONSE_BYTES, packet_size);
    }
    void TrackerStatistic::SubmitKeepAliveResponse(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_RESPONSE, 1);
        Submit(StatisticType::KEEP_ALIVE_RESPONSE, 1);
        // bytes
        Submit(StatisticType::ALL_RESPONSE_BYTES, packet_size);
        Submit(StatisticType::KEEP_ALIVE_RESPONSE_BYTES, packet_size);
    }

    // 出错请求计数
    void TrackerStatistic::SubmitRequestError(unsigned packet_size)
    {
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
    }
    void TrackerStatistic::SubmitListRequestError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
        Submit(StatisticType::LIST_REQUEST_ERROR, 1);
    }
    void TrackerStatistic::SubmitReportRequestError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
        Submit(StatisticType::REPORT_REQUEST_ERROR, 1);
    }
    void TrackerStatistic::SubmitLeaveRequestError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
        Submit(StatisticType::LEAVE_REQUEST_ERROR, 1);

    }
    void TrackerStatistic::SubmitCommitRequestError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
        Submit(StatisticType::COMMIT_REQUEST_ERROR, 1);

    }
    void TrackerStatistic::SubmitKeepAliveRequestError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::ALL_REQUEST_ERROR, 1);
        Submit(StatisticType::KEEP_ALIVE_REQUEST_ERROR, 1);

    }
    // 出错响应计数
    void TrackerStatistic::SubmitResponseError(unsigned packet_size)
    {
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
    }
    void TrackerStatistic::SubmitListResponseError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
        Submit(StatisticType::KEEP_ALIVE_RESPONSE_ERROR, 1);
    }
    void TrackerStatistic::SubmitReportResponseError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
        Submit(StatisticType::REPORT_RESPONSE_ERROR, 1);

    }
    void TrackerStatistic::SubmitLeaveResponseError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
        Submit(StatisticType::LEAVE_RESPONSE_ERROR, 1);
    }
    void TrackerStatistic::SubmitCommitResponseError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
        Submit(StatisticType::COMMIT_RESPONSE_ERROR, 1);
    }
    void TrackerStatistic::SubmitKeepAliveResponseError(unsigned packet_size)
    {
        // counter
        Submit(StatisticType::All_RESPONSE_ERROR, 1);
        Submit(StatisticType::KEEP_ALIVE_RESPONSE_ERROR, 1);
    }

    void TrackerStatistic::SubmitReportNonMatch(unsigned packet_size)
    {
        Submit(StatisticType::REPORT_REQUEST_NON_MATCH, 1);
    }

    StatisticInfo TrackerStatistic::QueryStatisticInfo(StatisticType::TrackerStatisticType type)
    {
        StatisticInfo info;
        if (type >= 0 && type < StatisticType::ENUM_SIZE)
        {
			measure::ByteSpeedMeter::p meter = meters_[type];
            info.AverageSpeed = meter->AverageByteSpeed();
            info.CurrentSpeed = meter->CurrentByteSpeed();
            info.RecentMinuteSpeed = meter->RecentMinuteByteSpeed();
            info.TotalCount = meter->TotalBytes();
        }
        return info;
    }
    AllStatisticInfo TrackerStatistic::QueryAllStatisticInfo()
    {
        AllStatisticInfo all_info;
        for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
        {
            all_info.Info[i] = QueryStatisticInfo((StatisticType::TrackerStatisticType)i);
        }
        all_info.NatTypeInfo = nat_type_info_;
        all_info.IdleInfo = idle_time_info_;
        return all_info;
    }

    NatStatisticInfo TrackerStatistic::QueryNatStatisticInfo()
    {
        return nat_type_info_;
    }

    IdleTimeInfo TrackerStatistic::QueryIdleTimeInfo()
    {
        return idle_time_info_;
    }

    // NAT类型统计
    void TrackerStatistic::IncPeerNatTypeCount(StatisticType::NatStatisticType type, unsigned count)
    {
        if (type >= 0 && type < StatisticType::NAT_ENUM_SIZE)
        {
            nat_type_info_.NatTypePeerCount[type] += count;
        }
    }

    void TrackerStatistic::DecPeerNatTypeCount(StatisticType::NatStatisticType type, unsigned count)
    {
        if (type >= 0 && type < StatisticType::NAT_ENUM_SIZE)
        {
            assert(count <= nat_type_info_.NatTypePeerCount[type]);
            if (count <= nat_type_info_.NatTypePeerCount[type])
            {
                nat_type_info_.NatTypePeerCount[type] -= count;
            }
        }
    }

    // IDLE Time 统计
    void TrackerStatistic::IncPeerIdleTimeCount(unsigned idle_time, unsigned count)
    {
        if (idle_time >= 0 && idle_time <= 255)
        {
            idle_time_info_.IdleTimeCount[idle_time] += count;
        }
    }
    void TrackerStatistic::DecPeerIdleTimeCount(unsigned idle_time, unsigned count)
    {
        if (idle_time >= 0 && idle_time <= 255 && idle_time_info_.IdleTimeCount[idle_time] >= count)
        {
            idle_time_info_.IdleTimeCount[idle_time] -= count;
        }
    }

    void TrackerStatistic::IncPeerIdleTime(unsigned idle_time)
    {
        IncPeerIdleTimeCount(idle_time, 1);
    }

    void TrackerStatistic::DecPeerIdleTime(unsigned idle_time)
    {
        DecPeerIdleTimeCount(idle_time, 1);
    }

    void TrackerStatistic::AddCmdTimes( int cmd )
    {
        ++cmd_times_v2_[cmd].first;
    }

    void TrackerStatistic::AddHitCmdTimes( int cmd )
    {
        ++cmd_times_v2_[cmd].second;
    }

    void TrackerStatistic::AddIpTimes( std::string ip )
    {
        ++query_ip_times_[ip];
    }

    void TrackerStatistic::AddRidTimes( RID rid )
    {
        ++rid_times_[rid];
    }

    void TrackerStatistic::AddPeerversionTimes( int version )
    {
        ++peerversion_times_[version];
    }

    void TrackerStatistic::AddNatTypeTimes( int nattype )
    {
        ++nattype_times_[nattype];
    }

    void TrackerStatistic::AddReportNumTimes( int report_num )
    {
        ++report_num_times_[report_num];
    }

    void TrackerStatistic::SetRidCount( int rid_count )
    {
        rid_count_ = rid_count;
    }

    void TrackerStatistic::SetPeerCount( int peer_count )
    {
        peer_count_ = peer_count;
    }

    void TrackerStatistic::ComputeQueryHitRate( float& hit_rate, int& reqest_times, int& hit_times )
    {
        reqest_times = 0;
        hit_times = 0;
        for (std::map<int, std::pair<int, int> >::iterator it = cmd_times_v2_.begin(); it != cmd_times_v2_.end(); ++it)
        {
            switch(it->first)
            {
            case protocol::ListPacket::Action:
            case protocol::ListTcpPacket::Action:       
            case protocol::ListWithIpPacket::Action:
            case protocol::ListTcpWithIpPacket::Action:
                {
                    reqest_times += it->second.first;
                    hit_times += it->second.second;
                    break;
                }
            default:
                break;
            }
        }

        if (reqest_times == 0)
        {
            hit_rate = 0;
            return ;
        }

        hit_rate = static_cast<float>(hit_times * 1000/reqest_times/1000.0);

        return ;
    }

    void TrackerStatistic::OutputSata()
    {
        std::ostringstream oss;

        oss << "Resource_count:" << rid_count_ << " Peer_count:" << peer_count_;

        for (map<int, std::pair< int, int > >::iterator it = cmd_times_v2_.begin();it != cmd_times_v2_.end();++it)
        {
            oss<<" Cmd:0x" << std::hex <<it->first<<" count:"<<std::dec<< it->second.first;
        }
        
        for (map<int,int>::iterator it = peerversion_times_.begin();it != peerversion_times_.end();++it)
        {
            oss<<" Version:0x" << std::hex <<it->first<<" count:"<<std::dec<<it->second;
        }
        
        for (map<int,int>::iterator it = nattype_times_.begin();it != nattype_times_.end();++it)
        {
            oss<<" nat_tpye:" <<it->first<<" count:"<<it->second;
        }

        //输出report的资源数量情况：
        for (map<int,int>::iterator it = report_num_times_.begin();it != report_num_times_.end();++it)
        {
            LOG4CPLUS_INFO(g_logger,"report num:"<<it->first<<" times:"<<it->second);
        }
        LOG4CPLUS_INFO(g_logger,"report num times size:"<<report_num_times_.size());

        //日志记录RID的查询次数
        for(map<RID,int>::iterator it = rid_times_.begin();it != rid_times_.end();++it )
        {
            const static int REQ_TIMES = 100;
            //只打出查询次数大于REQ_TIMES次的RID
            if(it->second > REQ_TIMES)
                LOG4CPLUS_INFO(g_logger,"Rid:"<<it->first.to_string()<<" times:"<<it->second);
        }
        LOG4CPLUS_INFO(g_logger,"rid times size:"<<rid_times_.size());
        rid_times_.clear();

        LOG4CPLUS_INFO(g_stat_logger,oss.str());
    }

    void TrackerStatistic::OutputStatisticData()
    {
        float hit_rate = 0.0;
        int request_times = 0;
        int hit_times = 0;
//        std::pair< std::string, int > max_ip_pair = GetMaxQueryIp();
//        std::pair< RID, int > max_Rid_pair = GetMaxQueryRID();
        std::pair< std::string, int > max_ip_pair = GetMaxQuery(query_ip_times_);
        std::pair< RID, int > max_Rid_pair = GetMaxQuery(rid_times_);
        ComputeQueryHitRate(hit_rate, request_times, hit_times);
        LOG4CPLUS_INFO(g_stat_logger, "Request_times:"<< request_times 
            <<" Hit_times:" << hit_times <<" hit_rate:" << hit_rate
            <<" Max_ip:" << max_ip_pair.first <<" Times:" << max_ip_pair.second
            <<" Max RID:" << max_Rid_pair.first << " Times:" << max_Rid_pair.second );
        
        OutputSata();
        ClearStatisticData();
    }

    void TrackerStatistic::ClearStatisticData()
    {
        cmd_times_v2_.clear();
        query_ip_times_.clear();
        rid_times_.clear();
        peerversion_times_.clear();
        nattype_times_.clear();
        report_num_times_.clear();
    }

    void TrackerStatistic::InsertStatRidHashMap( RID rid, uint32_t peer_count_for_request )
    {
#ifndef _MSC_VER
       hash_map<RID, RidStat, RidHash>::iterator iter = p_rid_peer_hash_map_->find(rid);
       if(iter != p_rid_peer_hash_map_->end())
        {
            iter->second.rid_count++;
            iter->second.total_peer_count += peer_count_for_request;
        }
        else
        {
            RidStat rid_stat;
            rid_stat.rid_count = 1;
            rid_stat.total_peer_count += peer_count_for_request;
            (*p_rid_peer_hash_map_)[rid] = rid_stat;
        }
#endif
    }

    void TrackerStatistic::OutputRidPeerHashMap()
    {
        LOG4CPLUS_INFO(g_rid_peer_logger, "OutputRidPeerHashMap...");
		double time_start = ns_pplive::utility::GetExactTimeNow();
        std::string strTime = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
        std::cout << strTime << endl;
//        LOG4CPLUS_INFO(g_rid_peer_logger, "Time: " << boost::posix_time::second_clock::local_time())
        LOG4CPLUS_INFO(g_rid_peer_logger, "Time: " << strTime);
#ifndef _MSC_VER
        for(hash_map<RID, RidStat, RidHash>::iterator iter = p_rid_peer_hash_map_->begin(); iter != p_rid_peer_hash_map_->end(); ++iter)
        {
            assert(iter->second.rid_count);
           /* LOG4CPLUS_INFO(g_rid_peer_logger, "RID:" << iter->first << " rid_count:" << iter->second.rid_count
                << " total_peer_count:" << iter->second.total_peer_count
                << " per_rid_peer_count:" << (iter->second.total_peer_count/iter->second.rid_count));*/
            LOG4CPLUS_INFO(g_rid_peer_logger, iter->first << " rc:" << iter->second.rid_count
                << " tpc:" << iter->second.total_peer_count);
        }

        cout << "now print elapsed time:" << ns_pplive::utility::GetExactTimeNow() - time_start << "s" << endl;
        LOG4CPLUS_DEBUG(g_rid_peer_logger, "ConsumeTime: " << ns_pplive::utility::GetExactTimeNow() - time_start << "s");       
        LOG4CPLUS_INFO(g_rid_peer_logger, "bucket max size:" << p_rid_peer_hash_map_->max_bucket_count() << " bucket size:" << p_rid_peer_hash_map_->bucket_count());
        p_rid_peer_hash_map_->clear();
        LOG4CPLUS_DEBUG(g_rid_peer_logger, "ConsumeTime1: " << ns_pplive::utility::GetExactTimeNow() - time_start << "s");   
#endif
    }

	std::string TrackerStatistic::GetTrackerStatisticForQuery()
	{
		std::stringstream ss;
		int tmptotal = 0;
		for (std::map<int, std::pair< int, int > >::iterator it = cmd_times_v2_.begin();it != cmd_times_v2_.end();++it)
		{
			ss<<"0x"<<std::hex<< it->first<<"--"<<std::dec<<it->second.second<<endl;
			tmptotal += it->second.second;
		}
		ss<<" total:"<<tmptotal<<endl;
		tmptotal = 0;
		ss<<" peerversion--times:"<<endl;
		for (map<int,int>::iterator it = peerversion_times_.begin();it != peerversion_times_.end();++it)
		{
			ss<<"0x"<<std::hex<< it->first<<"--"<<std::dec<<it->second<<endl;
			tmptotal += it->second;
		}
		ss<<" total:"<<tmptotal<<endl;
		tmptotal = 0;
		ss<<" nat type--times:"<<endl;
		for (map<int,int>::iterator it = nattype_times_.begin();it != nattype_times_.end();++it)
		{
			ss<< it->first<<"--"<<it->second<<endl;
			tmptotal += it->second;
		}
		ss<<" total:"<<tmptotal<<endl;

		return ss.str();
	}
}
