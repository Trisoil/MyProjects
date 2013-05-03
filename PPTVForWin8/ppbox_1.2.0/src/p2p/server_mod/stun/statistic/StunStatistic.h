#ifndef _STUN_STATISTIC_H_
#define _STUN_STATISTIC_H_

#include "Common.h"
#include "statistic/ByteSpeedMeter.h"

using namespace statistic;

namespace udpstun
{
    struct StunStatisticInfo
    {
        boost::uint64_t request_count_;
        boost::uint64_t response_count_;
        boost::uint64_t request_bytes_;
        boost::uint64_t response_bytes_;
        double request_frequency_;
        double response_frequency_;
        double request_speed_;
        double response_speed_;
        double recent_minute_request_frequency_;
        double recent_minute_response_frequency_;
        double recent_minute_request_speed_;
        double recent_minute_response_speed_;
    };

    class StunStatistic
        : public boost::noncopyable
        , public boost::enable_shared_from_this<StunStatistic>
    {
    public:
        typedef boost::shared_ptr<StunStatistic> p;

    public:
        static p Create();

    public:
        bool IsRunning() const {return is_running_;};
        void Start();
        void Stop();
        void Clear();

    public:
        //�������
        void SubmitHandSharkRequest(size_t packet_size);
        void SubmitKeepAliveRequest(size_t packet_size);
        void SubmitInvokeRequest(size_t packet_size);

        //��Ӧ����
        void SubmitHandSharkResponse(size_t packet_size);
        void SubmitInvokeResponse(size_t packet_size);

        //����ͳ����Ϣ
        StunStatisticInfo TotalInfo();
        StunStatisticInfo HandSharkInfo();
        StunStatisticInfo KeepAliveInfo();
        StunStatisticInfo InvokeInfo();

    private:
        bool is_running_;

    private:
        StunStatistic();

    private:

        enum CounterKind
        {
            //�������
            all_request_counter_,
            handshark_counter_,
            keepalive_counter_,
            invoke_counter_,

            //��Ӧ����
            all_response_counter_,
            response_handshark_counter_,
            response_invoke_counter_,

            //�������ݣ�����
            all_request_bytes_,
            handshark_bytes_,
            keepalive_bytes_,
            invoke_bytes_,

            //��Ӧ���ݣ�����
            all_response_bytes_,
            response_handshark_bytes_,
            response_invoke_bytes_,

            //����
            counter_num_
        };

        vector<ByteSpeedMeter::p> statistics_;

    };
}
#endif //_STUN_STATISTIC_H_
