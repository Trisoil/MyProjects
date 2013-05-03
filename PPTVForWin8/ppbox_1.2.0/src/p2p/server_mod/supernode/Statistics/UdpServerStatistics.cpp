//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "UdpServerStatistics.h"
#include "../UdpServer/SessionManager.h"
#include "../ServiceHealthMonitor.h"
#include "../AsioServiceRunner.h"
#include "../ServiceStatusDetector.h"

namespace super_node
{
    const size_t UdpServerStatistics::StatisticsIntervalInSeconds = 5;

    UdpServerStatistics::UdpServerStatistics()
    {
        ResetCounters();
    }

    void UdpServerStatistics::Start()
    {
        boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(ServiceComponents::UdpServer);
        request_count_ = service_component->RegisterStatusItem("Request Count", "requests/s");
        subpiece_response_count_ = service_component->RegisterStatusItem("Response - Subpieces", "subpieces/s");
        resource_not_found_error_count_ = service_component->RegisterStatusItem("Response - Resource Not Found Error", "errors/s");
        service_busy_error_count_ = service_component->RegisterStatusItem("Response - Service Busy Error", "errors/s");
        internal_error_count_ = service_component->RegisterStatusItem("Response - Internal Error", "errors/s");
        average_response_time_ = service_component->RegisterStatusItem("Average Response Time", "ms");
        session_count_ = service_component->RegisterStatusItem("Session - Current Count", "");
        unknown_packets_received_status_ = service_component->RegisterStatusItem("Unknown Packets Received", "packets/s");
        sessions_closed_by_client_status_ = service_component->RegisterStatusItem("Session - Closed By Client", "sessions/s");
        duplicate_requests_status_ = service_component->RegisterStatusItem("Duplicate Requests", "requests/s");
        new_session_requests_rejected_status_ = service_component->RegisterStatusItem("Session - Requests Rejected", "sessions/s");
        new_sessions_accepted_status_ = service_component->RegisterStatusItem("Session - Accepted New", "sessions/s");
        service_busy_status_ = service_component->RegisterStatusItem("Worst IO Service Queue Wait Time", "ms");

        avg_res_subpiece_range_size_status_ = service_component->RegisterStatusItem("Average Resource Subpiece Range Size", "");
        avg_res_subpiece_cnt_status_ = service_component->RegisterStatusItem("Average Resource Subpiece Count", "");
        avg_blk_subpiece_range_size_status_ = service_component->RegisterStatusItem("Average Block Subpiece Range Size", "");
        avg_blk_subpiece_cnt_status_ = service_component->RegisterStatusItem("Average Block Subpiece Count", "");
        
        ResetCounters();
    }

    void UdpServerStatistics::Stop()
    {
        ServiceHealthMonitor::Instance()->UnregisterComponent(ServiceComponents::UdpServer);
    }

    void UdpServerStatistics::OnError(const ErrorCode & error_code)
    {
        if (error_code == ErrorCodes::ResourceNotFound)
        {
            ++sent_resource_not_found_error_;
        }
        else if (error_code == ErrorCodes::ServiceBusy)
        {
            ++sent_service_busy_error_;
        }
        else
        {
            ++sent_internal_error_;
        }
    }

    void UdpServerStatistics::ResetCounters()
    {
        received_requests_ = 0;
        sent_subpiece_responses_ = 0;
        sent_resource_not_found_error_ = 0;
        sent_service_busy_error_ = 0;
        sent_internal_error_ = 0;
        sum_response_time_in_ms_ = 0;
        unknown_packets_received_ = 0;
        sessions_closed_by_client_ = 0;
        duplicate_requests_ = 0;
        new_session_requests_rejected_ = 0;
        new_sessions_accepted_ = 0;

        avg_res_subpiece_range_size_.Reset();
        avg_res_subpiece_cnt_.Reset();
        avg_blk_subpiece_range_size_.Reset();
        avg_blk_subpiece_cnt_.Reset();
    }

    void UdpServerStatistics::ReportStatus(boost::shared_ptr<SessionManager> session_manager)
    {
        request_count_->SetStatus(received_requests_/StatisticsIntervalInSeconds);
        subpiece_response_count_->SetStatus(sent_subpiece_responses_/StatisticsIntervalInSeconds);
        resource_not_found_error_count_->SetStatus(sent_resource_not_found_error_/StatisticsIntervalInSeconds);
        service_busy_error_count_->SetStatus(sent_service_busy_error_/StatisticsIntervalInSeconds);
        internal_error_count_->SetStatus(sent_internal_error_/StatisticsIntervalInSeconds);
        unknown_packets_received_status_->SetStatus(unknown_packets_received_/StatisticsIntervalInSeconds);
        sessions_closed_by_client_status_->SetStatus(sessions_closed_by_client_/StatisticsIntervalInSeconds);
        duplicate_requests_status_->SetStatus(duplicate_requests_/StatisticsIntervalInSeconds);
        new_session_requests_rejected_status_->SetStatus(new_session_requests_rejected_/StatisticsIntervalInSeconds);
        new_sessions_accepted_status_->SetStatus(new_sessions_accepted_/StatisticsIntervalInSeconds);

        avg_res_subpiece_range_size_status_->SetStatus(avg_res_subpiece_range_size_.Average());
        avg_res_subpiece_cnt_status_->SetStatus(avg_res_subpiece_cnt_.Average());
        avg_blk_subpiece_range_size_status_->SetStatus(avg_blk_subpiece_range_size_.Average());
        avg_blk_subpiece_cnt_status_->SetStatus(avg_blk_subpiece_cnt_.Average());

        size_t responses = sent_subpiece_responses_ + sent_resource_not_found_error_ + sent_service_busy_error_ + sent_internal_error_;
        average_response_time_->SetStatus(responses > 0 ? (sum_response_time_in_ms_ / responses) : 0);

        session_count_->SetStatus(session_manager->GetCurrentSessionNum());

        ServiceStatusDetector::Instance().TouchServices();
        std::map<std::string, size_t> services_queue_status = ServiceStatusDetector::Instance().GetServicesStatus();
        
        size_t worst_wait_time(0);
        for(std::map<std::string, size_t>::const_iterator iter = services_queue_status.begin();
            iter != services_queue_status.end();
            ++iter)
        {
            if (iter->second > worst_wait_time)
            {
                worst_wait_time = iter->second;
            }
        }
        
        service_busy_status_->SetStatus(worst_wait_time);

        ResetCounters();
    }

    void UdpServerStatistics::OnResourceSubpieceDensityStat(size_t first, size_t last, size_t cnt)
    {
        avg_res_subpiece_range_size_.Add(last - first + 1);
        avg_res_subpiece_cnt_.Add(cnt);
    }

    void UdpServerStatistics::OnBlockSubpieceDensityStat(size_t first, size_t last, size_t cnt)
    {
        avg_blk_subpiece_range_size_.Add(last - first + 1);
        avg_blk_subpiece_cnt_.Add(cnt);
    }
}
