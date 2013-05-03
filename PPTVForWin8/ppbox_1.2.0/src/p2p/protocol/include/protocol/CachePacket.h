//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef PROTOCOL_CACHEPACKET_H
#define PROTOCOL_CACHEPACKET_H

#include "struct/Base.h"
#include "protocol/Packet.h"

namespace protocol
{
    enum CACHE_ERROR_CODE
    {
        CACHE_HIT,
        CACHE_MISS,
        CACHE_BUSY
    };
    struct QUERY_CACHE_URL_BY_RID_RESPONSE
    {
        RID ResourceID;
        string CacheUrl;

        QUERY_CACHE_URL_BY_RID_RESPONSE() {
            memset(this, 0, sizeof(QUERY_CACHE_URL_BY_RID_RESPONSE));
        }
    };

    /**
    *@brief QueryCacheUrlByRID Request && Response
    */
    struct QueryCacheUrlByRID
        : public ServerPacketT<0x24>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_
                    & request.rid_info_
                    & request.url_info_;
            } else {
                ar & response.rid_
                    & response.url_;
            }
        }

        QueryCacheUrlByRID()
        {
            // IsRequest = 0;
        }

        // request
        QueryCacheUrlByRID(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid& peer_guid,
            const RidInfo& rid_info,
            const UrlInfo& url_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            // request.rid_info_ = rid_info;
            request.rid_info_.ResourceID = rid_info.rid_;
            request.rid_info_.FileLength = rid_info.file_length_;
            request.rid_info_.BlockCount = (boost::uint16_t)rid_info.block_md5_s_.size();
            request.rid_info_.BlockSize = rid_info.block_size_;
            request.rid_info_.BlockMD5S = rid_info.block_md5_s_;

            request.url_info_ = url_info;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryCacheUrlByRID(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }
        QueryCacheUrlByRID(
            boost::uint32_t transaction_id,
            const RID& resource_id,
            const std::string& url,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            response.rid_ = resource_id;
            response.url_ = url;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
            RID_INFO rid_info_;
            UrlInfo url_info_;
        } request;
        struct Response {
            RID rid_;
            std::string url_;
        } response;
    };
}

#endif  // PROTOCOL_CACHEPACKET_H
