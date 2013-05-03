#include "stdafx.h"
#include "protocal/TrackerPacket.h"
#include "protocal/Packet.h"

namespace protocal
{
	ListRequestPacket::ListRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_tracker_list_packet_(NULL)
	{
		assert( ListRequestPacket::Check(buffer));
		request_tracker_list_packet_ = (REQUEST_TRACKER_LIST_PACKET*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	ListRequestPacket::ListRequestPacket(UINT32 transaction_id, UINT32 peer_version,
		RID resource_id, Guid peer_guid, UINT16 request_peer_count)
		: RequestServerPacket(sizeof(REQUEST_TRACKER_LIST_PACKET), ListRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_tracker_list_packet_= (REQUEST_TRACKER_LIST_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_list_packet_->ResourceID = resource_id;
		offset+=16;
		request_tracker_list_packet_->PeerGUID = peer_guid;
		offset+=16;
		request_tracker_list_packet_->RequestPeerCount = request_peer_count;
		offset+= 2;
		assert(buffer_.length_ == offset);
	}

	ListRequestPacket::p ListRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		RID resource_id, Guid peer_guid, UINT16 request_peer_count)
	{
		ListRequestPacket::p pointer(
			new ListRequestPacket(
			transaction_id, peer_version,
			resource_id, peer_guid, request_peer_count
			)
			);
		return pointer;
	}

	bool ListRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		//UINT a = buffer.length_;
		UINT b = RequestServerPacket::HEADER_LENGTH + 34;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + 34 )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ListRequestPacket::ACTION)
			return false;

