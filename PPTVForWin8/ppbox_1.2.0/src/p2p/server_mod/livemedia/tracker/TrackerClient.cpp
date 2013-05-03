#include <pre.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost;

#include <framework/Framework.h>
#include <framework/configure/Profile.h>
#include <framework/string/Uuid.h>

#include <framework/timer/Timer.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/timer/TickCounter.h>

#include <framework/logger/Logger.h>
#include <framework/network/Endpoint.h>
#include <framework/network/Interface.h>

#include <util/serialization/Uuid.h>

#include <network/UdpServer.h>

#include <util/Util.h>

#include "tracker/TrackerClient.h"
#include "tracker/TrackerManager.h"

boost::shared_ptr<protocol::UdpServer> TrackerClient::udp_server_;
Guid TrackerClient::peer_guid_;
std::set<RID> TrackerClient::client_resource_ids_;

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


/// ����stl������(const��ʽ)
#define STL_FOR_EACH_CONST(containerType, container, iter)    \
    for ( containerType::const_iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )


void TrackerClient::Start()
{

    last_response_rid_count_ = 0;
    last_transaction_id_ = 0;
    local_resources_.clear();
    is_sync_ = false;
}

void TrackerClient::Stop()
{
    last_response_rid_count_ = 0;
    last_transaction_id_ = 0;
    local_resources_.clear();
    is_sync_ = false;
}

void TrackerClient::SetRidCount(uint32_t rid_count)
{
    // �ϴ��յ���keepalive����commit�з������Ĵ洢�ı�������Դ����
    last_response_rid_count_ = rid_count;
}

void TrackerClient::DoList(const RID& rid)
{
    last_transaction_id_ = protocol::Packet::NewTransactionID();
    protocol::ListPacket list_request_packet(last_transaction_id_, 
        protocol::PEER_VERSION, rid, peer_guid_, MAX_REQUEST_PEER_COUNT_, end_point_);

    // �Ӹ�TrackerClient����ListRequestPacket

    LOG4CPLUS_INFO(Loggers::Service(), "TrackerClient::DoList " << rid<<" "<<end_point_);
    //COUT("TrackerClient::DoList " << rid<<" "<<end_point_);

    this->udp_server_->send_packet(list_request_packet , list_request_packet.peer_version_);
    //	appmodule_->DoSendPacket(list_request_packet);

    // ͳ����Ϣ
    // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
    //	statistic::StatisticModule::Inst()->SubmitListRequest(tracker_info_);
}

void TrackerClient::DoQueriedPeerCount(const RID& rid)
{
    // ����rid,ƴ��ListRequestPacket
    last_transaction_id_ = protocol::Packet::NewTransactionID();
    protocol::QueryPeerCountPacket query_request_packet(last_transaction_id_, 
        protocol::PEER_VERSION, rid, end_point_);

    // �Ӹ�TrackerClient����ListRequestPacket
    //LOG(__EVENT, "tracker", "TrackerClient::OnListPacket RID=" << packet.response.ResourceID);
    udp_server_->send_packet(query_request_packet, query_request_packet.peer_version_); 
    //	appmodule_->DoSendPacket(query_request_packet);
}

void TrackerClient::OnListResponsePacket(protocol::ListPacket const & packet)
{
    // nightsuns: ���ﲻ����,��Ϊ����������Ҫ
    /*
    //��List����peer����ip pool
    // ��packet������ vector<PeerInfo::p> peers
    //LOG(__EVENT, "tracker", "TrackerClient::OnListPacket peers.size()=" << peers.size());
    //COUT("tracker TrackerClient::OnListPacket peers.size()=" << peers.size());
    std::vector<protocol::CandidatePeerInfo> peers = packet.response.peer_infos_;
    for (uint32_t i = 0; i < peers.size(); ++i) {
    if (peers[i].UploadPriority < 255 && peers[i].UploadPriority > 0) {
    peers[i].UploadPriority++;
    }
    }
    appmodule_->AddCandidatePeers(packet.response.resource_id_, peers);

    // ͳ����Ϣ
    // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
    //	statistic::StatisticModule::Inst()->SubmitListResponse(tracker_info_, packet.response.peer_infos_.size());
    */
}

