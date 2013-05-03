#include "Common.h"
#include "TrackeragStatistic.h"
#include <sstream>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include "server_mod/api/utility/util.h"
#include "TrackerRequestHandler.h"

using namespace std;

extern log4cplus::Logger g_stat_logger;
extern log4cplus::Logger g_logger;
//extern log4cplus::Logger g_rid_peer_logger;

namespace udptrackerag
{
	//////////////////////////////////////////////////////////////////////////
	// Statistic Info

	std::string StatisticInfo::ToString() const
	{
		static char buffer[1024];
		sprintf(buffer, "%10lld%15.2lf%12.2lf%12.2lf", TotalCount, AverageSpeed, CurrentSpeed, RecentMinuteSpeed);
		return buffer;
	}

	std::string AllStatisticInfo::ToString() const
	{
		static char buffer[1024] = {0};
		static char spliter[] = "------------------------------------------------------------------------------------";
		ostringstream oss;
		// header
		//sprintf(buffer, " %s %20s%12s%12s%12s", StatisticType::GetName(), 
		sprintf(buffer, " %s %29s%14s%12s%12s", " name",
			"total_count ",
			"avg_count",
			"recent_5s",
			"recent_1m");

		oss << buffer << endl;
		// all
		for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
		{
			if (i % 4 == 0) oss << spliter << endl;
			sprintf(buffer, " %s %s", StatisticType::GetName((StatisticType::TrackeragStatisticType)i), Info[i].ToString().c_str());
			oss << buffer << endl;
		}

		// idle
		oss << spliter << endl;

		return oss.str();
	}

	//////////////////////////////////////////////////////////////////////////
	// TrackeragStatistic

	TrackeragStatistic::p TrackeragStatistic::inst_(new TrackeragStatistic());

	TrackeragStatistic::p TrackeragStatistic::Inst()
	{
		return inst_;
	}

	TrackeragStatistic::TrackeragStatistic()
		: is_running_(false), peer_count_(0), rid_count_(0), unexpected_response_(0), timeout_response_(0)//, is_print_(false)//, print_times_(1000)
	{
		for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
		{
			meters_[i] = measure::ByteSpeedMeter::Create();
		}
	}

	void TrackeragStatistic::Start(size_t res_hash_map_size)
	{
		if (true == is_running_)
			return ;

		for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
		{
			meters_[i]->Start();
		}

#ifndef _MSC_VER
		p_rid_peer_hash_map_.reset(new hash_map<RID, RidStat, RidHash>(res_hash_map_size));
#endif

		//ClearNatStatistic();

		is_running_ = true;
	}

	void TrackeragStatistic::Stop()
	{
		if (false == is_running_)
			return ;

		for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
		{
			meters_[i]->Stop();
		}

		is_running_ = false;
	}


