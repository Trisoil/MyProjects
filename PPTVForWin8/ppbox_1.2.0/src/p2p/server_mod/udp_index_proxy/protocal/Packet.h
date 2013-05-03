#pragma once

/**
* @file
* @brief 协议包相关的基类
*/

#include "protocal/structs.h"
#include "protocal/Cryptography.h"

using namespace framework;

namespace protocal
{
	const UINT32 PEER_VERSION = 0x00000009;

	class Packet
		: public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<Packet> p; 
        virtual ~Packet() {}

		static p ParseFromBinary(Buffer buffer);
		static const size_t HEADER_LENGTH = Cryptography::CHECKSUM_LENGTH + sizeof(PACKET_HEADER);
		static UINT32 NewTransactionID();
	private:
		static bool Check(Buffer buffer);
    protected:
        static bool CheckAction(Buffer buffer, UINT8 action);
	public:
		inline UINT8 GetAction() const { assert(packet_head_); return packet_head_->Action; }
		inline UINT32 GetTransactionID() const { assert(packet_head_); return packet_head_->TransactionID; }
        inline void SetTransactionID(UINT32 transaction_id) { assert(packet_head_); packet_head_->TransactionID = transaction_id; }
	public:
        //virtual Packet::p Clone() const { return ParseFromBinary(buffer_.Clone()); }
		virtual Buffer GetBuffer() { return buffer_; }
	protected:
		Buffer buffer_;
		PACKET_HEADER* packet_head_;
	protected:
		Packet(Buffer buffer_);
		Packet(size_t append_length, UINT8 action, UINT32 transaction_id);
	};

	/**
	*@brief 和 Peer和 IndexServer 或者 TrackerServer 相关的交互类
	*/
	class ServerPacket : public Packet
	{
	public:
		typedef boost::shared_ptr<ServerPacket> p;
        virtual ~ServerPacket() {}
		static p ParseFromBinary(Buffer buffer);
		static const size_t HEADER_LENGTH = Packet::HEADER_LENGTH + sizeof(SERVER_PACKET_HEADER);
	private:
		static bool Check(Buffer buffer);
	public:
		inline UINT8 IsRequest() const { assert(server_packet_header_); return server_packet_header_->IsRequest; }
	private:
		SERVER_PACKET_HEADER* server_packet_header_;
	protected:
		ServerPacket(Buffer buffer_);
		ServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, bool is_request);
	};

	/**
	*@brief 和 Peer和 Peer
	*/
	class PeerPacket : public Packet
	{
	public:
		typedef boost::shared_ptr<PeerPacket> p;
        virtual ~PeerPacket() {}
		static p ParseFromBinary(Buffer buffer);
		static const size_t HEADER_LENGTH = Packet::HEADER_LENGTH + sizeof(PEER_PACKET_HEADER);
	protected:
		static bool Check(Buffer buffer);
	public:
		inline UINT32 GetSequeceID() const { assert(peer_packet_header_); return peer_packet_header_->SequeceID; }
		inline RID GetResourceID() const { assert(peer_packet_header_); return peer_packet_header_->ResourceID; }
		inline Guid GetPeerGuid() const { assert(peer_packet_header_); return peer_packet_header_->PeerGuid; }
	private:
		PEER_PACKET_HEADER* peer_packet_header_;
	protected:
		PeerPacket(Buffer buffer_);
		PeerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid);
	};

	/**
	*@brief 和 Peer -----------> IndexServer 或者 TrackerServer 交互类
	*/
	class RequestServerPacket : public ServerPacket
	{
	public:
		typedef boost::shared_ptr<RequestServerPacket> p;
        virtual ~RequestServerPacket() {}
		static p ParseFromBinary(Buffer buffer);
		static const size_t HEADER_LENGTH = ServerPacket::HEADER_LENGTH + sizeof(REQUEST_SERVER_PACKET_HEADER);
	private:
		static bool Check(Buffer buffer);
	public:
        inline UINT32 GetPeerVersion() const { assert(request_server_packet_header_); return request_server_packet_header_->PeerVersion; }
	private:
		REQUEST_SERVER_PACKET_HEADER* request_server_packet_header_;
	protected:
		RequestServerPacket(Buffer buffer_);
		RequestServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version);
	};

	/**
	*@brief 和 IndexServer 或者 TrackerServer -----------> Peer 交互类
	*/
	class ResponseServerPacket : public ServerPacket
	{
	public:
		typedef boost::shared_ptr<ResponseServerPacket> p;
        virtual ~ResponseServerPacket() {}
		static p ParseFromBinary(Buffer buffer);
		static const size_t HEADER_LENGTH = ServerPacket::HEADER_LENGTH + sizeof(RESPONSE_SERVER_PACKET_HEADER);

		static p CreatePacket(UINT8 action, UINT32 transaction_id, UINT8 error_code);
	private:
		static bool Check(Buffer buffer);
	public:
		inline UINT8 ErrorCode() const {assert(response_server_packet_header_); return response_server_packet_header_->ErrorCode;};
		inline bool IsError() const { return ErrorCode()>0; }
	private:
		RESPONSE_SERVER_PACKET_HEADER* response_server_packet_header_;
    protected:
        static UINT8 ErrorCode(Buffer buffer) 
        { 
            assert(buffer.length_ >=  ServerPacket::HEADER_LENGTH + sizeof(RESPONSE_SERVER_PACKET_HEADER)); 
            return ((RESPONSE_SERVER_PACKET_HEADER*)(buffer.data_.get() + ServerPacket::HEADER_LENGTH))->ErrorCode; 
        }
	protected:
		ResponseServerPacket(Buffer buffer_);
		ResponseServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT8 error_code);
	};
}

