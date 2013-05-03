#ifndef UDPTRACKER_TRACKERAGSTATISTIC_H
#define UDPTRACKER_TRACKERAGSTATISTIC_H

#include <utility>
#include "ByteSpeedMeter.h"
#include "Common.h"
#include <boost/date_time/posix_time/ptime.hpp>

#if defined(_MSC_VER) 
#include <hash_map> 
using stdext::hash_map; 
#else 
#include <ext/hash_map> 
using __gnu_cxx::hash_map; 
#endif 

namespace udptrackerag
{
    namespace StatisticType
    {
        //////////////////////////////////////////////////////////////////////////
        // Tracker Statistic Type
        enum TrackeragStatisticType
        {
            // Request
            REQUEST_TRACKER, 
            CLIENT_REQUEST, 
            // Response
            TRACKER_RESPONSE, 
            RESPONSE_CLIENT,
            // Request Bytes
            REQUEST_TRACKER_BYTES, 
            CLIENT_REQUEST_BYTES, 
            // Response Bytes
            TRACKER_RESPONSE_BYTES, 
            //LEAVE_RESPONSE_BYTES,
            RESPONSE_CLIENT_BYTES, 
            // size
            ENUM_SIZE
        };

        static const char* TrackeragStatisticTypeName[] =
        {
            // Request
            "REQUEST_TRACKER        ",
            "CLIENT_REQUEST         ",
            // Response
            "TRACKER_RESPONSE       ",
            "RESPONSE_CLIENT        ",
            // Request Bytes
            "REQUEST_TRACKER_BYTES  ",
            "CLIENT_REQUEST_BYTES   ",
            // Response Bytes
            "TRACKER_RESPONSE_BYTES ",
            "RESPONSE_CLIENT_BYTES  ",
            
            // No Such
            "                       ",
        };

        inline const char* GetName(TrackeragStatisticType type = ENUM_SIZE)
        {
            if (type >= 0 && type < ENUM_SIZE)
            {
                return TrackeragStatisticTypeName[type];
            }
            return TrackeragStatisticTypeName[ENUM_SIZE];
        }
    }

    struct StatisticInfo
    {
		boost::uint64_t TotalCount;
        double AverageSpeed;
        double CurrentSpeed; // 5s
        double RecentMinuteSpeed;

        std::string ToString() const;
    };




    struct AllStatisticInfo
    {
        StatisticInfo Info[StatisticType::ENUM_SIZE];

        std::string ToString() const;
    };

    struct RidStat {
        boost::uint32_t rid_count;
        boost::uint64_t total_peer_count;

        RidStat():rid_count(0), total_peer_count(0) {}
    };

#ifndef _MSC_VER
    struct RidHash {
        size_t operator() (const RID& rid) const
        {
            return rid.hash_value();
        }
    };
#endif

    //////////////////////////////////////////////////////////////////////////
    // Tracker Statistic

    class TrackeragStatistic
        : public boost::noncopyable
        , public boost::enable_shared_from_this<TrackeragStatistic>
    {
    public:

        typedef boost::shared_ptr<TrackeragStatistic> p;

    public:

        //static TrackerStatistic::p Create();
        static TrackeragStatistic::p Inst();

    public:

        void Start(size_t res_hash_map_size);
        void Stop();
        bool IsRunning() const { return is_running_; }
       // void ClearNatStatistic();
        void RestartStatistic();

    protected:

        void Submit(StatisticType::TrackeragStatisticType type, unsigned packet_size);

    public:

        StatisticInfo QueryStatisticInfo(StatisticType::TrackeragStatisticType type);
        AllStatisticInfo QueryAllStatisticInfo();
        unsigned GetElapsedTimeInMilliSeconds();

		std::string ReportStatistic();

    public:

        // 请求计数
        void SubmitClientRequest(unsigned packet_size);
		void SubmitTrackerRequest(unsigned packet_size, int send_num);

        // 响应计数
        void SubmitListResponse(unsigned packet_size);
        void SubmitCommitResponse(unsigned packet_size);

	private:

       TrackeragStatistic();

       static TrackeragStatistic::p inst_;

	private: //new added 
		//统计ag返回的list的各种次数结果
		map<int,int> list_count_times_;

		//tracker返回的list的各种次数结果
		map<int,int> tracker_list_count_times_;

		//统计每个tracker返回的次数
		map<boost::asio::ip::udp::endpoint,int> list_endpoint_request_;
		map<boost::asio::ip::udp::endpoint,int> list_endpoint_response_;
		//统计返回peer数为0的tracker数
		map<boost::asio::ip::udp::endpoint,int> list_endpoint_response_0;


		//记录有rid收到多少个tracker回复的次数
		map<int,int> list_response_num_;

		//不期望收到的回包，很可能是超时之后的回包。
		int unexpected_response_;
		int timeout_response_;

		//超时返回给用户的时候，如果没有任何数据，就用cache里的数据。
		int use_cache_result_;
		//总的返回客户次数
		int total_result_;

    private:

        bool is_running_;

        //////////////////////////////////////////////////////////////////////////
        // 统计信息

		measure::ByteSpeedMeter::p meters_[StatisticType::ENUM_SIZE];

        ////////////////////////////////////////////////////////////////////////
        //统计信息
    private:
        //统计每种command的查询次数和命中次数，目的是计算一定时间内每种command的命中率
        std::map<int, std::pair< int, int > > cmd_times_v2_;

        //统计查询请求每个ip出现的次数，目的是计算一定时间内最大查询次数的ip
        std::map<std::string, int> query_ip_times_;

        //统计RID查询的次数
        std::map<RID,int> rid_times_;

        //map<int,int> peerversion_times_;

        //统计网络类型
        //map<int,int> nattype_times_;

        //计算report命令里上报的资源的数量情况
        //由于同时有add和del资源，所以key表示为 del_num*1000+add (这里假定了add不会大于100)
        //map<int,int> report_num_times_;

        int rid_count_;
        int peer_count_;

    public:
        void AddCmdTimes(int cmd);
        void AddHitCmdTimes(int cmd);
        void AddIpTimes(std::string ip);
        void AddRidTimes(RID rid);

		void AddListCountTimes(int peer_count);
		void AddTrackerListCountTimes(int peer_count);
		void AddListEndpointResponseTimes(boost::asio::ip::udp::endpoint tracker_end_point);
        void AddListEndpointResponse0Times(boost::asio::ip::udp::endpoint tracker_end_point);
		void AddListResponseNum(int response_count);
		void AddUnexpectedResponse();
		void AddUseCacheResult();
		void AddTotalResult();
		void AddTotalFailResult(int no_response_count);

		//void OnDumpInfo();
        void OutputSata();
        void ComputeQueryHitRate(float& hit_rate, int& reqest_times, int& hit_times);

        template<class T>
        std::pair< T, int > GetMaxQuery(std::map<T, int> map)
        {
            typename std::pair< T, int > max_pair = make_pair(T(), 0);
            for (typename std::map<T, int>::iterator it = map.begin(); it != map.end(); ++it)
            {
                if (max_pair.second < it->second)
                {
                    max_pair = *it;
                }
            }

            return max_pair;
        }

        //void OutputStatisticData();
		private:
        void ClearStatisticData();

		void OnDumpInfo();                  //写到trackerag日志里面
		void OutputStatisticData();         //写到trackerag_stat日志里面

    public:

#ifndef _MSC_VER
        boost::shared_ptr< hash_map<RID, RidStat, RidHash> > p_rid_peer_hash_map_;
#endif
    };

}

#endif//UDPTRACKER_TRACKERAGSTATISTIC_H
