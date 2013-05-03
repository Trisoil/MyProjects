#include "stdafx.h"
#include "protocal/IndexPacket.h"
#include "protocal/Packet.h"

namespace protocal
{
	QueryHttpServerByRidRequestPacket::p QueryHttpServerByRidRequestPacket::ParseFromBinary(Buffer buffer)
	{
		QueryHttpServerByRidRequestPacket::p pointer;
		if( ! QueryHttpServerByRidRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryHttpServerByRidRequestPacket(buffer));

		return pointer;
	}

	bool QueryHttpServerByRidRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + 16 )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryHttpServerByRidRequestPacket::ACTION)
			return false;

		return true;	
	}

	QueryHttpServerByRidRequestPacket::QueryHttpServerByRidRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
		, request_index_query_http_server_by_rid_(NULL)
	{
		assert( QueryHttpServerByRidRequestPacket::Check(buffer));

		request_index_query_http_server_by_rid_ = (REQUEST_INDEX_QUERY_HTTP_SERVER_BY_RID*) (buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	QueryHttpServerByRidRequestPacket::QueryHttpServerByRidRequestPacket(UINT32 transaction_id, UINT32 peer_version,
		RID resource_id)
		: RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_HTTP_SERVER_BY_RID), QueryHttpServerByRidRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_index_query_http_server_by_rid_= (REQUEST_INDEX_QUERY_HTTP_SERVER_BY_RID*) (buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
		request_index_query_http_server_by_rid_->ResourceID = resource_id;
		offset+=16;
		assert(buffer_.length_ == offset);
	}

	QueryHttpServerByRidRequestPacket::p QueryHttpServerByRidRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		RID resource_id)
	{
		QueryHttpServerByRidRequestPacket::p pointer(
			new QueryHttpServerByRidRequestPacket(
			transaction_id, peer_version,
			resource_id
			)
			);
		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	bool QueryHttpServerByRidResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryHttpServerByRidResponsePacket::ACTION)
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + 18 )
			return false;

		UINT16 http_server_count_;
		http_server_count_ = *(UINT16 *) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH + 16);

		offset += 18;
		for (UINT i = 0; i < http_server_count_; i ++)
		{
			offset+=3;

			if( buffer.length_ < offset+2)
				return false;

			UINT16 url_length_;
			url_length_ = *(UINT16 *) (buffer.data_.get() + offset);

			offset+=2+url_length_;

			if( buffer.length_ < offset+2)
				return false;

			UINT16 refer_length_;
			refer_length_ = *(UINT16 *) (buffer.data_.get() + offset);

			offset+=2+refer_length_;
		}

		if (buffer.length_ < offset)
			return false;

		return true;
	}

	QueryHttpServerByRidResponsePacket::p QueryHttpServerByRidResponsePacket::ParseFromBinary(Buffer buffer)
	{
		QueryHttpServerByRidResponsePacket::p pointer;
		if( ! QueryHttpServerByRidResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryHttpServerByRidResponsePacket(buffer));

		return pointer;
	}

	QueryHttpServerByRidResponsePacket::QueryHttpServerByRidResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( QueryHttpServerByRidResponsePacket::Check(buffer));
		//memcpy(buffer_.data_.get()+ResponseServerPacket::HEADER_LENGTH, 
		//	   buffer.data_.get()+ResponseServerPacket::HEADER_LENGTH,
		//	   buffer.length_ - ResponseServerPacket::HEADER_LENGTH)
	}

	RID QueryHttpServerByRidResponsePacket::GetResourceID() const
	{
		assert(buffer_);
		assert(ErrorCode() ==0);
		return *(RID *) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	UINT16 QueryHttpServerByRidResponsePacket::GetHttpServerCount() const
	{
		assert(buffer_);
		assert(ErrorCode() ==0);
		return *(UINT16 *) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH + 16);
	}

	vector <UrlInfo> QueryHttpServerByRidResponsePacket::GetHttpServer () const
	{
		assert(buffer_);
		vector<UrlInfo> http_server_v;

		UINT offset = ResponseServerPacket::HEADER_LENGTH + 18;
		UINT16 http_server_count = *(UINT16 *) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH + 16);
		for (UINT i = 0; i < http_server_count; i ++)
		{
			UrlInfo http_server;
//			http_server.Length = *(UINT16 *) (buffer_.data_.get() + offset);
			offset+=2;
			http_server.type_ = *(UINT8 *) (buffer_.data_.get() + offset);
			offset+=1;

			UINT16 url_length;
			url_length = *(UINT16 *) (buffer_.data_.get() + offset);
			offset+=2;
			http_server.url_.assign((char*)(buffer_.data_.get() + offset), url_length);
			offset+=url_length;

			UINT16 refer_length;
			refer_length = *(UINT16 *) (buffer_.data_.get() + offset);
			offset+=2;
			http_server.refer_url_.assign((char*)(buffer_.data_.get() + offset), refer_length);
			offset+=refer_length;

			http_server_v.push_back(http_server);
		}

		return http_server_v;
		assert(buffer_.length_ == offset);
	}

	QueryHttpServerByRidResponsePacket::p QueryHttpServerByRidResponsePacket::CreatePacket(UINT32 transaction_id, UINT error_code, 
		RID ResourceID, std::vector<UrlInfo> http_server)
	{
		size_t this_length;
		this_length = 18;
		for (UINT i = 0; i < http_server.size(); i ++)
			this_length += 7 + http_server[i].url_.length() + http_server[i].refer_url_.length();

		QueryHttpServerByRidResponsePacket::p pointer(
			new QueryHttpServerByRidResponsePacket(
			this_length, transaction_id, error_code, 
			ResourceID, http_server
			)
			);
		return pointer;
	}

	QueryHttpServerByRidResponsePacket::p QueryHttpServerByRidResponsePacket::CreatePacket(RID ResourceID, vector<UrlInfo> http_server)
	{
		return CreatePacket(0,0,ResourceID, http_server);
	}

	void QueryHttpServerByRidResponsePacket::AttatchEndPointInfo(UINT32 trasaction_id)
	{
		assert(packet_head_);

		packet_head_->TransactionID = trasaction_id;
	}

	QueryHttpServerByRidResponsePacket::QueryHttpServerByRidResponsePacket(size_t this_length, UINT32 transaction_id, UINT error_code, 
		RID ResourceID, std::vector<UrlInfo> http_server)
		: ResponseServerPacket(this_length, QueryHttpServerByRidResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		*(RID*)(buffer_.data_.get() + offset) = ResourceID;
		offset+= 16;
		*(UINT16*)(buffer_.data_.get() + offset) = http_server.size();
		offset+=2;
		for (UINT i = 0; i < http_server.size(); i ++)
		{
			*(UINT16 *) (buffer_.data_.get() + offset) = http_server[i].refer_url_.length() + http_server[i].url_.length() + 7;
			offset+=2;
			*(UINT8 *) (buffer_.data_.get() + offset) = http_server[i].type_;
			offset+=1;

			*(UINT16 *) (buffer_.data_.get() + offset) = http_server[i].url_.length();
			offset+=2;
			memcpy(buffer_.data_.get() + offset, http_server[i].url_.c_str(), http_server[i].url_.length());
			offset+= http_server[i].url_.length();

			*(UINT16 *) (buffer_.data_.get() + offset) = http_server[i].refer_url_.length();
			offset+=2;
			memcpy(buffer_.data_.get() + offset, http_server[i].refer_url_.c_str(), http_server[i].refer_url_.length());
			offset+= http_server[i].refer_url_.length();
		}
		assert(offset == buffer_.length_);
	}