		return true;
	}

	ListRequestPacket::p ListRequestPacket::ParseFromBinary(Buffer buffer)
	{
		ListRequestPacket::p pointer;
		if( ! ListRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new ListRequestPacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	vector<CANDIDATE_PEER_INFO> ListResponsePacket::GetCandidatePeerInfo() const
	{
		assert(response_tracker_list_packet_);
		vector<CANDIDATE_PEER_INFO> candidate_peer_info_v;
		for (UINT i = 0; i < response_tracker_list_packet_->CandidatePeerCount; i ++)
			candidate_peer_info_v.push_back(response_tracker_list_packet_->CandidatePeerInfo[i]);
		return candidate_peer_info_v;
	}

	ListResponsePacket::ListResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
		, response_tracker_list_packet_(NULL)
	{
		assert( ListResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_tracker_list_packet_ = (RESPONSE_TRACKER_LIST_PACKET*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	ListResponsePacket::ListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
		RID resource_id, vector<CANDIDATE_PEER_INFO> candidate_peer_info)
		: ResponseServerPacket(this_length, ListResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_tracker_list_packet_= (RESPONSE_TRACKER_LIST_PACKET*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_tracker_list_packet_->ResourceID = resource_id;
		offset+=16;
		response_tracker_list_packet_->CandidatePeerCount = candidate_peer_info.size();
		offset+=2;
		for (UINT i = 0; i < candidate_peer_info.size(); i ++)
		{
			response_tracker_list_packet_->CandidatePeerInfo[i] = candidate_peer_info[i];
			offset+= sizeof(CANDIDATE_PEER_INFO);
		}
		assert(buffer_.length_ == offset);
	}

	ListResponsePacket::p ListResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		RID resource_id, vector<CANDIDATE_PEER_INFO> candidate_peer_info)
	{
		size_t this_length;
		this_length = sizeof(CANDIDATE_PEER_INFO) * candidate_peer_info.size() + 18;
		ListResponsePacket::p pointer(
			new ListResponsePacket(
			this_length, transaction_id, error_code, resource_id, candidate_peer_info
			)
			);
		return pointer;
	}

	bool ListResponsePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + 18 )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ListResponsePacket::ACTION)
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		UINT16 candidate_peer_count_;
		candidate_peer_count_ = *(UINT16 *) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH + 16);

		if( buffer.length_ < ListResponsePacket::HEADER_LENGTH + 18 + candidate_peer_count_ * sizeof(CANDIDATE_PEER_INFO))
			return false;
		

		return true;
	}

	ListResponsePacket::p ListResponsePacket::ParseFromBinary(Buffer buffer)
	{
		ListResponsePacket::p pointer;
		if( ! ListResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new ListResponsePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	vector<UINT32> CommitRequestPacket::GetRealIPs() const
	{
		assert(request_tracker_commit_packet_);
		vector<UINT32> real_ips_v;
		for (UINT i = 0; i < request_tracker_commit_ip_->RealIPCount; i ++)
			real_ips_v.push_back(request_tracker_commit_ip_->RealIPs[i]);
		return real_ips_v;
	}

	vector<RID> CommitRequestPacket::GetResourceIDs() const
	{
		assert(request_tracker_commit_packet_);
		vector<RID> resource_ids_v;
		for (UINT i = 0; i < request_tracker_commit_resource_->ResourceCount; i ++)
			resource_ids_v.push_back(request_tracker_commit_resource_->ResourceIDs[i]);
		return resource_ids_v;
	}

	UINT16 CommitRequestPacket::GetStunDetectUdpPort() const 
	{
		if (GetPeerVersion() >= 0x00000002)
		{
			assert(request_tracker_commit_stun_);
			return request_tracker_commit_stun_->StunDetectUdpPort;
		}
		else 
			return 0;
	}


	CommitRequestPacket::CommitRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_tracker_commit_packet_(NULL)
	{
		assert( CommitRequestPacket::Check(buffer));
		request_tracker_commit_packet_ =   (REQUEST_TRACKER_COMMIT_PACKET*)   (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_commit_ip_ =       (REQUEST_TRACKER_COMMIT_IP*)       (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET));
		request_tracker_commit_resource_ = (REQUEST_TRACKER_COMMIT_RESOURCE*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET)+ request_tracker_commit_ip_->RealIPCount* sizeof(UINT32)+ 1);
		if (GetPeerVersion() >= 0x00000002)
			request_tracker_commit_stun_ = (REQUEST_TRACKER_COMMIT_STUN*)     (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET)+ request_tracker_commit_ip_->RealIPCount* sizeof(UINT32)+ 1 + request_tracker_commit_resource_->ResourceCount*16 + 1);
	}

	CommitRequestPacket::CommitRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, vector<UINT32> real_ips, vector<RID> resource_ids, UINT16 stun_detected_udp_port)
		: RequestServerPacket(this_length, CommitRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_tracker_commit_packet_= (REQUEST_TRACKER_COMMIT_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_commit_packet_-> PeerGUID = peer_guid;
		offset+=16;
		request_tracker_commit_packet_->UdpPort = udp_port;
		offset+=2;
		request_tracker_commit_packet_->TcpPort = tcp_port;
		offset+=2;
		request_tracker_commit_packet_->StunPeerIP = stun_peer_ip;
		offset+=4;
		request_tracker_commit_packet_->StunPeerUdpPort = stun_peer_udpport;
		offset+=2;
		request_tracker_commit_ip_ = (REQUEST_TRACKER_COMMIT_IP*) (buffer_.data_.get() + offset);
		request_tracker_commit_ip_->RealIPCount = real_ips.size();
		offset+=1;
		for (UINT i = 0; i < real_ips.size(); i ++)
		{
			request_tracker_commit_ip_->RealIPs[i] = real_ips[i];
			offset += 4;
		}
		request_tracker_commit_resource_ = (REQUEST_TRACKER_COMMIT_RESOURCE*) (buffer_.data_.get() + offset);
		request_tracker_commit_resource_->ResourceCount = resource_ids.size();
		offset+=1;
		for (UINT i = 0; i < resource_ids.size(); i ++)
		{
			request_tracker_commit_resource_->ResourceIDs[i] = resource_ids[i];
			offset += 16;
		}
		request_tracker_commit_stun_ = (REQUEST_TRACKER_COMMIT_STUN *) (buffer_.data_.get() + offset);
		request_tracker_commit_stun_->StunDetectUdpPort = stun_detected_udp_port;
		offset += 2;
		assert(buffer_.length_ == offset);
	}

	CommitRequestPacket::p CommitRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, vector<UINT32> real_ips, vector<RID> resource_ids, UINT16 stun_detected_udp_port)
	{
		size_t this_length;

		UINT a = sizeof(REQUEST_TRACKER_COMMIT_PACKET);
		this_length = sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1 + 4 * real_ips.size() + 1 + 16 * resource_ids.size() + sizeof(REQUEST_TRACKER_COMMIT_STUN);
		CommitRequestPacket::p pointer(
			new CommitRequestPacket(this_length, transaction_id, peer_version,
			peer_guid, udp_port, tcp_port, stun_peer_ip, stun_peer_udpport, real_ips, resource_ids, stun_detected_udp_port
			)
			);
		return pointer;
	}

	bool CommitRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1)
			return false;

		UINT8 real_ip_count_ = *(UINT8 *) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET));

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1 + real_ip_count_ * 4 + 1)
			return false;
		
		UINT8 resource_count_ = *(UINT8 *) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1 + real_ip_count_ * 4 );

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1 + real_ip_count_ * 4 + 1 + resource_count_ * 16)
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::CommitRequestPacket::ACTION)
			return false;

		//if (GetPeerVersion() >= 0x00000206)
		//	if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_COMMIT_PACKET) + 1 + real_ip_count_ * 4 + 1 + resource_count_ * 16 + sizeof(REQUEST_TRACKER_COMMIT_STUN))
		//		return false;

		return true;
	}

	CommitRequestPacket::p CommitRequestPacket::ParseFromBinary(Buffer buffer)
	{
		CommitRequestPacket::p pointer;
		if( ! CommitRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new CommitRequestPacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	CommitResponsePacket::CommitResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
		, response_tracker_commit_packet_(NULL)
	{
		assert( CommitResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_tracker_commit_packet_ = (RESPONSE_TRACKER_COMMIT_PACKET*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	CommitResponsePacket::CommitResponsePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port)
		: ResponseServerPacket(sizeof(RESPONSE_TRACKER_COMMIT_PACKET), CommitResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_tracker_commit_packet_= (RESPONSE_TRACKER_COMMIT_PACKET*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_tracker_commit_packet_->KeepAliveInterval = keepalive_interval;
		offset+=2;
		response_tracker_commit_packet_->DetectedIP = detected_ip;
		offset+=4;
		response_tracker_commit_packet_->DetectedUdpPort = detected_udp_port;
		offset+=2;
		assert(buffer_.length_ == offset);
	}

	CommitResponsePacket::p CommitResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port)
	{
		CommitResponsePacket::p pointer(
			new CommitResponsePacket(
			transaction_id, error_code, keepalive_interval, detected_ip, detected_udp_port
			)
			);
		return pointer;
	}

	bool CommitResponsePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::CommitResponsePacket::ACTION)
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_TRACKER_COMMIT_PACKET) )
			return false;

		return true;
	}

	CommitResponsePacket::p CommitResponsePacket::ParseFromBinary(Buffer buffer)
	{
		CommitResponsePacket::p pointer;
		if( ! CommitResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new CommitResponsePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////
	UINT16 KeepAliveRequestPacket::GetStunDetectUdpPort() const 
	{
		if (GetPeerVersion() >= 0x00000002)
		{
			assert(request_tracker_keepalive_packet_); 
			return request_tracker_keepalive_packet_->StunDetectUdpPort;
		}
		else
			return 0;
	}

	KeepAliveRequestPacket::KeepAliveRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_tracker_keepalive_packet_(NULL)
	{
		assert( KeepAliveRequestPacket::Check(buffer));
		request_tracker_keepalive_packet_ = (REQUEST_TRACKER_KEEPALIVE_PACKET*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	KeepAliveRequestPacket::KeepAliveRequestPacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port)
		: RequestServerPacket(sizeof(REQUEST_TRACKER_KEEPALIVE_PACKET), KeepAliveRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_tracker_keepalive_packet_= (REQUEST_TRACKER_KEEPALIVE_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_keepalive_packet_->PeerGUID = peer_guid;
		offset+=16;
		request_tracker_keepalive_packet_->StunPeerIP = stun_peer_ip;
		offset+=4;
		request_tracker_keepalive_packet_->StunPeerUdpPort = stun_peer_udpport;
		offset+= 2;
		request_tracker_keepalive_packet_->StunDetectUdpPort = stun_detected_udp_port;
		offset+= 2;
		assert(buffer_.length_ == offset);
	}

	KeepAliveRequestPacket::p KeepAliveRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port)
	{
		KeepAliveRequestPacket::p pointer(
			new KeepAliveRequestPacket(
			transaction_id, peer_version,
			peer_guid, stun_peer_ip, stun_peer_udpport, stun_detected_udp_port
			)
			);
		return pointer;
	}

	bool KeepAliveRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		//UINT a = buffer.length_;
		//UINT b = PeerPacket::HEADER_LENGTH + sizeof(ErrorPacket);

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_KEEPALIVE_PACKET) - 2)
			return false;

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH)
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::KeepAliveRequestPacket::ACTION)
			return false;

		return true;
	}

	KeepAliveRequestPacket::p KeepAliveRequestPacket::ParseFromBinary(Buffer buffer)
	{
		KeepAliveRequestPacket::p pointer;
		if( ! KeepAliveRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new KeepAliveRequestPacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	KeepAliveResponsePacket::KeepAliveResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
		, response_tracker_keepalive_packet_(NULL)
	{
		assert( KeepAliveResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_tracker_keepalive_packet_ = (RESPONSE_TRACKER_KEEPALIVE_PACKET*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	KeepAliveResponsePacket::KeepAliveResponsePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count)
		: ResponseServerPacket(sizeof(RESPONSE_TRACKER_KEEPALIVE_PACKET), KeepAliveResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_tracker_keepalive_packet_= (RESPONSE_TRACKER_KEEPALIVE_PACKET*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_tracker_keepalive_packet_->KeepALiveInterval = keepalive_interval;
		offset+=2;
		response_tracker_keepalive_packet_->DetectedIP = detected_ip;
		offset+=4;
		response_tracker_keepalive_packet_->DetectedUdpPort = detected_udp_port;
		offset += 2;
		response_tracker_keepalive_packet_->ResourceCount = resource_count;
		offset += 2;
		assert(buffer_.length_ == offset);
	}

	KeepAliveResponsePacket::p KeepAliveResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count)
	{
		KeepAliveResponsePacket::p pointer(
			new KeepAliveResponsePacket(
			transaction_id, error_code,
			keepalive_interval, detected_ip, detected_udp_port, resource_count
			)
			);
		return pointer;
	}

	bool KeepAliveResponsePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::KeepAliveResponsePacket::ACTION)
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_TRACKER_KEEPALIVE_PACKET) )
			return false;

		return true;
	}

	KeepAliveResponsePacket::p KeepAliveResponsePacket::ParseFromBinary(Buffer buffer)
	{
		KeepAliveResponsePacket::p pointer;
		if( ! KeepAliveResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new KeepAliveResponsePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	LeaveRequestPacket::LeaveRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_tracker_leave_packet_(NULL)
	{
		assert( LeaveRequestPacket::Check(buffer));
		request_tracker_leave_packet_ = (REQUEST_TRACKER_LEAVE_PACKET*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	LeaveRequestPacket::LeaveRequestPacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid)
		: RequestServerPacket(sizeof(REQUEST_TRACKER_LEAVE_PACKET), LeaveRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_tracker_leave_packet_= (REQUEST_TRACKER_LEAVE_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_leave_packet_->PeerGUID = peer_guid;
		offset+=16;
		assert(buffer_.length_ == offset);
	}

	LeaveRequestPacket::p LeaveRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid)
	{
		LeaveRequestPacket::p pointer(
			new LeaveRequestPacket(
			transaction_id, peer_version,
			peer_guid
			)
			);
		return pointer;
	}

	bool LeaveRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		//UINT a = buffer.length_;
		//UINT b = PeerPacket::HEADER_LENGTH + sizeof(ErrorPacket);
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_LEAVE_PACKET) )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::LeaveRequestPacket::ACTION)
			return false;

		return true;
	}

	LeaveRequestPacket::p LeaveRequestPacket::ParseFromBinary(Buffer buffer)
	{
		LeaveRequestPacket::p pointer;
		if( ! LeaveRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new LeaveRequestPacket(buffer));

		return pointer;
	}