void TrackerClient::OnReportResponsePacket(protocol::ReportPacket const & packet)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerClient Report Response RID Count: " << last_response_rid_count_);

    // ͳ����Ϣ
    // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
    //	statistic::StatisticModule::Inst()->SubmitCommitResponse(tracker_info_);

    // IP��Ϣ
    UpdateIpStatistic(protocol::SocketAddr(packet.response.detected_ip_, packet.response.detected_udp_port_));

    if (packet.transaction_id_ == last_transaction_id_)
    {
        // ����updates
        last_response_rid_count_ = packet.response.resource_count_;

        // updates
        STL_FOR_EACH_CONST(std::vector<protocol::REPORT_RESOURCE_STRUCT>, last_updates_, iter)
        {
            if (iter->Type == 1)
            {
                // add
                local_resources_.insert(iter->ResourceID);
            }
            else if (iter->Type == 0)
            {
                // del
                local_resources_.erase(iter->ResourceID);
            }
            else
            {

                assert(0);
            }
        }
    }
    else
    {
        LOG4CPLUS_WARN(Loggers::Service(), "TrackerClient::OnReportPacket: Unexpected Transaction ID, " << packet.transaction_id_);
    }
}

void TrackerClient::OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet)
{
    // nightsuns: ���ﲻ����,��Ϊ����������Ҫ
    /*
    //LOG(__EVENT, "tracker", "TrackerClient::OnQueryResponsePacket " << packet->GetResourceID());
    appmodule_->AddQueriedPeerCount(packet.response.resource_id_, packet.response.peer_count_);
    */
}

boost::uint32_t TrackerClient::DoSubmit()
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerClient::DoSubmit ModNO:" << (uint32_t)tracker_info_.ModNo 
        << ", IP:" << framework::network::Endpoint(tracker_info_.IP, tracker_info_.Port).to_string());

    uint32_t result = 0;

    /*
    //������m_storage�в��ұ�����Դ  set<RID> now_resource_ = AppModule::Inst()->GetLocalResource();
    set<RID> now_resources = GetClientResource();
    //������������last_response_rid_count�Ƚ�
    if (now_resources.size() != last_response_rid_count_ || 
    now_resources.size() != local_resources_.size())
    {
    result = DoReport();
    }
    else
    {
    // �뱾����Դ�Ƚ�
    if (now_resources != local_resources_) // set compare
    result = DoReport();
    else
    result = DoKeepAlive();
    }

    // local_resources_ = now_resources;
    */

    // ����������һ��Interval�ڻذ���������Ϊ��ʱ��
    // ��ˣ�����������ذ��ˣ�ͬ����Դһ�����������ͬ���ˣ�
    // ��������ʹ��local_resources_.size()

    // ����ʵ����Դ
    std::set<RID> now_resources = GetClientResource();
    // ��ӡ������Դ��ͬ����Դ��release log����ʽrelease����û��

    // ���last_response_rid_countΪ0��������������������շ�����ͬ����Դ����report
    if (last_response_rid_count_ == 0)
    {
        local_resources_.clear();
    }

    // ���������Դ��ͬ����Դ����ͬ�������Report
    if (now_resources.size() != local_resources_.size() || now_resources != local_resources_)
    {
        is_sync_ = false;
        result = DoReport();
    }
    // ���������Դ��ͬ����Դ�Ѿ���ͬ�����Ƿ������ϱ���ĸ���С��ͬ����Դ����
    else if (last_response_rid_count_ < local_resources_.size())
    {
        is_sync_ = false;
        // �����������Դ̫�٣�����ͬ��
        if (last_response_rid_count_ <= (uint32_t)(local_resources_.size() * 0.7))
        {
            local_resources_.clear();
        }
        result = DoReport();
    }
    // ���������Դ��ͬ����Դ�Ѿ���ͬ���ҷ�����������ͬ����Դ����Ҳ��ͬ
    else if (last_response_rid_count_ == local_resources_.size())
    {
        is_sync_ = true;
        // ����KeepAlive(Ҳ���ǿ�Report)
        //! ��Report�ķ��Ϳ��Ż�
        result = DoReport();
    }
    // ���������Դ��ͬ����Դ�Ѿ���ͬ�����Ƿ������ϱ���ĸ�������ͬ����Դ����
    else if (last_response_rid_count_ > local_resources_.size())
    {
        // Nothing to do! ���������ۻ���ֱ��Peer�˳�
        // ���Ϳ�Report
    }

    return result;
    //�����ͬ��return DoCommit();
    //����
    //    ������Local_Resource()�Ƚ�
    //    �����ͬ��return DoCommit();
    //����         return DoKeepALive();
    //local_resource_ = now_resource_;
}