//////////////////////////////////////////////////////////////////////////

	Guid QueryRidByUrlRequestPacket::GetPeerGuid() const
	{
		if (GetPeerVersion()<=0x00000003)
		{
			return Guid();
		}
		assert(request_index_query_rid_by_url_extra);
		return request_index_query_rid_by_url_extra->PeerGuid;
	}


	QueryRidByUrlRequestPacket::QueryRidByUrlRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
		UINT32 session_id, string url_string, string refer_string, Guid peer_guid)
		: RequestServerPacket(this_length, QueryRidByUrlRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		*(UINT32*) (buffer_.data_.get() + offset) = session_id; 
		request_index_query_rid_by_url = (REQUEST_INDEX_QUERY_RID_BY_URL*)(buffer_.data_.get() + offset);
		offset+=4;
		*(UINT16*) (buffer_.data_.get() + offset) = url_string.length(); 
		request_index_query_rid_by_url_urlstring = (REQUEST_INDEX_QUERY_RID_BY_URL_URLSTRING*)(buffer_.data_.get() + offset);
		offset+=2;
		memcpy(buffer_.data_.get() + offset, url_string.c_str(), url_string.length());
		offset+=url_string.length();
		*(UINT16*) (buffer_.data_.get() + offset) = refer_string.length(); 
		request_index_query_rid_by_url_referstring = (REQUEST_INDEX_QUERY_RID_BY_URL_REFERSTRING*)(buffer_.data_.get() + offset);
		offset+=2;
		memcpy(buffer_.data_.get() + offset, refer_string.c_str(), refer_string.length());
		offset+=refer_string.length();
		request_index_query_rid_by_url_extra = (REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA *)(buffer_.data_.get() + offset);
		request_index_query_rid_by_url_extra->PeerGuid = peer_guid;
		offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA);

		assert(buffer_.length_ == offset);
	}

	QueryRidByUrlRequestPacket::p QueryRidByUrlRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		UINT32 session_id, string url_string, string refer_string, Guid peer_guid)
	{
		size_t this_length;

		this_length = 8 + url_string.length() + refer_string.length() + sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA);

		QueryRidByUrlRequestPacket::p pointer(
			new QueryRidByUrlRequestPacket(this_length,
			transaction_id, peer_version,
			session_id, url_string, refer_string, peer_guid
			)
			);
		return pointer;
	}

	bool QueryRidByUrlRequestPacket::Check(Buffer buffer)
	{
		UINT offset = RequestServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + 6 )
			return false;

		offset+= 4;

		UINT16 url_length = *(UINT16*) (buffer.data_.get() + offset);
		offset+=2;
		offset+=url_length;

		if (buffer.length_ < offset+2) 
			return false;
		UINT16 refer_length = *(UINT16*) (buffer.data_.get() + offset);
		offset+=2;
		offset+=refer_length;

		if (buffer.length_ < offset)
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryRidByUrlRequestPacket::ACTION)
			return false;

		if (*(UINT32*)(buffer.data_.get()+10) >= 0x00000004)
		{
			if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA))
				return false;
		}

		return true;
	}

	QueryRidByUrlRequestPacket::p QueryRidByUrlRequestPacket::ParseFromBinary(Buffer buffer)
	{
		QueryRidByUrlRequestPacket::p pointer;
		if( ! QueryRidByUrlRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryRidByUrlRequestPacket(buffer));

		return pointer;
	}

	QueryRidByUrlRequestPacket::QueryRidByUrlRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
	{
		UINT offset = RequestServerPacket::HEADER_LENGTH;

		assert( QueryRidByUrlRequestPacket::Check(buffer));
		request_index_query_rid_by_url = (REQUEST_INDEX_QUERY_RID_BY_URL*) (buffer.data_.get() + offset);
		offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_URL);
		request_index_query_rid_by_url_urlstring = (REQUEST_INDEX_QUERY_RID_BY_URL_URLSTRING*) (buffer.data_.get() + offset);
		offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_URLSTRING) + (request_index_query_rid_by_url_urlstring ->UrlLength)*sizeof(UINT8);
		request_index_query_rid_by_url_referstring = (REQUEST_INDEX_QUERY_RID_BY_URL_REFERSTRING*) (buffer.data_.get() + offset) ;
		offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_REFERSTRING) + (request_index_query_rid_by_url_referstring->ReferLength)*sizeof(UINT8);
		if (GetPeerVersion() >= 0x00000004)
		{
			request_index_query_rid_by_url_extra = (REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA *)(buffer.data_.get() + offset);
			offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA);
		}
		assert(buffer.length_ == offset);

		//memcpy(buffer_.data_.get()+ResponseServerPacket::HEADER_LENGTH, 
		//	   buffer.data_.get()+ResponseServerPacket::HEADER_LENGTH,
		//	   buffer.length_ - ResponseServerPacket::HEADER_LENGTH)
	}

