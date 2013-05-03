//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_UDP_SERVER_STATISTICS_H
#define SUPER_NODE_UDP_SERVER_STATISTICS_H

#include "Statistics/NumericStatistics.h"

namespace super_node
{
    class ComponentStatusItem;
    class SessionManager;

    class UdpServerStatistics
        : public boost::enable_shared_from_this<UdpServerStatistics>
        ,public count_object_allocate<UdpServerStatistics>
    {
    public:
        static const size_t StatisticsIntervalInSeconds;

        UdpServerStatistics();

        void Start();
        void Stop();

        void ReportStatus(boost::shared_ptr<SessionManager> session_manager);
        
        void OnError(const ErrorCode & error_code);

        void OnRequestForSubPieceReceived()
        {
            ++received_requests_;
        }

        void OnSubPieceResponseSent()
        {
            ++sent_subpiece_responses_;
        }

        void OnNewSessionRejected()
        {
            ++new_session_requests_rejected_;
        }
            
        void OnDuplicateRequestReceived()
        {
            ++duplicate_requests_;
        }
        
        void OnClientClosedSession()
        {
            ++sessions_closed_by_client_;
        }
        
        void OnUnknownPacketReceived()
        {
            ++unknown_packets_received_;
        }

        void OnNewSessionAccepted()
        {
            ++new_sessions_accepted_;
        }

        void OnResourceSubpieceDensityStat(size_t first, size_t last, size_t cnt);
        void OnBlockSubpieceDensityStat(size_t first, size_t last, size_t cnt);

    private:
        void ResetCounters();

    private:
        boost::shared_ptr<ComponentStatusItem> request_count_;
        boost::shared_ptr<ComponentStatusItem> subpiece_response_count_;
        boost::shared_ptr<ComponentStatusItem> resource_not_found_error_count_;
        boost::shared_ptr<ComponentStatusItem> service_busy_error_count_;
        boost::shared_ptr<ComponentStatusItem> internal_error_count_;
        boost::shared_ptr<ComponentStatusItem> average_response_time_;
        boost::shared_ptr<ComponentStatusItem> session_count_;
        boost::shared_ptr<ComponentStatusItem> unknown_packets_received_status_;
        boost::shared_ptr<ComponentStatusItem> sessions_closed_by_client_status_;
        boost::shared_ptr<ComponentStatusItem> duplicate_requests_status_;
        boost::shared_ptr<ComponentStatusItem> new_session_requests_rejected_status_;
        boost::shared_ptr<ComponentStatusItem> new_sessions_accepted_status_;
        boost::shared_ptr<ComponentStatusItem> service_busy_status_;

        boost::shared_ptr<ComponentStatusItem> avg_res_subpiece_range_size_status_;
        boost::shared_ptr<ComponentStatusItem> avg_res_subpiece_cnt_status_;
        boost::shared_ptr<ComponentStatusItem> avg_blk_subpiece_range_size_status_;
        boost::shared_ptr<ComponentStatusItem> avg_blk_subpiece_cnt_status_;

        boost::uint32_t received_requests_;
        boost::uint32_t sent_subpiece_responses_;
        boost::uint32_t sent_resource_not_found_error_;
        boost::uint32_t sent_service_busy_error_;
        boost::uint32_t sent_internal_error_;

        boost::uint32_t unknown_packets_received_;
        boost::uint32_t sessions_closed_by_client_;
        boost::uint32_t duplicate_requests_;
        boost::uint32_t new_session_requests_rejected_;
        boost::uint32_t new_sessions_accepted_;

        size_t sum_response_time_in_ms_;

        NumericStatistics  avg_res_subpiece_range_size_;
        NumericStatistics  avg_res_subpiece_cnt_;
        NumericStatistics  avg_blk_subpiece_range_size_;
        NumericStatistics  avg_blk_subpiece_cnt_;
    };
}

#endif //SUPER_NODE_UDP_SERVER_STATISTICS_H
