//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "HostManager.h"
#include "Statistics.h"


namespace hou
{
    boost::mutex HostManager::map_mutex_;

    Host::Host(const std::vector<std::string> & hosts)
        :hosts_(hosts)
    {
        time_counter_.reset(new framework::timer::TimeCounter());
    }

    
    HostManager::HostManager(boost::shared_ptr<boost::asio::io_service> io_service)
        :timer_(boost::ref(*io_service)), clear_timer_(boost::ref(*io_service)), io_service_(io_service)
    {

    }


    void HostManager::Ontimer()
    {
        {
            boost::mutex::scoped_lock lock(map_mutex_);
            for (std::map<std::string, boost::shared_ptr<Host> >::iterator iter = host_map_.begin(); 
                iter != host_map_.end(); iter++)
            {
                //���ϴν���ʧ�ܵ����������ٴγ��Խ��
                if (!(iter->second) ||
                    (iter->second)->GetTimeSinceLastResolve() > (1000 * 60 * 60 * update_resolve_result_in_hours) )
                {
                    ResolveHostName(iter->first);
                }
            }
        }

        timer_.expires_from_now(boost::posix_time::minutes(check_interval_in_minites_));
        timer_.async_wait(boost::bind(&hou::HostManager::Ontimer, shared_from_this()));
    }

    void HostManager::StartTimer()
    {
        timer_.expires_from_now(boost::posix_time::minutes(check_interval_in_minites_));
        timer_.async_wait(boost::bind(&hou::HostManager::Ontimer, shared_from_this()));
    }

	void HostManager::Oncleartimer()
	{
		//LOG4CPLUS_INFO(Loggers::HouService(), "before clear,host_check_times size: "<<host_check_times_.size());
		host_check_times_.clear();
		//LOG4CPLUS_INFO(Loggers::HouService(), "after clear,host_check_times size: "<<host_check_times_.size());
		clear_timer_.expires_from_now(boost::posix_time::hours(update_resolve_result_in_hours));
        clear_timer_.async_wait(boost::bind(&hou::HostManager::Oncleartimer, shared_from_this()));
	}

	void HostManager::StartClearTimer()
	{
		clear_timer_.expires_from_now(boost::posix_time::hours(update_resolve_result_in_hours));
		clear_timer_.async_wait(boost::bind(&hou::HostManager::Oncleartimer, shared_from_this()));
	}

	std::string HostManager::GetHostByName(const std::string & host_name)
	{
		boost::mutex::scoped_lock lock(map_mutex_);

		if (host_map_ .find(host_name) == host_map_.end())
		{
			ResolveHostName(host_name);
		}

		if (host_map_[host_name])
		{
			return host_map_[host_name]->GetHost();
		}

		return std::string();
	}

	bool HostManager::IsHostNameValid(const std::string & host_name)
	{
		int len = host_name.size();
		for (int i = 0; i < len; ++i)
		{
			if ((host_name[i]>='A' && host_name[i] <= 'Z') || (host_name[i]>='a' && host_name[i] <= 'z') || (host_name[i]>='0' && host_name[i] <= '9') || host_name[i] == '.' || host_name[i] == '-')
			{
				continue;
			}
			else 
			{
				LOG4CPLUS_ERROR(Loggers::HouService(),"host name error,PLS CHECK");
				return false;
			}

		}

		return true;
	}

    void HostManager::ResolveHostName(std::string host_name)
    {   
		host_check_times_[host_name]++;
		if (host_check_times_[host_name] > 4)
		{
			return;
		}
        std::cout << host_name << std::endl;
        std::vector<std::string> hosts;
        boost::asio::ip::udp::resolver resolver(boost::ref(*io_service_));
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host_name, "80");
        boost::system::error_code ec;
        boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query, ec);

        
        if (ec)
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), " ResolveHostName : " << host_name << " return an error. err_message: " << ec.message());
            LOG4CPLUS_ERROR(Loggers::Operations(), " ResolveHostName : " << host_name << " return an error. err_message: " << ec.message());
            host_map_[host_name] = boost::shared_ptr<Host>();
            return;
        }
        
        boost::asio::ip::udp::resolver::iterator end;
        for (; iter != end; iter++)
        {
            boost::asio::ip::udp::endpoint end_point(*iter);
            hosts.push_back(end_point.address().to_string());
            std::cout << end_point.address().to_string() << std::endl;
        }
        

        if (host_map_.find(host_name) == host_map_.end())
        {
            LOG4CPLUS_WARN(Loggers::HouService(), "New host: " << host_name );
        }

        boost::shared_ptr<Host> host(new Host(hosts));
        host_map_[host_name] = host;
    }
	}
