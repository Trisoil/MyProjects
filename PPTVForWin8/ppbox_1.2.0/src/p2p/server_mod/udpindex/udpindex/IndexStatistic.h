#pragma once
#include "statistic/ByteSpeedMeter.h"
using namespace statistic;

namespace udpindex
{
	struct IndexStatisticInfo
	{
		INT64 request_count_;
		INT64 response_count_;
		INT64 request_bytes_;
		INT64 response_bytes_;
		double request_frequency_;
		double response_frequency_;
		double request_speed_;
		double response_speed_;
		double recent_minute_request_frequency_;
		double recent_minute_response_frequency_;
		double recent_minute_request_speed_;
		double recent_minute_response_speed_;
	};

	class IndexStatistic
		: public boost::noncopyable
		, public boost::enable_shared_from_this<IndexStatistic>
	{
	public:
		typedef boost::shared_ptr<IndexStatistic> p;

	public:
		static p Create();

	public:
		bool IsRunning() const {return is_running_;};
		void Start();
		void Stop();
		void Clear();

	public:
		//请求计数
		void SubmitQueryRidByUrlRequest(size_t packet_size);
		void SubmitQueryRidByContentRequest(size_t packet_size);
		void SubmitQueryIndexListRequest(size_t packet_size);
		void SubmitAddUrlRidRequest(size_t packet_size);
		void SubmitQueryTrackerListRequest(size_t packet_size);
		void SubmitQueryStunListRequest(size_t packet_size);

		//响应计数
		void SubmitQueryRidByUrlResponse(size_t packet_size);
		void SubmitQueryRidByContentResponse(size_t packet_size);
		void SubmitQueryIndexListResponse(size_t packet_size);
		void SubmitAddUrlRidResponse(size_t packet_size);
		void SubmitQueryTrackerListResponse(size_t packet_size);
		void SubmitQueryStunListResponse(size_t packet_size);

		//返回统计信息
		IndexStatisticInfo TotalInfo();
		IndexStatisticInfo QueryRidByUrlInfo();
		IndexStatisticInfo QueryRidByContentInfo();
		IndexStatisticInfo QueryIndexListInfo();
		IndexStatisticInfo AddUrlRidInfo();
		IndexStatisticInfo QueryTrackerListInfo();
		IndexStatisticInfo QueryStunListInfo();

	private:
		bool is_running_;

	private:
		IndexStatistic();

	private:

		enum CounterKind
		{
		//请求计数
			all_request_counter_,
			query_rid_by_url_counter_,
			query_rid_by_content_counter_,
			add_url_rid_counter_,
			query_tracker_list_counter_,
			query_stun_list_counter_,
			query_index_list_counter_,

		//响应计数
			all_response_counter_,
			response_rid_by_url_counter_,
			response_rid_by_content_counter_,
			response_add_url_rid_counter_,
			response_tracker_list_counter_,
			response_stun_list_counter_,
			response_index_list_counter_,

		//请求数据（带宽）
			all_request_bytes_,
			query_rid_by_url_bytes_,
			query_rid_by_content_bytes_,
			add_url_rid_bytes_,
			query_tracker_list_bytes_,
			query_stun_list_bytes_,
			query_index_list_bytes_,

		//响应数据（带宽）
			all_response_bytes_,
			response_rid_by_url_bytes_,
			response_rid_by_content_bytes_,
			response_add_url_rid_bytes_,
			response_tracker_list_bytes_,
			response_stun_list_bytes_,
			response_index_list_bytes_,

		//个数
			counter_num_
		};

		vector<ByteSpeedMeter::p> statistics_;

	};
}