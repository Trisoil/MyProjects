/**
* @file
* @brief 协议包相关的基类
*/

#ifndef _PROTOCOL_INDEX_PACKET_H_
#define _PROTOCOL_INDEX_PACKET_H_

//#include <protocol/Base.h>
#include <protocol/ServerPacket.h>

#include <protocol/UrlInfo.h>

#include <util/serialization/stl/vector.h>

typedef framework::string::Uuid MD5;

using namespace storage;

namespace protocol
{
    /**
    *@brief Peer发向IndexServer的 QueryHttpServerByRid 包
    * IndexServer回给Peer的 QueryHttpServerByRid 包
    */
    struct QueryHttpServerByRidPacket
        : public ServerPacketT<0x11>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.rid_;
            } else {
                ar & response.rid_;
                ar & util::serialization::make_sized<boost::uint16_t>(response.url_infos_);
            } 
        }

        QueryHttpServerByRidPacket()
        {
            //IsRequest = 0;
        }
        
        //request
        QueryHttpServerByRidPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version, 
            RID resource_id, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.rid_ = resource_id;
            end_point_ = server_list_endpoint_;
            IsRequest = 1;
        }

        //response
        QueryHttpServerByRidPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            RID resource_id,
            std::vector<UrlInfo> http_server, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.rid_ = resource_id;
            response.url_infos_ = http_server;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryHttpServerByRidPacket(
            RID resource_id,
            std::vector<UrlInfo> http_server,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            QueryHttpServerByRidPacket(0, 0, resource_id, http_server, server_list_endpoint_);
        }

        struct Request {
            RID rid_;
        } request;
        struct Response {
            RID rid_;
            std::vector<UrlInfo> url_infos_;
        } response;
    };

    /**
    *@brief Peer发向IndexServer的 QueryRidByUrl 包
    * IndexServer回给Peer的 QueryRidByUrl 包
    */
    struct QueryRidByUrlPacket 
        : public ServerPacketT<0x12>
    {
    public:
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.session_id_;
                ar & util::serialization::make_sized<boost::uint16_t>(request.url_string_);
                ar & util::serialization::make_sized<boost::uint16_t>(request.refer_string_);
				ar & request.peer_guid_;
				ar & framework::container::make_array(request.reverce_, 10); 
            } else{
                boost::uint16_t BlockCount = (boost::uint16_t)response.block_md5_.size();
                ar & response.session_id_;
                ar & response.rid_;
                ar & response.detected_ip_;
                ar & response.detected_port_;
                ar & response.file_length_;
                ar & BlockCount;
                ar & response.block_size_;
                util::serialization::serialize_collection(ar, response.block_md5_, BlockCount);
                ar & response.content_sense_md5_;
                ar & response.content_bytes_;
            }
        }

        QueryRidByUrlPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryRidByUrlPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            boost::uint32_t session_id, 
            string url_string, 
            string refer_string,
			Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.session_id_ = session_id;
            request.url_string_ = url_string;
            request.refer_string_ = refer_string;
			request.peer_guid_ = peer_guid;
            IsRequest = 1;
        }

        //response
        QueryRidByUrlPacket(
            boost::uint32_t transaction_id, 
            boost::uint8_t error_code,
            boost::uint32_t session_id,
            RID resource_id,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_port,
            boost::uint32_t file_length,
            boost::uint32_t block_size,
            std::vector<MD5> block_md5,
            MD5 content_sense_md5,
            boost::uint32_t content_bytes, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.session_id_ = session_id;
            response.rid_ = resource_id;
            response.detected_ip_ = detected_ip;
            response.detected_port_ = detected_port;
            response.file_length_ = file_length;
            response.block_size_ = block_size;
            response.block_md5_ = block_md5;
            response.content_sense_md5_ = content_sense_md5;
            response.content_bytes_ = content_bytes;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryRidByUrlPacket(
            boost::uint32_t session_id,
            RID resource_id,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_port,
            boost::uint32_t file_length,
            boost::uint32_t block_size,
            std::vector<MD5> block_md5,
            MD5 content_sense_md5,
            boost::uint32_t content_bytes,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            QueryRidByUrlPacket(0, 0, session_id, resource_id, detected_ip, detected_port, file_length, block_size,
                block_md5, content_sense_md5, content_bytes, server_list_endpoint_);
        }
        
        struct Request {
            boost::uint32_t session_id_;
            std::string url_string_;
            std::string refer_string_;
			Guid peer_guid_;
			boost::uint8_t reverce_[10];
        } request;
        struct Response {
            boost::uint32_t session_id_;
            RID rid_;
            boost::uint32_t detected_ip_;        
            boost::uint16_t detected_port_;
            boost::uint32_t file_length_;
            boost::uint32_t block_size_;
            std::vector<MD5> block_md5_;
            MD5 content_sense_md5_;
            boost::uint32_t content_bytes_;
        } response;
    };

    /**
    *@brief Peer发向IndexServer的 AddRidUrl 包
    * IndexServer回给Peer的 AddRidUrl 包
    */
    struct AddRidUrlPacket 
        : public ServerPacketT<0x13>
    {
        struct ResourceInfo
        {
			boost::uint16_t resource_info_length_;
            RID rid_;
            boost::uint32_t file_length_;
            boost::uint16_t block_num_;
            boost::uint32_t block_size_;
            std::vector<MD5> md5_s_;
            std::vector<UrlInfo> url_info_s_;

            template <typename Archive>
            void serialize(Archive & ar)
            {
				block_num_ = md5_s_.size();
				resource_info_length_ = 30 + 16 * block_num_;
				ar & resource_info_length_;
                ar & rid_ & file_length_;
				ar & block_num_;
				ar & block_size_;
				util::serialization::serialize_collection(ar, md5_s_, block_num_); 
                ar & util::serialization::make_sized<boost::uint16_t>(url_info_s_); 
            }

            friend std::ostream& operator<<(
                std::ostream& os, 
                const ResourceInfo& info)
            {
                os << "(" << "ResourceID: " << info.rid_  << ", FileLength: " << info.file_length_ 
                    << ", BlockNumber: " << info.md5_s_.size() << ", BlockSize: " << info.block_size_
                    /*<< ", BlockMD5s: [" << info.md5_s_ << "], UrlInfos: [" << info.url_info_s_ << "] )"*/;
                return os;
            }
        };

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
                ar & request.resource_;
				
				ar & request.content_md5_;
				ar & request.content_bytes_;
            } else {
                ar & response.status_;
            }
        }

        AddRidUrlPacket()
        {
            //IsRequest = 0;
        }

        //request
        AddRidUrlPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            Guid peer_guid, 
            RID resource_id, 
            boost::uint32_t file_length, 
            boost::uint32_t  block_size,
            std::vector<MD5> const & block_md5, 
            std::vector<UrlInfo> const & http_server, 
			MD5 content_md5,
			boost::uint32_t content_bytes,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            request.resource_.rid_ = resource_id;
            request.resource_.file_length_ = file_length;
            request.resource_.block_size_ = block_size;
            request.resource_.md5_s_ = block_md5;
            request.resource_.url_info_s_ = http_server;
            request.resource_.rid_ = resource_id;
			request.content_md5_ = content_md5;
			request.content_bytes_ = content_bytes;
            IsRequest = 1;
        }

        //response
        AddRidUrlPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint8_t status, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.status_ = status;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        AddRidUrlPacket(boost::uint8_t status, boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            AddRidUrlPacket(0, 0, status, server_list_endpoint_);
        }

        struct Request {
            Guid peer_guid_;
            ResourceInfo resource_;
			MD5 content_md5_;
			boost::uint32_t content_bytes_;
        } request;
        struct Response {
            boost::uint8_t status_;
        } response;
    };

    /**
    *@brief Peer发向IndexServer的 QueryRidByContent 包
    * IndexServer回给Peer的 QueryRidByContent 包
    */
    struct QueryRidByContentPacket
        :public ServerPacketT<0x16>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.content_sense_md5_ 
                    & request.content_bytes_ 
                    & request.file_length_ 
                    & request.peer_guid_ 
                    & framework::container::make_array(request.reverce_, 10); 
            } else {
                boost::uint16_t block_num_ = response.block_md5_.size();
                ar & response.session_id_ 
                    & response.resource_id_
                    & response.detected_ip_ 
                    & response.detected_port_ 
                    & response.file_length_ 
                    & block_num_
                    & response.block_size_;
                util::serialization::serialize_collection(ar, response.block_md5_, block_num_) ; 
                ar & response.content_sense_md5_ 
                    & response.content_bytes_ ; 
            }
        }

        QueryRidByContentPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryRidByContentPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            MD5 content_sense_md5, 
            boost::uint32_t content_bytes, 
            boost::uint32_t file_length, 
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;       
            request.content_sense_md5_ = content_sense_md5;
            request.content_bytes_ = content_bytes;
            request.file_length_ = file_length;
            IsRequest = 1;
        }

        //response
        QueryRidByContentPacket(
            boost::uint32_t transaction_id, 
            boost::uint8_t error_code,
            boost::uint32_t session_id,
            RID resource_id,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_port,
            boost::uint32_t file_length,
            boost::uint32_t block_size,
            std::vector<MD5> block_md5,
            MD5 content_sense_md5,
            boost::uint32_t content_bytes,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.session_id_ = session_id;
            response.resource_id_ = resource_id;
            response.detected_ip_ = detected_ip;
            response.detected_port_ = detected_port;
            response.file_length_ = file_length;
            response.block_size_ = block_size;
            response.block_md5_ = block_md5;
            response.content_sense_md5_ = content_sense_md5;
            response.content_bytes_ = content_bytes;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryRidByContentPacket(
            RID resource_id,
            boost::uint32_t file_length,
            boost::uint32_t block_size,
            std::vector<MD5> block_md5,
            MD5 content_sense_md5,
            boost::uint32_t content_bytes,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            QueryRidByContentPacket(0, 0, 0, resource_id, 0, 0, file_length, block_size, block_md5, content_sense_md5, content_bytes, server_list_endpoint_);
        }

        struct Request {
            MD5 content_sense_md5_;
            boost::uint32_t content_bytes_;
            boost::uint32_t file_length_;
            Guid peer_guid_;
            boost::uint8_t reverce_[10];
        } request;
        struct Response {
            boost::uint32_t session_id_;
            RID resource_id_;
            boost::uint32_t detected_ip_;
            boost::uint16_t detected_port_;
            boost::uint32_t file_length_;
            boost::uint32_t block_size_;
            std::vector<MD5> block_md5_;
            MD5 content_sense_md5_;
            boost::uint32_t content_bytes_;
        } response;

    private:
        QueryRidByContentPacket(QueryRidByContentPacket const &);
    };

    /**
    * 查询测试网Url列表及其返回结果
    * @version 0.0.0.5
    */
    struct QueryTestUrlListPacket
        :public ServerPacketT<0x18>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if(IsRequest){
                ar & request.peer_guid_;
            }else{
                ar & util::serialization::make_sized<boost::uint16_t>(response.test_urls_);
            }
        }

        QueryTestUrlListPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryTestUrlListPacket(
            boost::uint32_t transaction_id, 
            Guid peer_guid,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;   
            end_point_ = server_list_endpoint_;
            IsRequest = 1;
        }
        
        //response
        QueryTestUrlListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryTestUrlListPacket(
            boost::uint32_t transaction_id,
            const std::vector<String>& url_list,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            response.test_urls_ = url_list;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            std::vector<String> test_urls_;
        } response;

    };

    /**
    * 查询关键字列表及其返回结果
    * @version 0.0.0.5
    */
    struct QueryKeyWordListPacket
        :public ServerPacketT<0x19>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            } else {
                ar & util::serialization::make_sized<boost::uint16_t>(response.keywords_);
            }
        }

        QueryKeyWordListPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryKeyWordListPacket(
            boost::uint32_t transaction_id, 
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;   
            IsRequest = 1;
        }

        //response
        QueryKeyWordListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryKeyWordListPacket(
            boost::uint32_t transaction_id,
            const std::vector<String>& keyword_list,
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            response.keywords_ = keyword_list;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            std::vector<String> keywords_;
        } response;
    };

    /**
    *@brief 查询是否上报
    */
    struct QueryNeedReportPacket
        :public ServerPacketT<0x20>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if(IsRequest){
                ar & request.peer_guid_;
            }else{
                ar & response.need_report_;
                ar & response.interval_time_;
            }
        }

        QueryNeedReportPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryNeedReportPacket(
            boost::uint32_t transaction_id, 
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point_ = server_list_endpoint_;
            request.peer_guid_ = peer_guid;
            IsRequest = 1;
        }

        //response
        QueryNeedReportPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryNeedReportPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t need_report,
            boost::uint8_t interval_time, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            response.need_report_ = need_report;
            response.interval_time_ = interval_time;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            boost::uint8_t need_report_;
            boost::uint8_t interval_time_;
        } response;
    };

    /**
    *@brief 查询DataCollection服务器
    */
    struct QueryDataCollectionServerPacket 
        : public ServerPacketT<0x21>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            }else{
                ar & response.server_info_;
            }
        }

        QueryDataCollectionServerPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryDataCollectionServerPacket(
            boost::uint32_t transaction_id, 
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;
            IsRequest = 1;
        }

        //response
        QueryDataCollectionServerPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryDataCollectionServerPacket(
            boost::uint32_t transaction_id,
            const DATACOLLECTION_SERVER_INFO& datacollection_server_info, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            response.server_info_ = datacollection_server_info;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            DATACOLLECTION_SERVER_INFO server_info_;
        } response;
    };
    
    /**
    *@brief 概率
    */
    struct QueryUploadPicProbabilityPacket 
        : public ServerPacketT<0x22>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            }else{
                ar & response.upload_pic_probability_;
            }
        }

        QueryUploadPicProbabilityPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryUploadPicProbabilityPacket(
            boost::uint32_t transaction_id, 
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            end_point_ = server_list_endpoint_;
            transaction_id_ = transaction_id;
            request.peer_guid_ = peer_guid;
            IsRequest = 1;
        }

        //response
        QueryUploadPicProbabilityPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }
        QueryUploadPicProbabilityPacket(
            boost::uint32_t transaction_id,
            float upload_pic_probability, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            response.upload_pic_probability_ = upload_pic_probability;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            float upload_pic_probability_;
        } response;
    };

    /**
    *@brief notify
    */
    struct QueryNotifyListPacket
        :public ServerPacketT<0x24>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            } else {
                ar & util::serialization::make_sized<boost::uint16_t>(response.notify_server_info_);
            }
        }

        QueryNotifyListPacket()
        {
            //IsRequest = 0;
        }

        //request
        QueryNotifyListPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            Guid peer_guid, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point_ = server_list_endpoint_;
            IsRequest = 1;
        }

        //response
        QueryNotifyListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            std::vector<NOTIFY_SERVER_INFO> notify_server_info, 
            boost::asio::ip::udp::endpoint server_list_endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.notify_server_info_ = notify_server_info;
            end_point_ = server_list_endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            std::vector<NOTIFY_SERVER_INFO> notify_server_info_;
        } response;
    };

    template <typename PacketHandler>
    void register_index_packet(
        PacketHandler & handler)
      {
        handler.template register_packet<QueryHttpServerByRidPacket>();
        handler.template register_packet<QueryRidByUrlPacket>();
        handler.template register_packet<AddRidUrlPacket>();
        //handler.template register_packet<QueryTrackerListPacket>();
        //handler.template register_packet<QueryStunServerListPacket>();
        handler.template register_packet<QueryRidByContentPacket>();
        //handler.template register_packet<QueryIndexServerListPacket>();
        handler.template register_packet<QueryTestUrlListPacket>();
        handler.template register_packet<QueryKeyWordListPacket>();
        handler.template register_packet<QueryNeedReportPacket>();
        handler.template register_packet<QueryDataCollectionServerPacket>();
        handler.template register_packet<QueryUploadPicProbabilityPacket>();
        handler.template register_packet<QueryNotifyListPacket>();
    }

}

#endif // _PROTOCOL_INDEX_PACKET_H_
