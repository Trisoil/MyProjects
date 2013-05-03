#ifndef UDPTRACKER_TRACKERSTATISTIC_H
#define UDPTRACKER_TRACKERSTATISTIC_H

#include <utility>
#include "measure/ByteSpeedMeter.h"
#include "Common.h"
#include <boost/date_time/posix_time/ptime.hpp>

#if defined(_MSC_VER) 
#include <hash_map> 
using stdext::hash_map; 
#else 
#include <ext/hash_map> 
using __gnu_cxx::hash_map; 
#endif 

namespace udptracker
{
    namespace StatisticType
    {
        //////////////////////////////////////////////////////////////////////////
        // Tracker Statistic Type

        enum TrackerStatisticType
        {
            // Request
            ALL_REQUEST,
            REPORT_REQUEST,
            LIST_REQUEST,
            LEAVE_REQUEST,
            COMMIT_REQUEST,
            KEEP_ALIVE_REQUEST,
            // Response
            ALL_RESPONSE,
            REPORT_RESPONSE,
            LIST_RESPONSE,
            LEAVE_RESPONSE,
            COMMIT_RESPONSE,
            KEEP_ALIVE_RESPONSE,
            // Request Bytes
            ALL_REQUEST_BYTES,
            REPORT_REQUEST_BYTES,
            LIST_REQUEST_BYTES,
            LEAVE_REQUEST_BYTES,
            COMMIT_REQUEST_BYTES,
            KEEP_ALIVE_REQUEST_BYTES,
            // Response Bytes
            ALL_RESPONSE_BYTES,
            REPORT_RESPONSE_BYTES,
            LIST_RESPONSE_BYTES,
            LEAVE_RESPONSE_BYTES,
            COMMIT_RESPONSE_BYTES,
            KEEP_ALIVE_RESPONSE_BYTES,

            // Request Error
            ALL_REQUEST_ERROR,
            REPORT_REQUEST_ERROR,
            LIST_REQUEST_ERROR,
            LEAVE_REQUEST_ERROR,
            COMMIT_REQUEST_ERROR,
            KEEP_ALIVE_REQUEST_ERROR,
            // Response Error
            All_RESPONSE_ERROR,
            REPORT_RESPONSE_ERROR,
            LIST_RESPONSE_ERROR,
            LEAVE_RESPONSE_ERROR,
            COMMIT_RESPONSE_ERROR,
            KEEP_ALIVE_RESPONSE_ERROR,

            // Report Request Non match
            REPORT_REQUEST_NON_MATCH,

            // size
            ENUM_SIZE
        };

        static const char* TrackerStatisticTypeName[] =
        {
            // Request
            "ALL_REQUEST              ",
            "REPORT_REQUEST           ",
            "LIST_REQUEST             ",
            "LEAVE_REQUEST            ",
            "COMMIT_REQUEST           ",
            "KEEP_ALIVE_REQUEST       ",
            // Response
            "ALL_RESPONSE             ",
            "REPORT_RESPONSE          ",
            "LIST_RESPONSE            ",
            "LEAVE_RESPONSE           ",
            "COMMIT_RESPONSE          ",
            "KEEP_ALIVE_RESPONSE      ",
            // Request Bytes
            "ALL_REQUEST_BYTES        ",
            "REPORT_REQUEST_BYTES     ",
            "LIST_REQUEST_BYTES       ",
            "LEAVE_REQUEST_BYTES      ",
            "COMMIT_REQUEST_BYTES     ",
            "KEEP_ALIVE_REQUEST_BYTES ",
            // Response Bytes
            "ALL_RESPONSE_BYTES       ",
            "REPORT_RESPONSE_BYTES    ",
            "LIST_RESPONSE_BYTES      ",
            "LEAVE_RESPONSE_BYTES     ",
            "COMMIT_RESPONSE_BYTES    ",
            "KEEP_ALIVE_RESPONSE_BYTES",

            // Request Error
            "ALL_REQUEST_ERROR        ",
            "REPORT_REQUEST_ERROR     ",
            "LIST_REQUEST_ERROR       ",
            "LEAVE_REQUEST_ERROR      ",
            "COMMIT_REQUEST_ERROR     ",
            "KEEP_ALIVE_REQUEST_ERROR ",
            // Response Error
            "All_RESPONSE_ERROR       ",
            "REPORT_RESPONSE_ERROR    ",
            "LIST_RESPONSE_ERROR      ",
            "LEAVE_RESPONSE_ERROR     ",
            "COMMIT_RESPONSE_ERROR    ",
            "KEEP_ALIVE_RESPONSE_ERROR",

            // report request non-match
            "REPORT_REQUEST_NON_MATCH ",
            
            // No Such
            "                         ",
        };

