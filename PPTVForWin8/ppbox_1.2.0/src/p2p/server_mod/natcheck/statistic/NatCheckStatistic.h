#ifndef _NATCHECK_STATISTIC_H_
#define _NATCHECK_STATISTIC_H_

#include "Common.h"
#include "statistic/ByteSpeedMeter.h"

using namespace statistic;

namespace udpnatcheck
{
    //struct StunStatisticInfo
    //{
    //    boost::uint64_t request_count_;
    //    boost::uint64_t response_count_;
    //    boost::uint64_t request_bytes_;
    //    boost::uint64_t response_bytes_;
    //    double request_frequency_;
    //    double response_frequency_;
    //    double request_speed_;
    //    double response_speed_;
    //    double recent_minute_request_frequency_;
    //    double recent_minute_response_frequency_;
    //    double recent_minute_request_speed_;
    //    double recent_minute_response_speed_;
    //};

    class NatCheckStatistic
        : public boost::noncopyable
        , public boost::enable_shared_from_this<NatCheckStatistic>
    {
    public:
        typedef boost::shared_ptr<NatCheckStatistic> p;

    public:
        static p Create();

    public:
        bool IsRunning() const {return is_running_;};
        void Start();
        void Stop();
        void Clear();

    public:
        //�������
        void SubmitSameRouteRequest(size_t packet_size);
        void SubmitDiffPortRequest(size_t packet_size);
        void SubmitDiffIpRequest(size_t packet_size); 

        //��Ӧ����
        void SubmitSameRouteResponse(size_t packet_size);
        void SubmitDiffPortResponse(size_t packet_size);
        void SubmitDiffIpResponse(size_t packet_size); 

        //ͳ�Ʋ�ѯ�Ĵ���
        void SubmitQueryTimes(size_t cmd,size_t querytime);

        ////����ͳ����Ϣ
        string DumpInfo();

    private:
        bool is_running_;

    private:
        NatCheckStatistic();

    private:
        map<size_t,size_t> cmd_querytimes_times_;

        enum CounterKind
        {
            //�������
            all_request_counter_,
            sameroute_counter_,
            diffport_counter_,
            diffip_counter_,

            //��Ӧ����
            all_response_counter_,
            response_sameroute_counter_,
            response_diffport_counter_,
            response_diffip_counter_,

            //�������ݣ�����
            all_request_bytes_,
            sameroute_bytes_,
            diffport_bytes_,
            diffip_bytes_,

            //��Ӧ���ݣ�����
            all_response_bytes_,
            response_sameroute_bytes_,            
            response_diffport_bytes_,
            response_diffip_bytes_,

            //����
            counter_num_
        };

        vector<ByteSpeedMeter::p> statistics_;

    };
}
#endif //_NATCHECK_STATISTIC_H_
