//#include "stdafx.h"
#include "Common.h"
#include "NatCheckStatistic.h"
#include <sstream>
#include <iomanip>

namespace udpnatcheck
{
    NatCheckStatistic::p NatCheckStatistic::Create()
    {
        return NatCheckStatistic::p(new NatCheckStatistic());
    }

    NatCheckStatistic::NatCheckStatistic(void):is_running_(false)
    {
        for (unsigned char i = 0; i < counter_num_; i++)
        {
            statistics_.push_back(ByteSpeedMeter::Create());
        }
    }

    void NatCheckStatistic::Start()
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

    void NatCheckStatistic::Stop()
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

    void NatCheckStatistic::Clear()
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
    void NatCheckStatistic::SubmitSameRouteRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[sameroute_counter_]->SubmitBytes(1);
        statistics_[sameroute_bytes_]->SubmitBytes(packet_size);
    }
    void NatCheckStatistic::SubmitDiffPortRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[diffport_counter_]->SubmitBytes(1);
        statistics_[diffport_bytes_]->SubmitBytes(packet_size);
    }
    void NatCheckStatistic::SubmitDiffIpRequest(size_t packet_size)
    {
        statistics_[all_request_counter_]->SubmitBytes(1);
        statistics_[all_request_bytes_]->SubmitBytes(packet_size);
        statistics_[diffip_counter_]->SubmitBytes(1);
        statistics_[diffip_bytes_]->SubmitBytes(packet_size);
    }

    //响应计数
    void NatCheckStatistic::SubmitSameRouteResponse(size_t packet_size)
    {
        statistics_[all_response_counter_]->SubmitBytes(1);
        statistics_[all_response_bytes_]->SubmitBytes(packet_size);
        statistics_[response_sameroute_counter_]->SubmitBytes(1);
        statistics_[response_sameroute_bytes_]->SubmitBytes(packet_size);        
    }
    void NatCheckStatistic::SubmitDiffPortResponse(size_t packet_size)
    {
        statistics_[all_response_counter_]->SubmitBytes(1);
        statistics_[all_response_bytes_]->SubmitBytes(packet_size);
        statistics_[response_diffport_counter_]->SubmitBytes(1);
        statistics_[response_diffport_bytes_]->SubmitBytes(packet_size);
    }
    void NatCheckStatistic::SubmitDiffIpResponse(size_t packet_size)
    {
        statistics_[all_response_counter_]->SubmitBytes(1);
        statistics_[all_response_bytes_]->SubmitBytes(packet_size);
        statistics_[response_diffip_counter_]->SubmitBytes(1);
        statistics_[response_diffip_bytes_]->SubmitBytes(packet_size);
    }

    void NatCheckStatistic::SubmitQueryTimes(size_t cmd,size_t querytime)
    {
        size_t store_value = cmd << 8 | (querytime%256);
        ++cmd_querytimes_times_[store_value];        
    }

    string NatCheckStatistic::DumpInfo()
    {
        stringstream ss;
        ss<<endl;
        ss<<"cmd------------currentcount---currentbytes-------minbytes"<<endl;
        ss<<"reqallcmd   ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_request_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_request_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_request_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"reqsameroute";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[sameroute_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[sameroute_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[sameroute_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"reqdiffport ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffport_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffport_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffport_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"reqdiffip   ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffip_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffip_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[diffip_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"rspallcmd   ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_response_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_response_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[all_response_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"rspsameroute";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_sameroute_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_sameroute_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_sameroute_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"rspdiffport ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffport_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffport_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffport_bytes_]->RecentMinuteByteSpeed()<<endl;

        ss<<"rspdiffip   ";
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffip_counter_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffip_bytes_]->CurrentByteSpeed();
        ss<<setw(15)<<setiosflags(ios::right)<<statistics_[response_diffip_bytes_]->RecentMinuteByteSpeed()<<endl;

        for(map<size_t,size_t>::iterator it = cmd_querytimes_times_.begin();it != cmd_querytimes_times_.end();++it)
        {
            ss<<"cmd:0x"<<hex<<(it->first>>8)<<" querytime:"<<dec  <<(it->first%256)<<" times:"<<it->second<<endl;
        }
        cmd_querytimes_times_.clear();

        ss<<"-------------------------dump finish--------------------------"<<endl;


        return ss.str();
    }
}