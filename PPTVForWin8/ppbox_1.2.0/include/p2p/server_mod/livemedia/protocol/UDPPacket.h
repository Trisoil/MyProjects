
/**
* @file
* @brief Peer和Peer相关协议包相关的基类
*/

#ifndef UDP_PACKET_H_
#define UDP_PACKET_H_

#include "protocol/ServerPacket.h"

#include <util/serialization/stl/vector.h>

#include <protocol/Packet.h>

namespace protocol
{
	struct udp_common_packet : public Packet
	{
		//  true: 直播, false: 点播
		boost::uint8_t is_live_packet_;
		boost::uint8_t reserved_;
		boost::uint16_t protocol_version_;
		Guid resource_id_;

		virtual boost::uint32_t length() const
		{
			// live header
			return Packet::length() + 20;
		}
		
		// @TODO: protocol_version_ 需要再定义
		udp_common_packet() : reserved_(0) , is_live_packet_(128) , protocol_version_(13)
		{
		}
		
        template <typename Archive>
        void serialize(Archive & ar)
        {
			Packet::serialize( ar );
			ar & is_live_packet_ & reserved_ & protocol_version_ & resource_id_;
        }
	};

	template<boost::uint8_t act>
	struct udp_common_packet_T : public PacketT<act> , public udp_common_packet
	{
	};
	
