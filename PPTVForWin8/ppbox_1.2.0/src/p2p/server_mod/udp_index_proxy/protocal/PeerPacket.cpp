#include "stdafx.h"
#include "protocal/PeerPacket.h"

namespace protocal
{
	ErrorPacket::ErrorPacket(Buffer buffer)
		: PeerPacket(buffer)
		, error_packet_(NULL)
	{
		assert( ErrorPacket::Check(buffer));
		error_packet_ = (ERROR_PACKET*) (buffer.data_.get() + PeerPacket::HEADER_LENGTH);
	}


	ErrorPacket::ErrorPacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT16 error_code, UINT16 error_info_length, Buffer error_info)
		: PeerPacket(this_length, ErrorPacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		size_t offset = PeerPacket::HEADER_LENGTH;
		error_packet_ = (ERROR_PACKET *) (buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		error_packet_->ErrorCode = error_code;
		offset+=2;
		error_packet_->ErrorInfoLength = error_info_length;
		offset+=2;
		memcpy(error_packet_->ErrorInfo, error_info.data_.get(), error_info_length);
		offset+= error_info_length;
		assert(buffer_.length_ == offset);
	}

	ErrorPacket::p ErrorPacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT16 error_code, UINT16 error_info_length, Buffer error_info)
	{
		size_t this_length;
		this_length = 4 + error_info_length;

		ErrorPacket::p pointer(
			new ErrorPacket(
				this_length, transaction_id, sequece_id, rid, peer_guid,
				error_code, error_info_length, error_info
			)
			);
		return pointer;
	}

	bool ErrorPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		//UINT a = buffer.length_;
		//UINT b = PeerPacket::HEADER_LENGTH + sizeof(ErrorPacket);
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 4 )
			return false;
		
		UINT16 error_length = *(UINT16*)(buffer.data_.get() + PeerPacket::HEADER_LENGTH + 2);

		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 4 + sizeof(UINT8) * error_length)
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ErrorPacket::ACTION)
			return false;

		return true;
	}

	ErrorPacket::p ErrorPacket::ParseFromBinary(Buffer buffer)
	{
		ErrorPacket::p pointer;
		if( ! ErrorPacket::Check(buffer) )
			return pointer;
		pointer = p(new ErrorPacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	ConnectPacket::ConnectPacket(Buffer buffer)
		: PeerPacket(buffer)
		, connect_packet_(NULL)
	{
		assert( ConnectPacket::Check(buffer) );
		connect_packet_ = (CONNECT_PACKET_EX*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
	}

	ConnectPacket::ConnectPacket(
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT8 basic_info, UINT32 send_off_time, UINT32 peer_version, 
		CANDIDATE_PEER_INFO peer_info, UINT8 peer_type, PEER_DOWNLOAD_INFO download_info, UINT16 ip_pool_size )
		: PeerPacket(sizeof(CONNECT_PACKET_EX), ConnectPacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		connect_packet_ = (CONNECT_PACKET_EX*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		connect_packet_->BasicInfo = basic_info;
		connect_packet_->SendOffTime = send_off_time;
		connect_packet_->PeerVersion = peer_version;
		connect_packet_->CandidatePeerInfo = peer_info;
		connect_packet_->PeerType = peer_type;
		connect_packet_->PeerDownloadInfo = download_info;
        connect_packet_->IpPoolSize = ip_pool_size;
	}

	ConnectPacket::p ConnectPacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT8 basic_info, UINT32 send_off_time, UINT32 peer_version, 
		CANDIDATE_PEER_INFO peer_info, UINT8 peer_type, PEER_DOWNLOAD_INFO download_info, UINT16 ip_pool_size  )
	{
		ConnectPacket::p pointer(
			new ConnectPacket(
				transaction_id, sequece_id, rid, peer_guid,
				basic_info, send_off_time, peer_version,
				peer_info, peer_type, download_info, ip_pool_size
			) 
		);
		return pointer;
	}

	bool ConnectPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + sizeof(CONNECT_PACKET) )
			return false;
		//UINT8 a = (UINT8)(buffer.data_.get()+4);
		//UINT8 b = protocal::ConnectPacket::ACTION;
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::ConnectPacket::ACTION)
			return false;

        // new version
        const CONNECT_PACKET* packet = (const CONNECT_PACKET*)(buffer.data_.get() + PeerPacket::HEADER_LENGTH);
        if (packet->PeerVersion >= 0x00000009)
        {
            if (buffer.length_ < PeerPacket::HEADER_LENGTH + sizeof(CONNECT_PACKET_EX))
                return false;
        }

		return true;
	}

	ConnectPacket::p ConnectPacket::ParseFromBinary(Buffer buffer)
	{
		ConnectPacket::p pointer;
		if( ! ConnectPacket::Check(buffer) )
			return pointer;
		pointer = p(new ConnectPacket(buffer));
		
		return pointer;
	}

	//////////////////////////////////////////////////////////////////////////
	BlockMap::p AnnouncePacket::GetBlockMap() const
	{
		assert(announce_packet_);
		TempBuffer temp_buffer( buffer_, (byte*)(announce_packet_->BlockMap) );
		return BlockMap::ParseFromBinary(temp_buffer);
	}

	AnnouncePacket::AnnouncePacket(Buffer buffer)
		: PeerPacket(buffer)
		, announce_packet_(NULL)
	{
		assert( AnnouncePacket::Check(buffer) );
		announce_packet_ = (ANNOUNCE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
	}

	AnnouncePacket::AnnouncePacket(
		size_t this_length,
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		PEER_DOWNLOAD_INFO peer_download_info, BlockMap::p block_map)
		: PeerPacket(this_length, AnnouncePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		announce_packet_ = (ANNOUNCE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		announce_packet_ ->peer_download_info= peer_download_info;
		// BlockMap 不在此处构造，在 CreatePacket 的时候构造
	}

	AnnouncePacket::p AnnouncePacket::CreatePacket(
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		PEER_DOWNLOAD_INFO peer_download_info, BlockMap::p block_map)
	{
		size_t this_length;
		Buffer block_map_buffer = block_map->MakeToBinary();
		this_length = sizeof(PEER_DOWNLOAD_INFO) + block_map_buffer.length_;
		AnnouncePacket::p pointer(
			new AnnouncePacket(
				this_length,
				transaction_id, sequece_id, rid, peer_guid,
				peer_download_info, block_map
			) 
		);
		assert( pointer->buffer_.length_ == PeerPacket::HEADER_LENGTH + sizeof(PEER_DOWNLOAD_INFO) + block_map_buffer.length_ );
		memcpy( pointer->buffer_.data_.get() + PeerPacket::HEADER_LENGTH + sizeof(PEER_DOWNLOAD_INFO), block_map_buffer.data_.get(), block_map_buffer.length_);
		return pointer;
	}

	bool AnnouncePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + sizeof(ANNOUNCE_PACKET) )
			return false;
		//UINT8 a = (UINT8)(buffer.data_.get()+4);
		//UINT8 b = protocal::ConnectPacket::ACTION;
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::AnnouncePacket::ACTION)
			return false;
		return true;
	}

	AnnouncePacket::p AnnouncePacket::ParseFromBinary(Buffer buffer)
	{
		AnnouncePacket::p pointer;
		if( ! AnnouncePacket::Check(buffer) )
			return pointer;
		pointer = p(new AnnouncePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////
	RequestAnnouncePacket::RequestAnnouncePacket(Buffer buffer)
		: PeerPacket(buffer)
	{
		assert( RequestAnnouncePacket::Check(buffer) );
	}

	RequestAnnouncePacket::RequestAnnouncePacket(
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid)
		: PeerPacket(0, RequestAnnouncePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
	}

	RequestAnnouncePacket::p RequestAnnouncePacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid)
	{
		RequestAnnouncePacket::p pointer(
			new RequestAnnouncePacket(
			transaction_id, sequece_id, rid, peer_guid
			) 
			);
		return pointer;
	}

	bool RequestAnnouncePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ <  PeerPacket::HEADER_LENGTH)
			return false;
		//UINT8 a = (UINT8)(buffer.data_.get()+4);
		//UINT8 b = protocal::ConnectPacket::ACTION;
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::RequestAnnouncePacket::ACTION)
			return false;
		return true;
	}

	RequestAnnouncePacket::p RequestAnnouncePacket::ParseFromBinary(Buffer buffer)
	{
		RequestAnnouncePacket::p pointer;
		if( ! RequestAnnouncePacket::Check(buffer) )
			return pointer;
		pointer = p(new RequestAnnouncePacket(buffer));

		return pointer;
	}

	vector<SUB_PIECE_INFO> RequestSubPiecePacket::GetSubPieceInfo() const 
	{
		assert(request_subpiece_packet_); 
		vector <SUB_PIECE_INFO> sub_piece_info_v;
		for (u_int i = 0; i < request_subpiece_packet_->PieceCount; i ++)
			sub_piece_info_v.push_back(request_subpiece_packet_->SubPieceInfo[i]);
		return sub_piece_info_v;
	}

	RequestSubPiecePacket::RequestSubPiecePacket(Buffer buffer)
		: PeerPacket(buffer)
		, request_subpiece_packet_(NULL)
	{
		assert( RequestSubPiecePacket::Check(buffer) );
		request_subpiece_packet_= (REQUEST_SUBPIECE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
	}

	RequestSubPiecePacket::RequestSubPiecePacket(
		size_t this_length,
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		vector<SUB_PIECE_INFO> subpiece_info_vector)
		: PeerPacket(this_length, RequestSubPiecePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		size_t offset = PeerPacket::HEADER_LENGTH;
		request_subpiece_packet_= (REQUEST_SUBPIECE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		request_subpiece_packet_->PieceCount = subpiece_info_vector.size();
		offset += 2;
		for(u_int i = 0; i < subpiece_info_vector.size(); i ++)
		{
			request_subpiece_packet_->SubPieceInfo[i] = subpiece_info_vector[i];
			offset += sizeof(SUB_PIECE_INFO);
		}
		assert(offset == buffer_.length_);
	}

	RequestSubPiecePacket::RequestSubPiecePacket(
		size_t this_length,
		UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		SUB_PIECE_INFO subpiece_info
		)
		: PeerPacket(this_length, RequestSubPiecePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		request_subpiece_packet_= (REQUEST_SUBPIECE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		request_subpiece_packet_->PieceCount = 1;
		request_subpiece_packet_->SubPieceInfo[0] = subpiece_info;
	}

	RequestSubPiecePacket::p RequestSubPiecePacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			vector<SUB_PIECE_INFO> subpiece_info_vector)
	{
		size_t this_length = 2 + sizeof(SUB_PIECE_INFO)*subpiece_info_vector.size();
		RequestSubPiecePacket::p pointer(			
			new RequestSubPiecePacket(
				this_length, 
				transaction_id, sequece_id, rid, peer_guid,
				subpiece_info_vector
			) 
		);
		return pointer;
	}

	RequestSubPiecePacket::p RequestSubPiecePacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		SUB_PIECE_INFO subpiece_info)
	{
		size_t this_length = 2 + sizeof(SUB_PIECE_INFO)*1;
		RequestSubPiecePacket::p pointer(
			new RequestSubPiecePacket(
				this_length,
				transaction_id, sequece_id, rid, peer_guid,
				subpiece_info
			) 
		);
		return pointer;
	}

	bool RequestSubPiecePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 2)
			return false;
		//byte *piece_index = buffer.data_.get()+PeerPacket::HEADER_LENGTH + 2;
		UINT16 piece_count = *(UINT16*)(buffer.data_.get()+PeerPacket::HEADER_LENGTH);
		//UINT16 a = PeerPacket::HEADER_LENGTH;
		//UINT16 b = PeerPacket::HEADER_LENGTH + sizeof(REQUEST_SUBPIECE_PACKET);
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 2 + sizeof(SUB_PIECE_INFO) * piece_count)
			return false;
		//UINT8 a = (UINT8)(buffer.data_.get()+4);
		//UINT8 b = protocal::ConnectPacket::ACTION;
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::RequestSubPiecePacket::ACTION)
			return false;
		return true;
	}

	RequestSubPiecePacket::p RequestSubPiecePacket::ParseFromBinary(Buffer buffer)
	{
		RequestSubPiecePacket::p pointer;
		if( ! RequestSubPiecePacket::Check(buffer) )
			return pointer;
		pointer = p(new RequestSubPiecePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	SubPiecePacket::SubPiecePacket(Buffer buffer)
		: PeerPacket(buffer)
		, subpiece_packet_(NULL)
	{
		assert( SubPiecePacket::Check(buffer) );
		subpiece_packet_ = (SUBPIECE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
	}

	SubPiecePacket::SubPiecePacket(
		size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		SUB_PIECE_INFO sub_piece_info, UINT16 sub_piece_length, Buffer sub_piece_content)
		: PeerPacket(this_length, SubPiecePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		size_t offset = PeerPacket::HEADER_LENGTH;
		subpiece_packet_ = (SUBPIECE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		subpiece_packet_->SubPieceInfo = sub_piece_info;			
		offset+= 4;
		subpiece_packet_->SubPieceLength = sub_piece_length;		
		offset+= 2;
		offset+= sub_piece_length*sizeof(byte);
		assert (offset == buffer_.length_);
		memcpy(subpiece_packet_->SubPieceContent, sub_piece_content.data_.get(), sub_piece_length);
		//for ( u_int i = 0; i < sub_piece_length; i ++)
		//	subpiece_packet_->SubPieceContent[i] = sub_piece_content.data_.get()[i];

	}

	SubPiecePacket::p SubPiecePacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		SUB_PIECE_INFO sub_piece_info, UINT16 sub_piece_length, Buffer sub_piece_content)
	{
		size_t this_length;
		this_length = sizeof(SUB_PIECE_INFO) + 2 + sub_piece_length;
		SubPiecePacket::p pointer(
			new SubPiecePacket(this_length, transaction_id, sequece_id, rid, peer_guid,
			sub_piece_info, sub_piece_length, sub_piece_content) 
			);
		return pointer;
	}

	bool SubPiecePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 6)
			return false;
		//byte *piece_index = buffer.data_.get()+PeerPacket::HEADER_LENGTH + 2;
		UINT16 piece_count = *(UINT16*)(buffer.data_.get()+PeerPacket::HEADER_LENGTH + 4);
		//UINT16 a = PeerPacket::HEADER_LENGTH;
		//UINT16 b = PeerPacket::HEADER_LENGTH + sizeof(REQUEST_SUBPIECE_PACKET);
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 6 + sizeof(UINT8) * piece_count)
			return false;
		//UINT8 a = (UINT8)(buffer.data_.get()+4);
		//UINT8 b = protocal::ConnectPacket::ACTION;
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::SubPiecePacket::ACTION)
			return false;
		return true;
	}

	SubPiecePacket::p SubPiecePacket::ParseFromBinary(Buffer buffer)
	{
		SubPiecePacket::p pointer;
		if( ! SubPiecePacket::Check(buffer) )
			return pointer;
		pointer = p(new SubPiecePacket(buffer));

		return pointer;
	}

