//#include "stdafx.h"
#include "Common.h"
#include "StunStatistic.h"

namespace udpstun
{
    StunStatistic::p StunStatistic::Create()
    {
        return StunStatistic::p(new StunStatistic());
    }

    StunStatistic::StunStatistic(void):is_running_(false)
    {
        for (unsigned char i = 0; i < counter_num_; i++)
        {
            statistics_.push_back(ByteSpeedMeter::Create());
        }
    }

    void StunStatistic::Start()
    {
        if (is_running_ == true)
        {
            return;
        }

        for (unsigned char i = 0; i < statistics_.size(); i++)
        {
            statistics_[i]->Start();
        }

        is_running_ = true;
    }

    void StunStatistic::Stop()
    {
        if (is_running_ == false)
        {
            return;
        }
        is_running_ = false;

        for (unsigned char i = 0; i < statistics_.size(); i++)
        {
            statistics_[i]->Stop();
        }

    }

    void StunStatistic::Clear()
    {
        if (is_running_ == false)
        {
            return;
        }

        for (unsigned char i = 0; i < statistics_.size(); i++)
        {
            statistics_[i]->Clear();
        }
    }

    //请求计数
    void StunStatistic::SubmitHandSharkRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[handshark_counter_]->SubmitBytes(1);
        statistics_[handshark_bytes_]->SubmitBytes(packet_size);
    }
    void StunStatistic::SubmitKeepAliveRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[keepalive_counter_]->SubmitBytes(1);
        statistics_[keepalive_bytes_]->SubmitBytes(packet_size);
    }
    void StunStatistic::SubmitInvokeRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[invoke_counter_]->SubmitBytes(1);
        statistics_[invoke_bytes_]->SubmitBytes(packet_size);
    }

    //响应计数
    void StunStatistic::SubmitHandSharkResponse(size_t packet_size)
    {
        statistics_[all_response_counter_]->SubmitBytes(1);
        statistics_[all_response_bytes_]->SubmitBytes(packet_size);
        statistics_[response_handshark_counter_]->SubmitBytes(1);
        statistics_[response_handshark_bytes_]->SubmitBytes(packet_size);
    }
    void StunStatistic::SubmitInvokeResponse(size_t packet_size)
    {
        statistics_[all_response_counter_]->SubmitBytes(1);
        statistics_[all_response_bytes_]->SubmitBytes(packet_size);
        statistics_[response_invoke_counter_]->SubmitBytes(1);
        statistics_[response_invoke_bytes_]->SubmitBytes(packet_size);
    }

    //返回统计信息
    StunStatisticInfo StunStatistic::TotalInfo()
    {
        StunStatisticInfo statistic_info;
        statistic_info.request_count_ = statistics_[all_request_counter_]->TotalBytes();
        statistic_info.response_count_ = statistics_[all_response_counter_]->TotalBytes();
        statistic_info.request_frequency_ = statistics_[all_request_counter_]->AverageByteSpeed();
        statistic_info.response_frequency_ = statistics_[all_response_counter_]->AverageByteSpeed();
        statistic_info.request_bytes_ = statistics_[all_request_bytes_]->TotalBytes();
        statistic_info.response_bytes_ = statistics_[all_response_bytes_]->TotalBytes();
        statistic_info.request_speed_ = statistics_[all_request_bytes_]->AverageByteSpeed();
        statistic_info.response_speed_ = statistics_[all_response_bytes_]->AverageByteSpeed();
        statistic_info.recent_minute_request_frequency_ = statistics_[all_request_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_frequency_ = statistics_[all_response_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_request_speed_ = statistics_[all_request_bytes_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_speed_ = statistics_[all_response_bytes_]->RecentMinuteByteSpeed();
        return statistic_info;
    }
    StunStatisticInfo StunStatistic::HandSharkInfo()
    {
        StunStatisticInfo statistic_info;
        statistic_info.request_count_ = statistics_[handshark_counter_]->TotalBytes();
        statistic_info.response_count_ = statistics_[response_handshark_counter_]->TotalBytes();
        statistic_info.request_frequency_ = statistics_[handshark_counter_]->AverageByteSpeed();
        statistic_info.response_frequency_ = statistics_[response_handshark_counter_]->AverageByteSpeed();
        statistic_info.request_bytes_ = statistics_[handshark_bytes_]->TotalBytes();
        statistic_info.response_bytes_ = statistics_[response_handshark_bytes_]->TotalBytes();
        statistic_info.request_speed_ = statistics_[handshark_bytes_]->AverageByteSpeed();
        statistic_info.response_speed_ = statistics_[response_handshark_bytes_]->AverageByteSpeed();
        statistic_info.recent_minute_request_frequency_ = statistics_[handshark_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_frequency_ = statistics_[response_handshark_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_request_speed_ = statistics_[handshark_bytes_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_speed_ = statistics_[response_handshark_bytes_]->RecentMinuteByteSpeed();
        return statistic_info;
    }
    StunStatisticInfo StunStatistic::KeepAliveInfo()
    {
        StunStatisticInfo statistic_info;
        statistic_info.request_count_ = statistics_[keepalive_counter_]->TotalBytes();
        statistic_info.response_count_ = 0;
        statistic_info.request_frequency_ = statistics_[keepalive_counter_]->AverageByteSpeed();
        statistic_info.response_frequency_ = 0;
        statistic_info.request_bytes_ = statistics_[keepalive_bytes_]->TotalBytes();
        statistic_info.response_bytes_ = 0;
        statistic_info.request_speed_ = statistics_[keepalive_bytes_]->AverageByteSpeed();
        statistic_info.response_speed_ = 0;
        statistic_info.recent_minute_request_frequency_ = statistics_[keepalive_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_frequency_ = 0;
        statistic_info.recent_minute_request_speed_ = statistics_[keepalive_bytes_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_speed_ = 0;
        return statistic_info;
    }
    StunStatisticInfo StunStatistic::InvokeInfo()
    {
        StunStatisticInfo statistic_info;
        statistic_info.request_count_ = statistics_[invoke_counter_]->TotalBytes();
        statistic_info.response_count_ = statistics_[response_invoke_counter_]->TotalBytes();
        statistic_info.request_frequency_ = statistics_[invoke_counter_]->AverageByteSpeed();
        statistic_info.response_frequency_ = statistics_[response_invoke_counter_]->AverageByteSpeed();
        statistic_info.request_bytes_ = statistics_[invoke_bytes_]->TotalBytes();
        statistic_info.response_bytes_ = statistics_[response_invoke_bytes_]->TotalBytes();
        statistic_info.request_speed_ = statistics_[invoke_bytes_]->AverageByteSpeed();
        statistic_info.response_speed_ = statistics_[response_invoke_bytes_]->AverageByteSpeed();
        statistic_info.recent_minute_request_frequency_ = statistics_[invoke_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_frequency_ = statistics_[response_invoke_counter_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_request_speed_ = statistics_[invoke_bytes_]->RecentMinuteByteSpeed();
        statistic_info.recent_minute_response_speed_ = statistics_[response_invoke_bytes_]->RecentMinuteByteSpeed();
        return statistic_info;
    }
}