	void TrackeragStatistic::RestartStatistic()
	{
		if (false == is_running_) return ;
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

	unsigned TrackeragStatistic::GetElapsedTimeInMilliSeconds()
	{
		if (is_running_)
		{
			return meters_[0]->GetElapsedTimeInMilliSeconds();
		}
		return 0;
	}

	string TrackeragStatistic::ReportStatistic()
	{
		static char buffer[1024];

		unsigned millisec = GetElapsedTimeInMilliSeconds();
		sprintf(buffer, "%03d:%02d:%02d", millisec / 3600000, (millisec % 3600000) / 60000, (millisec % 3600000 % 60000) / 1000);

		ostringstream oss;

		oss << "statistics time:" 
			<< buffer << endl;

		oss << QueryAllStatisticInfo().ToString() << endl;

		return oss.str();
	}

	void TrackeragStatistic::Submit(StatisticType::TrackeragStatisticType type, unsigned packet_size)
	{
		if (is_running_ && type >= 0 && type < StatisticType::ENUM_SIZE)
		{
			meters_[type]->SubmitBytes(packet_size);
		}
	}

	// 请求计数
	void TrackeragStatistic::SubmitClientRequest(unsigned packet_size)
	{
		// counter
		Submit(StatisticType::CLIENT_REQUEST, 1);
		// bytes
		Submit(StatisticType::CLIENT_REQUEST_BYTES, packet_size);
	}

	// 请求计数
	void TrackeragStatistic::SubmitTrackerRequest(unsigned packet_size, int send_num)
	{
		// counter
		Submit(StatisticType::REQUEST_TRACKER, send_num);
		// bytes
		Submit(StatisticType::REQUEST_TRACKER_BYTES, packet_size*send_num);
	}

	// 响应计数
	void TrackeragStatistic::SubmitListResponse(unsigned packet_size)
	{
		// counter
		Submit(StatisticType::TRACKER_RESPONSE, 1);
		// bytes
		Submit(StatisticType::TRACKER_RESPONSE_BYTES, packet_size);
	}

	void TrackeragStatistic::SubmitCommitResponse(unsigned packet_size)
	{
		// counter
		Submit(StatisticType::RESPONSE_CLIENT, 1);
		// bytes
		Submit(StatisticType::RESPONSE_CLIENT_BYTES, packet_size);
	}

	StatisticInfo TrackeragStatistic::QueryStatisticInfo(StatisticType::TrackeragStatisticType type)
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
	AllStatisticInfo TrackeragStatistic::QueryAllStatisticInfo()
	{
		AllStatisticInfo all_info;
		for (unsigned i = 0; i < StatisticType::ENUM_SIZE; i++)
		{
			all_info.Info[i] = QueryStatisticInfo((StatisticType::TrackeragStatisticType)i);
		}

		return all_info;
	}

	void TrackeragStatistic::AddCmdTimes( int cmd )
	{
		++cmd_times_v2_[cmd].first;
	}

	void TrackeragStatistic::AddHitCmdTimes( int cmd )
	{
		++cmd_times_v2_[cmd].second;
	}

	void TrackeragStatistic::AddIpTimes( std::string ip )
	{
		++query_ip_times_[ip];
	}

	void TrackeragStatistic::AddRidTimes( RID rid )
	{
		++rid_times_[rid];
	}

	void TrackeragStatistic::AddListCountTimes(int peer_count)
	{
		++list_count_times_[peer_count];
	}

	void TrackeragStatistic::AddTrackerListCountTimes(int peer_count)
	{
		++tracker_list_count_times_[peer_count];
	}

	void TrackeragStatistic::AddListEndpointResponseTimes(boost::asio::ip::udp::endpoint tracker_end_point)
	{
		++list_endpoint_response_[tracker_end_point];
	}

	void TrackeragStatistic::AddListEndpointResponse0Times(boost::asio::ip::udp::endpoint tracker_end_point)
	{
		++list_endpoint_response_0[tracker_end_point];
	}

	void TrackeragStatistic::AddListResponseNum(int response_count)
	{
		++list_response_num_[response_count];
	}

	void TrackeragStatistic::AddUnexpectedResponse()
	{
		++unexpected_response_;
	}

	void TrackeragStatistic::AddUseCacheResult()
	{
		++use_cache_result_;
	}

	void TrackeragStatistic::AddTotalResult()
	{
		++total_result_;
	}

	void TrackeragStatistic::AddTotalFailResult(int no_response_count)
	{
		timeout_response_ += no_response_count;
	}


	void TrackeragStatistic::ComputeQueryHitRate( float& hit_rate, int& reqest_times, int& hit_times )
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
/*
	void TrackeragStatistic::OnDumpInfo()//
	{
         LOG4CPLUS_INFO(g_logger,"OnDumpInfo");
        
         int tmptotal = 0;
         for(map<int,int>::iterator it = list_count_times_.begin();it != list_count_times_.end();++it)
         {
             tmptotal += fabs((double)it->second);
             LOG4CPLUS_INFO(g_logger,"list_"<<it->first<<" "<<it->second);
         }
         LOG4CPLUS_INFO(g_logger,"total list count:"<<tmptotal);
         list_count_times_.clear();

         tmptotal = 0;
         for(map<int,int>::iterator it = tracker_list_count_times_.begin();it != tracker_list_count_times_.end();++it)
         {
             tmptotal += fabs((double)it->second);
             LOG4CPLUS_INFO(g_logger,"list_"<<it->first<<" "<<it->second);
         }
         LOG4CPLUS_INFO(g_logger,"tracker list count:"<<tmptotal);
         tracker_list_count_times_.clear();

		 double ratio = 0.0;
		 double request_times = 0.0;
		 double response_times = 0.0;

         tmptotal = 0;
         for(map<int,int>::iterator it = list_response_num_.begin();it != list_response_num_.end();++it)
         {
             tmptotal += fabs((double)it->second);
             LOG4CPLUS_INFO(g_logger,"tracker response num:"<<it->first<<" times:"<<it->second);
         }
         LOG4CPLUS_INFO(g_logger,"total tracker response num:"<<tmptotal);
         list_response_num_.clear();

         int tmp_request_total = 0;
		 int tmp_response_total = 0;
		 int tmp_response_total_0 = 0;

         map<boost::asio::ip::udp::endpoint,int> list_endpoint_request = TrackerRequestHandler::Instance()->ClearListEndRequest();
         for(map<boost::asio::ip::udp::endpoint,int>::iterator it = list_endpoint_request.begin();it != list_endpoint_request.end();++it)
         {
             tmp_request_total += fabs((double)it->second);
             LOG4CPLUS_INFO(g_logger,it->first<<" times:"<<it->second);
			 request_times = (double)it->second;
			 //tmp_response_total += (double)list_endpoint_response_[it->first];
			 tmp_response_total_0 += (double)list_endpoint_response_0[it->first];
         }
		 if (tmp_request_total != 0)
		 {
			 ratio = (1.0*timeout_response_)/tmp_request_total;
			 LOG4CPLUS_INFO(g_stat_logger,"total request times: "<<tmp_request_total<<", total timeout response: "<<timeout_response_<<", loss ratio: "<<ratio<<", total response times with 0 peer: "<<tmp_response_total_0);
			 LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		 }
		 else
		 {
			 LOG4CPLUS_INFO(g_stat_logger,"total request times: 0, "<<"total timeout response: 0, "<<"loss ratio: 0");
			 LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		 }
		 timeout_response_ = 0;
         list_endpoint_request.clear();
         list_endpoint_response_.clear();
         list_endpoint_response_0.clear();

         tmptotal = 0;
		 ratio = 0;
         for(map<boost::asio::ip::udp::endpoint,int>::iterator it = list_endpoint_response_.begin();it != list_endpoint_response_.end();++it)
         {        
             tmptotal += fabs((double)it->second);
             LOG4CPLUS_INFO(g_logger,it->first<<" times:"<<it->second);
         }
         LOG4CPLUS_INFO(g_logger,"total tracker list response count:"<<tmptotal);
         list_endpoint_response_.clear();
 
         LOG4CPLUS_INFO(g_logger,"unexpected_response_:"<<unexpected_response_);
         unexpected_response_ = 0;

		 LOG4CPLUS_INFO(g_logger,"use_cache_result_:"<<use_cache_result_);
		 if (total_result_ != 0)
		 {
			 ratio = (1.0*use_cache_result_)/total_result_;
			 LOG4CPLUS_INFO(g_stat_logger,"total success result:"<<total_result_<<",use_cache_result_:"<<use_cache_result_<<",use cache ratio:"<<ratio);
		 }
		 else
		 {
			 LOG4CPLUS_INFO(g_stat_logger, "total_result_ is 0");
		 }
         use_cache_result_ = 0;
		 total_result_ = 0;
	}

	void TrackeragStatistic::OutputSata()
	{
		std::ostringstream oss;

		for (map<int, std::pair< int, int > >::iterator it = cmd_times_v2_.begin();it != cmd_times_v2_.end();++it)
		{
			oss<<" Cmd :0x" << std::hex <<it->first<<" count:"<<std::dec<< it->second.first;
		}
		if (cmd_times_v2_.size() == 0)
		{
			oss<<"Cmd count: "<<0;
		}

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

		if (oss.str().length() > 0)
		{
			LOG4CPLUS_INFO(g_stat_logger,oss.str());
		}

        LOG4CPLUS_INFO(g_stat_logger, this->ReportStatistic());

	}

	void TrackeragStatistic::OutputStatisticData()
	{
		float hit_rate = 0.0;
		int request_times = 0;
		int hit_times = 0;
		std::pair< std::string, int > max_ip_pair = GetMaxQuery(query_ip_times_);
		std::pair< RID, int > max_Rid_pair = GetMaxQuery(rid_times_);
		ComputeQueryHitRate(hit_rate, request_times, hit_times);
		if (request_times)
		{
			LOG4CPLUS_INFO(g_stat_logger, "Request_times:"<< request_times 
				<<" Hit_times:" << hit_times <<" hit_rate:" << hit_rate
				<<" Max_ip:" << max_ip_pair.first <<" Times:" << max_ip_pair.second
				<<" Max RID:" << max_Rid_pair.first << " Times:" << max_Rid_pair.second );
		}
		else
		{
			LOG4CPLUS_INFO(g_stat_logger, "Request_times:"<< request_times);
		}
		OutputSata();
		ClearStatisticData();
	}*/

	void TrackeragStatistic::OnDumpInfo()
	{
		LOG4CPLUS_INFO(g_logger,"OnDumpInfo");

		int tmptotal = 0;
		for(map<int,int>::iterator it = list_count_times_.begin();it != list_count_times_.end();++it)
		{
			tmptotal += fabs((double)it->second);
			LOG4CPLUS_INFO(g_logger,"list_"<<it->first<<" "<<it->second);
		}
		LOG4CPLUS_INFO(g_logger,"total list count:"<<tmptotal);
		//list_count_times_.clear();

		tmptotal = 0;
		for(map<int,int>::iterator it = tracker_list_count_times_.begin();it != tracker_list_count_times_.end();++it)
		{
			tmptotal += fabs((double)it->second);
			LOG4CPLUS_INFO(g_logger,"list_"<<it->first<<" "<<it->second);
		}
		LOG4CPLUS_INFO(g_logger,"tracker list count:"<<tmptotal);
		//tracker_list_count_times_.clear();

		double ratio = 0.0;
		double request_times = 0.0;
		double response_times = 0.0;

		tmptotal = 0;
		for(map<int,int>::iterator it = list_response_num_.begin();it != list_response_num_.end();++it)
		{
			tmptotal += fabs((double)it->second);
			LOG4CPLUS_INFO(g_logger,"tracker response num:"<<it->first<<" times:"<<it->second);
		}
		LOG4CPLUS_INFO(g_logger,"total tracker response num:"<<tmptotal);
		//list_response_num_.clear();

		int tmp_request_total = 0;
		int tmp_response_total = 0;
		int tmp_response_total_0 = 0;

		map<boost::asio::ip::udp::endpoint,int> list_endpoint_request = TrackerRequestHandler::Instance()->ClearListEndRequest();
		for(map<boost::asio::ip::udp::endpoint,int>::iterator it = list_endpoint_request.begin();it != list_endpoint_request.end();++it)
		{
			tmp_request_total += fabs((double)it->second);
			LOG4CPLUS_INFO(g_logger,it->first<<" times:"<<it->second);
			request_times = (double)it->second;
			//tmp_response_total += (double)list_endpoint_response_[it->first];
			tmp_response_total_0 += (double)list_endpoint_response_0[it->first];
		}
		if (tmp_request_total != 0)
		{
			ratio = (1.0*timeout_response_)/tmp_request_total;
			//LOG4CPLUS_INFO(g_stat_logger,"total request times: "<<tmp_request_total<<", total timeout response: "<<timeout_response_<<", loss ratio: "<<ratio<<", total response times with 0 peer: "<<tmp_response_total_0);
			LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		}
		else
		{
			//LOG4CPLUS_INFO(g_stat_logger,"total request times: 0, "<<"total timeout response: 0, "<<"loss ratio: 0");
			LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		}
		timeout_response_ = 0;
		//list_endpoint_request.clear();
		//list_endpoint_response_.clear();
		//list_endpoint_response_0.clear();

		tmptotal = 0;
		ratio = 0;
		for(map<boost::asio::ip::udp::endpoint,int>::iterator it = list_endpoint_response_.begin();it != list_endpoint_response_.end();++it)
		{        
			tmptotal += fabs((double)it->second);
			LOG4CPLUS_INFO(g_logger,it->first<<" times:"<<it->second);
		}
		LOG4CPLUS_INFO(g_logger,"total tracker list response count:"<<tmptotal);
		//list_endpoint_response_.clear();

		LOG4CPLUS_INFO(g_logger,"unexpected_response_:"<<unexpected_response_);
		unexpected_response_ = 0;

		LOG4CPLUS_INFO(g_logger,"use_cache_result_:"<<use_cache_result_);
		if (total_result_ != 0)
		{
			ratio = (1.0*use_cache_result_)/total_result_;
			//LOG4CPLUS_INFO(g_stat_logger,"total success result:"<<total_result_<<",use_cache_result_:"<<use_cache_result_<<",use cache ratio:"<<ratio);
		}
		else
		{
			//LOG4CPLUS_INFO(g_stat_logger, "total_result_ is 0");
		}
		use_cache_result_ = 0;
		total_result_ = 0;

		//日志记录RID的查询次数
		for(map<RID,int>::iterator it = rid_times_.begin();it != rid_times_.end();++it )
		{
			const static int REQ_TIMES = 100;
			//只打出查询次数大于REQ_TIMES次的RID
			if(it->second > REQ_TIMES)
				LOG4CPLUS_INFO(g_logger,"Rid:"<<it->first.to_string()<<" times:"<<it->second);
		}
		LOG4CPLUS_INFO(g_logger,"rid times size:"<<rid_times_.size());
		//rid_times_.clear();

	}

	void TrackeragStatistic::OutputStatisticData()
	{
		float hit_rate = 0.0;
		int request_times = 0;
		int hit_times = 0;
		std::pair< std::string, int > max_ip_pair = GetMaxQuery(query_ip_times_);
		std::pair< RID, int > max_Rid_pair = GetMaxQuery(rid_times_);
		ComputeQueryHitRate(hit_rate, request_times, hit_times);
		if (request_times)
		{
			LOG4CPLUS_INFO(g_stat_logger, "Request_times:"<< request_times 
				<<" Hit_times:" << hit_times <<" hit_rate:" << hit_rate
				<<" Max_ip:" << max_ip_pair.first <<" Times:" << max_ip_pair.second
				<<" Max RID:" << max_Rid_pair.first << " Times:" << max_Rid_pair.second );
		}
		else
		{
			LOG4CPLUS_INFO(g_stat_logger, "Request_times:"<< request_times);
		}

		std::ostringstream oss;

		for (map<int, std::pair< int, int > >::iterator it = cmd_times_v2_.begin();it != cmd_times_v2_.end();++it)
		{
			oss<<" Cmd :0x" << std::hex <<it->first<<" count:"<<std::dec<< it->second.first;
		}
		if (cmd_times_v2_.size() == 0)
		{
			oss<<"Cmd count: "<<0;
		}

		//rid_times_.clear();

		if (oss.str().length() > 0)
		{
			LOG4CPLUS_INFO(g_stat_logger,oss.str());
		}

		LOG4CPLUS_INFO(g_stat_logger, this->ReportStatistic());

		int tmp_request_total = 0;
		int tmp_response_total = 0;
		int tmp_response_total_0 = 0;
		double ratio = 0.0;
		map<boost::asio::ip::udp::endpoint,int> list_endpoint_request = TrackerRequestHandler::Instance()->ClearListEndRequest();
		for(map<boost::asio::ip::udp::endpoint,int>::iterator it = list_endpoint_request.begin();it != list_endpoint_request.end();++it)
		{
			tmp_request_total += fabs((double)it->second);
			//LOG4CPLUS_INFO(g_logger,it->first<<" times:"<<it->second);
			request_times = (double)it->second;
			//tmp_response_total += (double)list_endpoint_response_[it->first];
			tmp_response_total_0 += (double)list_endpoint_response_0[it->first];
		}
		if (tmp_request_total != 0)
		{
			ratio = (1.0*timeout_response_)/tmp_request_total;
			LOG4CPLUS_INFO(g_stat_logger,"total request times: "<<tmp_request_total<<", total timeout response: "<<timeout_response_<<", loss ratio: "<<ratio<<", total response times with 0 peer: "<<tmp_response_total_0);
			//LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		}
		else
		{
			LOG4CPLUS_INFO(g_stat_logger,"total request times: 0, "<<"total timeout response: 0, "<<"loss ratio: 0");
			//LOG4CPLUS_INFO(g_logger,"total tracker list request count:"<<tmptotal);
		}
	}

	void TrackeragStatistic::ClearStatisticData()
	{
		cmd_times_v2_.clear();
		query_ip_times_.clear();
		rid_times_.clear();

		list_count_times_.clear();
		tracker_list_count_times_.clear();
		list_response_num_.clear();
		list_endpoint_request_.clear();
		list_endpoint_response_.clear();
		list_endpoint_response_0.clear();
        list_endpoint_response_.clear();
	}

	void TrackeragStatistic::OutputSata()
	{
		OnDumpInfo();
        OutputStatisticData();
        ClearStatisticData();
	}

}
