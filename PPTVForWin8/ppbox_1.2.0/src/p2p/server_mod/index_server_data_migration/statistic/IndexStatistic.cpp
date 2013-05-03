#include "StdAfx.h"
#include "IndexStatistic.h"

namespace udpindex
{
	IndexStatistic::p IndexStatistic::Create()		
	{
		return IndexStatistic::p(new IndexStatistic());
	}

	IndexStatistic::IndexStatistic(void)
		:is_running_(false)
	{
		for (unsigned char i = 0; i < counter_num_; i++)
		{
			statistics_.push_back(ByteSpeedMeter::Create());
		}
	}

	void IndexStatistic::Start()
	{
		if (is_running_ == true)
		{
			return;
		}

		for (unsigned char i = 0; i < statistics_.size(); i++)
		{
			statistics_[i]->Start();
		}

		is_running_ = true;
	}

	void IndexStatistic::Stop()
	{
		if (is_running_ == false)
		{
			return;
		}
		is_running_ = false;

		for (unsigned char i = 0; i < statistics_.size(); i++)
		{
			statistics_[i]->Stop();
		}

	}

	void IndexStatistic::Clear()
	{
		if (is_running_ == false)
		{
			return;
		}

		for (unsigned char i = 0; i < statistics_.size(); i++)
		{
			statistics_[i]->Clear();
		}
	}

