#include "stdafx.h"
#include "udpindex/DataBaseConnectionPool.h"
#include "IndexFunc.h"
#include "udpindex/Config.h"
#include "framework/io/stdfile.h"


namespace udpindex
{
	DataBaseConnectionPool::p DataBaseConnectionPool::inst_ = DataBaseConnectionPool::p(new DataBaseConnectionPool);

	void DataBaseConnectionPool::Start()
	{
		Lock lock(&critical_section_);

		if( is_running_ == true ) return;

		is_running_ = true;
	}

	void DataBaseConnectionPool::Stop()
	{
		Lock lock(&critical_section_);

		if( is_running_ == false) return;
		is_running_ = false;
		INDEX_EVENT("DataBaseConnectionPool::Stop()");

		stop_connections_timer_ = framework::timer::PeriodicTimer::create(1000, shared_from_this());
		for (size_t i = 0; i < tokyo_tyrant_s_.size(); i++)
		{
			freeing_connection_no_.push_back(tokyo_tyrant_s_[i].size());
		}
		freeing_database_no_ = freeing_connection_no_.size();

		stop_connections_timer_->Start();
		timer_is_start_ = true;
	}

	void DataBaseConnectionPool::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
	{
		Lock lock(&critical_section_);

		assert(timer_is_start_);

		if (pointer == stop_connections_timer_)
		{
			size_t start_num = is_running_?databases_.size():0;
			for(size_t i = 0; i < tokyo_tyrant_s_.size(); i++)
			{
				while (free_connections_[i].size() > 0)
				{
					INDEX_EVENT("Stop Master Connection " << free_connections_[i].front());
					//tokyo_tyrant_s_[i][free_connections_[i].front()]->Stop();
					//Stop操作交由析构函数去做 这里不管了
					free_connections_[i].pop_front();
					freeing_connection_no_[i]--;
					if (freeing_connection_no_[i] <= 0)
					{
						freeing_database_no_--;
					}
				}
			}
			
			if (freeing_database_no_ <= 0)
			{
				INDEX_EVENT("Stop stop_connections_timer_");
				freeing_connection_no_.clear();
				stop_connections_timer_->Stop();
				timer_is_start_ = false;
			}
		}
	}

	bool DataBaseConnectionPool::RequestConnection(TokyoTyrantClient::p &mysql, size_t mod, size_t &pool_index, bool is_new)
	{
		Lock lock(&critical_section_);

		if( is_running_ == false || is_have_db_info_ == false || mod > 255) return false;

		assert(timer_is_start_ == false);

		size_t i_tmp = is_new?db_infos_[mod].new_mode:db_infos_[mod].old_mode;
		
		if (free_connections_[i_tmp].size() == 0)
		{
			INDEX_EVENT("DataBaseConnectionPool::RequestMasterDBConnection() free_connections_.size() == 0");
			TokyoTyrantClient::p mysql_p = TokyoTyrantClient::Create(databases_[i_tmp].ip_,databases_[i_tmp].port_);
			
			pool_index = tokyo_tyrant_s_[i_tmp].size();
			tokyo_tyrant_s_[i_tmp].push_back(mysql_p);
			mysql = mysql_p;
		} 
		else
		{
			pool_index = free_connections_[i_tmp].front();
			assert(pool_index < tokyo_tyrant_s_[i_tmp].size());

			INDEX_EVENT("DataBaseConnectionPool::RequestDBConnection() " <<pool_index);
			free_connections_[i_tmp].pop_front();
			if (tokyo_tyrant_s_[i_tmp][pool_index]->host_ != databases_[i_tmp] || tokyo_tyrant_s_[i_tmp][pool_index]->IsGoodConnection() == false)
			{
				//tokyo_tyrant_s_[i_tmp][pool_index]->Stop();
				//Stop操作交由析构函数去做 这里不管了
				tokyo_tyrant_s_[i_tmp][pool_index] = TokyoTyrantClient::Create(databases_[i_tmp].ip_,databases_[i_tmp].port_);
			}
			mysql = tokyo_tyrant_s_[i_tmp][pool_index];
		}
		return true;
	}

	boost::asio::ip::udp::endpoint DataBaseConnectionPool::GetStatEndPoint(size_t mod)
	{
		Lock lock(&critical_section_);
		return stat_list_[db_infos_[mod].new_mode];
	}
	
	void DataBaseConnectionPool::FreeConnection(TokyoTyrantClient::p &mysql, size_t mod, size_t pool_index, bool is_new)
	{
		Lock lock(&critical_section_);
		size_t i_tmp = is_new?db_infos_[mod].new_mode:db_infos_[mod].old_mode;

		INDEX_EVENT("DataBaseConnectionPool::FreeMasterDBConnection() " <<pool_index);
		assert(pool_index < tokyo_tyrant_s_[i_tmp].size());
		free_connections_[i_tmp].push_back(pool_index);
	}

	bool DataBaseConnectionPool::IsDBChanged(size_t mod)
	{
		Lock lock(&critical_section_);
		return is_running_ && is_have_db_info_ && mod < 256 && db_infos_[mod].if_need_trans;
	}
	size_t DataBaseConnectionPool::Mod2DBNum(size_t mod, bool is_new)
	{
		Lock lock(&critical_section_);
		return is_new?db_infos_[mod].new_mode:db_infos_[mod].old_mode;
	}

	void DataBaseConnectionPool::OnGetDBList(vector<DBInfo> db_info, vector<HostInfo> db_host, vector<boost::asio::ip::udp::endpoint> stat_list)
	{		
        Lock lock(&critical_section_);

		if(db_info.size() != 256 || db_host.size() != stat_list.size())
		{
			CONSOLE_OUTPUT("The DB_Info is Bad!!!");
			return;
		}

		databases_ = db_host;
		db_infos_ = db_info;
		stat_list_ = stat_list;

		for (size_t i = tokyo_tyrant_s_.size(); i < databases_.size(); i++)
		{
			tokyo_tyrant_s_.push_back(vector<TokyoTyrantClient::p>());
			free_connections_.push_back(deque<size_t>());
		}

		is_have_db_info_ = true;

		if (tokyo_tyrant_s_.size() > databases_.size())
		{
			stop_connections_timer_ = framework::timer::PeriodicTimer::create(1000, shared_from_this());
			for (size_t i = 0; i < tokyo_tyrant_s_.size(); i++)
			{
				freeing_connection_no_.push_back(tokyo_tyrant_s_[i].size());
			}
			freeing_database_no_ = freeing_connection_no_.size() - databases_.size();

			stop_connections_timer_->Start();
			timer_is_start_ = true;
		}
	}
}