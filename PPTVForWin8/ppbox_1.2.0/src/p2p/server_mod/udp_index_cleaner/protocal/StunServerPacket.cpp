#include "stdafx.h"
#include "protocal/StunServerPacket.h"
#include "protocal/Packet.h"

namespace protocal
{

	//////////////////////////////////////////////////////////////////////////

	StunHandShakeRequestPacket::StunHandShakeRequestPacket(UINT32 transaction_id, UINT32 peer_version)
		: RequestServerPacket(0, StunHandShakeRequestPacket::ACTION, transaction_id, peer_version)
	{
	}

	StunHandShakeRequestPacket::p StunHandShakeRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version)
	{
		StunHandShakeRequestPacket::p pointer(
			new StunHandShakeRequestPacket(transaction_id, peer_version
			)
			);
		return pointer;
	}

	StunHandShakeRequestPacket::p StunHandShakeRequestPacket::ParseFromBinary(Buffer buffer)
	{
		StunHandShakeRequestPacket::p pointer;
		if( ! StunHandShakeRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new StunHandShakeRequestPacket(buffer));

		return pointer;

	}

	StunHandShakeRequestPacket::StunHandShakeRequestPacket(Buffer buffer)
		:RequestServerPacket(buffer)
	{

	}

	bool StunHandShakeRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH)
			return false;


		if (*(UINT8*)(buffer.data_.get()+4) != protocal::StunHandShakeRequestPacket::ACTION)
			return false;

		return true;
	}


	//////////////////////////////////////////////////////////////////////////

	bool StunHandShakeResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::StunHandShakeResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_STUN_HANDSHAKE) )
			return false;

		return true;
	}

	StunHandShakeResponsePacket::p StunHandShakeResponsePacket::ParseFromBinary(Buffer buffer)
	{
		StunHandShakeResponsePacket::p pointer;
		if( ! StunHandShakeResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new StunHandShakeResponsePacket(buffer));

		return pointer;
	}

	StunHandShakeResponsePacket::StunHandShakeResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( StunHandShakeResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_stun_handshake_ = (RESPONSE_STUN_HANDSHAKE*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	StunHandShakeResponsePacket::p StunHandShakeResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port)
	{
		StunHandShakeResponsePacket::p pointer(
			new StunHandShakeResponsePacket(sizeof(RESPONSE_STUN_HANDSHAKE), transaction_id, error_code, 
			keepalive_interval, detected_ip, detected_udp_port
			)
			);
		return pointer;

	}

	StunHandShakeResponsePacket::p StunHandShakeResponsePacket::CreatePacket(UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port)
	{
		return CreatePacket(Packet::NewTransactionID(),0,keepalive_interval, detected_ip, detected_udp_port);
	}


	StunHandShakeResponsePacket::StunHandShakeResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
		UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port)
		: ResponseServerPacket(this_length, StunHandShakeResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_stun_handshake_ = (RESPONSE_STUN_HANDSHAKE*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_stun_handshake_->KeepALiveInterval = keepalive_interval;
		offset+=2;
		response_stun_handshake_->DetectedIP = detected_ip;
		offset+=4;
		response_stun_handshake_->detectedPort = detected_udp_port;
		offset+=2;
		assert(offset==buffer_.length_);
	}


	//////////////////////////////////////////////////////////////////////////

	StunKPLPacket::StunKPLPacket(UINT32 transaction_id, UINT32 peer_version)
		: RequestServerPacket(0, StunKPLPacket::ACTION, transaction_id, peer_version)
	{
	}

	StunKPLPacket::p StunKPLPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version)
	{
		StunKPLPacket::p pointer(
			new StunKPLPacket(transaction_id, peer_version
			)
			);
		return pointer;
	}

	StunKPLPacket::p StunKPLPacket::ParseFromBinary(Buffer buffer)
	{
		StunKPLPacket::p pointer;
		if( ! StunKPLPacket::Check(buffer) )
			return pointer;
		pointer = p(new StunKPLPacket(buffer));

		return pointer;

	}

	StunKPLPacket::StunKPLPacket(Buffer buffer)
		:RequestServerPacket(buffer)
	{

	}

	bool StunKPLPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH)
			return false;


		if (*(UINT8*)(buffer.data_.get()+4) != protocal::StunKPLPacket::ACTION)
			return false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////

	StunInvokePacket::StunInvokePacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_stun_invoke_(NULL)
	{
		assert( StunInvokePacket::Check(buffer));
		request_stun_invoke_ = (REQUEST_STUN_INVOKE_EX*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	StunInvokePacket::StunInvokePacket(UINT32 transaction_id, UINT32 peer_version,
		RID ResourceID, Guid PeerGuid, UINT32 send_off_time, CANDIDATE_PEER_INFO peer_info_mine, UINT8 peer_type_mine, CANDIDATE_PEER_INFO peer_info_his, PEER_DOWNLOAD_INFO peer_download_info, UINT16 ip_pool_size )
		: RequestServerPacket(sizeof(REQUEST_STUN_INVOKE_EX), StunInvokePacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_stun_invoke_= (REQUEST_STUN_INVOKE_EX*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_stun_invoke_->ResourceID = ResourceID;
		offset+=16;
		request_stun_invoke_->PeerGuid = PeerGuid;
		offset+=16;
		request_stun_invoke_->SendOffTime = send_off_time;
		offset+=4;
		request_stun_invoke_->CandidatePeerInfoMine = peer_info_mine;
		offset+=sizeof(CANDIDATE_PEER_INFO);
		request_stun_invoke_->PeerTypeMine = peer_type_mine;
		offset+=1;
		request_stun_invoke_->CandidatePeerInfoHis = peer_info_his;
		offset+=sizeof(CANDIDATE_PEER_INFO);
		request_stun_invoke_->PeerDownloadInfoMine = peer_download_info;
		offset+=13;
        request_stun_invoke_->IpPoolSize = ip_pool_size;
        offset += sizeof(UINT16);
		assert(buffer_.length_ == offset);
	}

	StunInvokePacket::p StunInvokePacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		RID ResourceID, Guid PeerGuid, UINT32 send_off_time, CANDIDATE_PEER_INFO peer_info_mine, UINT8 peer_type_mine, CANDIDATE_PEER_INFO peer_info_his, PEER_DOWNLOAD_INFO peer_download_info, UINT16 ip_pool_size )
	{
		StunInvokePacket::p pointer(
			new StunInvokePacket(
			    transaction_id, peer_version,
			    ResourceID, PeerGuid, send_off_time, peer_info_mine, peer_type_mine, peer_info_his, peer_download_info, ip_pool_size
			)
		);
		return pointer;
	}

	bool StunInvokePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_STUN_INVOKE) )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::StunInvokePacket::ACTION)
			return false;

        // check version
        UINT32 peer_version = RequestServerPacket::ParseFromBinary(buffer)->GetPeerVersion();
        if (peer_version >= 0x00000009)
        {
            if (buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_STUN_INVOKE_EX))
                return false;
        }

		return true;
	}

	StunInvokePacket::p StunInvokePacket::ParseFromBinary(Buffer buffer)
	{
		StunInvokePacket::p pointer;
		if( ! StunInvokePacket::Check(buffer) )
			return pointer;
		pointer = p(new StunInvokePacket(buffer));

		return pointer;
	}

	void StunInvokePacket::SetDetectPort(UINT16 detectport)
	{
		request_stun_invoke_->CandidatePeerInfoMine.DetectUdpPort = detectport;
	}

	void StunInvokePacket::SetDetectIP(UINT32 detectip)
	{
		request_stun_invoke_->CandidatePeerInfoMine.DetectIP = detectip;

	}


}