        inline const char* GetName(TrackerStatisticType type = ENUM_SIZE)
        {
            if (type >= 0 && type < ENUM_SIZE)
            {
                return TrackerStatisticTypeName[type];
            }
            return TrackerStatisticTypeName[ENUM_SIZE];
        }

        //////////////////////////////////////////////////////////////////////////
        // NAT Statistic Type

        enum NatStatisticType
        {
            NAT_ERROR,
            NAT_FULLCONENAT,
            NAT_IP_RESTRICTEDNAT,
            NAT_IP_PORT_RESTRICTEDNAT,
            NAT_SYMNAT,
            NAT_PUBLIC,
            // size
            NAT_ENUM_SIZE,
        };

        static const char *NatStatisticTypeName[] = 
        {
            "NAT_ERROR                ",
            "NAT_FULLCONENAT          ",
            "NAT_IP_RESTRICTEDNAT     ",
            "NAT_IP_PORT_RESTRICTEDNAT",
            "NAT_SYMNAT               ",
            "NAT_PUBLIC               ",
            // No Such
            "                         ",
        };

        inline const char* GetNatTypeName(NatStatisticType type = NAT_ENUM_SIZE)
        {
            if (type >= 0 && type < NAT_ENUM_SIZE)
            {
                return NatStatisticTypeName[type];
            }
            return NatStatisticTypeName[NAT_ENUM_SIZE];
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

    struct NatStatisticInfo
    {
        unsigned NatTypePeerCount[StatisticType::NAT_ENUM_SIZE];

        std::string ToString() const;
    };

    struct IdleTimeInfo
    {
        unsigned IdleTimeCount[256];

        IdleTimeInfo() {
            Clear();
        }
        void Clear() {
            memset(this, 0, sizeof(IdleTimeInfo));
        }
        std::string ToString() const;
    };

    struct AllStatisticInfo
    {
        StatisticInfo Info[StatisticType::ENUM_SIZE];
        NatStatisticInfo NatTypeInfo;
        IdleTimeInfo IdleInfo;

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

    class TrackerStatistic
        : public boost::noncopyable
        , public boost::enable_shared_from_this<TrackerStatistic>
    {
    public:

        typedef boost::shared_ptr<TrackerStatistic> p;

    public:

        //static TrackerStatistic::p Create();
        static TrackerStatistic::p Inst();

    public:

        void Start(size_t res_hash_map_size);
        void Stop();
        bool IsRunning() const { return is_running_; }
        void ClearNatStatistic();
        void RestartStatistic();

    protected:

        void Submit(StatisticType::TrackerStatisticType type, unsigned packet_size);

    public:

        StatisticInfo QueryStatisticInfo(StatisticType::TrackerStatisticType type);
        AllStatisticInfo QueryAllStatisticInfo();
        NatStatisticInfo QueryNatStatisticInfo();
        IdleTimeInfo QueryIdleTimeInfo();
        unsigned GetElapsedTimeInMilliSeconds();

		std::string ReportStatistic();

    public:

        // 请求计数
        void SubmitListRequest(unsigned packet_size);
        void SubmitReportRequest(unsigned packet_size);
        void SubmitLeaveRequest(unsigned packet_size);
        void SubmitCommitRequest(unsigned packet_size);
        void SubmitKeepAliveRequest(unsigned packet_size);
        // 响应计数
        void SubmitListResponse(unsigned packet_size);
        void SubmitReportResponse(unsigned packet_size);
        void SubmitLeaveResponse(unsigned packet_size);
        void SubmitCommitResponse(unsigned packet_size);
        void SubmitKeepAliveResponse(unsigned packet_size);

        // ignore error bytes
        // 出错请求计数
        void SubmitRequestError(unsigned packet_size);
        void SubmitListRequestError(unsigned packet_size);
        void SubmitReportRequestError(unsigned packet_size);
        void SubmitLeaveRequestError(unsigned packet_size);
        void SubmitCommitRequestError(unsigned packet_size);
        void SubmitKeepAliveRequestError(unsigned packet_size);
        // 出错响应计数
        void SubmitResponseError(unsigned packet_size);
        void SubmitListResponseError(unsigned packet_size);
        void SubmitReportResponseError(unsigned packet_size);
        void SubmitLeaveResponseError(unsigned packet_size);
        void SubmitCommitResponseError(unsigned packet_size);
        void SubmitKeepAliveResponseError(unsigned packet_size);

        void SubmitReportNonMatch(unsigned packet_size);

        // NAT类型统计
        void IncPeerNatTypeCount(StatisticType::NatStatisticType type, unsigned count);
        void DecPeerNatTypeCount(StatisticType::NatStatisticType type, unsigned count);
        void IncPeerNatType(StatisticType::NatStatisticType type) { ++nat_type_info_.NatTypePeerCount[type]; }
        void DecPeerNatType(StatisticType::NatStatisticType type) { if (nat_type_info_.NatTypePeerCount[type] > 0) --nat_type_info_.NatTypePeerCount[type]; }

        // IDLE Time 统计
        void IncPeerIdleTimeCount(unsigned idle_time, unsigned count);
        void DecPeerIdleTimeCount(unsigned idle_time, unsigned count);
        void IncPeerIdleTime(unsigned idle_time);
        void DecPeerIdleTime(unsigned idle_time);

	private:

       TrackerStatistic();

       static TrackerStatistic::p inst_;

    private:

        bool is_running_;

        //////////////////////////////////////////////////////////////////////////
        // 统计信息

		measure::ByteSpeedMeter::p meters_[StatisticType::ENUM_SIZE];

        NatStatisticInfo nat_type_info_;

        IdleTimeInfo idle_time_info_;

        ////////////////////////////////////////////////////////////////////////
        //统计信息
    private:
        //统计每种command的查询次数和命中次数，目的是计算一定时间内每种command的命中率
        std::map<int, std::pair< int, int > > cmd_times_v2_;

        //统计查询请求每个ip出现的次数，目的是计算一定时间内最大查询次数的ip
        std::map<std::string, int> query_ip_times_;

        //统计RID查询的次数
        std::map<RID,int> rid_times_;

        map<int,int> peerversion_times_;

        //统计网络类型
        map<int,int> nattype_times_;

        //计算report命令里上报的资源的数量情况
        //由于同时有add和del资源，所以key表示为 del_num*1000+add (这里假定了add不会大于100)
        map<int,int> report_num_times_;

        int rid_count_;
        int peer_count_;

    public:
        void AddCmdTimes(int cmd);
        void AddHitCmdTimes(int cmd);
        void AddIpTimes(std::string ip);
        void AddRidTimes(RID rid);
        void AddPeerversionTimes(int version);
        void AddNatTypeTimes(int nattype);
        void AddReportNumTimes(int report_num);
        void SetRidCount(int rid_count);
        void SetPeerCount(int peer_count);

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

        void OutputStatisticData();
        void ClearStatisticData();

		std::string GetTrackerStatisticForQuery();

    public:
        void InsertStatRidHashMap(RID rid, uint32_t peer_count_for_request);
        void OutputRidPeerHashMap();
        boost::uint16_t GetPrintTimes() { return print_times_; }
        void SetPrintTimes(const boost::uint16_t times) { print_times_ = times; }
        bool IsPrintable(){ return is_print_; }
        void SetIsPrint(bool isprint){ is_print_ = isprint; }
        void DecPrintTimes() { --print_times_; }
        bool IsPrintCompelted() { return (!(print_times_ > 0));}

    private:
        boost::uint16_t print_times_;//Default 1000 times
        bool is_print_;
#ifndef _MSC_VER
        boost::shared_ptr< hash_map<RID, RidStat, RidHash> > p_rid_peer_hash_map_;
#endif
    };

}

#endif//UDPTRACKER_TRACKERSTATISTIC_H
