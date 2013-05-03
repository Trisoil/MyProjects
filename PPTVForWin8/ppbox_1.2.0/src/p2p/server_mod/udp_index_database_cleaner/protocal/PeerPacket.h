#pragma once

/**
* @file
* @brief Peer和Peer相关协议包相关的基类
*/

#include "Packet.h"
#include <vector>
#include "base/BlockMap.h"
#include "base/base.h"

namespace protocal
{
	/**
	*@brief  Error 包
	*/
	class ErrorPacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<ErrorPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT16 error_code, UINT16 error_info_length, Buffer error_info);
	public:
		static const UINT8 ACTION = 0x51;
	public:
		static const UINT16 PPV_EXCHANGE_NO_RESOURCEID = 0x0011;
		static const UINT16 PPV_EXCHANGE_NOT_DOWNLOADING = 0x0012;
		static const UINT16 PPV_CONNECT_NO_RESOURCEID = 0x0021;
		static const UINT16 PPV_CONNECT_CONNECTION_FULL = 0x0022;
		static const UINT16 PPV_ANNOUCE_NO_RESOURCEID = 0x0031;
		static const UINT16 PPV_SUBPIECE_NO_RESOURCEID = 0x0041;
		static const UINT16 PPV_SUBPIECE_SUBPIECE_NOT_FOUND = 0x0042;
        static const UINT16 PPV_SUBPIECE_UPLOAD_BUSY = 0x0043;
        static const UINT16 PPV_RIDINFO_NO_RESOURCEID = 0x0051;

	private:
		ERROR_PACKET* error_packet_;
	public:
		inline UINT16 GetErrorCode() const {assert(error_packet_); return error_packet_->ErrorCode; }
		inline UINT16 GetErrorInfoLength() const {assert(error_packet_); return error_packet_->ErrorInfoLength;}
		inline TempBuffer GetErrorInfo() const {assert(error_packet_); return TempBuffer(buffer_, buffer_.data_.get() + PeerPacket::HEADER_LENGTH + 4, buffer_.length_);}
	private:
		static bool Check(Buffer buffer);
	protected:
		ErrorPacket(Buffer buffer);
		ErrorPacket(size_t size_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT16 error_code, UINT16 error_info_length, Buffer error_info);
	};

	/**
	*@brief  Connect 包
	*/
	class ConnectPacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<ConnectPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT8 basic_info, UINT32 send_off_time, UINT32 peer_version, 
			CANDIDATE_PEER_INFO peer_info, UINT8 type, PEER_DOWNLOAD_INFO download_info, UINT16 ip_pool_size = 0);
		static const UINT8 ACTION = 0x52;
	public:
		inline UINT8 GetBasicInfo() const { assert(connect_packet_); return connect_packet_->BasicInfo; }
		inline bool IsRequest() const{ assert(connect_packet_); if (GetBasicInfo()%2 == 0) return true; return false;}
		inline UINT32 GetSendOffTime() const { assert(connect_packet_); return connect_packet_->SendOffTime; }
		inline UINT32 GetPeerVersion() const { assert(connect_packet_); return connect_packet_->PeerVersion; }
		inline CANDIDATE_PEER_INFO GetCandidatePeerInfo() const { assert(connect_packet_); return connect_packet_->CandidatePeerInfo; }
		inline UINT8 GetPeerType() const { assert(connect_packet_); return connect_packet_->PeerType; }
		inline PEER_DOWNLOAD_INFO GetPeerDownloadInfo() const { assert(connect_packet_); return connect_packet_->PeerDownloadInfo; }
        inline UINT16 GetIpPoolSize() const { assert(connect_packet_); return (connect_packet_->PeerVersion >= 0x00000009 ? connect_packet_->IpPoolSize : 0); }
	private:
		static bool Check(Buffer buffer);
	protected:
		CONNECT_PACKET_EX* connect_packet_;
	protected:
		ConnectPacket(Buffer buffer);
		ConnectPacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT8 basic_info, UINT32 send_off_time, UINT32 peer_version, 
			CANDIDATE_PEER_INFO peer_info, UINT8 peer_type, PEER_DOWNLOAD_INFO download_info, UINT16 ip_pool_size );
	};

	/**
	*@brief  RequestAnnounce 包
	*/
	class RequestAnnouncePacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<RequestAnnouncePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid);
	public:
		static const UINT8 ACTION = 0x53;
	private:
		static bool Check(Buffer buffer);
	protected:
		RequestAnnouncePacket(Buffer buffer);
		RequestAnnouncePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid);
	};

	/**
	*@brief  Announce 包
	*/
	class AnnouncePacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<AnnouncePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			PEER_DOWNLOAD_INFO peer_download_info, BlockMap::p block_map);
	public:
		static const UINT8 ACTION = 0x54;
	public:
		PEER_DOWNLOAD_INFO &GetPeerDownloadInfo() const {assert(announce_packet_); return announce_packet_->peer_download_info;}
		BlockMap::p GetBlockMap() const;
	private:
		static bool Check(Buffer buffer);
	private:
		ANNOUNCE_PACKET* announce_packet_;
	protected:
		AnnouncePacket(Buffer buffer);
		AnnouncePacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			PEER_DOWNLOAD_INFO peer_download_info, BlockMap::p block_map);
	};

	/**
	*@brief  RequestSubPiece 包
	*/
	class RequestSubPiecePacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<RequestSubPiecePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			vector<SUB_PIECE_INFO> subpiece_info_vector);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			SUB_PIECE_INFO sub_piece_info);
	public:
		static const UINT8 ACTION = 0x55;
	public:
		inline UINT16 GetPieceCount() const { assert(request_subpiece_packet_); return request_subpiece_packet_->PieceCount; }
		vector<SUB_PIECE_INFO> GetSubPieceInfo() const ;
	private:
		static bool Check(Buffer buffer);
	protected:
		REQUEST_SUBPIECE_PACKET* request_subpiece_packet_;
	protected:
		RequestSubPiecePacket(Buffer buffer);
		RequestSubPiecePacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			vector<SUB_PIECE_INFO> subpiece_info_vector);
		RequestSubPiecePacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			SUB_PIECE_INFO subpiece_info);
	};

	/**
	*@brief  SubPiece 包
	*/
	class SubPiecePacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<SubPiecePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			SUB_PIECE_INFO sub_piece_info, UINT16 sub_piece_length, Buffer sub_piece_content);
	public:
		static const UINT8 ACTION = 0x56;
	public:
		inline SUB_PIECE_INFO GetSubPieceInfo() const { assert(subpiece_packet_); return subpiece_packet_->SubPieceInfo;}
		inline UINT16 GetSubPieceLength() const { assert(subpiece_packet_);return subpiece_packet_->SubPieceLength;}
		inline TempBuffer GetSubPieceContent() const { return TempBuffer (buffer_, buffer_.data_.get()+PeerPacket::HEADER_LENGTH + 6, buffer_.length_ - PeerPacket::HEADER_LENGTH - 6);};
	private:
		static bool Check(Buffer buffer);
	protected:
		SUBPIECE_PACKET* subpiece_packet_;
	protected:
		SubPiecePacket(Buffer buffer);
		SubPiecePacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			SUB_PIECE_INFO sub_piece_info, UINT16 sub_piece_length, Buffer sub_piece_content);
	};

	/**
	*@brief  PeerExchange 包
	*/
	class PeerExchangePacket : public PeerPacket
	{
	public:
		typedef boost::shared_ptr<PeerExchangePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT8 basic_info, UINT8 peer_count, vector<CANDIDATE_PEER_INFO> candidate_peer_info);
	public:
		static const UINT8 ACTION = 0x57;
	public:
		inline UINT8 GetBasicInfo() const{ assert(peer_exchange_packet_); return peer_exchange_packet_->BasicInfo;};
		inline UINT8 GetPeerCount() const{ assert(peer_exchange_packet_); return peer_exchange_packet_->PeerCount;};
		inline bool IsRequest() const{ assert(peer_exchange_packet_); if (GetBasicInfo()%2 == 0) return true; return false;}
		vector<CANDIDATE_PEER_INFO> GetCandidatePeerInfo() const;
	private:
		static bool Check(Buffer buffer);
	protected:
		PEER_EXCHANGE_PACKET* peer_exchange_packet_;

	protected:
		PeerExchangePacket(Buffer buffer);
		PeerExchangePacket(size_t this_length, UINT32 transaction_id, UINT32 sequece_id, RID rid, Guid peer_guid,
			UINT8 basic_info, UINT8 peer_count, vector<CANDIDATE_PEER_INFO> candidate_peer_info);
	};

    /**
     * RIDInfoRequestPacket
     */
    class RIDInfoRequestPacket : public PeerPacket
    {
    public:
        typedef boost::shared_ptr<RIDInfoRequestPacket> p;
    public:
        static RIDInfoRequestPacket::p ParseFromBinary(Buffer buffer);
        static RIDInfoRequestPacket::p CreatePacket(UINT32 transaction_id, UINT32 sequence_id, RID rid, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x58;

    private:
        static bool Check(Buffer buffer);

    protected:
        RIDInfoRequestPacket(Buffer buffer);
        RIDInfoRequestPacket(UINT32 transaction_id, UINT32 sequence_id, RID rid, Guid peer_guid);
    };

    /**
     * RIDInfoResponsePacket
     */
    class RIDInfoResponsePacket : public PeerPacket
    {
    public:
        typedef boost::shared_ptr<RIDInfoResponsePacket> p;
    public:
        static RIDInfoResponsePacket::p ParseFromBinary(Buffer buffer);
        static RIDInfoResponsePacket::p CreatePacket(UINT32 transaction_id, UINT32 sequence_id, Guid peer_guid, const RidInfo& rid_info);

    public:
        static const UINT8 ACTION = 0x59;

    public:
        RidInfo GetRIDInfo() const;

    private:
        static bool Check(Buffer buffer);

    private:
        PEER_RIDINFO_RESPONSE_PACKET *rid_info_response_;

    protected:
        RIDInfoResponsePacket(Buffer buffer);
        RIDInfoResponsePacket(UINT32 transaction_id, UINT32 sequence_id, Guid peer_guid, const RidInfo& rid_info);

    };
}