	struct candidate_peer_info
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & local_ip_ & local_udp_port_ & local_tcp_port_ & detected_ip_ & detected_port_ & stun_ip_ & stun_port_ & reseved_;
        }

		boost::uint32_t length() const
		{
			return sizeof(local_ip_) + sizeof( local_udp_port_ ) + sizeof( local_tcp_port_ ) + sizeof( detected_ip_ )
			+ sizeof( detected_port_ ) + sizeof( stun_ip_ ) + sizeof( stun_port_ ) + sizeof( reseved_ );
		}
		
		candidate_peer_info( boost::uint32_t ip , boost::uint16_t udp_port , boost::uint16_t tcp_port , 
							boost::uint32_t detect_ip , boost::uint16_t detect_port , boost::uint32_t st_ip , boost::uint16_t st_port )
		{
			this->local_ip_ = ip;
			this->local_udp_port_ = udp_port;
			this->local_tcp_port_ = tcp_port;
			
			this->detected_ip_ = detect_ip;
			this->detected_port_ = detect_port;
			
			this->stun_ip_ = st_ip;
			this->stun_port_ = st_port;
			this->reseved_ = 0;
		}
		
		candidate_peer_info( const candidate_peer_info & pi )
		{
			this->local_ip_ = pi.local_ip_;
			this->local_udp_port_ = pi.local_udp_port_;
			this->local_tcp_port_ = pi.local_tcp_port_;
			
			this->detected_ip_ = pi.detected_ip_;
			this->detected_port_ = pi.detected_port_;
			
			this->stun_ip_ = pi.stun_ip_;
			this->stun_port_ = pi.stun_port_;
			this->reseved_ = 0;
		}
		
		candidate_peer_info()
		{
			this->local_ip_ = 0;
			this->local_udp_port_ = 0;
			this->local_tcp_port_ = 0;
			
			this->detected_ip_ = 0;
			this->detected_port_ = 0;
			
			this->stun_ip_ = 0;
			this->stun_port_ = 0;
			this->reseved_ = 0;
		}
		
		boost::uint32_t local_ip_;
		boost::uint16_t local_udp_port_;
		boost::uint16_t local_tcp_port_;
		
		boost::uint32_t detected_ip_;
		boost::uint16_t detected_port_;

		boost::uint32_t stun_ip_;
		boost::uint16_t stun_port_;

		boost::uint32_t reseved_;
	};
	
	struct peer_download_info
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & is_downloading_ & online_time_ & 
			averge_download_speed_in_KB_ & current_download_speed_in_KB_ & 
			averge_upload_speed_in_KB_ & current_upload_speed_in_KB_;
        }

		boost::uint32_t length() const
		{
			return sizeof(is_downloading_) + sizeof( online_time_ ) + sizeof( averge_download_speed_in_KB_ )
			+ sizeof( current_download_speed_in_KB_ ) + sizeof( averge_upload_speed_in_KB_ ) + sizeof( current_upload_speed_in_KB_ );
		}

		boost::uint8_t is_downloading_;
		boost::uint32_t online_time_;

		boost::uint16_t averge_download_speed_in_KB_;
		boost::uint16_t current_download_speed_in_KB_;
		boost::uint16_t averge_upload_speed_in_KB_;
		boost::uint16_t current_upload_speed_in_KB_;
	};
	
	struct p2p_connect_packet 
		: public udp_common_packet_T<0x52>
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
			udp_common_packet::serialize(ar);
			
            ar & peer_id_ & basic_info_ & 
			send_off_time_ & peer_version_ & candidate_peer_info_ & 
			peer_type_ & peer_download_info_ & ip_pool_size_;
        }
		
		p2p_connect_packet()
		{
		}
		
		virtual boost::uint32_t length() const
		{
			return udp_common_packet::length() + sizeof(peer_id_) + sizeof( basic_info_ ) + sizeof( send_off_time_ ) + sizeof(peer_version_)
			+ sizeof( candidate_peer_info_ ) + sizeof( peer_type_ ) + sizeof( peer_download_info_ ) + sizeof( ip_pool_size_ );
		}
		
		Guid peer_id_;
		boost::uint8_t basic_info_;
		boost::uint32_t send_off_time_;
		boost::uint32_t peer_version_;
		candidate_peer_info candidate_peer_info_;
		boost::uint8_t peer_type_;
		
		peer_download_info peer_download_info_;
		boost::uint16_t ip_pool_size_;
	};
	
	struct p2p_request_announce_packet
		: public udp_common_packet_T<0xC0>
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
			udp_common_packet::serialize(ar);
			
            ar & request_pieceid_ & unknown_pieceid_;
        }

		virtual boost::uint32_t length() const
		{
			return udp_common_packet::length() + sizeof(request_pieceid_) + sizeof( unknown_pieceid_ );
		}
		
		boost::uint32_t request_pieceid_;
		boost::uint32_t unknown_pieceid_;
	};
	
	struct live_buffer_map_packet
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
			ar & start_piece_id_ ;
			if ( is_request_ ) {
				length_ = buffer_map_.size();
				ar & length_;
			} else {
				ar & length_;
				this->buffer_map_.resize( length_ );
			}

			for ( size_t i = 0 ; i < this->length_ ; ++i ) {
				ar & this->buffer_map_[i];
			}
			
        }

		boost::uint32_t length() const
		{
			boost::uint32_t len = sizeof(start_piece_id_) + sizeof( length_ ) + buffer_map_.size() * sizeof(boost::uint8_t);
			return len;
		}

		boost::uint32_t start_piece_id_;
		boost::uint16_t length_;
		std::vector<boost::uint8_t> buffer_map_;
		bool is_request_;
	};
	
	struct p2p_annouce_packet
		: public udp_common_packet_T<0xC1>
	{
		p2p_annouce_packet() : is_request_(false) , header_piece_id_(0) , piece_info_count_(0)
		{
		}
		
		virtual boost::uint32_t length() const
		{
			return udp_common_packet::length() + live_buffer_map_.length() + sizeof(header_piece_id_) + sizeof( piece_info_count_ ) 
			+ sizeof( boost::uint16_t ) * subpiece_no_.size();
		}
		
        template <typename Archive>
        void serialize(Archive & ar)
        {
			this->live_buffer_map_.is_request_ = is_request_;
			
			udp_common_packet::serialize(ar);
			
			if ( this->is_request_ ) {
				this->piece_info_count_ = this->subpiece_no_.size();
				ar & this->piece_info_count_;
			} else {
				ar & this->piece_info_count_;
				this->subpiece_no_.resize( this->piece_info_count_ );
			}

            ar & header_piece_id_ ;

			for ( size_t i = 0 ; i < this->piece_info_count_ ; i++ ) {
				ar & subpiece_no_[i];
			}
			
			ar & live_buffer_map_;
        }

		live_buffer_map_packet live_buffer_map_;
		boost::uint32_t header_piece_id_;
		boost::uint16_t piece_info_count_;
//		boost::uint32_t start_piece_info_id_;
		std::vector<boost::uint16_t> subpiece_no_;
		bool is_request_;
	};

	struct sub_piece_info_t
	{
		boost::uint32_t piece_id_;
		boost::uint16_t sub_piece_id_;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & piece_id_ & sub_piece_id_;
        }

		static boost::uint32_t length()
		{
			return sizeof( boost::uint32_t ) + sizeof( boost::uint16_t );
		}
	};
	
	struct p2p_request_sub_piece_packet
		: public udp_common_packet_T<0xC2>
	{
		p2p_request_sub_piece_packet() : is_request_(false)
		{
		}

        template <typename Archive>
        void serialize(Archive & ar)
        {
			udp_common_packet::serialize(ar);
			
			if ( is_request_ ) {
				this->request_sub_piece_count_ = this->sub_piece_infos_.size();
				ar & this->request_sub_piece_count_;
				for ( size_t i = 0 ; i < this->sub_piece_infos_.size() ; ++i ) {
					ar & sub_piece_infos_[i];
				}
			} else {
				ar & this->request_sub_piece_count_;
				for ( size_t i = 0 ; i < this->request_sub_piece_count_ ; ++i ) {
					sub_piece_info_t pi;
					ar & pi;
					sub_piece_infos_.push_back( pi );
				}
			}

//            ar & util::serialization::make_sized<sub_piece_info_t>( sub_piece_infos_ );
        }

		boost::uint32_t length() const
		{
			boost::uint32_t len = udp_common_packet::length() + sizeof( request_sub_piece_count_ ) + sub_piece_infos_.size() * sub_piece_info_t::length();
			
			return len;
		}
		
		boost::uint8_t request_sub_piece_count_;
		std::vector<sub_piece_info_t> sub_piece_infos_;
		bool is_request_;
	};

	struct p2p_sub_piece_packet
		: public udp_common_packet_T<0xC3>
	{
		p2p_sub_piece_packet() :is_request_(false)
		{
		}

        template <typename Archive>
        void serialize(Archive & ar)
        {
			udp_common_packet::serialize(ar);
			
            ar & sub_piece_info_;
			if ( is_request_ ) {
				ar & util::serialization::make_sized<boost::uint16_t>( sub_piece_content_ );
/*
				this->sub_piece_length_ = this->sub_piece_content_.size();
				ar & sub_piece_length_ ;
				// 发送包
				for ( size_t i = 0 ; i < sub_piece_length_ ; i++ ) {
					ar & this->sub_piece_content_[i];
				}
 */
			} else {
				ar & sub_piece_length_ ;
				// 接收包
				for ( size_t i = 0 ; i < sub_piece_length_ ; i++ ) {
					boost::uint8_t c;
					ar & c;
					sub_piece_content_.push_back( c );
				}
			}

        }

		boost::uint32_t length() const
		{
			return udp_common_packet::length() + sizeof(sub_piece_info_) + sizeof( sub_piece_length_ ) + sub_piece_content_.size() * sizeof(boost::uint8_t) ;
		}

		sub_piece_info_t sub_piece_info_;
		boost::uint16_t sub_piece_length_;
		std::vector<boost::uint8_t> sub_piece_content_;
		bool is_request_;
	};
	
	struct error_packet
	: public udp_common_packet_T<0x51>
	{
        template <typename Archive>
        void serialize(Archive & ar)
        {
			udp_common_packet::serialize(ar);
			
            ar & error_code_ & error_detail_length_;
			ar & util::serialization::make_sized<boost::uint8_t>( error_detail_content_ );
        }
		
		boost::uint32_t length() const
		{
			return udp_common_packet::length() + sizeof(error_code_) + sizeof( error_detail_length_ ) + error_detail_content_.size() * sizeof(boost::uint8_t) ;
		}

		boost::uint16_t error_code_;
		boost::uint16_t error_detail_length_;
		std::vector<boost::uint8_t> error_detail_content_;
	};
	
	template <typename PacketHandler>
	void register_p2p_packet(
		PacketHandler & handler)
	{
		handler.template register_packet<p2p_connect_packet>();
		handler.template register_packet<p2p_request_announce_packet>();
		handler.template register_packet<p2p_annouce_packet>();
		handler.template register_packet<p2p_request_sub_piece_packet>();
		handler.template register_packet<p2p_sub_piece_packet>();
		/*
		handler.template register_packet<CommitPacket>();
		handler.template register_packet<KeepAlivePacket>();
		handler.template register_packet<LeavePacket>();
		handler.template register_packet<ReportPacket>();
		handler.template register_packet<QueryPeerCountPacket>();
		handler.template register_packet<QueryBatchPeerCountPacket>();
		 */
	}

}

#endif // UDP_PACKET_H_