void LoadLocalIPs(std::vector<uint32_t>& ipArray)
{
    static std::vector<uint32_t> ip_array;
    if ( !ip_array.size() ) {
#ifndef __APPLE__
        std::vector<framework::network::Interface> interfaces;
        framework::network::enum_interface(interfaces);

        for ( size_t i = 0 ; i < interfaces.size() ; ++i ) {
            unsigned long tmp_ip = inet_addr( interfaces[i].addr.to_string().c_str() );
            if( tmp_ip == inet_addr( "127.0.0.1" ) || tmp_ip == 0 )
                continue;
            // 
            ip_array.push_back( interfaces[i].addr.to_v4().to_ulong() );
        }
#else
        int socktemp;
        struct ifreq ifr;
        struct sockaddr_in clientadd;
        socktemp = socket(AF_INET, SOCK_DGRAM, 0);
        if (socktemp != -1) {
            strncpy(ifr.ifr_name, "en0", IFNAMSIZ);
            ifr.ifr_name[IFNAMSIZ-1] = 0;
            if (ioctl(socktemp, SIOCGIFADDR, &ifr) >= 0) {
                memcpy(&clientadd, &ifr.ifr_addr, sizeof(clientadd));

                boost::asio::ip::tcp::endpoint ep;
                memcpy(ep.data(), &ifr.ifr_addr, sizeof(ifr.ifr_addr));
                ip_array.push_back( ep.address().to_v4().to_ulong() );
            }
        }

#endif
    }

    ipArray = ip_array;
}

/**
* @return Transaction ID.
*/
uint32_t TrackerClient::DoReport()
{
    // ͳ����Ϣ
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerClient::DoReport ");

    // TODO: ������ʱȥ���� static �Ĺ���,����Ժ���Ҫ�����ٲ���
    //	statistic::StatisticModule::Inst()->SubmitCommitRequest(tracker_info_);

    //��m_storageƴ��protocol::CommitRequestPacket����
    //	const AppModule::p app = appmodule_;

    // transaction id
    last_transaction_id_ = protocol::Packet::NewTransactionID();

    // local resource
    std::set<RID> local_resource_set = GetClientResource();


    // ����50��update��Դ
    std::vector<protocol::REPORT_RESOURCE_STRUCT> update_resource_set;
    std::set<RID>::const_iterator real_iter, server_iter;
    real_iter = local_resource_set.begin();
    server_iter = local_resources_.begin();
    // merge
    while (update_resource_set.size() < MAX_REPORT_RID_COUNT)
    {
        if (real_iter == local_resource_set.end() && server_iter == local_resources_.end())
        {
            // both end
            break;
        }
        else if (server_iter == local_resources_.end() || 
            (real_iter != local_resource_set.end() && *real_iter < *server_iter))
        {
            protocol::REPORT_RESOURCE_STRUCT update;
            update.ResourceID = *real_iter;
            update.Type = 1; // ADD
            // push back
            update_resource_set.push_back(update);
            //                ++last_response_rid_count_;
            ++real_iter;
        }
        else if (real_iter == local_resource_set.end() || 
            (server_iter != local_resources_.end() && *server_iter < *real_iter))
        {
            protocol::REPORT_RESOURCE_STRUCT update;
            update.ResourceID = *server_iter;
            update.Type = 0; // DEL
            // push back
            update_resource_set.push_back(update);
            //                --last_response_rid_count_;
            ++server_iter;
        }
        else // should be equal and both real_iter, server_iter not ends
        {
            // assert
            assert(real_iter != local_resource_set.end() && server_iter != local_resources_.end());
            assert(*real_iter == *server_iter);
            // next
            ++real_iter;
            ++server_iter;
        }
    }

    // ip
    std::vector<uint32_t> real_ips;

    // nightsuns: ��������ʱӲ����IP��PORT
    // stun info
    uint32_t stun_ip = 0;
    boost::uint16_t stun_port = 0;
    //	StunModule::Inst()->GetStunEndpoint(stun_ip, stun_port);

    //	boost::uint16_t stun_detect_port = statistic::StatisticModule::Inst()->GetLocalPeerInfo().DetectUdpPort;
    boost::uint16_t stun_detect_port = udp_server_->GetUdpPort();
    boost::uint32_t stun_detect_ip = 0;

    LoadLocalIPs(real_ips);

    boost::uint16_t local_tcp_port, local_udp_port;

    // nightsuns: ��������ʱӲ����IP��PORT
    //	local_tcp_port = app->GetLocalTcpPort();
    //	local_udp_port = app->GetLocalUdpPort();
    local_tcp_port = 0;
    local_udp_port = udp_server_->GetUdpPort();
    //        }

    // request
    protocol::ReportPacket report_request(
        last_transaction_id_,
        protocol::PEER_VERSION,
        //			app->GetPeerGuid(),
        peer_guid_ ,							  
        local_resource_set.size(),
        local_resources_.size(),
        local_udp_port,
        stun_ip,
        stun_port,
        stun_detect_ip,
        stun_detect_port,
        real_ips, 
        update_resource_set,
        // nightsuns: �������̶���NAT������ PUBLIC
        NAT_PUBLIC,
        // nightsuns: ��������ʱӲ����
        255 ,
        60 * 24 ,
        //			app->GenUploadPriority(),
        //			app->GetIdleTimeInMins(),
        0 , 
        0 , 
        0 ,
        end_point_
        );


    // post
    this->udp_server_->send_packet( report_request , report_request.peer_version_ );
    //	app->DoSendPacket(report_request);

    last_updates_ = update_resource_set;

    return last_transaction_id_;
}

