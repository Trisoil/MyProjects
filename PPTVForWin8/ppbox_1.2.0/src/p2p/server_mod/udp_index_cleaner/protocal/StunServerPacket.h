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
	*@brief Peer发向StunServer的 QueryStunServerList 包
	*/
	class StunHandShakeRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<StunHandShakeRequestPacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version);
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x71;
	private:
		static bool Check(Buffer buffer);
	protected:
		StunHandShakeRequestPacket(UINT32 transaction_id, UINT32 peer_version);
		StunHandShakeRequestPacket(Buffer buffer);
	};

	/**
	*@brief StunServer回给Peer的 StunHandShakeResponsePacket包
	*/
	class StunHandShakeResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<StunHandShakeResponsePacket> p;	
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port);
		static p CreatePacket(UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port);
	public:
		static const UINT8 ACTION = 0x71;
	public:
		inline UINT16 GetKeepAliveInterval() const {assert(response_stun_handshake_);assert(ErrorCode() ==0);return response_stun_handshake_->KeepALiveInterval;}
		inline UINT32 GetDetectIP() const {assert(response_stun_handshake_);assert(ErrorCode() ==0);return response_stun_handshake_->DetectedIP;}
		inline UINT16 GetDetectPort() const {assert(response_stun_handshake_);assert(ErrorCode() ==0);return response_stun_handshake_->detectedPort;}
	private:
		RESPONSE_STUN_HANDSHAKE* response_stun_handshake_;
		static bool Check(Buffer buffer);
	protected:
		StunHandShakeResponsePacket(Buffer buffer);
		StunHandShakeResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
			UINT16 keepalive_interval, UINT32 detected_ip, UINT16 detected_udp_port);
	};

	/**
	*@brief Peer发向StunServer的 StunKPLPacket 包
	*/
	class StunKPLPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<StunKPLPacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version);
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x72;
	private:
		static bool Check(Buffer buffer);
	protected:
		StunKPLPacket(UINT32 transaction_id, UINT32 peer_version);
		StunKPLPacket(Buffer buffer);
	};

	/**
	*@brief Peer发向StunServer的 StunInvoke 包
	*/
	class StunInvokePacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<StunInvokePacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version
			, RID ResourceID, Guid PeerGuid, UINT32 send_off_time, CANDIDATE_PEER_INFO peer_info_mine, UINT8 peer_type_mine, CANDIDATE_PEER_INFO peer_info_his, PEER_DOWNLOAD_INFO peer_download_info, UINT16 ip_pool_size);
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x73;

		inline RID GetResourceID() const {assert(request_stun_invoke_); return request_stun_invoke_->ResourceID;}
		inline Guid GetPeerGuid() const {assert(request_stun_invoke_); return request_stun_invoke_->PeerGuid;}
		inline UINT32 GetSendOffTime() const {assert(request_stun_invoke_); return request_stun_invoke_->SendOffTime;}
		inline CANDIDATE_PEER_INFO GetCandidatePeerInfoMine() const {assert(request_stun_invoke_); return request_stun_invoke_->CandidatePeerInfoMine;}
		inline UINT8 GetPeerTypeMine() const {assert(request_stun_invoke_); return request_stun_invoke_->PeerTypeMine;}
		inline CANDIDATE_PEER_INFO GetCandidatePeerInfoHis() const {assert(request_stun_invoke_); return request_stun_invoke_->CandidatePeerInfoHis;}
		inline PEER_DOWNLOAD_INFO GetPeerDownloadInfoMine() const {assert(request_stun_invoke_); return request_stun_invoke_->PeerDownloadInfoMine;}
		void SetDetectPort(UINT16 detectport);
		void SetDetectIP(UINT32 detectip);
        UINT16 GetIpPoolSize() const { assert(request_stun_invoke_); return GetPeerVersion() < 0x00000009 ? 0 : request_stun_invoke_->IpPoolSize; }

	private:
		REQUEST_STUN_INVOKE_EX* request_stun_invoke_;
		static bool Check(Buffer buffer);
	protected:
		StunInvokePacket(UINT32 transaction_id, UINT32 peer_version
			, RID ResourceID, Guid PeerGuid, UINT32 send_off_time, CANDIDATE_PEER_INFO peer_info_mine, UINT8 peer_type_mine, CANDIDATE_PEER_INFO peer_info_his, PEER_DOWNLOAD_INFO peer_download_info, UINT16 ip_pool_size );
		StunInvokePacket(Buffer buffer);
	};

}
