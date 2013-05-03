#pragma once

#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <hash_map>
#include <map>
#include <WinBase.h>
#include "protocal/IndexPacket.h"
#include "framework/network/UdpServer.h"
#include "framework/timer/Timer.h"
#include "framework/timer/TimeCounter.h"
#include "protocal/IndexPacket.h"
#include "udpindex/Config.h"
#include "udpindex/DataBaseConnectionPool.h"
#include "protocal/StatisticPacket.h"
#include "base/guid.h"
#include "IndexFunc.h"

#include "list_config.h"
#include "IndexStatistic.h"
#include <Windows.h>

using namespace std;
using namespace stdext;
using namespace protocal;


namespace udpindex
{

	struct pair_hash
	{
		pair<MD5,u_int> pr_;

		pair_hash(pair<MD5,u_int> pr):pr_(pr){}

		enum{bucket_size = 1000, min_buckets = 1000};

		bool operator == (const pair_hash &ph) const
		{
			return pr_ == ph.pr_;
		}

		bool operator < (const pair_hash &ph) const
		{
			return pr_ < ph.pr_;
		}
	};

	struct ContentHashmapStruct
	{
		QueryRidByContentResponsePacket::p packet;
		LARGE_INTEGER index_;
	};

	struct UrlHashmapStruct
	{
		QueryRidByUrlResponsePacket::p packet;
		LARGE_INTEGER index_;
	};


	class GUIDHash : public hash_compare<RID>
	{
	public:
		bool operator()(const RID &val1, const RID &val2) const
		{
			return memcmp(&val1, &val2, sizeof(RID)) != 0;
		}

		size_t operator()(const RID &val) const
		{
			return boost::hash_value(val);
		}
	};

	class DataBase;
	typedef boost::shared_ptr<DataBase> DataBase__p;

	class UdpIndexModule
		: public boost::noncopyable
		, public boost::enable_shared_from_this<UdpIndexModule>
		, public framework::timer::ITimerListener
		, public framework::network::IUdpServerListener

	{
	public:
		typedef boost::shared_ptr<UdpIndexModule> p;
		static p Create() { return p(new UdpIndexModule); }
	public:
		void Start(u_short local_udp_port);
		void Stop();
		bool IsRunning() const { return is_running_; }
		//消息
		//void OnSQLQueryUrlByRid(boost::asio::ip::udp::endpoint& end_point, protocal::QueryHttpServerByRidRequestPacket::p request_packet, vector<protocal::QueryHttpServerByRidResponsePacket::p> response_packet);
		void OnSQLQueryRidByUrl(boost::asio::ip::udp::endpoint& end_point, protocal::QueryRidByUrlRequestPacket::p request_packet, protocal::QueryRidByUrlResponsePacket::p resposne_packet);
		void OnSQLQueryRidByContent(boost::asio::ip::udp::endpoint& end_point, protocal::QueryRidByContentRequestPacket::p request_packet, protocal::QueryRidByContentResponsePacket::p resposne_packet);

		void OnGetTrackerList(QueryTrackerListResponsePacket::p packet);
		void OnGetStunServerList(QueryStunServerListResponsePacket::p packet);
		void OnGetIndexServerList(QueryIndexServerListResponsePacket::p packet);
		void OnGetDBList(vector<DBInfo> db_info, vector<HostInfo> db_host, vector<boost::asio::ip::udp::endpoint> stat_list);


		void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf);

		//
		void OnQueryHttpServerByRid(boost::asio::ip::udp::endpoint& end_point, QueryHttpServerByRidRequestPacket::p packet);
		void OnQueryRidByUrl(boost::asio::ip::udp::endpoint& end_point, QueryRidByUrlRequestPacket::p packet);
		void OnQueryRidByContent(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p packet);
		void OnAddRidUrl(boost::asio::ip::udp::endpoint& end_point, AddRidUrlRequestPacket::p packet);
		void OnQueryTrackerList(boost::asio::ip::udp::endpoint& end_point, QueryTrackerListRequestPacket::p packet);
		void OnQueryStunServerList(boost::asio::ip::udp::endpoint& end_point, QueryStunServerListRequestPacket::p packet);
		void OnQueryIndexServerList(boost::asio::ip::udp::endpoint& end_point, QueryIndexServerListRequestPacket::p packet);

		//void OnVoteRemoveCacheRID(RID rid);
		void OnVoteRemoveCacheURL(string url);
		void OnVoteRemoveCacheContent(MD5 md5, u_int file_length);

		//void OnQueryHttpServerByRidNoFound(boost::asio::ip::udp::endpoint& end_point, QueryHttpServerByRidRequestPacket::p packet);
		void OnQueryRidByUrlNoFound(boost::asio::ip::udp::endpoint& end_point, QueryRidByUrlRequestPacket::p packet);
		void OnQueryRidByContentNoFound(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p packet);
		void OnQueryRidByContentClash(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p packet);

        void OnQueryRidByUrlDenial(boost::asio::ip::udp::endpoint& end_point, QueryRidByUrlRequestPacket::p packet);
        void OnQueryRidByCttDenial(boost::asio::ip::udp::endpoint& end_point, QueryRidByContentRequestPacket::p packet);
        void OnAddRidUrlDenial(boost::asio::ip::udp::endpoint& end_point, AddRidUrlRequestPacket::p packet);

		void OnPrintDatabaseInfo(map<size_t,size_t> vtct, bool is_timer);

		//
		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		//发包
		void DoSendPacket(const boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet, int peer_version);

		void DoSendBuffer(boost::asio::ip::udp::endpoint& end_point, const Buffer& buffer);

		void PrintCache(int op, string str, u_int int_value);

