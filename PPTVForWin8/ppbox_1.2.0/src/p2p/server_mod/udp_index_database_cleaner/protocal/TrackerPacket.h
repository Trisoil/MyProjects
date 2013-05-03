#pragma once

/**
* @file
* @brief Peer和IndexServer相关协议包相关的基类
*/

#include "Packet.h"
#include <vector>

namespace protocal
{
	/**
	*@brief Peer发向TrackerServer的 List 包
	*/
	class ListRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<ListRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			RID resource_id, Guid peer_guid, UINT16 request_peer_count);
	public:
		static const UINT8 ACTION = 0x31;
	public:
		inline RID GetResourceID() const{ assert(request_tracker_list_packet_);return request_tracker_list_packet_->ResourceID;}
		inline Guid GetPeerGUID() const{ assert(request_tracker_list_packet_);return request_tracker_list_packet_->PeerGUID;}
		inline UINT16 GetPeerRequestCount() const{ assert(request_tracker_list_packet_);return request_tracker_list_packet_->RequestPeerCount;}
	private:
		REQUEST_TRACKER_LIST_PACKET *request_tracker_list_packet_;
		static bool Check(Buffer buffer);

	protected:
		ListRequestPacket(Buffer buffer);
		ListRequestPacket(UINT32 transaction_id, UINT32 peer_version,
			RID resource_id, Guid peer_guid, UINT16 request_peer_count);
	};

	/**
	*@brief TrackerServer回给Peer的 List 包
	*/
	class ListResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<ListResponsePacket> p;	
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			RID resource_id, vector<CANDIDATE_PEER_INFO> candidate_peer_info);
	public:
		static const UINT8 ACTION = 0x31;
	public:
		inline RID GetResourceID() const{ assert(response_tracker_list_packet_);assert(ErrorCode() ==0);return response_tracker_list_packet_->ResourceID;}
		inline UINT16 GetCandidateCount() const { assert(response_tracker_list_packet_);assert(ErrorCode() ==0);return response_tracker_list_packet_->CandidatePeerCount;}
		vector<CANDIDATE_PEER_INFO> GetCandidatePeerInfo() const;

	private:
		RESPONSE_TRACKER_LIST_PACKET *response_tracker_list_packet_;
		static bool Check(Buffer buffer);

	protected:
		ListResponsePacket(Buffer buffer);
		ListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
			RID resource_id, vector<CANDIDATE_PEER_INFO> candidate_peer_info);
	};

	/**
	*@brief Peer发向TrackerServer的 Commit 包
	*/
	class CommitRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<CommitRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, vector<UINT32> real_ips, vector<RID> resource_ids, UINT16 stun_detected_udp_port);
	public:
		static const UINT8 ACTION = 0x32;
	public:
		inline Guid GetPeerGUID() const {assert(request_tracker_commit_packet_); return request_tracker_commit_packet_->PeerGUID;}
		inline UINT16 GetUdpPort() const {assert(request_tracker_commit_packet_); return request_tracker_commit_packet_->UdpPort;}
		inline UINT16 GetTcpPort() const {assert(request_tracker_commit_packet_); return request_tracker_commit_packet_->TcpPort;}
		inline UINT32 GetStunPeerIP() const {assert(request_tracker_commit_packet_); return request_tracker_commit_packet_->StunPeerIP;}
		inline UINT16 GetStunPeerUdpPort() const {assert(request_tracker_commit_packet_); return request_tracker_commit_packet_->StunPeerUdpPort;}
		inline UINT8 GetRealIPCount() const {assert(request_tracker_commit_packet_); return request_tracker_commit_ip_->RealIPCount;}
		vector<UINT32> GetRealIPs() const;
		inline UINT8 GetResourceCount() const {assert(request_tracker_commit_packet_); return request_tracker_commit_resource_->ResourceCount;}
		vector<RID> GetResourceIDs() const;
		UINT16 GetStunDetectUdpPort() const;
	private:
		REQUEST_TRACKER_COMMIT_PACKET *request_tracker_commit_packet_;
		REQUEST_TRACKER_COMMIT_IP *request_tracker_commit_ip_;
		REQUEST_TRACKER_COMMIT_RESOURCE *request_tracker_commit_resource_;
		REQUEST_TRACKER_COMMIT_STUN *request_tracker_commit_stun_;
		static bool Check(Buffer buffer);
	protected:
		CommitRequestPacket(Buffer buffer);
		CommitRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, vector<UINT32> real_ips, vector<RID> resource_ids, UINT16 stun_detected_udp_port);

	};

	/**
	*@brief TrackerServer回给Peer的 Commit 包
	*/
	class CommitResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<CommitResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port);
	public:
		static const UINT8 ACTION = 0x32;
	public:
		inline UINT16 GetKeepAliveInterval() const{assert(response_tracker_commit_packet_);assert(ErrorCode() ==0);return response_tracker_commit_packet_->KeepAliveInterval;}
		inline UINT32 GetDetectedIP() const{assert(response_tracker_commit_packet_);assert(ErrorCode() ==0);return response_tracker_commit_packet_->DetectedIP;}
		inline UINT32 GetDetectedUdpPort() const{assert(response_tracker_commit_packet_);assert(ErrorCode() ==0);return response_tracker_commit_packet_->DetectedUdpPort;}
	private:
		RESPONSE_TRACKER_COMMIT_PACKET *response_tracker_commit_packet_;
		static bool Check(Buffer buffer);
	protected:
		CommitResponsePacket(Buffer buffer);
		CommitResponsePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port);
	};

	/**
	*@brief Peer发向TrackerServer的 KeepAlive 包
	*/
	class KeepAliveRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<KeepAliveRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port);
	public:
		static const UINT8 ACTION = 0x33;
	public:
		inline Guid GetPeerGUID() const {assert(request_tracker_keepalive_packet_); return request_tracker_keepalive_packet_->PeerGUID;}
		inline UINT32 GetStunPeerIP() const {assert(request_tracker_keepalive_packet_); return request_tracker_keepalive_packet_->StunPeerIP;}
		inline UINT16 GetStunPeerUdpPort() const {assert(request_tracker_keepalive_packet_); return request_tracker_keepalive_packet_->StunPeerUdpPort;}
		UINT16 GetStunDetectUdpPort() const ;
	private:
		REQUEST_TRACKER_KEEPALIVE_PACKET *request_tracker_keepalive_packet_;
		static bool Check(Buffer buffer);
	protected:
		KeepAliveRequestPacket(Buffer buffer);
		KeepAliveRequestPacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port);
	};

	/**
	*@brief TrackerServer回给Peer的 KeepAlive 包
	*/
	class KeepAliveResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<KeepAliveResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count
			);
	public:
		static const UINT8 ACTION = 0x33;
	public:
		inline UINT16 GetKeepAliveInterval() const{assert(response_tracker_keepalive_packet_);assert(ErrorCode() ==0);return response_tracker_keepalive_packet_->KeepALiveInterval;}
		inline UINT32 GetDetectedIP() const{assert(response_tracker_keepalive_packet_);assert(ErrorCode() ==0);return response_tracker_keepalive_packet_->DetectedIP;}
		inline UINT32 GetDetectedUdpPort() const{assert(response_tracker_keepalive_packet_);assert(ErrorCode() ==0);return response_tracker_keepalive_packet_->DetectedUdpPort;}
		inline UINT16 GetResourceCount() const {assert(response_tracker_keepalive_packet_); assert(ErrorCode() ==0);return response_tracker_keepalive_packet_->ResourceCount;}
	private:
		RESPONSE_TRACKER_KEEPALIVE_PACKET *response_tracker_keepalive_packet_;
		static bool Check(Buffer buffer);
	protected:
		KeepAliveResponsePacket (Buffer buffer);
		KeepAliveResponsePacket (UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count);
	};

	/**
	*@brief Peer发向TrackerServer的 Leave 包
	*/
	class LeaveRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<LeaveRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid);
	public:
		static const UINT8 ACTION = 0x34;
	public:
		inline Guid GetPeerGUID() const {assert(request_tracker_leave_packet_); return request_tracker_leave_packet_->PeerGUID;}
	private:
		REQUEST_TRACKER_LEAVE_PACKET *request_tracker_leave_packet_;
		static bool Check(Buffer buffer);
	protected:
		LeaveRequestPacket(Buffer buffer);
		LeaveRequestPacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid);
	};

	/**
	*@brief Peer发向TrackerServer的 Report 包
	*/
	class ReportRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<ReportRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT16 local_resource_count, UINT16 server_resource_count, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port, vector<UINT32> real_ips, vector<REPORT_RESOURCE_STRUCT> resource_ids, UINT8 nat_type);
	public:
		static const UINT8 ACTION = 0x35;
	public:
		inline Guid GetPeerGUID() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->PeerGUID;}
		inline UINT16 GetLocalResourceCount() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->LocalResourceCount;}
		inline UINT16 GetServerResourceCount() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->ServerResourceCount;}
		inline UINT16 GetUdpPort() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->UdpPort;}
		inline UINT16 GetTcpPort() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->TcpPort;}
		inline UINT32 GetStunPeerIP() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->StunPeerIP;}
		inline UINT16 GetStunPeerUdpPort() const {assert(request_tracker_report_packet_); return request_tracker_report_packet_->StunPeerUdpPort;}
		inline UINT16 GetStunDetectUdpPort() const{assert(request_tracker_report_packet_); return request_tracker_report_packet_->StunDetectedUdpPort;};
		inline UINT8 GetRealIPCount() const {assert(request_tracker_report_packet_); return request_tracker_report_ip_->RealIPCount;}
		vector<UINT32> GetRealIPs() const;
		inline UINT8 GetResourceCount() const {assert(request_tracker_report_packet_); return request_tracker_report_resource_->ResourceCount;}
		vector<REPORT_RESOURCE_STRUCT> GetResourceIDs() const;
        inline UINT8 GetPeerNatType() const {assert(request_tracker_report_packet_); return GetPeerVersion() >= 0x00000005 ? request_tracker_report_packet_->PeerNatType : -1; }
	private:
		REQUEST_TRACKER_REPORT_PACKET *request_tracker_report_packet_;
		REQUEST_TRACKER_REPORT_IP *request_tracker_report_ip_;
		REQUEST_TRACKER_REPORT_RESOURCE *request_tracker_report_resource_;
		static bool Check(Buffer buffer);
	protected:
		ReportRequestPacket(Buffer buffer);
		ReportRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT16 local_resource_count, UINT16 server_resource_count, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port, vector<UINT32> real_ips, vector<REPORT_RESOURCE_STRUCT> resource_ids, UINT8 nat_type);

	};

	/**
	*@brief TrackerServer回给Peer的 KeepAlive 包
	*/
	class ReportResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<ReportResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count
			);
	public:
		static const UINT8 ACTION = 0x35;
	public:
		inline UINT16 GetKeepAliveInterval() const{assert(response_tracker_report_packet_);assert(ErrorCode() ==0);return response_tracker_report_packet_->KeepALiveInterval;}
		inline UINT32 GetDetectedIP() const{assert(response_tracker_report_packet_);assert(ErrorCode() ==0);return response_tracker_report_packet_->DetectedIP;}
		inline UINT32 GetDetectedUdpPort() const{assert(response_tracker_report_packet_);assert(ErrorCode() ==0);return response_tracker_report_packet_->DetectedUdpPort;}
		inline UINT16 GetResourceCount() const {assert(response_tracker_report_packet_); assert(ErrorCode() ==0);return response_tracker_report_packet_->ResourceCount;}
	private:
		RESPONSE_TRACKER_KEEPALIVE_PACKET *response_tracker_report_packet_;
		static bool Check(Buffer buffer);
	protected:
		ReportResponsePacket (Buffer buffer);
		ReportResponsePacket (UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count);
	};

    /**
     * @brief Peer向TrackerServer查询资源拥有的PeerCount
     */
    class QueryPeerCountRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryPeerCountRequestPacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid resource_id);
    public:
        static const UINT8 ACTION = 0x36;
    public:
        inline Guid GetResourceID() const { assert(request_tracker_query_peer_count_packet_); return request_tracker_query_peer_count_packet_->ResourceID; }
    private:
        REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET* request_tracker_query_peer_count_packet_;
        static bool Check(Buffer buffer);
    protected:
        QueryPeerCountRequestPacket(Buffer buffer);
        QueryPeerCountRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid resource_id);
    };

    /**
     * @brief Tracker向Peer发送查询结果
     */
    class QueryPeerCountResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryPeerCountResponsePacket> p;
        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT8 error_code, Guid resource_id, UINT32 peer_count);
    public:
        static const UINT8 ACTION = 0x36;
    public:
        inline Guid GetResourceID() const { assert(response_tracker_query_peer_count_packet_); return response_tracker_query_peer_count_packet_->ResourceID; }
        inline UINT32 GetPeerCount() const { assert(response_tracker_query_peer_count_packet_); return response_tracker_query_peer_count_packet_->PeerCount; }
    private:
        RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET* response_tracker_query_peer_count_packet_;
        static bool Check(Buffer buffer);
    protected:
        QueryPeerCountResponsePacket(Buffer buffer);
        QueryPeerCountResponsePacket(UINT32 transaction_id, UINT8 error_code, Guid resource_id, UINT32 peer_count);
    };
}
