#include "stdafx.h"
#include "protocal/Packet.h"

namespace protocal
{
	//////////////////////////////////////////////////////////////////////////

	Packet::Packet(Buffer buffer) 
		: buffer_(buffer)
		, packet_head_(NULL)
	{
		assert( Packet::Check(buffer) );
		packet_head_ = (PACKET_HEADER*)(buffer_.data_.get() + Cryptography::CHECKSUM_LENGTH);
	}

	Packet::Packet(size_t append_length, UINT8 action, UINT32 transaction_id)
		: buffer_(append_length + sizeof(PACKET_HEADER) + Cryptography::CHECKSUM_LENGTH )
	{
		packet_head_ = (PACKET_HEADER*)(buffer_.data_.get() + Cryptography::CHECKSUM_LENGTH);
		packet_head_->Action = action;
		packet_head_->TransactionID = transaction_id;
	}

	bool Packet::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < Cryptography::CHECKSUM_LENGTH + sizeof(PACKET_HEADER) )
			return false;
		return true;
	}

    bool Packet::CheckAction(Buffer buffer, UINT8 action)
    {
        if (Packet::Check(buffer))
        {
            UINT8 buffer_action = *(UINT8*)(buffer.data_.get() + Cryptography::CHECKSUM_LENGTH);
            if (buffer_action == action)
                return true;
        }
        return false;
    }

	Packet::p Packet::ParseFromBinary(Buffer buffer)
	{
		Packet::p pointer;
		if( ! Packet::Check(buffer) )
			return pointer;
		pointer = p(new Packet(buffer));
		return pointer;	
	}

	u_int Packet::NewTransactionID()
	{
		static u_int glocal_transaction_id = 0;
		glocal_transaction_id ++;
		return glocal_transaction_id;
	}

	//////////////////////////////////////////////////////////////////////////

	ServerPacket::ServerPacket(Buffer buffer)
		: Packet(buffer)
		, server_packet_header_(NULL)
	{
		assert( ServerPacket::Check(buffer) );
		server_packet_header_ = (SERVER_PACKET_HEADER*)(buffer_.data_.get() + Packet::HEADER_LENGTH);
	}

	ServerPacket::ServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, bool is_request )
		: Packet(append_length+sizeof(SERVER_PACKET_HEADER), action, transaction_id)
	{
		server_packet_header_ = (SERVER_PACKET_HEADER*)(buffer_.data_.get() + Packet::HEADER_LENGTH);
		if( is_request == true )
			server_packet_header_->IsRequest = 1;
		else
			server_packet_header_->IsRequest = 0;
	}

	bool ServerPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ <  + Packet::HEADER_LENGTH + sizeof(SERVER_PACKET_HEADER) )
			return false;
		return true;
	}

	ServerPacket::p ServerPacket::ParseFromBinary(Buffer buffer)
	{
		ServerPacket::p pointer;
		if( ! ServerPacket::Check(buffer) )
			return pointer;
		pointer = p(new ServerPacket(buffer));
		return pointer;
	}

	//////////////////////////////////////////////////////////////////////////

	PeerPacket::PeerPacket(Buffer buffer)
		: Packet(buffer)
		, peer_packet_header_(NULL)
	{
		assert( PeerPacket::Check(buffer) );
		peer_packet_header_ = (PEER_PACKET_HEADER*)(buffer_.data_.get() + Packet::HEADER_LENGTH);
	}

	PeerPacket::PeerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid )
		: Packet(append_length+sizeof(PEER_PACKET_HEADER), action, transaction_id)
	{
		peer_packet_header_ = (PEER_PACKET_HEADER*)(buffer_.data_.get() + Packet::HEADER_LENGTH);
		peer_packet_header_->SequeceID = sequece_id;
		peer_packet_header_->ResourceID = rid;
		peer_packet_header_->PeerGuid = peer_guid;
	}

	bool PeerPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ <  + Packet::HEADER_LENGTH + sizeof(PEER_PACKET_HEADER) )
			return false;
		return true;
	}

	PeerPacket::p PeerPacket::ParseFromBinary(Buffer buffer)
	{
		PeerPacket::p pointer;
		if( ! PeerPacket::Check(buffer) )
			return pointer;
		pointer = p(new PeerPacket(buffer));
		return pointer;
	}

	//////////////////////////////////////////////////////////////////////////

	RequestServerPacket::RequestServerPacket(Buffer buffer)
		: ServerPacket(buffer)
		, request_server_packet_header_(NULL)
	{
		assert( RequestServerPacket::Check(buffer) );
		request_server_packet_header_ = (REQUEST_SERVER_PACKET_HEADER*)(buffer_.data_.get() + ServerPacket::HEADER_LENGTH);
	}

	RequestServerPacket::RequestServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT32 peer_version )
		: ServerPacket(append_length+sizeof(REQUEST_SERVER_PACKET_HEADER), action, transaction_id, true)
	{
		request_server_packet_header_ = (REQUEST_SERVER_PACKET_HEADER*)(buffer_.data_.get() + ServerPacket::HEADER_LENGTH);
		request_server_packet_header_->PeerVersion = peer_version;
	}

	bool RequestServerPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ <  + ServerPacket::HEADER_LENGTH + sizeof(REQUEST_SERVER_PACKET_HEADER) )
			return false;
		return true;
	}

	RequestServerPacket::p RequestServerPacket::ParseFromBinary(Buffer buffer)
	{
		RequestServerPacket::p pointer;
		if( ! RequestServerPacket::Check(buffer) )
			return pointer;
		pointer = p(new RequestServerPacket(buffer));
		return pointer;
	}

	//////////////////////////////////////////////////////////////////////////

	ResponseServerPacket::ResponseServerPacket(Buffer buffer)
		: ServerPacket(buffer)
		, response_server_packet_header_(NULL)
	{
		assert( ResponseServerPacket::Check(buffer) );
		response_server_packet_header_ = (RESPONSE_SERVER_PACKET_HEADER*)(buffer_.data_.get() + ServerPacket::HEADER_LENGTH);
	}

	ResponseServerPacket::ResponseServerPacket(size_t append_length, UINT8 action, UINT32 transaction_id, UINT8 error_code)
		: ServerPacket(append_length+sizeof(RESPONSE_SERVER_PACKET_HEADER), action, transaction_id, false)
	{
		response_server_packet_header_ = (RESPONSE_SERVER_PACKET_HEADER*)(buffer_.data_.get() + ServerPacket::HEADER_LENGTH);
		response_server_packet_header_->ErrorCode = error_code;
	}

	bool ResponseServerPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ <  + ServerPacket::HEADER_LENGTH + sizeof(RESPONSE_SERVER_PACKET_HEADER) )
			return false;
		return true;
	}

	ResponseServerPacket::p ResponseServerPacket::ParseFromBinary(Buffer buffer)
	{
		ResponseServerPacket::p pointer;
		if( ! ResponseServerPacket::Check(buffer) )
			return pointer;
		pointer = p(new ResponseServerPacket(buffer));
		return pointer;
	}

	ResponseServerPacket::p ResponseServerPacket::CreatePacket(UINT8 action, UINT32 transaction_id, UINT8 error_code)
	{

		ResponseServerPacket::p pointer (
			new ResponseServerPacket(
			0,action,transaction_id,error_code
			));
		return pointer;
	}



}