//////////////////////////////////////////////////////////////////////////
	PeerExchangePacket::PeerExchangePacket(Buffer buffer)
		: PeerPacket(buffer)
		, peer_exchange_packet_(NULL)
	{
		assert( PeerExchangePacket::Check(buffer) );
		peer_exchange_packet_= (PEER_EXCHANGE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
	}

	PeerExchangePacket::PeerExchangePacket(
		size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT8 basic_info, UINT8 peer_count, vector<CANDIDATE_PEER_INFO> candidate_peer_info)
		: PeerPacket(this_length, PeerExchangePacket::ACTION, transaction_id, sequece_id, rid, peer_guid)
	{
		size_t offset = PeerPacket::HEADER_LENGTH;
		peer_exchange_packet_ = (PEER_EXCHANGE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
		peer_exchange_packet_->BasicInfo = basic_info;			
		offset+= 1;
		peer_exchange_packet_->PeerCount = peer_count;		
		offset+= 1;
		offset+= peer_count*sizeof(CANDIDATE_PEER_INFO);
		UINT a  = sizeof(CANDIDATE_PEER_INFO);
		assert (offset == buffer_.length_);
		for ( u_int i = 0; i < peer_count; i ++)
			peer_exchange_packet_->CandidatePeerInfo[i] = candidate_peer_info[i];

	}

	PeerExchangePacket::p PeerExchangePacket::CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
		UINT8 basic_info, UINT8 peer_count, vector<CANDIDATE_PEER_INFO> candidate_peer_info)
	{
		size_t this_length;
		this_length = 2 + candidate_peer_info.size() * sizeof(CANDIDATE_PEER_INFO);
		PeerExchangePacket::p pointer(
			new PeerExchangePacket(this_length, transaction_id, sequece_id, rid, peer_guid,
			basic_info, peer_count, candidate_peer_info) 
			);
		return pointer;
	}

	bool PeerExchangePacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 2)
			return false;
		
		UINT8 peer_count = *(UINT8*)(buffer.data_.get()+PeerPacket::HEADER_LENGTH + 1);
		
		if( buffer.length_ < PeerPacket::HEADER_LENGTH + 2 + sizeof(CANDIDATE_PEER_INFO) * peer_count)
			return false;		
		
		if (*(UINT8*)(buffer.data_.get()+4) != protocal::PeerExchangePacket::ACTION)
			return false;
		return true;
	}

	PeerExchangePacket::p PeerExchangePacket::ParseFromBinary(Buffer buffer)
	{
		PeerExchangePacket::p pointer;
		if( ! PeerExchangePacket::Check(buffer) )
			return pointer;
		pointer = p(new PeerExchangePacket(buffer));

		return pointer;
	}

	vector<CANDIDATE_PEER_INFO> PeerExchangePacket::GetCandidatePeerInfo() const
	{
		assert(peer_exchange_packet_); 
		vector <CANDIDATE_PEER_INFO> candidate_peer_info_v;
		for (u_int i = 0; i < peer_exchange_packet_->PeerCount; i ++)
			candidate_peer_info_v.push_back(peer_exchange_packet_->CandidatePeerInfo[i]);
		return candidate_peer_info_v;
	}

    //////////////////////////////////////////////////////////////////////////
    // RIDInfoRequestPacket
    RIDInfoRequestPacket::p RIDInfoRequestPacket::ParseFromBinary(Buffer buffer)
    {
        RIDInfoRequestPacket::p packet;
        if (RIDInfoRequestPacket::Check(buffer))
        {
            packet = RIDInfoRequestPacket::p(new RIDInfoRequestPacket(buffer));
        }
        return packet;
    }

    RIDInfoRequestPacket::p RIDInfoRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 sequence_id, RID rid, Guid peer_guid)
    {
        return RIDInfoRequestPacket::p(new RIDInfoRequestPacket(transaction_id, sequence_id, rid, peer_guid));
    }

    bool RIDInfoRequestPacket::Check(Buffer buffer)
    {
        if (!PeerPacket::Check(buffer))
            return false;
        if (!Packet::CheckAction(buffer, RIDInfoRequestPacket::ACTION))
            return false;
        return true;
    }

    RIDInfoRequestPacket::RIDInfoRequestPacket(Buffer buffer)
        : PeerPacket(buffer)
    {
        assert(Check(buffer_));
    }

    RIDInfoRequestPacket::RIDInfoRequestPacket(UINT32 transaction_id, UINT32 sequence_id, RID rid, Guid peer_guid)
        : PeerPacket(0, RIDInfoRequestPacket::ACTION, transaction_id, sequence_id, rid, peer_guid)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // RIDInfoResponsePacket

    RIDInfoResponsePacket::p RIDInfoResponsePacket::ParseFromBinary(Buffer buffer)
    {
        RIDInfoResponsePacket::p packet;
        if (RIDInfoResponsePacket::Check(buffer))
        {
            packet = RIDInfoResponsePacket::p(new RIDInfoResponsePacket(buffer));
        }
        return packet;
    }

    RIDInfoResponsePacket::p RIDInfoResponsePacket::CreatePacket(UINT32 transaction_id, UINT32 sequence_id, Guid peer_guid, const RidInfo& rid_info)
    {
        return RIDInfoResponsePacket::p(new RIDInfoResponsePacket(transaction_id, sequence_id, peer_guid, rid_info));
    }

    RidInfo RIDInfoResponsePacket::GetRIDInfo() const
    {
        assert(rid_info_response_);

        RidInfo rid_info;

        rid_info.rid_ = GetResourceID();
        rid_info.file_length_ = rid_info_response_->FileLength;
        rid_info.block_size_ = rid_info_response_->BlockSize;
        rid_info.block_count_ = rid_info_response_->BlockCount;
        rid_info.block_md5_s_ = vector<MD5>( rid_info_response_->BlockMD5s, rid_info_response_->BlockMD5s + rid_info.block_count_ );

        return rid_info;
    }

    bool RIDInfoResponsePacket::Check(Buffer buffer)
    {
        if (!PeerPacket::Check(buffer))
            return false;
        if (!Packet::CheckAction(buffer, RIDInfoResponsePacket::ACTION))
            return false;
        // check PEER_RIDINFO_RESPONSE_PACKET
        u_int offset = PeerPacket::HEADER_LENGTH;
        if (buffer.length_ < offset + sizeof(PEER_RIDINFO_RESPONSE_PACKET))
            return false;
        PEER_RIDINFO_RESPONSE_PACKET const *packet = (PEER_RIDINFO_RESPONSE_PACKET const*)(buffer.data_.get() + offset);
        if (buffer.length_ < offset + sizeof(PEER_RIDINFO_RESPONSE_PACKET) + sizeof(MD5) * packet->BlockCount)
            return false;
        return true;
    }

    RIDInfoResponsePacket::RIDInfoResponsePacket(Buffer buffer)
        : PeerPacket(buffer)
        , rid_info_response_(NULL)
    {
        assert(RIDInfoResponsePacket::Check(buffer_));
        rid_info_response_ = (PEER_RIDINFO_RESPONSE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
    }

    RIDInfoResponsePacket::RIDInfoResponsePacket(UINT32 transaction_id, UINT32 sequence_id, Guid peer_guid, const RidInfo& rid_info)
        : PeerPacket(sizeof(PEER_RIDINFO_RESPONSE_PACKET) + rid_info.block_count_ * sizeof(MD5), RIDInfoResponsePacket::ACTION, transaction_id, sequence_id, rid_info.rid_, peer_guid)
        , rid_info_response_(NULL)
    {
        rid_info_response_ = (PEER_RIDINFO_RESPONSE_PACKET*)(buffer_.data_.get() + PeerPacket::HEADER_LENGTH);
        rid_info_response_->BlockCount = rid_info.block_count_;
        rid_info_response_->BlockSize = rid_info.block_size_;
        rid_info_response_->FileLength = rid_info.file_length_;
        assert(rid_info.block_md5_s_.size() == rid_info.block_count_);
        for (u_int i = 0; i < rid_info.block_md5_s_.size(); ++i)
        {
            rid_info_response_->BlockMD5s[i] = rid_info.block_md5_s_[i];
        }
    }

}