//////////////////////////////////////////////////////////////////////////

	vector<MD5> QueryRidByUrlResponsePacket::GetBlockMD5() const
	{
		assert(response_index_query_rid_by_url);
		assert(ErrorCode() ==0);
		vector<MD5> block_md5_v;
		for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			block_md5_v.push_back(response_index_query_rid_by_url->BlockMD5[i]);
		return block_md5_v;
	}
	
	RidInfo QueryRidByUrlResponsePacket::GetRidInfo() const
	{
		assert(response_index_query_rid_by_url);
		assert(ErrorCode() ==0);
		RidInfo rid_info_;
		rid_info_.block_count_ = response_index_query_rid_by_url->BlockNum;
		for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			rid_info_.block_md5_s_.push_back(response_index_query_rid_by_url->BlockMD5[i]);
		rid_info_.block_size_ = response_index_query_rid_by_url->BlockSize;
		rid_info_.file_length_ = response_index_query_rid_by_url->FileLength;
		rid_info_.rid_ = response_index_query_rid_by_url->ResourceID;
		return rid_info_;
	}

	bool QueryRidByUrlResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH + 36;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryRidByUrlResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL) )
			return false;

        const RESPONSE_INDEX_QUERY_RID_BY_URL* response = 
            (const RESPONSE_INDEX_QUERY_RID_BY_URL*)(buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);

        if (buffer.length_ > ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL) + response->BlockNum * sizeof(MD5))
        {
		    UINT16 block_no_;
		    block_no_ = *(UINT16 *) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH + 30);

		    if (buffer.length_ < offset + block_no_ * sizeof(MD5) + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT))
			    return false;
        }

		return true;
	}

	QueryRidByUrlResponsePacket::p QueryRidByUrlResponsePacket::ParseFromBinary(Buffer buffer)
	{
		QueryRidByUrlResponsePacket::p pointer;
		if( ! QueryRidByUrlResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryRidByUrlResponsePacket(buffer));

		return pointer;
	}

	QueryRidByUrlResponsePacket::QueryRidByUrlResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
        , response_index_query_rid_by_url(NULL)
        , response_index_query_rid_by_url_content(NULL)
	{
		assert( QueryRidByUrlResponsePacket::Check(buffer));
		if (ErrorCode()==0)
		{
			size_t offset = ResponseServerPacket::HEADER_LENGTH;
			response_index_query_rid_by_url = (RESPONSE_INDEX_QUERY_RID_BY_URL*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
			offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL);
			for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			{
				offset += 16;
			}
            if (buffer_.length_ > offset)
            {
			    response_index_query_rid_by_url_content = (RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT *)(buffer_.data_.get() + offset);
            }
		}
	}

	QueryRidByUrlResponsePacket::QueryRidByUrlResponsePacket(size_t this_length, UINT32 transaction_id, UINT error_code,
		UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
		: ResponseServerPacket(this_length, QueryRidByUrlResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_index_query_rid_by_url = (RESPONSE_INDEX_QUERY_RID_BY_URL*)(buffer_.data_.get() + offset);
		offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL);

		response_index_query_rid_by_url->SessionID = session_id;
		response_index_query_rid_by_url->ResourceID = resource_id;
		response_index_query_rid_by_url->DetectedIP = detected_ip;
		response_index_query_rid_by_url->detectedPort = detected_port;
		response_index_query_rid_by_url->FileLength = file_length;
		response_index_query_rid_by_url->BlockSize = block_size;
		response_index_query_rid_by_url->BlockNum = block_md5.size();
		for (UINT i = 0; i < block_md5.size(); i ++)
		{
			response_index_query_rid_by_url->BlockMD5[i] = block_md5[i];
			offset += 16;
		}

		response_index_query_rid_by_url_content = (RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT*)(buffer_.data_.get() + offset);
		offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT);
		response_index_query_rid_by_url_content->ContentSenseMD5 = content_sense_md5;
		response_index_query_rid_by_url_content->ContentBytes = content_bytes;
		assert (offset == buffer_.length_);
	}

	QueryRidByUrlResponsePacket::p QueryRidByUrlResponsePacket::CreatePacket(UINT32 transaction_id, UINT error_code,
		UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
	{
		size_t this_length;

		this_length = 36 + 16 * block_md5.size() + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT);

		QueryRidByUrlResponsePacket::p pointer(
			new QueryRidByUrlResponsePacket(
			this_length, transaction_id, error_code,
			session_id, resource_id, detected_ip, detected_port, file_length, block_size, block_md5, content_sense_md5, content_bytes)
			);
		return pointer;
	}

	QueryRidByUrlResponsePacket::p QueryRidByUrlResponsePacket::CreatePacket(RID resource_id, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
	{
		return CreatePacket(0,0,0,resource_id,0,0,file_length,block_size,block_md5, content_sense_md5, content_bytes);
	}

	void QueryRidByUrlResponsePacket::AttatchEndPointInfo(UINT32 transaction_id,
			UINT32 session_id, UINT32 detected_ip, UINT16 detected_port)
	{
		assert(packet_head_);
		packet_head_->TransactionID = transaction_id;
		assert(response_index_query_rid_by_url);
		response_index_query_rid_by_url->SessionID = session_id;
		response_index_query_rid_by_url->DetectedIP = detected_ip;
		response_index_query_rid_by_url->detectedPort = detected_port;
	}

//////////////////////////////////////////////////////////////////////////

	Guid AddRidUrlRequestPacket::GetContentMD5() 
	{ 
		if (GetPeerVersion()<=0x00000003)
			return MD5();
		assert(request_index_add_rid_url_);
		return request_index_add_rid_url_->ContentSenseMD5;
	}

	UINT32 AddRidUrlRequestPacket::GetContentBytes()
	{
		if (GetPeerVersion()<=0x00000003)
			return 0;
		assert(request_index_add_rid_url_); 
		return request_index_add_rid_url_->ContentBytes;
	}

	AddRidUrlRequestPacket::AddRidUrlRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, UINT16 length, RID resource_id, UINT32 file_length, UINT32 block_size,
		vector<MD5> block_md5, vector<UrlInfo> http_server, MD5 content_md5, size_t content_bytes)
		: RequestServerPacket(this_length, AddRidUrlRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		*(Guid*) (buffer_.data_.get() + offset) = peer_guid;
		offset+=16;
		*(UINT16*) (buffer_.data_.get() + offset) = length;
		offset+=2;
		*(RID*) (buffer_.data_.get() + offset) = resource_id; resource_.rid_ = resource_id;
		offset+=16;
		*(UINT32*) (buffer_.data_.get() + offset) = file_length; resource_.file_length_ = file_length;
		offset+=4;
		*(UINT16*) (buffer_.data_.get() + offset) = block_md5.size(); resource_.md5_s_ = block_md5; resource_.block_num_ = block_md5.size();
		offset+=2;
		*(UINT32*) (buffer_.data_.get() + offset) = block_size; resource_.block_size_ = block_size;
		offset+=4;
		for (UINT i = 0; i < block_md5.size(); i ++)
		{
			*(MD5*) (buffer_.data_.get() + offset) = block_md5[i];
			offset+=16;
		}
		*(UINT16*) (buffer_.data_.get() + offset) = http_server.size(); resource_.url_info_s_ = http_server;
		offset+=2;
		for (UINT i = 0; i < http_server.size(); i ++)
		{
			*(UINT16*) (buffer_.data_.get() + offset) = http_server[i].url_.length() + http_server[i].refer_url_.length() + 7;
			offset+=2;
			*(UINT8*) (buffer_.data_.get() + offset) = http_server[i].type_;
			offset+=1;
			*(UINT16*) (buffer_.data_.get() + offset) = http_server[i].url_.length();
			offset+=2;
			memcpy(buffer_.data_.get() + offset, http_server[i].url_.c_str(), http_server[i].url_.length());
			offset+=http_server[i].url_.length();

			*(UINT16*) (buffer_.data_.get() + offset) = http_server[i].refer_url_.length();
			offset+=2;
			memcpy(buffer_.data_.get() + offset, http_server[i].refer_url_.c_str(), http_server[i].refer_url_.length());
			offset+=http_server[i].refer_url_.length();
		}

		request_index_add_rid_url_ = (REQUEST_INDEX_ADD_RID_URL*)(buffer_.data_.get() + offset);
		request_index_add_rid_url_->ContentBytes = content_bytes;
		request_index_add_rid_url_->ContentSenseMD5 = content_md5;
		offset += sizeof(REQUEST_INDEX_ADD_RID_URL);

		assert(buffer_.length_ == offset);
	}

	AddRidUrlRequestPacket::p AddRidUrlRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid peer_guid, Guid resource_id, UINT32 file_length, UINT32 block_size,
		vector<MD5> block_md5, vector<UrlInfo> http_server, MD5 content_md5, size_t content_bytes)
	{
		size_t this_length, resource_info_length;

		this_length = 16+30+16*block_md5.size();

		resource_info_length = 30+16*block_md5.size();

		for (UINT i = 0; i < http_server.size(); i ++)
			this_length += 7 + http_server[i].url_.length() + http_server[i].refer_url_.length();

		this_length += sizeof(REQUEST_INDEX_ADD_RID_URL);

		AddRidUrlRequestPacket::p pointer(
			new AddRidUrlRequestPacket(this_length, transaction_id, peer_version,
			peer_guid, resource_info_length, resource_id, file_length, block_size,
			block_md5, http_server, content_md5, content_bytes
			)
			);
		return pointer;
	}

	AddRidUrlRequestPacket::p AddRidUrlRequestPacket::ParseFromBinary(Buffer buffer)
	{
		AddRidUrlRequestPacket::p pointer;
		if (! AddRidUrlRequestPacket::Check(buffer))
			return pointer;
		pointer = p(new AddRidUrlRequestPacket(buffer));
		return pointer;
	}

	bool AddRidUrlRequestPacket::Check(Buffer buffer)
	{
		UINT offset = RequestServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;

		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + 16+28)
			return false;

		offset+=16+22;

		UINT16 block_num = *(UINT16 *)(buffer.data_.get()+offset);
		offset+=6+16*block_num;

		if( buffer.length_ < offset + 2)
			return false;

		UINT16 http_count = *(UINT16 *)(buffer.data_.get()+offset);
		offset+=2;

		for (UINT i = 0; i < http_count; i ++)
		{
			offset+=3;

			if( buffer.length_ < offset+2)
				return false;

			UINT16 url_length_;
			url_length_ = *(UINT16 *) (buffer.data_.get() + offset);

			offset+=2+url_length_;

			if( buffer.length_ < offset+2)
				return false;

			UINT16 refer_length_;
			refer_length_ = *(UINT16 *) (buffer.data_.get() + offset);

			offset+=2+refer_length_;
		}

		if (buffer.length_ < offset)
			return false;

		if (*(UINT32*)(buffer.data_.get()+10) >= 0x00000004)
		{
			if (buffer.length_ < offset + sizeof(REQUEST_INDEX_ADD_RID_URL))
				return false;
		}

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::AddRidUrlRequestPacket::ACTION)
			return false;

		return true;
	}

	AddRidUrlRequestPacket::AddRidUrlRequestPacket (Buffer buffer)
		:RequestServerPacket(buffer)
	{
		//resource_info_ = (RESOURCE_INFO*)(buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
		assert(AddRidUrlRequestPacket::Check(buffer));

		size_t offset = RequestServerPacket::HEADER_LENGTH;
		offset+=16;
//		resource_.Length = *(UINT16*) (buffer_.data_.get() + offset);
		offset+=2;
		resource_.rid_ = *(RID*) (buffer_.data_.get() + offset);
		offset+=16;
		resource_.file_length_ = *(UINT32*) (buffer_.data_.get() + offset);
		offset+=4;
		resource_.block_num_ = *(UINT16*) (buffer_.data_.get() + offset);
		offset+=2;
		resource_.block_size_ = *(UINT32*) (buffer_.data_.get() + offset);
		offset+=4;
		for (UINT i = 0; i < resource_.block_num_; i ++)
		{
			resource_.md5_s_.push_back(*(MD5*) (buffer_.data_.get() + offset));
			offset+=16;
		}
		size_t http_server_count = *(UINT16*) (buffer_.data_.get() + offset);
		offset+=2;
		for (UINT i = 0; i < http_server_count; i ++)
		{
			UrlInfo http_server_;
			//http_server_.Length = *(UINT16*) (buffer_.data_.get() + offset);
			offset+=2;
			http_server_.type_ = *(UINT8*) (buffer_.data_.get() + offset);
			offset+=1;
			size_t url_length = *(UINT16*) (buffer_.data_.get() + offset);
			//http_server_.url_.assign(*(UINT16*) (buffer_.data_.get() + offset+2),*(UINT16*) (buffer_.data_.get() + offset + 2 + url_length));
			http_server_.url_ = string(buffer_.data_.get() + offset+2, buffer_.data_.get() + offset+2+url_length);
			offset+=2;
			offset+=url_length;

			size_t refer_length = *(UINT16*) (buffer_.data_.get() + offset);
			//http_server_.refer_url_.assign(*(UINT16*) (buffer_.data_.get() + offset+2),*(UINT16*) (buffer_.data_.get() + offset + 2 + refer_length));
			http_server_.refer_url_ = string(buffer_.data_.get() + offset+2,buffer_.data_.get() + offset+2+refer_length);
			offset+=2;
			offset+=refer_length;
			resource_.url_info_s_.push_back(http_server_);
		}

		if (GetPeerVersion()>= 0x00000004)
		{
			request_index_add_rid_url_ = (REQUEST_INDEX_ADD_RID_URL*)(buffer_.data_.get() + offset);
			offset += sizeof(REQUEST_INDEX_ADD_RID_URL);
			assert(offset == buffer_.length_);
		}
	}

