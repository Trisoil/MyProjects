#pragma once

#include "base/base.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "framework/timer/Timer.h"
#include "framework/timer/TimeCounter.h"
#include "udpindex/TokyoTyrantClient.h"
#include "udpindex/Lock.h"
#include "udpindex/list_config.h"
using namespace TokyoTyrant;

namespace udpindex
{
	class DataBaseConnectionPool
		: public boost::noncopyable
		, public framework::timer::ITimerListener
		, public boost::enable_shared_from_this<DataBaseConnectionPool>
	{
	public:
		typedef boost::shared_ptr<DataBaseConnectionPool> p;
		//static p create() { return p(new DataBaseConnectionPool()); }
	public:
		// ÆôÍ£
		void Start();
		void Stop();

		static DataBaseConnectionPool& Inst() { return *inst_; }

		// ²Ù×÷
		
		bool RequestConnection(TokyoTyrantClient::p &mysql, size_t mod, size_t &pool_index, bool is_new);
		void FreeConnection(TokyoTyrantClient::p &mysql, size_t mod, size_t pool_index, bool is_new);
		boost::asio::ip::udp::endpoint GetStatEndPoint(size_t mod);

		bool IsDBChanged(size_t mod);
		void OnGetDBList(vector<DBInfo> db_info, vector<HostInfo> db_host, vector<boost::asio::ip::udp::endpoint> stat_list);

		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		size_t Mod2DBNum(size_t mod, bool is_new);
	private:
		bool is_running_;
		bool is_have_db_info_;

		bool timer_is_start_;

		vector<size_t> freeing_connection_no_;
		size_t freeing_database_no_;

		vector<vector<TokyoTyrantClient::p>> tokyo_tyrant_s_;
		vector<deque<size_t>> free_connections_;
		
		vector<HostInfo> databases_;
		vector<DBInfo> db_infos_;
		vector<boost::asio::ip::udp::endpoint> stat_list_;

		CriticalSection critical_section_;

		framework::timer::PeriodicTimer::p stop_connections_timer_;
		//framework::timer::TimeCounter t;
		static p inst_;

	private:
		DataBaseConnectionPool() 
			: is_running_(false), timer_is_start_(false), is_have_db_info_(false){}
		bool GetDataBaseInfo();
	};
}