//////////////////////////////////////////////////////////////////////////
// Tracker Info

void TrackerClient::SetTrackerInfo(const protocol::TRACKER_INFO& tracker_info)
{
    LOG4CPLUS_INFO(Loggers::Service(), "TrackerClient::SetTrackerInfo ModNO:" << (uint32_t)tracker_info.ModNo
        << ", IP:" << framework::network::Endpoint(tracker_info.IP, tracker_info.Port).to_string());
    tracker_info_ = tracker_info;
}

const protocol::TRACKER_INFO& TrackerClient::GetTrackerInfo() const
{
    return tracker_info_;
}

void TrackerClient::SetGroupCount(uint32_t group_count)
{
    group_count_ = group_count;
}

uint32_t TrackerClient::GetGroupCount() const
{
    return group_count_;
}

std::set<RID> TrackerClient::GetClientResource() const
{
    std::set<RID> moded_rids;

    // �����ｫ client_resource_ids_ ���з���
    for ( std::set<RID>::iterator i = client_resource_ids_.begin() ; i != this->client_resource_ids_.end() ; ++i ) {
        // 
        if ( GuidMod( *i , this->group_count_ ) == tracker_info_.ModNo  ) {
            moded_rids.insert( *i );
        }
    }

    return moded_rids;
    //	return appmodule_->GetLocalResource(tracker_info_.ModNo, group_count_);
}

//////////////////////////////////////////////////////////////////////////
// Update Ips
void TrackerClient::UpdateIpStatistic(const protocol::SocketAddr& detected_addr)
{
    // nightsuns: ��������ʱ����Ҫ��Щ��Ϣ
    /*
    std::vector<uint32_t> local_ips;

    LoadLocalIPs(local_ips);
    statistic::StatisticModule::Inst()->SetLocalIPs(local_ips);


    if (find(local_ips.begin(), local_ips.end(), detected_addr.IP) != local_ips.end())
    {
    statistic::StatisticModule::Inst()->SetLocalPeerIp(detected_addr.IP);
    }
    else if (local_ips.size() > 0)
    {
    statistic::StatisticModule::Inst()->SetLocalPeerIp(local_ips.front());
    }

    if (statistic::StatisticModule::Inst()->GetLocalPeerInfo().IP == detected_addr.IP)
    {
    //MainThread::Post(boost::bind(&StunModule::SetIsNeededStun,StunModule::Inst(), false));
    StunModule::Inst()->SetIsNeededStun(false);
    }
    //statistic::StatisticModule::Inst()->SetLocalDetectSocketAddress(detected_addr);
    */
}

void TrackerClient::PPLeave()
{
    protocol::LeavePacket leave_packet(
        protocol::Packet::NewTransactionID(), 
        protocol::PEER_VERSION, 
        peer_guid_ , 
        end_point_ );

    udp_server_->send_packet( leave_packet , leave_packet.peer_version_ );
    //appmodule_->DoSendPacket(leave_packet);
    is_sync_ = false;
}