        void OnVisitKey(boost::asio::ip::udp::endpoint &end_point, StatisticRequestPacket::PEER_ACTION peer_action, Guid peer_guid, size_t peer_vertion, size_t tran_id, string key);
		//void Test();

		//DEBUG

 		void FinishAddUrlRid()
 		{
			if( is_running_ == false ) return;
 			//waiting_add_url_rid_no_--;
            InterlockedDecrement(&waiting_add_url_rid_no_);
 			//database_threadpool_test_++;
 			//if (database_threadpool_test_ % 100 ==0)
 				//CONSOLE_LOG("database_threadpool_test_ "<<database_threadpool_test_<<" time:"<<t.GetElapsed());
 			INDEX_EVENT("FinishAddUrlRid " << waiting_add_url_rid_no_);
 		}
 
 		void NeedToDoAddUrlRid()
 		{
			if( is_running_ == false ) return;
 			//waiting_add_url_rid_no_++;
            InterlockedIncrement(&waiting_add_url_rid_no_);
 			INDEX_EVENT("NeedToDoAddUrlRid " << waiting_add_url_rid_no_);
 		}

		void FinishQueryRidByUrl()
		{
			if( is_running_ == false ) return;
			//waiting_query_rid_by_url_no_--;
            InterlockedDecrement(&waiting_query_rid_by_url_no_);
		}

		void NeedToDoQueryRidByUrl()
		{
			if( is_running_ == false ) return;
			//waiting_query_rid_by_url_no_++;
            InterlockedIncrement(&waiting_query_rid_by_url_no_);
		}

		void FinishQueryRidByContent()
		{
			if( is_running_ == false ) return;
			//waiting_query_rid_by_content_no_--;
            InterlockedDecrement(&waiting_query_rid_by_content_no_);
		}

		void NeedToDoQueryRidByContent()
		{
			if( is_running_ == false ) return;
			//waiting_query_rid_by_content_no_++;
            InterlockedIncrement(&waiting_query_rid_by_content_no_);
		}

	private:
		framework::network::UdpServer::p udp_server_;
		bool is_running_;

		framework::timer::PeriodicTimer::p get_tracker_list_timer_;
		framework::timer::PeriodicTimer::p print_statistic_timer_;
		
		hash_map<string, UrlHashmapStruct> url_hash_map_;
		multimap<UINT64, string> url_multimap_;

		hash_map<pair_hash, ContentHashmapStruct> content_hash_map_;
		multimap<UINT64, pair_hash> content_multimap_;

		QueryTrackerListResponsePacket::p tracker_list_packet_;
		QueryStunServerListResponsePacket::p stun_list_packet_;
		QueryIndexServerListResponsePacket::p index_list_packet_;

		//size_t max_rid_cache_size_;
		size_t max_url_cache_size_;
		size_t max_content_cache_size_;

		volatile long waiting_add_url_rid_no_;
		//size_t waiting_query_url_by_rid_no_;
		volatile long waiting_query_rid_by_url_no_;
		volatile long waiting_query_rid_by_content_no_;

		size_t max_add_waiting_length_;
		size_t max_query_by_url_waiting_length_;
		size_t max_query_by_content_waiting_length_;

		size_t min_file_length_;

		size_t database_threadpool_test_;
		
		framework::timer::TimeCounter url_cache_timer_;

		bool is_send_packet_to_log_server_;

		//统计cache命中率
		INT64 url_cache_attach_times_;
		INT64 url_cache_query_times_;
		INT64 url_busy_times_;
		INT64 url_sql_faild_query_times_;
		INT64 url_sql_succed_query_times_;
		INT64 url_sql_file_too_small_times_;

		INT64 add_ridurl_times_;
		INT64 add_busy_times_;

		INT64 content_cache_attach_times_;
		INT64 content_cache_query_times_;
		INT64 content_busy_times_;
		INT64 content_sql_faild_query_times_;
		INT64 content_sql_succed_query_times_;
		INT64 content_sql_file_too_small_times_;

		IndexStatistic::p statistic_;

		boost::asio::ip::udp::endpoint statistic_endpoint_;

        // Modified by jeffrey 2011/3/15 
        // 最低支持的协议版本
        boost::uint16_t min_protocol_version_;

	private:
		UdpIndexModule() : is_running_(false){}
		static UdpIndexModule::p inst_;
	public:
		static UdpIndexModule::p Inst() { return inst_; };

	private:
		pair<string, string> FormatOutputStr(IndexStatisticInfo statistic_info);

	public:
		//void OnStatisticURL(boost::asio::ip::udp::endpoint &end_point, StatisticRequestPacket::PEER_ACTION peer_action, Guid peer_guid, size_t peer_vertion, size_t tran_id, string url);
		//void OnStatisticRID(boost::asio::ip::udp::endpoint &end_point, StatisticRequestPacket::PEER_ACTION peer_action, Guid peer_guid, size_t peer_vertion, size_t tran_id, RID rid);
		//void OnStatisticContent(boost::asio::ip::udp::endpoint &end_point, StatisticRequestPacket::PEER_ACTION peer_action, Guid peer_guid, size_t peer_vertion, size_t tran_id, MD5 content_md5);
	};

#define OutputInfo(sstr, is_timer) do\
	{\
		if (is_timer == true)\
		{\
			STATISTIC_OUTPUT(sstr);\
		}\
		CONSOLE_OUTPUT(sstr);\
	}while(false);
}


_STDEXT_BEGIN
inline size_t hash_value(const udpindex::pair_hash& p)
{
	return boost::hash_value(p.pr_.first) ^ p.pr_.second;
}
inline size_t hash_value(const base::Guid& g)
{
	return boost::hash_value(g);
}
_STDEXT_END