	//请求计数
	void IndexStatistic::SubmitQueryRidByUrlRequest(size_t packet_size)
	{
		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
		statistics_[query_rid_by_url_counter_]->SubmitBytes(1);
		statistics_[query_rid_by_url_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitQueryRidByContentRequest(size_t packet_size)
	{
		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
		statistics_[query_rid_by_content_counter_]->SubmitBytes(1);
		statistics_[query_rid_by_content_bytes_]->SubmitBytes(packet_size);
	}
 	void IndexStatistic::SubmitQueryIndexListRequest(size_t packet_size)
 	{
 		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
 		statistics_[query_index_list_counter_]->SubmitBytes(1);
 		statistics_[query_index_list_bytes_]->SubmitBytes(packet_size);
 	}
	void IndexStatistic::SubmitAddUrlRidRequest(size_t packet_size)
	{
		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
		statistics_[add_url_rid_counter_]->SubmitBytes(1);
		statistics_[add_url_rid_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitQueryTrackerListRequest(size_t packet_size)
	{
		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
		statistics_[query_tracker_list_counter_]->SubmitBytes(1);
		statistics_[query_tracker_list_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitQueryStunListRequest(size_t packet_size)
	{
		statistics_[all_request_counter_]->SubmitBytes(1);
		statistics_[all_request_bytes_]->SubmitBytes(packet_size);
		statistics_[query_stun_list_counter_]->SubmitBytes(1);
		statistics_[query_stun_list_bytes_]->SubmitBytes(packet_size);
	}

	//响应计数
	void IndexStatistic::SubmitQueryRidByUrlResponse(size_t packet_size)
	{
		statistics_[all_response_counter_]->SubmitBytes(1);
		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
		statistics_[response_rid_by_url_counter_]->SubmitBytes(1);
		statistics_[response_rid_by_url_bytes_]->SubmitBytes(packet_size);
	}
 	void IndexStatistic::SubmitQueryIndexListResponse(size_t packet_size)
 	{
 		statistics_[all_response_counter_]->SubmitBytes(1);
 		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
 		statistics_[response_index_list_counter_]->SubmitBytes(1);
 		statistics_[response_index_list_bytes_]->SubmitBytes(packet_size);
 	}
	void IndexStatistic::SubmitQueryRidByContentResponse(size_t packet_size)
	{
		statistics_[all_response_counter_]->SubmitBytes(1);
		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
		statistics_[response_rid_by_content_counter_]->SubmitBytes(1);
		statistics_[response_rid_by_content_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitAddUrlRidResponse(size_t packet_size)
	{
		statistics_[all_response_counter_]->SubmitBytes(1);
		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
		statistics_[response_add_url_rid_counter_]->SubmitBytes(1);
		statistics_[response_add_url_rid_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitQueryTrackerListResponse(size_t packet_size)
	{
		statistics_[all_response_counter_]->SubmitBytes(1);
		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
		statistics_[response_tracker_list_counter_]->SubmitBytes(1);
		statistics_[response_tracker_list_bytes_]->SubmitBytes(packet_size);
	}
	void IndexStatistic::SubmitQueryStunListResponse(size_t packet_size)
	{
		statistics_[all_response_counter_]->SubmitBytes(1);
		statistics_[all_response_bytes_]->SubmitBytes(packet_size);
		statistics_[response_stun_list_counter_]->SubmitBytes(1);
		statistics_[response_stun_list_bytes_]->SubmitBytes(packet_size);
	}

	//返回统计信息
	IndexStatisticInfo IndexStatistic::TotalInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[all_request_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[all_response_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[all_request_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[all_response_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[all_request_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[all_response_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[all_request_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[all_response_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[all_request_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[all_response_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[all_request_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[all_response_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}
	IndexStatisticInfo IndexStatistic::QueryRidByUrlInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[query_rid_by_url_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[response_rid_by_url_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[query_rid_by_url_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[response_rid_by_url_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[query_rid_by_url_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[response_rid_by_url_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[query_rid_by_url_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[response_rid_by_url_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[query_rid_by_url_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[response_rid_by_url_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[query_rid_by_url_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[response_rid_by_url_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}
	IndexStatisticInfo IndexStatistic::QueryRidByContentInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[query_rid_by_content_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[response_rid_by_content_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[query_rid_by_content_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[response_rid_by_content_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[query_rid_by_content_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[response_rid_by_content_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[query_rid_by_content_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[response_rid_by_content_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[query_rid_by_content_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[response_rid_by_content_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[query_rid_by_content_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[response_rid_by_content_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}
 	IndexStatisticInfo IndexStatistic::QueryIndexListInfo()
 	{
 		IndexStatisticInfo statistic_info;
 		statistic_info.request_count_ = statistics_[query_index_list_counter_]->TotalBytes();
 		statistic_info.response_count_ = statistics_[response_index_list_counter_]->TotalBytes();
 		statistic_info.request_frequency_ = statistics_[query_index_list_counter_]->AverageByteSpeed();
 		statistic_info.response_frequency_ = statistics_[response_index_list_counter_]->AverageByteSpeed();
 		statistic_info.request_bytes_ = statistics_[query_index_list_bytes_]->TotalBytes();
 		statistic_info.response_bytes_ = statistics_[response_index_list_bytes_]->TotalBytes();
 		statistic_info.request_speed_ = statistics_[query_index_list_bytes_]->AverageByteSpeed();
 		statistic_info.response_speed_ = statistics_[response_index_list_bytes_]->AverageByteSpeed();
 		statistic_info.recent_minute_request_frequency_ = statistics_[query_index_list_counter_]->RecentMinuteByteSpeed();
 		statistic_info.recent_minute_response_frequency_ = statistics_[response_index_list_counter_]->RecentMinuteByteSpeed();
 		statistic_info.recent_minute_request_speed_ = statistics_[query_index_list_bytes_]->RecentMinuteByteSpeed();
 		statistic_info.recent_minute_response_speed_ = statistics_[response_index_list_bytes_]->RecentMinuteByteSpeed();
 		return statistic_info;
 	}
	IndexStatisticInfo IndexStatistic::AddUrlRidInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[add_url_rid_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[response_add_url_rid_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[add_url_rid_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[response_add_url_rid_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[add_url_rid_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[response_add_url_rid_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[add_url_rid_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[response_add_url_rid_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[add_url_rid_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[response_add_url_rid_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[add_url_rid_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[response_add_url_rid_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}
	IndexStatisticInfo IndexStatistic::QueryTrackerListInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[query_tracker_list_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[response_tracker_list_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[query_tracker_list_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[response_tracker_list_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[query_tracker_list_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[response_tracker_list_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[query_tracker_list_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[response_tracker_list_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[query_tracker_list_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[response_tracker_list_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[query_tracker_list_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[response_tracker_list_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}
	IndexStatisticInfo IndexStatistic::QueryStunListInfo()
	{
		IndexStatisticInfo statistic_info;
		statistic_info.request_count_ = statistics_[query_stun_list_counter_]->TotalBytes();
		statistic_info.response_count_ = statistics_[response_stun_list_counter_]->TotalBytes();
		statistic_info.request_frequency_ = statistics_[query_stun_list_counter_]->AverageByteSpeed();
		statistic_info.response_frequency_ = statistics_[response_stun_list_counter_]->AverageByteSpeed();
		statistic_info.request_bytes_ = statistics_[query_stun_list_bytes_]->TotalBytes();
		statistic_info.response_bytes_ = statistics_[response_stun_list_bytes_]->TotalBytes();
		statistic_info.request_speed_ = statistics_[query_stun_list_bytes_]->AverageByteSpeed();
		statistic_info.response_speed_ = statistics_[response_stun_list_bytes_]->AverageByteSpeed();
		statistic_info.recent_minute_request_frequency_ = statistics_[query_stun_list_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_frequency_ = statistics_[response_stun_list_counter_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_request_speed_ = statistics_[query_stun_list_bytes_]->RecentMinuteByteSpeed();
		statistic_info.recent_minute_response_speed_ = statistics_[response_stun_list_bytes_]->RecentMinuteByteSpeed();
		return statistic_info;
	}

}