//////////////////////////////////////////////////////////////////////////

	bool AddRidUrlResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH + 36;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::AddRidUrlResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_ADD_RID_URL) )
			return false;

		return true;
	}

	AddRidUrlResponsePacket::p AddRidUrlResponsePacket::ParseFromBinary(Buffer buffer)
	{
		AddRidUrlResponsePacket::p pointer;
		if( ! AddRidUrlResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new AddRidUrlResponsePacket(buffer));

		return pointer;
	}

	AddRidUrlResponsePacket::AddRidUrlResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( AddRidUrlResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_index_add_rid_url = (RESPONSE_INDEX_ADD_RID_URL*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	AddRidUrlResponsePacket::AddRidUrlResponsePacket(UINT32 transaction_id, UINT8 error_code, UINT8 status)
		: ResponseServerPacket(sizeof(RESPONSE_INDEX_ADD_RID_URL), AddRidUrlResponsePacket::ACTION, transaction_id,error_code)
	{
		//*(UINT8*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH) = status;
		UINT len = ResponseServerPacket::HEADER_LENGTH + 1;
		response_index_add_rid_url = (RESPONSE_INDEX_ADD_RID_URL*) (buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_index_add_rid_url ->Status = status;
		assert(buffer_.length_ == ResponseServerPacket::HEADER_LENGTH + 1);
	}

	AddRidUrlResponsePacket::p AddRidUrlResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code, UINT8 status)
	{
		AddRidUrlResponsePacket::p pointer(
			new AddRidUrlResponsePacket(transaction_id, error_code, status
			)
			);
		return pointer;
	}

//////////////////////////////////////////////////////////////////////////

	Guid QueryTrackerListRequestPacket::GetPeerGuid() 
	{
		if (GetPeerVersion()<=0x00000003)
			return Guid();
		assert(request_index_query_tracker_list); 
		return request_index_query_tracker_list->PeerGUID;
	}


	QueryTrackerListRequestPacket::QueryTrackerListRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid)
		: RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_TRACKER_LIST), QueryTrackerListRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_index_query_tracker_list = (REQUEST_INDEX_QUERY_TRACKER_LIST*)(buffer_.data_.get() + offset);
		request_index_query_tracker_list->PeerGUID = peer_guid;
		offset += sizeof(REQUEST_INDEX_QUERY_TRACKER_LIST);
		assert(offset == buffer_.length_);
	}

	QueryTrackerListRequestPacket::p QueryTrackerListRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid)
	{
		QueryTrackerListRequestPacket::p pointer(
			new QueryTrackerListRequestPacket(transaction_id, peer_version, peer_guid
			)
			);
		return pointer;
	}
	
	QueryTrackerListRequestPacket::p QueryTrackerListRequestPacket::ParseFromBinary(Buffer buffer)
	{
		QueryTrackerListRequestPacket::p pointer;
		if( ! QueryTrackerListRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryTrackerListRequestPacket(buffer));

		return pointer;

	}

	QueryTrackerListRequestPacket::QueryTrackerListRequestPacket(Buffer buffer)
		:RequestServerPacket(buffer)
	{
		if (GetPeerVersion() >= 0x00000004)
		{
			size_t offset = RequestServerPacket::HEADER_LENGTH;
			request_index_query_tracker_list = (REQUEST_INDEX_QUERY_TRACKER_LIST*)(buffer_.data_.get() + offset);
			offset += sizeof(REQUEST_INDEX_QUERY_TRACKER_LIST);
			assert(offset == buffer.length_);
		}
		
	}

	bool QueryTrackerListRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH)
			return false;


		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryTrackerListRequestPacket::ACTION)
			return false;

		if (*(UINT32*)(buffer.data_.get()+10)>=0x00000004)
		{
			if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_INDEX_QUERY_TRACKER_LIST))
				return false;

		}

		return true;
	}


	//////////////////////////////////////////////////////////////////////////

	vector<TRACKER_INFO> QueryTrackerListResponsePacket::GetTrackerInfo() const
	{
		assert(response_index_query_tracker_list);
		vector<TRACKER_INFO> tracker_info_;
		for (UINT i = 0; i < response_index_query_tracker_list->TrackerCount; i ++)
			tracker_info_.push_back(response_index_query_tracker_list->TrackerInfo[i]);
		return tracker_info_;
	}
	bool QueryTrackerListResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH + 2;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryTrackerListResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_TRACKER_LIST) )
			return false;

		UINT16 tracker_count_ = *(UINT16*)(buffer.data_.get()+offset);

		//UINT a, b, c;
		//a = ResponseServerPacket::HEADER_LENGTH;
		//b = sizeof(RESPONSE_INDEX_QUERY_TRACKER_LIST);
		//c = tracker_count_ * sizeof(TRACKER_INFO);
		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_TRACKER_LIST) + tracker_count_ * sizeof(TRACKER_INFO))
			return false;

		return true;
	}

	QueryTrackerListResponsePacket::p QueryTrackerListResponsePacket::ParseFromBinary(Buffer buffer)
	{
		QueryTrackerListResponsePacket::p pointer;
		if( ! QueryTrackerListResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryTrackerListResponsePacket(buffer));

		return pointer;
	}

	QueryTrackerListResponsePacket::QueryTrackerListResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( QueryTrackerListResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_index_query_tracker_list = (RESPONSE_INDEX_QUERY_TRACKER_LIST*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	QueryTrackerListResponsePacket::p QueryTrackerListResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info)
	{
		size_t this_length;
		this_length = 4 + tracker_info.size() * sizeof(TRACKER_INFO);
		QueryTrackerListResponsePacket::p pointer(
			new QueryTrackerListResponsePacket(this_length, transaction_id, error_code, 
				tracker_group_count, tracker_info
			)
			);
		return pointer;
		
	}

	QueryTrackerListResponsePacket::p QueryTrackerListResponsePacket::CreatePacket(UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info)
	{
		return CreatePacket(0,0,tracker_group_count,tracker_info);
	}

	void QueryTrackerListResponsePacket::AttachEndPointInfo(UINT32 transaction_id)
	{
		assert(packet_head_);
		packet_head_->TransactionID = transaction_id;

	}


	QueryTrackerListResponsePacket::QueryTrackerListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
		UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info)
		: ResponseServerPacket(this_length, QueryTrackerListResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_index_query_tracker_list = (RESPONSE_INDEX_QUERY_TRACKER_LIST*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_index_query_tracker_list->TrackerGroupCount = tracker_group_count;
		offset+=2;
		response_index_query_tracker_list->TrackerCount = tracker_info.size();
		offset+=2;
		for (UINT  i = 0; i < tracker_info.size(); i ++)
		{
			response_index_query_tracker_list->TrackerInfo[i] = tracker_info[i];
			offset+=sizeof(TRACKER_INFO);
		}
		assert(offset==buffer_.length_);
	}
	//////////////////////////////////////////////////////////////////////////

	QueryStunServerListRequestPacket::QueryStunServerListRequestPacket(UINT32 transaction_id, UINT32 peer_version)
		: RequestServerPacket(0, QueryStunServerListRequestPacket::ACTION, transaction_id, peer_version)
	{
	}

	QueryStunServerListRequestPacket::p QueryStunServerListRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version)
	{
		QueryStunServerListRequestPacket::p pointer(
			new QueryStunServerListRequestPacket(transaction_id, peer_version
			)
			);
		return pointer;
	}

	QueryStunServerListRequestPacket::p QueryStunServerListRequestPacket::ParseFromBinary(Buffer buffer)
	{
		QueryStunServerListRequestPacket::p pointer;
		if( ! QueryStunServerListRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryStunServerListRequestPacket(buffer));

		return pointer;

	}

	QueryStunServerListRequestPacket::QueryStunServerListRequestPacket(Buffer buffer)
		:RequestServerPacket(buffer)
	{

	}

	bool QueryStunServerListRequestPacket::Check(Buffer buffer)
	{
		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH)
			return false;


		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryStunServerListRequestPacket::ACTION)
			return false;

		return true;
	}


	//////////////////////////////////////////////////////////////////////////

	vector<STUN_SERVER_INFO> QueryStunServerListResponsePacket::GetStunServerInfo() const
	{
		assert(response_index_query_stun_server_list);
		vector<STUN_SERVER_INFO> stun_server_info_;
		for (UINT i = 0; i < response_index_query_stun_server_list->StunServerCount; i ++)
			stun_server_info_.push_back(response_index_query_stun_server_list->StunServerInfo[i]);
		return stun_server_info_;
	}
	bool QueryStunServerListResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryStunServerListResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_STUN_SERVER_LIST) )
			return false;

		UINT16 stun_server_count_ = *(UINT16*)(buffer.data_.get()+offset);

		//UINT a, b, c;
		//a = ResponseServerPacket::HEADER_LENGTH;
		//b = sizeof(RESPONSE_INDEX_QUERY_TRACKER_LIST);
		//c = tracker_count_ * sizeof(TRACKER_INFO);
		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_STUN_SERVER_LIST) + stun_server_count_ * sizeof(STUN_SERVER_INFO))
			return false;

		return true;
	}

	QueryStunServerListResponsePacket::p QueryStunServerListResponsePacket::ParseFromBinary(Buffer buffer)
	{
		QueryStunServerListResponsePacket::p pointer;
		if( ! QueryStunServerListResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryStunServerListResponsePacket(buffer));

		return pointer;
	}

	QueryStunServerListResponsePacket::QueryStunServerListResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( QueryStunServerListResponsePacket::Check(buffer));
		if (ErrorCode() == 0)
			response_index_query_stun_server_list = (RESPONSE_INDEX_QUERY_STUN_SERVER_LIST*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
	}

	QueryStunServerListResponsePacket::p QueryStunServerListResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 error_code,
		vector<STUN_SERVER_INFO> stun_server_info)
	{
		size_t this_length;
		this_length = 2 + stun_server_info.size() * sizeof(STUN_SERVER_INFO);
		QueryStunServerListResponsePacket::p pointer(
			new QueryStunServerListResponsePacket(this_length, transaction_id, error_code, 
			stun_server_info
			)
			);
		return pointer;

	}

	QueryStunServerListResponsePacket::p QueryStunServerListResponsePacket::CreatePacket(vector<STUN_SERVER_INFO> stun_server_info)
	{
		return CreatePacket(Packet::NewTransactionID(),0,stun_server_info);
	}

	void QueryStunServerListResponsePacket::AttachEndPointInfo(UINT32 transaction_id)
	{
		assert(packet_head_);
		packet_head_->TransactionID = transaction_id;

	}


	QueryStunServerListResponsePacket::QueryStunServerListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
		vector<STUN_SERVER_INFO> stun_server_info)
		: ResponseServerPacket(this_length, QueryStunServerListResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_index_query_stun_server_list = (RESPONSE_INDEX_QUERY_STUN_SERVER_LIST*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
		response_index_query_stun_server_list->StunServerCount = stun_server_info.size();
		offset+=2;
		for (UINT  i = 0; i < stun_server_info.size(); i ++)
		{
			response_index_query_stun_server_list->StunServerInfo[i] = stun_server_info[i];
			offset+=sizeof(STUN_SERVER_INFO);
		}
		assert(offset==buffer_.length_);
	}
	//////////////////////////////////////////////////////////////////////////

	QueryRidByContentRequestPacket::QueryRidByContentRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
		Guid content_sense_md5, UINT32 content_bytes, UINT32 file_length, Guid peer_guid )
		: RequestServerPacket(this_length, QueryRidByContentRequestPacket::ACTION, transaction_id, peer_version)
	{
		size_t offset = RequestServerPacket::HEADER_LENGTH;
		request_index_query_rid_by_content = (REQUEST_INDEX_QUERY_RID_BY_CONTENT *)(buffer_.data_.get() + offset);
		request_index_query_rid_by_content->ContentBytes = content_bytes;
		request_index_query_rid_by_content->ContentSenseMD5 = content_sense_md5;
		request_index_query_rid_by_content->FileLength = file_length;
		request_index_query_rid_by_content->PeerGuid = peer_guid;
		offset += sizeof(REQUEST_INDEX_QUERY_RID_BY_CONTENT);
		assert(buffer_.length_ == offset);
	}

	QueryRidByContentRequestPacket::p QueryRidByContentRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version,
		Guid content_sense_md5, UINT32 content_bytes, UINT32 file_length, Guid peer_guid )
	{
		size_t this_length;

		this_length = sizeof(REQUEST_INDEX_QUERY_RID_BY_CONTENT);

		QueryRidByContentRequestPacket::p pointer(
			new QueryRidByContentRequestPacket(this_length,
			transaction_id, peer_version,
			content_sense_md5, content_bytes, file_length, peer_guid
			)
			);
		return pointer;
	}

	bool QueryRidByContentRequestPacket::Check(Buffer buffer)
	{
		UINT offset = RequestServerPacket::HEADER_LENGTH;

		if( ! buffer )
			return false;
		if( buffer.length_ < RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_INDEX_QUERY_RID_BY_CONTENT) )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryRidByContentRequestPacket::ACTION)
			return false;

		return true;
	}

	QueryRidByContentRequestPacket::p QueryRidByContentRequestPacket::ParseFromBinary(Buffer buffer)
	{
		QueryRidByContentRequestPacket::p pointer;
		if( ! QueryRidByContentRequestPacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryRidByContentRequestPacket(buffer));

		return pointer;
	}

	QueryRidByContentRequestPacket::QueryRidByContentRequestPacket(Buffer buffer)
		: RequestServerPacket(buffer)
	{
		assert( QueryRidByContentRequestPacket::Check(buffer));
		request_index_query_rid_by_content = (REQUEST_INDEX_QUERY_RID_BY_CONTENT*)(buffer.data_.get() + RequestServerPacket::HEADER_LENGTH);
	}

	//////////////////////////////////////////////////////////////////////////

	vector<MD5> QueryRidByContentResponsePacket::GetBlockMD5() const
	{
		assert(response_index_query_rid_by_url);
		assert(ErrorCode() ==0);
		vector<MD5> block_md5_v;
		for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			block_md5_v.push_back(response_index_query_rid_by_url->BlockMD5[i]);
		return block_md5_v;
	}

	RidInfo QueryRidByContentResponsePacket::GetRidInfo() const
	{
		assert(response_index_query_rid_by_url);
		assert(ErrorCode() ==0);
		RidInfo rid_info_;
		rid_info_.block_count_ = response_index_query_rid_by_url->BlockNum;
		for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			rid_info_.block_md5_s_.push_back(response_index_query_rid_by_url->BlockMD5[i]);
		rid_info_.block_size_ = response_index_query_rid_by_url->BlockSize;
		rid_info_.file_length_ = response_index_query_rid_by_url->FileLength;
		rid_info_.rid_ = response_index_query_rid_by_url->ResourceID;
		return rid_info_;
	}

	bool QueryRidByContentResponsePacket::Check(Buffer buffer)
	{
		UINT offset = ResponseServerPacket::HEADER_LENGTH + 36;

		if( ! buffer )
			return false;

		if (*(UINT8*)(buffer.data_.get()+4) != protocal::QueryRidByContentResponsePacket::ACTION)
			return false;

		if (buffer.length_ < ResponseServerPacket::HEADER_LENGTH )
			return false;

		if (*(UINT8*)(buffer.data_.get()+10) != 0)
			return true;

		if( buffer.length_ < ResponseServerPacket::HEADER_LENGTH + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL) )
			return false;

		UINT16 block_no_;
		block_no_ = *(UINT16 *) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH + 30);

		if (buffer.length_ < offset + block_no_ * sizeof(MD5) + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT))
			return false;

		return true;
	}

	QueryRidByContentResponsePacket::p QueryRidByContentResponsePacket::ParseFromBinary(Buffer buffer)
	{
		QueryRidByContentResponsePacket::p pointer;
		if( ! QueryRidByContentResponsePacket::Check(buffer) )
			return pointer;
		pointer = p(new QueryRidByContentResponsePacket(buffer));

		return pointer;
	}

	QueryRidByContentResponsePacket::QueryRidByContentResponsePacket(Buffer buffer)
		: ResponseServerPacket(buffer)
	{
		assert( QueryRidByContentResponsePacket::Check(buffer));
		if (ErrorCode()==0)
		{
			size_t offset = ResponseServerPacket::HEADER_LENGTH;
			response_index_query_rid_by_url = (RESPONSE_INDEX_QUERY_RID_BY_URL*) (buffer.data_.get() + ResponseServerPacket::HEADER_LENGTH);
			offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL);
			for (UINT i = 0; i < response_index_query_rid_by_url->BlockNum; i ++)
			{
				offset += 16;
			}
			response_index_query_rid_by_url_content = (RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT *)(buffer.data_.get() + offset);
		}
	}

	QueryRidByContentResponsePacket::QueryRidByContentResponsePacket(size_t this_length, UINT32 transaction_id, UINT error_code,
		UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
		: ResponseServerPacket(this_length, QueryRidByContentResponsePacket::ACTION, transaction_id, error_code)
	{
		size_t offset = ResponseServerPacket::HEADER_LENGTH;
		response_index_query_rid_by_url = (RESPONSE_INDEX_QUERY_RID_BY_URL*)(buffer_.data_.get() + offset);
		offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL);

		response_index_query_rid_by_url->SessionID = session_id;
		response_index_query_rid_by_url->ResourceID = resource_id;
		response_index_query_rid_by_url->DetectedIP = detected_ip;
		response_index_query_rid_by_url->detectedPort = detected_port;
		response_index_query_rid_by_url->FileLength = file_length;
		response_index_query_rid_by_url->BlockSize = block_size;
		response_index_query_rid_by_url->BlockNum = block_md5.size();
		for (UINT i = 0; i < block_md5.size(); i ++)
		{
			response_index_query_rid_by_url->BlockMD5[i] = block_md5[i];
			offset += 16;
		}
		response_index_query_rid_by_url_content = (RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT*)(buffer_.data_.get() + offset);
		response_index_query_rid_by_url_content->ContentBytes = content_bytes;
		response_index_query_rid_by_url_content->ContentSenseMD5 = content_sense_md5;
		offset += sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT);
		assert (offset == buffer_.length_);
	}

	QueryRidByContentResponsePacket::p QueryRidByContentResponsePacket::CreatePacket(UINT32 transaction_id, UINT error_code,
		UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
	{
		size_t this_length;

		this_length = 36 + 16 * block_md5.size() + sizeof(RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT);

		QueryRidByContentResponsePacket::p pointer(
			new QueryRidByContentResponsePacket(
			this_length, transaction_id, error_code,
			session_id, resource_id, detected_ip, detected_port, file_length, block_size, block_md5, content_sense_md5, content_bytes)
			);
		return pointer;
	}

	QueryRidByContentResponsePacket::p QueryRidByContentResponsePacket::CreatePacket(RID resource_id, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes)
	{
		return CreatePacket(0,0,0,resource_id,0,0,file_length,block_size,block_md5, content_sense_md5, content_bytes);
	}

	void QueryRidByContentResponsePacket::AttatchEndPointInfo(UINT32 transaction_id,
		UINT32 session_id, UINT32 detected_ip, UINT16 detected_port)
	{
		assert(packet_head_);
		packet_head_->TransactionID = transaction_id;
		assert(response_index_query_rid_by_url);
		response_index_query_rid_by_url->SessionID = session_id;
		response_index_query_rid_by_url->DetectedIP = detected_ip;
		response_index_query_rid_by_url->detectedPort = detected_port;
	}

    //////////////////////////////////////////////////////////////////////////
    // QueryIndexServerListRequestPacket

    QueryIndexServerListRequestPacket::p QueryIndexServerListRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryIndexServerListRequestPacket::p packet;
        if (QueryIndexServerListRequestPacket::Check(buffer))
        {
            packet = QueryIndexServerListRequestPacket::p(new QueryIndexServerListRequestPacket(buffer));
        }
        return packet;
    }

    QueryIndexServerListRequestPacket::p QueryIndexServerListRequestPacket::CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid)
    {
        return QueryIndexServerListRequestPacket::p(new QueryIndexServerListRequestPacket(transaction_id, peer_version, peer_guid));
    }

    bool QueryIndexServerListRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) 
            return false;
        if (buffer.length_ != RequestServerPacket::HEADER_LENGTH + sizeof(REQUEST_INDEX_QUERY_INDEXSERVER_PACKET))
            return false;
        if (false == Packet::CheckAction(buffer, QueryIndexServerListRequestPacket::ACTION))
            return false;
        return true;
    }

    QueryIndexServerListRequestPacket::QueryIndexServerListRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_indexserver_(NULL)
    {
        assert( QueryIndexServerListRequestPacket::Check(buffer) );
        request_index_query_indexserver_ = (REQUEST_INDEX_QUERY_INDEXSERVER_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }

    QueryIndexServerListRequestPacket::QueryIndexServerListRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_INDEXSERVER_PACKET), QueryIndexServerListRequestPacket::ACTION, transaction_id, peer_version)
        , request_index_query_indexserver_(NULL)
    {
        request_index_query_indexserver_ = (REQUEST_INDEX_QUERY_INDEXSERVER_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_indexserver_->PeerGuid = peer_guid;
    }

    //////////////////////////////////////////////////////////////////////////
    // QueryIndexServerListResponsePacket

    QueryIndexServerListResponsePacket::p QueryIndexServerListResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryIndexServerListResponsePacket::p packet;
        if (QueryIndexServerListResponsePacket::Check(buffer))
        {
            packet = QueryIndexServerListResponsePacket::p(new QueryIndexServerListResponsePacket(buffer));
        }
        return packet;
    }

    QueryIndexServerListResponsePacket::p QueryIndexServerListResponsePacket::CreateErrorPacket(UINT32 transaction_id, UINT32 error_code)
    {
        QueryIndexServerListResponsePacket::p packet;
        if (error_code != 0)
        {
            packet = QueryIndexServerListResponsePacket::p(new QueryIndexServerListResponsePacket(transaction_id, error_code));
        }
        return packet;
    }

    // mod_index_map 大小必须是 256, 这里不检查mod_index_map[i]是否越界
    QueryIndexServerListResponsePacket::p QueryIndexServerListResponsePacket::CreatePacket(UINT32 transaction_id, const vector<UINT8>& mod_index_map, const vector<INDEX_SERVER_INFO>& index_servers)
    {
        // check
        QueryIndexServerListResponsePacket::p packet;
        if (mod_index_map.size() != INDEX_LIST_MOD_COUNT)
            return packet;
        if (index_servers.size() > INDEX_LIST_MOD_COUNT)
            return packet;
        packet = QueryIndexServerListResponsePacket::p(new QueryIndexServerListResponsePacket(transaction_id, mod_index_map, index_servers));
        return packet;
    }

    bool QueryIndexServerListResponsePacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;

        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryIndexServerListResponsePacket::ACTION))
            return false;

        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET))
            return false;

        offset += INDEX_LIST_MOD_COUNT * sizeof (UINT8);
        UINT8 index_count = *(UINT8*)(buffer.data_.get() + offset);

        offset += sizeof (UINT8);
        if (buffer.length_ < offset + index_count * sizeof(INDEX_SERVER_INFO))
            return false;

        return true;
    }

    QueryIndexServerListResponsePacket::QueryIndexServerListResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_indexserver_(NULL)
    {
        assert(QueryIndexServerListResponsePacket::Check(buffer));
        response_index_query_indexserver_ = (RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
    }

    QueryIndexServerListResponsePacket::QueryIndexServerListResponsePacket(UINT32 transaction_id, const vector<UINT8>& mod_index_map, const vector<INDEX_SERVER_INFO>& index_servers)
        : ResponseServerPacket(sizeof(RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET) + index_servers.size() * sizeof(INDEX_SERVER_INFO), QueryIndexServerListResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_indexserver_(NULL)
    {
        assert(mod_index_map.size() == INDEX_LIST_MOD_COUNT);
        assert(index_servers.size() <= INDEX_LIST_MOD_COUNT);
        response_index_query_indexserver_ = (RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);

        for (size_t i = 0; i < INDEX_LIST_MOD_COUNT && i < mod_index_map.size(); ++i)
        {
            response_index_query_indexserver_->ModIndexMap[i] = mod_index_map[i];
        }
        response_index_query_indexserver_->IndexServerCount = min(INDEX_LIST_MOD_COUNT, index_servers.size());
        for (size_t i = 0; i < response_index_query_indexserver_->IndexServerCount; ++i)
        {
            response_index_query_indexserver_->IndexServerInfos[i] = index_servers[i];
        }
    }

    QueryIndexServerListResponsePacket::QueryIndexServerListResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryIndexServerListResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_indexserver_(NULL)
    {
        assert(error_code != 0);
    }

    //////////////////////////////////////////////////////////////////////////
    // QueryTestUrlListRequestPacket
    QueryTestUrlListRequestPacket::p QueryTestUrlListRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryTestUrlListRequestPacket::p packet;
        if (QueryTestUrlListRequestPacket::Check(buffer))
        {
            packet = QueryTestUrlListRequestPacket::p(new QueryTestUrlListRequestPacket(buffer));
        }
        return packet;
    }
    QueryTestUrlListRequestPacket::p QueryTestUrlListRequestPacket::CreatePacket(UINT32 transaction_id, Guid peer_guid)
    {
        return QueryTestUrlListRequestPacket::p(new QueryTestUrlListRequestPacket(transaction_id, peer_guid));
    }
    QueryTestUrlListRequestPacket::QueryTestUrlListRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_string_list_(NULL)
    {
        assert(QueryTestUrlListRequestPacket::Check(buffer));
        request_index_query_string_list_ = (REQUEST_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    QueryTestUrlListRequestPacket::QueryTestUrlListRequestPacket(UINT32 transaction_id, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_STRING_LIST_PACKET), QueryTestUrlListRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION)
        , request_index_query_string_list_(NULL)
    {
        request_index_query_string_list_ = (REQUEST_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_string_list_->PeerGuid = peer_guid;
    }
    bool QueryTestUrlListRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        UINT32 offset = RequestServerPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryTestUrlListRequestPacket::ACTION))
            return false;

        if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_STRING_LIST_PACKET))
            return false;

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // QueryTestUrlListResponsePacket

    QueryTestUrlListResponsePacket::p QueryTestUrlListResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryTestUrlListResponsePacket::p packet;
        if (QueryTestUrlListResponsePacket::Check(buffer))
        {
            packet = QueryTestUrlListResponsePacket::p(new QueryTestUrlListResponsePacket(buffer));
        }
        return packet;
    }
    QueryTestUrlListResponsePacket::p QueryTestUrlListResponsePacket::CreateErrorPacket(UINT32 transaction_id, UINT32 error_code)
    {
        return QueryTestUrlListResponsePacket::p(new QueryTestUrlListResponsePacket(transaction_id, error_code));
    }
    QueryTestUrlListResponsePacket::p QueryTestUrlListResponsePacket::CreatePacket(UINT32 transaction_id, const vector<string>& url_list)
    {
        // check size
        if (url_list.size() >= 0xFFFF)
        {
            return QueryTestUrlListResponsePacket::p();
        }
        // buffer length
        UINT32 buffer_length = 0;
        buffer_length += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        for (int i = 0; i < url_list.size(); ++i)
        {
            buffer_length += sizeof(STRING);
            buffer_length += url_list[i].length();
        }
        // construct
        return QueryTestUrlListResponsePacket::p(new QueryTestUrlListResponsePacket(transaction_id, url_list, buffer_length));
    }
    bool QueryTestUrlListResponsePacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryTestUrlListResponsePacket::ACTION))
            return false;

        if (ResponseServerPacket::ErrorCode(buffer) != 0)
            return true;

        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET))
            return false;

        RESPONSE_INDEX_QUERY_STRING_LIST_PACKET* response = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer.data_.get() + offset);
        UINT16 count = response->StringCount;
        offset += sizeof(UINT16);
        for (int i = 0; i < count; ++i)
        {
            STRING* str = (STRING*)(buffer.data_.get() + offset);
            // STRING
            if (buffer.length_ < offset + sizeof(STRING))
                return false;
            offset += sizeof(STRING);
            // BUFFEr
            if (buffer.length_ < offset + str->Length)
                return false;
            offset += str->Length;
        }

        return true;
    }
    QueryTestUrlListResponsePacket::QueryTestUrlListResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_string_list_(NULL)
    {
        assert(QueryTestUrlListResponsePacket::Check(buffer));
        if (ErrorCode() == 0)
        {
            response_index_query_string_list_ = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        }
    }
    QueryTestUrlListResponsePacket::QueryTestUrlListResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryTestUrlListResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_string_list_(NULL)
    {
    }
    QueryTestUrlListResponsePacket::QueryTestUrlListResponsePacket(UINT32 transaction_id, const vector<string>& url_list, UINT32 buffer_length)
        : ResponseServerPacket(buffer_length, QueryTestUrlListResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_string_list_(NULL)
    {
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;
        response_index_query_string_list_ = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + offset);
        response_index_query_string_list_->StringCount = url_list.size();
        offset += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        for (UINT16 i = 0; i < url_list.size(); ++i)
        {
            STRING *str = (STRING*)(buffer_.data_.get() + offset);
            str->Length = url_list[i].length();
            memcpy(str->StringData, url_list[i].c_str(), str->Length);
            // offset
            offset += sizeof(STRING) + str->Length;
        }
    }
    vector<string> QueryTestUrlListResponsePacket::GetTestUrlList() const
    {
        if (ErrorCode() != 0)
        {
            return vector<string>();
        }

        assert(response_index_query_string_list_);
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;
        const RESPONSE_INDEX_QUERY_STRING_LIST_PACKET* packet 
            = (const RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + offset);
        // count
        UINT16 count = packet->StringCount;
        offset += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        // url list
        vector<string> url_list;
        for (UINT16 i = 0; i < count; ++i)
        {
            const STRING *str = (const STRING*)(buffer_.data_.get() + offset);
            string data((const char*)(str->StringData), str->Length);
            url_list.push_back(data);
            // offset
            offset += sizeof(STRING) + str->Length;
        }
        // return 
        return url_list;
    }

    //////////////////////////////////////////////////////////////////////////
    // QueryKeyWordListRequestPacket
    QueryKeyWordListRequestPacket::p QueryKeyWordListRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryKeyWordListRequestPacket::p packet;
        if (QueryKeyWordListRequestPacket::Check(buffer))
        {
            packet = QueryKeyWordListRequestPacket::p(new QueryKeyWordListRequestPacket(buffer));
        }
        return packet;
    }
    QueryKeyWordListRequestPacket::p QueryKeyWordListRequestPacket::CreatePacket(UINT32 transaction_id, Guid peer_guid)
    {
        return QueryKeyWordListRequestPacket::p(new QueryKeyWordListRequestPacket(transaction_id, peer_guid));
    }
    QueryKeyWordListRequestPacket::QueryKeyWordListRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_string_list_(NULL)
    {
        assert(QueryKeyWordListRequestPacket::Check(buffer));
        request_index_query_string_list_ = (REQUEST_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    QueryKeyWordListRequestPacket::QueryKeyWordListRequestPacket(UINT32 transaction_id, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_STRING_LIST_PACKET), QueryKeyWordListRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION)
        , request_index_query_string_list_(NULL)
    {
        request_index_query_string_list_ = (REQUEST_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_string_list_->PeerGuid = peer_guid;
    }
    bool QueryKeyWordListRequestPacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        UINT32 offset = RequestServerPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryKeyWordListRequestPacket::ACTION))
            return false;

        if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_STRING_LIST_PACKET))
            return false;

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // QueryKeyWordListResponsePacket

    QueryKeyWordListResponsePacket::p QueryKeyWordListResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryKeyWordListResponsePacket::p packet;
        if (QueryKeyWordListResponsePacket::Check(buffer))
        {
            packet = QueryKeyWordListResponsePacket::p(new QueryKeyWordListResponsePacket(buffer));
        }
        return packet;
    }
    QueryKeyWordListResponsePacket::p QueryKeyWordListResponsePacket::CreateErrorPacket(UINT32 transaction_id, UINT32 error_code)
    {
        return QueryKeyWordListResponsePacket::p(new QueryKeyWordListResponsePacket(transaction_id, error_code));
    }
    QueryKeyWordListResponsePacket::p QueryKeyWordListResponsePacket::CreatePacket(UINT32 transaction_id, const vector<string>& keyword_list)
    {
        // check size
        if (keyword_list.size() >= 0xFFFF)
        {
            return QueryKeyWordListResponsePacket::p();
        }
        // buffer length
        UINT32 buffer_length = 0;
        buffer_length += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        for (int i = 0; i < keyword_list.size(); ++i)
        {
            buffer_length += sizeof(STRING);
            buffer_length += keyword_list[i].length();
        }
        // construct
        return QueryKeyWordListResponsePacket::p(new QueryKeyWordListResponsePacket(transaction_id, keyword_list, buffer_length));
    }
    bool QueryKeyWordListResponsePacket::Check(Buffer buffer)
    {
        if (!buffer)
            return false;
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryKeyWordListResponsePacket::ACTION))
            return false;

        if (ResponseServerPacket::ErrorCode(buffer) != 0)
            return true;

        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET))
            return false;

        RESPONSE_INDEX_QUERY_STRING_LIST_PACKET* response = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer.data_.get() + offset);
        UINT16 count = response->StringCount;
        offset += sizeof(UINT16);
        for (int i = 0; i < count; ++i)
        {
            STRING* str = (STRING*)(buffer.data_.get() + offset);
            // STRING
            if (buffer.length_ < offset + sizeof(STRING))
                return false;
            offset += sizeof(STRING);
            // BUFFEr
            if (buffer.length_ < offset + str->Length)
                return false;
            offset += str->Length;
        }

        return true;
    }
    QueryKeyWordListResponsePacket::QueryKeyWordListResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_string_list_(NULL)
    {
        assert(QueryKeyWordListResponsePacket::Check(buffer));
        if (ErrorCode() == 0)
        {
            response_index_query_string_list_ = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        }
    }
    QueryKeyWordListResponsePacket::QueryKeyWordListResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryKeyWordListResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_string_list_(NULL)
    {
        assert(error_code != 0);
    }
    QueryKeyWordListResponsePacket::QueryKeyWordListResponsePacket(UINT32 transaction_id, const vector<string>& keyword_list, UINT32 buffer_length)
        : ResponseServerPacket(buffer_length, QueryKeyWordListResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_string_list_(NULL)
    {
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;
        response_index_query_string_list_ = (RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + offset);
        response_index_query_string_list_->StringCount = keyword_list.size();
        offset += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        for (UINT16 i = 0; i < keyword_list.size(); ++i)
        {
            STRING *str = (STRING*)(buffer_.data_.get() + offset);
            str->Length = keyword_list[i].length();
            memcpy(str->StringData, keyword_list[i].c_str(), str->Length);
            // offset
            offset += sizeof(STRING) + str->Length;
        }
    }
    vector<string> QueryKeyWordListResponsePacket::GetKeyWordList() const
    {
        if (ErrorCode() != 0)
        {
            return vector<string>();
        }

        assert(response_index_query_string_list_);
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;
        const RESPONSE_INDEX_QUERY_STRING_LIST_PACKET* packet 
            = (const RESPONSE_INDEX_QUERY_STRING_LIST_PACKET*)(buffer_.data_.get() + offset);
        // count
        UINT16 count = packet->StringCount;
        offset += sizeof(RESPONSE_INDEX_QUERY_STRING_LIST_PACKET);
        // url list
        vector<string> url_list;
        for (UINT16 i = 0; i < count; ++i)
        {
            const STRING *str = (const STRING*)(buffer_.data_.get() + offset);
            string data((const char*)(str->StringData), str->Length);
            url_list.push_back(data);
            // offset
            offset += sizeof(STRING) + str->Length;
        }
        // return 
        return url_list;
    }

    //////////////////////////////////////////////////////////////////////////
    // 查询是否上报
    QueryNeedReportRequestPacket::p QueryNeedReportRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryNeedReportRequestPacket::p packet;
        if (QueryNeedReportRequestPacket::Check(buffer))
        {
            packet = QueryNeedReportRequestPacket::p(new QueryNeedReportRequestPacket(buffer));
        }
        return packet;
    }
    QueryNeedReportRequestPacket::p QueryNeedReportRequestPacket::CreatePacket(UINT32 transaction_id, Guid peer_guid)
    {
        return QueryNeedReportRequestPacket::p(new QueryNeedReportRequestPacket(transaction_id, peer_guid));
    }
    bool QueryNeedReportRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) return false;

        UINT32 offset = RequestServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryNeedReportRequestPacket::ACTION))
            return false;

        if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_NEEDREPORT_PACKET))
            return false;

        return true;
    }
    QueryNeedReportRequestPacket::QueryNeedReportRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_needreport_(NULL)
    {
        assert(QueryNeedReportRequestPacket::Check(buffer));
        request_index_query_needreport_ = (REQUEST_INDEX_QUERY_NEEDREPORT_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    QueryNeedReportRequestPacket::QueryNeedReportRequestPacket(UINT32 transaction_id, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_NEEDREPORT_PACKET), QueryNeedReportRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION)
        , request_index_query_needreport_(NULL)
    {
        request_index_query_needreport_ = (REQUEST_INDEX_QUERY_NEEDREPORT_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_needreport_->PeerGuid = peer_guid;
    }

    // Response

    QueryNeedReportResponsePacket::p QueryNeedReportResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryNeedReportResponsePacket::p packet;
        if (QueryNeedReportResponsePacket::Check(buffer))
        {
            packet = QueryNeedReportResponsePacket::p(new QueryNeedReportResponsePacket(buffer));
        }
        return packet;
    }
    QueryNeedReportResponsePacket::p QueryNeedReportResponsePacket::CreateErrorPacket(UINT transaction_id, UINT32 error_code)
    {
        return QueryNeedReportResponsePacket::p(new QueryNeedReportResponsePacket(transaction_id, error_code));
    }
    QueryNeedReportResponsePacket::p QueryNeedReportResponsePacket::CreatePacket(UINT32 transaction_id, UINT8 need_report)
    {
        return QueryNeedReportResponsePacket::p(new QueryNeedReportResponsePacket(transaction_id, need_report));
    }
    bool QueryNeedReportResponsePacket::Check(Buffer buffer)
    {
        if (!buffer) return false;

        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryNeedReportResponsePacket::ACTION))
            return false;

        if (ResponseServerPacket::ErrorCode(buffer) != 0)
            return true;

        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET))
            return false;

        return true;
    }
    QueryNeedReportResponsePacket::QueryNeedReportResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_needreport_(NULL)
    {
        assert(QueryNeedReportResponsePacket::Check(buffer));
        if (ErrorCode() == 0)
        {
            response_index_query_needreport_ = (RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        }
    }
    QueryNeedReportResponsePacket::QueryNeedReportResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryNeedReportResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_needreport_(NULL)
    {
        assert(error_code != 0);
    }
    QueryNeedReportResponsePacket::QueryNeedReportResponsePacket(UINT32 transaction_id, UINT8 need_report)
        : ResponseServerPacket(sizeof(RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET), QueryNeedReportResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_needreport_(NULL)
    {
        response_index_query_needreport_ = (RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        response_index_query_needreport_->NeedReport = need_report;
    }

    //////////////////////////////////////////////////////////////////////////
    // 查询DataCollection服务器
    QueryDataCollectionServerRequestPacket::p QueryDataCollectionServerRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryDataCollectionServerRequestPacket::p packet;
        if (QueryDataCollectionServerRequestPacket::Check(buffer))
        {
            packet = QueryDataCollectionServerRequestPacket::p(new QueryDataCollectionServerRequestPacket(buffer));
        }
        return packet;
    }
    QueryDataCollectionServerRequestPacket::p QueryDataCollectionServerRequestPacket::CreatePacket(UINT32 transaction_id, Guid peer_guid)
    {
        return QueryDataCollectionServerRequestPacket::p(new QueryDataCollectionServerRequestPacket(transaction_id, peer_guid));
    }
    bool QueryDataCollectionServerRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) return false;
        UINT32 offset = RequestServerPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;
        if (false == Packet::CheckAction(buffer, QueryDataCollectionServerRequestPacket::ACTION))
            return false;
        if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET))
            return false;
        return true;
    }
    QueryDataCollectionServerRequestPacket::QueryDataCollectionServerRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_datacollection_(NULL)
    {
        assert(QueryDataCollectionServerRequestPacket::Check(buffer));
        request_index_query_datacollection_ = (REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    QueryDataCollectionServerRequestPacket::QueryDataCollectionServerRequestPacket(UINT32 transaction_id, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET), QueryDataCollectionServerRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION)
        , request_index_query_datacollection_(NULL)
    {
        request_index_query_datacollection_ = (REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_datacollection_->PeerGuid = peer_guid;
    }

    // response
    QueryDataCollectionServerResponsePacket::p QueryDataCollectionServerResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryDataCollectionServerResponsePacket::p packet;
        if (QueryDataCollectionServerResponsePacket::Check(buffer))
        {
            packet = QueryDataCollectionServerResponsePacket::p(new QueryDataCollectionServerResponsePacket(buffer));
        }
        return packet;
    }
    QueryDataCollectionServerResponsePacket::p QueryDataCollectionServerResponsePacket::CreateErrorPacket(UINT32 transaction_id, UINT32 error_code)
    {
        return QueryDataCollectionServerResponsePacket::p(new QueryDataCollectionServerResponsePacket(transaction_id, error_code));
    }
    QueryDataCollectionServerResponsePacket::p QueryDataCollectionServerResponsePacket::CreatePacket(UINT32 transaction_id, const DATACOLLECTION_SERVER_INFO& datacollection_server_info)
    {
        return QueryDataCollectionServerResponsePacket::p(new QueryDataCollectionServerResponsePacket(transaction_id, datacollection_server_info));
    }
    bool QueryDataCollectionServerResponsePacket::Check(Buffer buffer)
    {
        if (!buffer) return false;
        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;
        if (buffer.length_ < offset)
            return false;
        if (false == Packet::CheckAction(buffer, QueryDataCollectionServerResponsePacket::ACTION))
            return false;
        if (ResponseServerPacket::ErrorCode(buffer) != 0)
            return true;
        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET))
            return false;
        return true;
    }
    QueryDataCollectionServerResponsePacket::QueryDataCollectionServerResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_datacollection_(NULL)
    {
        assert(QueryDataCollectionServerResponsePacket::Check(buffer));
        if (ErrorCode() == 0)
        {
            response_index_query_datacollection_ = (RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        }
    }
    QueryDataCollectionServerResponsePacket::QueryDataCollectionServerResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryDataCollectionServerResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_datacollection_(NULL)
    {
        assert(error_code != 0);
    }
    QueryDataCollectionServerResponsePacket::QueryDataCollectionServerResponsePacket(UINT32 transaction_id, const DATACOLLECTION_SERVER_INFO& datacollection_server_info)
        : ResponseServerPacket(sizeof(RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET), QueryDataCollectionServerResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_datacollection_(NULL)
    {
        response_index_query_datacollection_ = (RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        response_index_query_datacollection_->DataCollectionServerInfo = datacollection_server_info;
    }

    //////////////////////////////////////////////////////////////////////////
    // 查询概率
    QueryUploadPicProbabilityRequestPacket::p QueryUploadPicProbabilityRequestPacket::ParseFromBinary(Buffer buffer)
    {
        QueryUploadPicProbabilityRequestPacket::p packet;
        if (QueryUploadPicProbabilityRequestPacket::Check(buffer))
        {
            packet = QueryUploadPicProbabilityRequestPacket::p(new QueryUploadPicProbabilityRequestPacket(buffer));
        }
        return packet;
    }
    QueryUploadPicProbabilityRequestPacket::p QueryUploadPicProbabilityRequestPacket::CreatePacket(UINT32 transaction_id, Guid peer_guid)
    {
        return QueryUploadPicProbabilityRequestPacket::p(new QueryUploadPicProbabilityRequestPacket(transaction_id, peer_guid));
    }
    bool QueryUploadPicProbabilityRequestPacket::Check(Buffer buffer)
    {
        if (!buffer) return false;

        UINT32 offset = RequestServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryUploadPicProbabilityRequestPacket::ACTION))
            return false;

        if (buffer.length_ < offset + sizeof(REQUEST_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET))
            return false;

        return true;
    }
    QueryUploadPicProbabilityRequestPacket::QueryUploadPicProbabilityRequestPacket(Buffer buffer)
        : RequestServerPacket(buffer)
        , request_index_query_uploadpicprobability_(NULL)
    {
        assert(QueryUploadPicProbabilityRequestPacket::Check(buffer));
        request_index_query_uploadpicprobability_ = (REQUEST_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
    }
    QueryUploadPicProbabilityRequestPacket::QueryUploadPicProbabilityRequestPacket(UINT32 transaction_id, Guid peer_guid)
        : RequestServerPacket(sizeof(REQUEST_INDEX_QUERY_NEEDREPORT_PACKET), QueryUploadPicProbabilityRequestPacket::ACTION, transaction_id, protocal::PEER_VERSION)
        , request_index_query_uploadpicprobability_(NULL)
    {
        request_index_query_uploadpicprobability_ = (REQUEST_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);
        request_index_query_uploadpicprobability_->PeerGuid = peer_guid;
    }

    // Response

    QueryUploadPicProbabilityResponsePacket::p QueryUploadPicProbabilityResponsePacket::ParseFromBinary(Buffer buffer)
    {
        QueryUploadPicProbabilityResponsePacket::p packet;
        if (QueryUploadPicProbabilityResponsePacket::Check(buffer))
        {
            packet = QueryUploadPicProbabilityResponsePacket::p(new QueryUploadPicProbabilityResponsePacket(buffer));
        }
        return packet;
    }
    QueryUploadPicProbabilityResponsePacket::p QueryUploadPicProbabilityResponsePacket::CreateErrorPacket(UINT transaction_id, UINT32 error_code)
    {
        return QueryUploadPicProbabilityResponsePacket::p(new QueryUploadPicProbabilityResponsePacket(transaction_id, error_code));
    }
    QueryUploadPicProbabilityResponsePacket::p QueryUploadPicProbabilityResponsePacket::CreatePacket(UINT32 transaction_id, FLOAT probability)
    {
        return QueryUploadPicProbabilityResponsePacket::p(new QueryUploadPicProbabilityResponsePacket(transaction_id, probability));
    }
    bool QueryUploadPicProbabilityResponsePacket::Check(Buffer buffer)
    {
        if (!buffer) return false;

        UINT32 offset = ResponseServerPacket::HEADER_LENGTH;

        if (buffer.length_ < offset)
            return false;

        if (false == Packet::CheckAction(buffer, QueryUploadPicProbabilityResponsePacket::ACTION))
            return false;

        if (ResponseServerPacket::ErrorCode(buffer) != 0)
            return true;

        if (buffer.length_ < offset + sizeof(RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET))
            return false;

        return true;
    }
    QueryUploadPicProbabilityResponsePacket::QueryUploadPicProbabilityResponsePacket(Buffer buffer)
        : ResponseServerPacket(buffer)
        , response_index_query_uploadpicprobability_(NULL)
    {
        assert(QueryUploadPicProbabilityResponsePacket::Check(buffer));
        if (ErrorCode() == 0)
        {
            response_index_query_uploadpicprobability_ = (RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        }
    }
    QueryUploadPicProbabilityResponsePacket::QueryUploadPicProbabilityResponsePacket(UINT32 transaction_id, UINT32 error_code)
        : ResponseServerPacket(0, QueryUploadPicProbabilityResponsePacket::ACTION, transaction_id, error_code)
        , response_index_query_uploadpicprobability_(NULL)
    {
        assert(error_code != 0);
    }
    QueryUploadPicProbabilityResponsePacket::QueryUploadPicProbabilityResponsePacket(UINT32 transaction_id, FLOAT upload_pic_probability)
        : ResponseServerPacket(sizeof(RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET), QueryUploadPicProbabilityResponsePacket::ACTION, transaction_id, 0)
        , response_index_query_uploadpicprobability_(NULL)
    {
        response_index_query_uploadpicprobability_ = (RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET*)(buffer_.data_.get() + ResponseServerPacket::HEADER_LENGTH);
        response_index_query_uploadpicprobability_->UploadPicProbability = upload_pic_probability;
    }

}
