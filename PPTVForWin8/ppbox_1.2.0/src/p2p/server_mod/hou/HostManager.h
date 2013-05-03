//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

namespace hou
{
    class Host
    {
    private:
        std::vector<std::string> hosts_;
        boost::uint16_t host_index_;
        boost::shared_ptr<framework::timer::TimeCounter> time_counter_;

    public:
        Host(const std::vector<std::string> & hosts);
         
        
        std::vector<std::string> GetHosts() const
        {
            return hosts_;
        }

        std::string GetHost() 
        {
            if (hosts_.empty())
            {
                return std::string();
            }

            host_index_ = host_index_ % hosts_.size();
            return hosts_[host_index_++];
        }
        
        

        boost::uint32_t GetTimeSinceLastResolve() const
        {
            return time_counter_->elapse();
        }

    };

    class HostManager
        :public boost::enable_shared_from_this<HostManager>
    {
    public:
        HostManager(boost::shared_ptr<boost::asio::io_service> io_service);
        std::string GetHostByName(const std::string & host_name);
		bool IsHostNameValid(const std::string & host_name);
        void ResolveHostName(std::string host_name);
        void StartTimer();
		void StartClearTimer();
        const std::map<std::string, boost::shared_ptr<Host> > & GetHostMap()
        {
            return host_map_;
        }

        
    private:
        void Ontimer();
		void Oncleartimer();

    private:
        std::map<std::string, boost::shared_ptr<Host> > host_map_;
		std::map<std::string, int> host_check_times_;
        boost::asio::deadline_timer timer_;
		boost::asio::deadline_timer clear_timer_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        const static boost::uint32_t check_interval_in_minites_ = 5;
        const static boost::uint32_t update_resolve_result_in_hours = 1;

    public:
        static boost::mutex map_mutex_;
    };
}