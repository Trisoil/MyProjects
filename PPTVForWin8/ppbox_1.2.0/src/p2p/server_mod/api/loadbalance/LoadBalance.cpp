#include "LoadBalance.h"
#include <iostream>
#include "PPlivePubApi.h"
#include <time.h>
#include <vector>
#include <algorithm>
#include "PPlivePubApi.h"


using namespace std;
using namespace protocol;
using namespace boost;


namespace ns_pplive
{
	namespace ns_loadbalance
	{
		bool tracker_less(const pair<boost::asio::ip::udp::endpoint, SelectParam>& left, const pair<boost::asio::ip::udp::endpoint, SelectParam>& right)
		{

			return left.second.score < right.second.score;

		}
		LoadBalance::LoadBalance()
		{
			total_success_times = 0.0;
			average_time = 0.0;
			srand(time(NULL));
		};
		LoadBalance::LoadBalance(const LoadBalance& lb)
		{
			tracker_param_ = lb.tracker_param_;
			total_success_times = 0.0;
			average_time = 0.0;
			srand(time(NULL));
		}
		LoadBalance::~LoadBalance()
		{

		};
		string LoadBalance::GetLoadBalanceInfo()
		{
			vector<pair<boost::asio::ip::udp::endpoint, SelectParam> > v_load_tracker(tracker_param_.begin(), tracker_param_.end());
			sort(v_load_tracker.begin(), v_load_tracker.end(), tracker_less);
			ostringstream			select_info_;


			typedef vector<pair<boost::asio::ip::udp::endpoint, SelectParam> >::const_iterator it;
			for (it v_iter = v_load_tracker.begin(); v_iter != v_load_tracker.end(); ++v_iter)
			{
				select_info_<<v_iter->first<<" "<<v_iter->second.score<<" "<<v_iter->second.average_time<<" "<<v_iter->second.total_times<<" "<<v_iter->second.total_success_times<<"\n";		
			}
			select_info_<<average_time<<"\n";
			return select_info_.str();
		}
		bool LoadBalance::AddEndpoint(const boost::asio::ip::udp::endpoint& endpoint_)
		{
			SelectParam tmp_SelectParam;

			bool ret = tracker_param_.insert(make_pair(endpoint_, tmp_SelectParam)).second;
			return ret;
		};
		void LoadBalance::ClearEndpoint()
		{
			tracker_param_.clear();
		};
		int LoadBalance::UpdatePara(const boost::asio::ip::udp::endpoint& endpoint_, int ret, double time)
		{
			double start = ns_pplive::PubApi::GetExactTimeNow();
			SelectParam& tmp = tracker_param_[endpoint_];

			if( time<0 )
			{
				time = 0.001;
			}

			if (ret != 0)
			{
				tmp.score /= 3;
				tmp.score = tmp.score > 0.0001 ? tmp.score:0.0001;
				tmp.total_times++;
				return 1;
			}
			tmp.average_time = (tmp.average_time*tmp.total_success_times+time)/(tmp.total_success_times+1);
			++tmp.total_success_times;
			++tmp.total_times;

			average_time = (average_time*total_success_times+time)/(total_success_times+1);
			++total_success_times;

			if (tmp.average_time <= average_time)
			{
				tmp.score = 1.0;
			}
			else
			{
				tmp.score = average_time/tmp.average_time;
			}
			return 0;
		}
		double LoadBalance::GetScore(const boost::asio::ip::udp::endpoint &endpoint_) const
		{
			map<boost::asio::ip::udp::endpoint, SelectParam>::const_iterator it = tracker_param_.find(endpoint_);
			if (it != tracker_param_.end())
			{
				return it->second.score;    //todo : avoid score data aged
			}

			return 1.0;
		}
		void LoadBalance::RangeForSelect(set<boost::asio::ip::udp::endpoint>& select_endpoint, set<boost::asio::ip::udp::endpoint>& exclude_endpoint) const
		{
			typedef map<boost::asio::ip::udp::endpoint, SelectParam>::const_iterator iter;
			set<boost::asio::ip::udp::endpoint> candiate_endpoint;

			for (iter it = tracker_param_.begin(); it != tracker_param_.end(); ++it)
			{
				candiate_endpoint.insert(it->first);
			}
			set_difference(candiate_endpoint.begin(), candiate_endpoint.end(), exclude_endpoint.begin(), exclude_endpoint.end(), inserter(select_endpoint, select_endpoint.begin()));
		}
		bool LoadBalance::SelectOne(boost::asio::ip::udp::endpoint& endpoint_, set<boost::asio::ip::udp::endpoint>& select_endpoint) const
		{
			unsigned rand_num = rand();
			if (select_endpoint.size() <= 0)
			{
				return false;
			}
			boost::uint32_t i = rand_num % select_endpoint.size();
			double probablity_= (double)(rand_num%100)/100;
			set<boost::asio::ip::udp::endpoint>::const_iterator it = select_endpoint.begin();
			advance(it,i);
			endpoint_ = *it;
			double tracker_probablity_ = GetScore(endpoint_);
			if (tracker_probablity_ >= probablity_)
			{
				return true;
			}

			return false;
		}
		int LoadBalance::GetResult(boost::asio::ip::udp::endpoint& endpoint_) const
		{
			set<boost::asio::ip::udp::endpoint> select_endpoint;
			set<boost::asio::ip::udp::endpoint> exclude_endpoint;
			boost::asio::ip::udp::endpoint select_endpoint_;
			RangeForSelect(select_endpoint, exclude_endpoint);

			int i = 10;
			while (i-- > 0)
			{
				if (SelectOne(select_endpoint_, select_endpoint))
				{
					endpoint_ = select_endpoint_;
					return 0;
				}
			}

			endpoint_ = select_endpoint_;

			return -1;
		}
		int LoadBalance::GetResult(set<boost::asio::ip::udp::endpoint>& endpoint_, int neednumber) const
		{
			set<boost::asio::ip::udp::endpoint> exclude_endpoint;
			set<boost::asio::ip::udp::endpoint> select_endpoint;


			endpoint_.clear();
			RangeForSelect(select_endpoint, exclude_endpoint);

			if (select_endpoint.size() <= neednumber)
			{
				endpoint_.swap(select_endpoint);
				return -2;
			}

			while (endpoint_.size() < neednumber)
			{
				boost::asio::ip::udp::endpoint select_endpoint_;
				if (this->SelectOne(select_endpoint_, select_endpoint))
				{
					endpoint_.insert(select_endpoint_);
					select_endpoint.erase(select_endpoint_);
				}
			}


			return 0;
		}
		int LoadBalance::GetResult(set<boost::asio::ip::udp::endpoint>& endpoint_, set<boost::asio::ip::udp::endpoint>& exclude_endpoint, int neednumber)
		{
			set<boost::asio::ip::udp::endpoint> select_endpoint;

			endpoint_.clear();
			RangeForSelect(select_endpoint, exclude_endpoint);

			if (select_endpoint.size() < neednumber)
			{
				endpoint_.swap(select_endpoint);
				return -2;
			}
			boost::asio::ip::udp::endpoint select_endpoint_;
			while (endpoint_.size() < neednumber)
			{
				if (SelectOne(select_endpoint_, select_endpoint))
				{
					endpoint_.insert(select_endpoint_);
					select_endpoint.erase(select_endpoint_);
				}
			}

			return 0;
		}
		int LoadBalance::Size() const
		{
			return tracker_param_.size();
		}
		int LoadBalance::GetResult(boost::asio::ip::udp::endpoint& endpoint_, set<boost::asio::ip::udp::endpoint>& exclude_endpoint)
		{
			set<boost::asio::ip::udp::endpoint> select_endpoint;
			RangeForSelect(select_endpoint, exclude_endpoint);

			if (select_endpoint.size() < 1)
			{
				return -2;
			}

			int i = 3;
			boost::asio::ip::udp::endpoint select_endpoint_;
			while (i-- > 0)
			{
				if (SelectOne(select_endpoint_, select_endpoint))
				{
					endpoint_ = select_endpoint_;
					return 0;
				}
			}

			endpoint_ = select_endpoint_;
			return 0;
		}
	};

};