//////////////////////////////////////////////////////////////////////////

	vector<UINT32> ReportRequestPacket::GetRealIPs() const
	{
		assert(request_tracker_report_ip_);
		vector<UINT32> real_ips_v;
		for (UINT i = 0; i < request_tracker_report_ip_->RealIPCount; i ++)
			real_ips_v.push_back(request_tracker_report_ip_->RealIPs[i]);
		return real_ips_v;
	}

	vector<REPORT_RESOURCE_STRUCT> ReportRequestPacket::GetResourceIDs() const
	{
		assert(request_tracker_report_resource_);
		vector<REPORT_RESOURCE_STRUCT> resource_ids_v;
		for (UINT i = 0; i < request_tracker_report_resource_->ResourceCount; i ++)
			resource_ids_v.push_back(request_tracker_report_resource_->ReportResources[i]);
		return resource_ids_v;
	}

	ReportRequestPacket::ReportRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
	{
		assert( ReportRequestPacket::Check(buffer));
		request_tracker_report_packet_ =   (REQUEST_TRACKER_REPORT_PACKET*)   (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_report_ip_ =       (REQUEST_TRACKER_REPORT_IP*)       (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET));
		request_tracker_report_resource_ = (REQUEST_TRACKER_REPORT_RESOURCE*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET)+ request_tracker_report_ip_->RealIPCount* sizeof(UINT32)+ 1);
	}

	ReportRequestPacket::ReportRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT16 local_resource_count, UINT16 server_resource_count, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port, vector<UINT32> real_ips, vector<REPORT_RESOURCE_STRUCT> resource_ids, UINT8 nat_type)
		: RequestServerPacket(this_length, ReportRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_tracker_report_packet_= (REQUEST_TRACKER_REPORT_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_tracker_report_packet_->PeerGUID = peer_guid;
		offset+=16;
		request_tracker_report_packet_->LocalResourceCount = local_resource_count;
		offset+=2;
		request_tracker_report_packet_->ServerResourceCount= server_resource_count;
		offset+=2;
        request_tracker_report_packet_->PeerNatType = nat_type;
        offset+=1;
		offset+=5; // reserved
		request_tracker_report_packet_->UdpPort = udp_port;
		offset+=2;
		request_tracker_report_packet_->TcpPort = tcp_port;
		offset+=2;
		request_tracker_report_packet_->StunPeerIP = stun_peer_ip;
		offset+=4;
		request_tracker_report_packet_->StunPeerUdpPort = stun_peer_udpport;
		offset+=2;
		request_tracker_report_packet_->StunDetectedUdpPort = stun_detected_udp_port;
		offset+=2;
		request_tracker_report_ip_ = (REQUEST_TRACKER_REPORT_IP*) (buffer_.data_.get() + offset);
		request_tracker_report_ip_->RealIPCount = real_ips.size();
		offset+=1;
		for (UINT i = 0; i < real_ips.size(); i ++)
		{
			request_tracker_report_ip_->RealIPs[i] = real_ips[i];
			offset += 4;
		}
		request_tracker_report_resource_ = (REQUEST_TRACKER_REPORT_RESOURCE*) (buffer_.data_.get() + offset);
		request_tracker_report_resource_->ResourceCount = resource_ids.size();
		offset+=1;
		for (UINT i = 0; i < resource_ids.size(); i ++)
		{
			request_tracker_report_resource_->ReportResources[i] = resource_ids[i];
			offset += sizeof (REPORT_RESOURCE_STRUCT);
		}
		assert(buffer_.length_ == offset);
	}

	ReportRequestPacket::p ReportRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT16 local_resource_count, UINT16 server_resource_count, UINT16 udp_port, UINT16 tcp_port, UINT32 stun_peer_ip, UINT16 stun_peer_udpport, UINT16 stun_detected_udp_port, vector<UINT32> real_ips, vector<REPORT_RESOURCE_STRUCT> resource_ids, UINT8 nat_type)
	{
		size_t this_length;

		this_length = sizeof(REQUEST_TRACKER_REPORT_PACKET) + 1 + 4 * real_ips.size() + 1 + sizeof(REPORT_RESOURCE_STRUCT) * resource_ids.size();
		ReportRequestPacket::p pointer(
			new ReportRequestPacket(this_length, transaction_id, peer_version,
			    peer_guid, local_resource_count, server_resource_count, udp_port, tcp_port, stun_peer_ip, stun_peer_udpport, stun_detected_udp_port, real_ips, resource_ids, nat_type
			)
		);
		return pointer;
	}

	bool ReportRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET) + 1)
			return false;

		UINT8 real_ip_count_ = *(UINT8 *) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET));

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET) + 1 + real_ip_count_ * 4 + 1)
			return false;
		
		UINT8 resource_count_ = *(UINT8 *) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET) + 1 + real_ip_count_ * 4 );

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_REPORT_PACKET) + 1 + real_ip_count_ * 4 + 1 + resource_count_ * 17)
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ReportRequestPacket::ACTION)
			return false;

		return true;
	}

	ReportRequestPacket::p ReportRequestPacket::ParseFromBinary(Buffer buffer)
	{
		ReportRequestPacket::p pointer;
		if( ! ReportRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new ReportRequestPacket(buffer));

		return pointer;
	}

	//////////////////////////////////////////////////////////////////////////

	ReportResponsePacket::ReportResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
		, response_tracker_report_packet_(NULL)
	{
		assert( ReportResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_tracker_report_packet_ = (RESPONSE_TRACKER_KEEPALIVE_PACKET*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	ReportResponsePacket::ReportResponsePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count)
		: ResponseServerPacket(sizeof(RESPONSE_TRACKER_KEEPALIVE_PACKET), ReportResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_tracker_report_packet_= (RESPONSE_TRACKER_KEEPALIVE_PACKET*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_tracker_report_packet_->KeepALiveInterval = keepalive_interval;
		offset+=2;
		response_tracker_report_packet_->DetectedIP = detected_ip;
		offset+=4;
		response_tracker_report_packet_->DetectedUdpPort = detected_udp_port;
		offset += 2;
		response_tracker_report_packet_->ResourceCount = resource_count;
		offset += 2;
		assert(buffer_.length_ == offset);
	}

	ReportResponsePacket::p ReportResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port, UINT16 resource_count)
	{
		ReportResponsePacket::p pointer(
			new ReportResponsePacket(
			transaction_id, error_code,
			keepalive_interval, detected_ip, detected_udp_port, resource_count
			)
			);
		return pointer;
	}

	bool ReportResponsePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ReportResponsePacket::ACTION)
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_TRACKER_KEEPALIVE_PACKET) )
			return false;

		return true;
	}

	ReportResponsePacket::p ReportResponsePacket::ParseFromBinary(Buffer buffer)
	{
		ReportResponsePacket::p pointer;
		if( ! ReportResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new ReportResponsePacket(buffer));

		return pointer;
	}


    //////////////////////////////////////////////////////////////////////////

    QueryPeerCountRequestPacket::QueryPeerCountRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_tracker_query_peer_count_packet_(NULL)
    {
        assert( QueryPeerCountRequestPacket::Check(buffer) );
        request_tracker_query_peer_count_packet_ = (REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }

    QueryPeerCountRequestPacket::QueryPeerCountRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid resource_id)
        : RequestServerPacket(sizeof(REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET), QueryPeerCountRequestPacket::ACTION, transaction_id, peer_version)
        , request_tracker_query_peer_count_packet_(NULL)
    {
        request_tracker_query_peer_count_packet_ = (REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_tracker_query_peer_count_packet_->ResourceID = resource_id;
    }

    QueryPeerCountRequestPacket::p QueryPeerCountRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryPeerCountRequestPacket::p packet;
        if (QueryPeerCountRequestPacket::Check(buffer))
        {
            packet = QueryPeerCountRequestPacket::p(new QueryPeerCountRequestPacket(buffer));
        }
        return packet;
    }

    QueryPeerCountRequestPacket::p QueryPeerCountRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid resource_id)
    {
        return QueryPeerCountRequestPacket::p(new QueryPeerCountRequestPacket(transaction_id, peer_version, resource_id));
    }

    bool QueryPeerCountRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ != RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET))
            return false;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    QueryPeerCountResponsePacket::QueryPeerCountResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_tracker_query_peer_count_packet_(NULL)
    {
        assert( QueryPeerCountResponsePacket::Check(buffer_) );
        response_tracker_query_peer_count_packet_ = (RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
    }

    QueryPeerCountResponsePacket::QueryPeerCountResponsePacket(UINT32 transaction_id, UINT8 error_code, Guid resource_id, UINT32 peer_count)
        : ResponseServerPacket(sizeof(RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET), QueryPeerCountResponsePacket::ACTION, transaction_id, error_code)
    {
        response_tracker_query_peer_count_packet_ = (RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        response_tracker_query_peer_count_packet_->PeerCount = peer_count;
        response_tracker_query_peer_count_packet_->ResourceID = resource_id;
    }

    QueryPeerCountResponsePacket::p QueryPeerCountResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryPeerCountResponsePacket::p packet;
        if (QueryPeerCountResponsePacket::Check(buffer))
        {
            packet = QueryPeerCountResponsePacket::p(new QueryPeerCountResponsePacket(buffer));
        }
        return packet;
    }

    QueryPeerCountResponsePacket::p QueryPeerCountResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code, Guid resource_id, UINT32 peer_count)
    {
        return QueryPeerCountResponsePacket::p(new QueryPeerCountResponsePacket(transaction_id, error_code, resource_id, peer_count));
    }

    bool QueryPeerCountResponsePacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        if (buffer.length_ != ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET))
            return false;
        return true;
    }
}
