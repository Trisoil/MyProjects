#pragma once

/**
* @file
* @brief 协议包相关的基类
*/

#include "Packet.h"
#include "base/base.h"

namespace protocal
{
	class HttpServerInfo;
	/**
	*@brief Peer发向IndexServer的 QueryHttpServerByRid 包
	*/
	class QueryHttpServerByRidRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryHttpServerByRidRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			RID resource_id);
	public:
		static const UINT8 ACTION = 0x11;
	public:
		inline RID GetResourceID() const{ assert(request_index_query_http_server_by_rid_); return request_index_query_http_server_by_rid_->ResourceID;}
	private:
		REQUEST_INDEX_QUERY_HTTP_SERVER_BY_RID* request_index_query_http_server_by_rid_;
		static bool Check(Buffer buffer);
	protected:
		QueryHttpServerByRidRequestPacket(Buffer buffer);
		QueryHttpServerByRidRequestPacket(UINT32 transaction_id, UINT32 peer_version,
			RID resource_id);
	};

	/**
	*@brief IndexServer回给Peer的 QueryHttpServerByRid 包
	*/
	class QueryHttpServerByRidResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryHttpServerByRidResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT error_code,
			RID ResourceID, vector<UrlInfo> http_server);
		static p CreatePacket(RID ResourceID, vector<UrlInfo> http_server);
	public:
		static const UINT8 ACTION = 0x11;
		RID GetResourceID() const;
		UINT16 GetHttpServerCount() const;
		vector <UrlInfo> GetHttpServer () const;
		void AttatchEndPointInfo(UINT32 trasaction_id);
	private:
		static bool Check(Buffer buffer);
	protected:
		QueryHttpServerByRidResponsePacket(Buffer buffer);
		QueryHttpServerByRidResponsePacket(size_t this_length, UINT32 transaction_id, UINT error_code,
			RID ResourceID, vector<UrlInfo> http_server);
	};

	/**
	*@brief Peer发向IndexServer的 QueryRidByUrl 包
	*/
	class QueryRidByUrlRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryRidByUrlRequestPacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			UINT32 session_id, string url_string, string refer_string, Guid peer_guid);
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x12;
	private:
		static bool Check(Buffer buffer);
		REQUEST_INDEX_QUERY_RID_BY_URL * request_index_query_rid_by_url;
		REQUEST_INDEX_QUERY_RID_BY_URL_URLSTRING * request_index_query_rid_by_url_urlstring;
		REQUEST_INDEX_QUERY_RID_BY_URL_REFERSTRING * request_index_query_rid_by_url_referstring;
		REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA *request_index_query_rid_by_url_extra;
	public:
		inline UINT32 GetSessionID() const{ assert(request_index_query_rid_by_url); return request_index_query_rid_by_url->SessionID;}
		inline UINT16 GetUrlLength() const { assert(request_index_query_rid_by_url_urlstring); return request_index_query_rid_by_url_urlstring->UrlLength;}
		inline string GetUrlString() const { assert(request_index_query_rid_by_url_urlstring); return string((char*)request_index_query_rid_by_url_urlstring->UrlString, request_index_query_rid_by_url_urlstring->UrlLength);}
		inline UINT16 GetReferLength() const { assert(request_index_query_rid_by_url_referstring); return request_index_query_rid_by_url_referstring->ReferLength;}
		inline string GetReferString() const { assert(request_index_query_rid_by_url_referstring); return string((char*)request_index_query_rid_by_url_referstring->ReferString, request_index_query_rid_by_url_referstring->ReferLength);}
		Guid GetPeerGuid() const;
	protected:
		QueryRidByUrlRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
			UINT32 session_id, string url_string, string refer_string, Guid peer_guid);
		QueryRidByUrlRequestPacket(Buffer buffer);
	};

	/**
	*@brief IndexServer回给Peer的 QueryRidByUrl 包
	*/
	class QueryRidByUrlResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryRidByUrlResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT error_code,
			UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);
		static p CreatePacket(RID resource_id, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);

	public:
		static const UINT8 ACTION = 0x12;
		inline UINT32 GetSessionID() const{assert(response_index_query_rid_by_url);assert(ErrorCode() ==0);return response_index_query_rid_by_url->SessionID;}
		inline RID GetResourceID() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->ResourceID;}
		inline UINT32 GetDetectedIP() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->DetectedIP;}
		inline UINT16 GetDetectedPort() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->detectedPort;}
		inline UINT32 GetFileLength() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->FileLength;}
		inline UINT16 GetBlockNum() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->BlockNum;}
		inline UINT32 GetBlockSize() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->BlockSize;}
		RidInfo GetRidInfo() const;
		vector<MD5> GetBlockMD5() const;
		inline MD5 GetContentSenseMD5() const{ if (response_index_query_rid_by_url_content) return response_index_query_rid_by_url_content->ContentSenseMD5; else return MD5(); }
		inline UINT32 GetContentBytes() const{ if (response_index_query_rid_by_url_content) return response_index_query_rid_by_url_content->ContentBytes; else return 0; }

		void AttatchEndPointInfo(UINT32 transaction_id,
			UINT32 session_id, UINT32 detected_ip, UINT16 detected_port);

	private:
		RESPONSE_INDEX_QUERY_RID_BY_URL *response_index_query_rid_by_url;
		RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT *response_index_query_rid_by_url_content;
		static bool Check(Buffer buffer);
	protected:
		QueryRidByUrlResponsePacket (Buffer buffer);
		QueryRidByUrlResponsePacket (size_t this_length, UINT32 transaction_id, UINT error_code,
			UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);
	};
	
	/**
	*@brief Peer发向IndexServer的 AddRidUrl 包
	*/
	class AddRidUrlRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<AddRidUrlRequestPacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, RID resource_id, UINT32 file_length, UINT32 block_size,
			vector<MD5> block_md5, vector<UrlInfo> http_server, MD5 content_md5, size_t content_bytes);
	public:
		static const UINT8 ACTION = 0x13;
	private:
		static bool Check(Buffer buffer);
		//RESOURCE_INFO* resource_info_;
		base::ResourceInfo resource_;
		REQUEST_INDEX_ADD_RID_URL *request_index_add_rid_url_;
	public:
		inline Guid GetPeerGUID() const {assert(buffer_); return *(Guid*)(buffer_.data_.get() + RequestServerPacket::HEADER_LENGTH);}
		inline base::ResourceInfo GetResourceInfo() const { return resource_;}
		Guid GetContentMD5();
		UINT32 GetContentBytes();

	protected:
		AddRidUrlRequestPacket (size_t this_length, UINT32 transaction_id, UINT32 peer_version,
			Guid peer_guid, UINT16 length, RID resource_id, UINT32 file_length, UINT32 block_size,
			vector<MD5> block_md5, vector<UrlInfo> http_server, MD5 content_md5, size_t content_bytes);
		AddRidUrlRequestPacket (Buffer buffer);
	};

	/**
	*@brief IndexServer回给Peer的 AddRidUrl 包
	*/
	class AddRidUrlResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<AddRidUrlResponsePacket> p;	
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT8 status);
	public:
		static const UINT8 ACTION = 0x13;
	public:
		inline UINT8 GetStatus() const {assert(response_index_add_rid_url); assert(ErrorCode() ==0);return response_index_add_rid_url->Status;}
	private:
		RESPONSE_INDEX_ADD_RID_URL *response_index_add_rid_url;
		static bool Check(Buffer buffer);
	protected:
		AddRidUrlResponsePacket(Buffer buffer);	
		AddRidUrlResponsePacket(UINT32 transaction_id, UINT8 error_code,
			UINT8 status);
	};

	/**
	*@brief Peer发向IndexServer的 QueryTrackerList 包
	*/
	class QueryTrackerListRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryTrackerListRequestPacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid);
		static p ParseFromBinary(Buffer buffer);
		Guid GetPeerGuid() ;
	public:
		static const UINT8 ACTION = 0x14;
	private:
		static bool Check(Buffer buffer);
		REQUEST_INDEX_QUERY_TRACKER_LIST* request_index_query_tracker_list;

	protected:
		QueryTrackerListRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid);
		QueryTrackerListRequestPacket(Buffer buffer);
	};

	/**
	*@brief IndexServer回给Peer的 QueryTrackerList 包
	*/
	class QueryTrackerListResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryTrackerListResponsePacket> p;	
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info);
		static p CreatePacket(UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info);
	public:
		static const UINT8 ACTION = 0x14;
	public:
		inline UINT16 GetTrackerGroupCount() const {assert(response_index_query_tracker_list);assert(ErrorCode() ==0);return response_index_query_tracker_list->TrackerGroupCount;}
		inline UINT16 GetTrackerCount() const {assert(response_index_query_tracker_list);assert(ErrorCode() ==0);return response_index_query_tracker_list->TrackerCount;}
		vector<TRACKER_INFO> GetTrackerInfo() const;
		void AttachEndPointInfo(UINT32 transaction_id);
	private:
		RESPONSE_INDEX_QUERY_TRACKER_LIST* response_index_query_tracker_list;
		static bool Check(Buffer buffer);
	protected:
		QueryTrackerListResponsePacket(Buffer buffer);
		QueryTrackerListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
			UINT16 tracker_group_count, vector<TRACKER_INFO> tracker_info);
	};

	/**
	*@brief Peer发向IndexServer的 QueryStunServerList 包
	*/
	class QueryStunServerListRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryStunServerListRequestPacket> p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version);
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x15;
	private:
		static bool Check(Buffer buffer);
	protected:
		QueryStunServerListRequestPacket(UINT32 transaction_id, UINT32 peer_version);
		QueryStunServerListRequestPacket(Buffer buffer);
	};

	/**
	*@brief IndexServer回给Peer的 QueryStunServerList 包
	*/
	class QueryStunServerListResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryStunServerListResponsePacket> p;	
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT8 error_code,
			vector<STUN_SERVER_INFO> stun_server_info);
		static p CreatePacket(vector<STUN_SERVER_INFO> stun_server_info);
	public:
		static const UINT8 ACTION = 0x15;
	public:
		inline UINT16 GetStunServerCount() const {assert(response_index_query_stun_server_list);assert(ErrorCode() ==0);return response_index_query_stun_server_list->StunServerCount;}
		vector<STUN_SERVER_INFO> GetStunServerInfo() const;
		void AttachEndPointInfo(UINT32 transaction_id);
	private:
		RESPONSE_INDEX_QUERY_STUN_SERVER_LIST* response_index_query_stun_server_list;
		static bool Check(Buffer buffer);
	protected:
		QueryStunServerListResponsePacket(Buffer buffer);
		QueryStunServerListResponsePacket(size_t this_length, UINT32 transaction_id, UINT8 error_code,
			vector<STUN_SERVER_INFO> stun_server_info);
	};

	/**
	*@brief Peer发向IndexServer的 QueryRidByContent 包
	*/
	class QueryRidByContentRequestPacket : public RequestServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryRidByContentRequestPacket > p;
		static p CreatePacket(UINT32 transaction_id, UINT32 peer_version,
			MD5 content_sense_md5, UINT32 content_bytes, UINT32 file_length, Guid peer_guid );
		static p ParseFromBinary(Buffer buffer);
	public:
		static const UINT8 ACTION = 0x16;
	private:
		static bool Check(Buffer buffer);
		REQUEST_INDEX_QUERY_RID_BY_CONTENT * request_index_query_rid_by_content;
	public:
		inline MD5 GetContentSenseMD5() const{ assert(request_index_query_rid_by_content); return request_index_query_rid_by_content->ContentSenseMD5;}
		inline UINT32 GetContentBytes() const{ assert(request_index_query_rid_by_content); return request_index_query_rid_by_content->ContentBytes;}
		inline UINT32 GetFileLength() const{ assert(request_index_query_rid_by_content); return request_index_query_rid_by_content->FileLength;}
		inline Guid GetPeerGuid() const {assert(request_index_query_rid_by_content); return request_index_query_rid_by_content->PeerGuid;}
	protected:
		QueryRidByContentRequestPacket(size_t this_length, UINT32 transaction_id, UINT32 peer_version,
			MD5 content_sense_md5, UINT32 content_bytes, UINT32 file_length, Guid peer_guid );
		QueryRidByContentRequestPacket(Buffer buffer);
	};

	/**
	*@brief IndexServer回给Peer的 QueryRidByContent 包
	*/

	class QueryRidByContentResponsePacket : public ResponseServerPacket
	{
	public:
		typedef boost::shared_ptr<QueryRidByContentResponsePacket> p;
		static p ParseFromBinary(Buffer buffer);
		static p CreatePacket(UINT32 transaction_id, UINT error_code,
			UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);
		static p CreatePacket(RID resource_id, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);

	public:
		static const UINT8 ACTION = 0x16;
		inline UINT32 GetSessionID() const{assert(response_index_query_rid_by_url);assert(ErrorCode() ==0);return response_index_query_rid_by_url->SessionID;}
		inline RID GetResourceID() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->ResourceID;}
		inline UINT32 GetDetectedIP() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->DetectedIP;}
		inline UINT16 GetDetectedPort() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->detectedPort;}
		inline UINT32 GetFileLength() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->FileLength;}
		inline UINT16 GetBlockNum() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->BlockNum;}
		inline UINT32 GetBlockSize() const {assert(response_index_query_rid_by_url); assert(ErrorCode() ==0);return response_index_query_rid_by_url->BlockSize;}
		RidInfo GetRidInfo() const;
		vector<MD5> GetBlockMD5() const;
		inline MD5 GetContentSenseMD5() const{assert (response_index_query_rid_by_url_content);return response_index_query_rid_by_url_content->ContentSenseMD5;}
		inline UINT32 GetContentBytes() const{ assert (response_index_query_rid_by_url_content); return response_index_query_rid_by_url_content->ContentBytes;}
		void AttatchEndPointInfo(UINT32 transaction_id,
			UINT32 session_id, UINT32 detected_ip, UINT16 detected_port);

	private:
		RESPONSE_INDEX_QUERY_RID_BY_URL *response_index_query_rid_by_url;
		RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT *response_index_query_rid_by_url_content;
		static bool Check(Buffer buffer);
	protected:
		QueryRidByContentResponsePacket (Buffer buffer);
		QueryRidByContentResponsePacket (size_t this_length, UINT32 transaction_id, UINT error_code,
			UINT32 session_id, RID resource_id, UINT32 detected_ip, UINT16 detected_port, UINT32 file_length, UINT32 block_size, vector<MD5> block_md5, MD5 content_sense_md5, UINT32 content_bytes);
	};

    /**
     * 查询IndexServerList
     * @version
     */
    class QueryIndexServerListRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryIndexServerListRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x17;
        Guid GetPeerGuid() const { assert(request_index_query_indexserver_); return request_index_query_indexserver_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);

        REQUEST_INDEX_QUERY_INDEXSERVER_PACKET *request_index_query_indexserver_;

    protected:
        QueryIndexServerListRequestPacket(Buffer buffer);
        QueryIndexServerListRequestPacket(UINT32 transaction_id, UINT32 peer_version, Guid peer_guid);
    };

    /**
     * 查询IndexServerList结果
     * @version
     */
    class QueryIndexServerListResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryIndexServerListResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        // error_code 不能是0
        static p CreateErrorPacket(UINT32 transaction_id, UINT32 error_code);
        // mod_index_map 大小必须是 256, 这里不检查mod_index_map[i]是否越界
        static p CreatePacket(UINT32 transaction_id, const vector<UINT8>& mod_index_map, const vector<INDEX_SERVER_INFO>& index_servers);

    public:
        static const UINT8 ACTION = 0x17;
        vector<UINT8> GetModIndexMap() const { if (ErrorCode() != 0) return vector<UINT8>(); assert(response_index_query_indexserver_); return vector<UINT8>(response_index_query_indexserver_->ModIndexMap, response_index_query_indexserver_->ModIndexMap + INDEX_LIST_MOD_COUNT); }
        vector<INDEX_SERVER_INFO> GetIndexServers() const { if (ErrorCode() != 0) return vector<INDEX_SERVER_INFO>(); assert(response_index_query_indexserver_); return vector<INDEX_SERVER_INFO>(response_index_query_indexserver_->IndexServerInfos, response_index_query_indexserver_->IndexServerInfos + response_index_query_indexserver_->IndexServerCount); }
        UINT8 GetIndexServerCount() const { if (ErrorCode() != 0) return 0; assert(response_index_query_indexserver_); return response_index_query_indexserver_->IndexServerCount; }

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET *response_index_query_indexserver_;

    protected:
        QueryIndexServerListResponsePacket(Buffer buffer);
        QueryIndexServerListResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryIndexServerListResponsePacket(UINT32 transaction_id, const vector<UINT8>& mod_index_map, const vector<INDEX_SERVER_INFO>& index_servers);
    };

    /**
     * 查询测试网Url列表
     * @version 0.0.0.5
     */
    class QueryTestUrlListRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryTestUrlListRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x18;
        Guid GetPeerGuid() const { assert(request_index_query_string_list_); return request_index_query_string_list_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);
        REQUEST_INDEX_QUERY_STRING_LIST_PACKET *request_index_query_string_list_;

    protected:
        QueryTestUrlListRequestPacket(Buffer buffer);
        QueryTestUrlListRequestPacket(UINT32 transaction_id, Guid peer_guid);

    };

    /**
     * 返回查询测试网Url列表结果
     * @version 0.0.0.5
     */
    class QueryTestUrlListResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryTestUrlListResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreateErrorPacket(UINT32 transaction_id, UINT32 error_code);
        static p CreatePacket(UINT32 transaction_id, const vector<string>& url_list);

    public:
        static const UINT8 ACTION = 0x18;
        vector<string> GetTestUrlList() const;

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_STRING_LIST_PACKET *response_index_query_string_list_;

    protected:
        QueryTestUrlListResponsePacket(Buffer buffer);
        QueryTestUrlListResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryTestUrlListResponsePacket(UINT32 transaction_id, const vector<string>& url_list, UINT32 buffer_length);
    };


    /**
    * 查询关键字列表
    * @version 0.0.0.5
    */
    class QueryKeyWordListRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryKeyWordListRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x19;
        Guid GetPeerGuid() const { assert(request_index_query_string_list_); return request_index_query_string_list_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);
        REQUEST_INDEX_QUERY_STRING_LIST_PACKET *request_index_query_string_list_;

    protected:
        QueryKeyWordListRequestPacket(Buffer buffer);
        QueryKeyWordListRequestPacket(UINT32 transaction_id, Guid peer_guid);

    };

    /**
    * 返回查询关键字列表结果
    * @version 0.0.0.5
    */
    class QueryKeyWordListResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryKeyWordListResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreateErrorPacket(UINT32 transaction_id, UINT32 error_code);
        static p CreatePacket(UINT32 transaction_id, const vector<string>& keyword_list);

    public:
        static const UINT8 ACTION = 0x19;
        vector<string> GetKeyWordList() const;

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_STRING_LIST_PACKET *response_index_query_string_list_;

    protected:
        QueryKeyWordListResponsePacket(Buffer buffer);
        QueryKeyWordListResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryKeyWordListResponsePacket(UINT32 transaction_id, const vector<string>& keyword_list, UINT32 buffer_length);
    };

    //////////////////////////////////////////////////////////////////////////
    // 查询是否上报
    class QueryNeedReportRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryNeedReportRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x20;
        Guid GetPeerGuid() const { assert(request_index_query_needreport_); return request_index_query_needreport_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);
        REQUEST_INDEX_QUERY_NEEDREPORT_PACKET *request_index_query_needreport_;

    protected:
        QueryNeedReportRequestPacket(Buffer buffer);
        QueryNeedReportRequestPacket(UINT32 transaction_id, Guid peer_guid);
    };

    class QueryNeedReportResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryNeedReportResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreateErrorPacket(UINT transaction_id, UINT32 error_code);
        static p CreatePacket(UINT32 transaction_id, UINT8 need_report);

    public:
        static const UINT8 ACTION = 0x20;
        UINT8 NeedReport() const { if (ErrorCode() != 0) return 0; assert(response_index_query_needreport_); return response_index_query_needreport_->NeedReport; }

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET *response_index_query_needreport_;

    protected:
        QueryNeedReportResponsePacket(Buffer buffer);
        QueryNeedReportResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryNeedReportResponsePacket(UINT32 transaction_id, UINT8 need_report);

    };

    //////////////////////////////////////////////////////////////////////////
    //  查询DataCollection服务器
    // request
    class QueryDataCollectionServerRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryDataCollectionServerRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x21;
        Guid GetPeerGuid() const { assert(request_index_query_datacollection_); return request_index_query_datacollection_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);
        REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET *request_index_query_datacollection_;

    protected:
        QueryDataCollectionServerRequestPacket(Buffer buffer);
        QueryDataCollectionServerRequestPacket(UINT32 transaction_id, Guid peer_guid);
    };

    // response
    class QueryDataCollectionServerResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryDataCollectionServerResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreateErrorPacket(UINT32 transaction_id, UINT32 error_code);
        static p CreatePacket(UINT32 transaction_id, const DATACOLLECTION_SERVER_INFO& datacollection_server_info);

    public:
        static const UINT8 ACTION = 0x21;
        DATACOLLECTION_SERVER_INFO GetDataCollectionServerInfo() const { if (ErrorCode() != 0) return DATACOLLECTION_SERVER_INFO(); assert(response_index_query_datacollection_); return response_index_query_datacollection_->DataCollectionServerInfo; }

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET* response_index_query_datacollection_;

    protected:
        QueryDataCollectionServerResponsePacket(Buffer buffer);
        QueryDataCollectionServerResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryDataCollectionServerResponsePacket(UINT32 transaction_id, const DATACOLLECTION_SERVER_INFO& datacollection_server_info);
    };

    //////////////////////////////////////////////////////////////////////////
    // 概率

    class QueryUploadPicProbabilityRequestPacket : public RequestServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryUploadPicProbabilityRequestPacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreatePacket(UINT32 transaction_id, Guid peer_guid);

    public:
        static const UINT8 ACTION = 0x22;
        Guid GetPeerGuid() const { assert(request_index_query_uploadpicprobability_); return request_index_query_uploadpicprobability_->PeerGuid; }

    private:
        static bool Check(Buffer buffer);
        REQUEST_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET *request_index_query_uploadpicprobability_;

    protected:
        QueryUploadPicProbabilityRequestPacket(Buffer buffer);
        QueryUploadPicProbabilityRequestPacket(UINT32 transaction_id, Guid peer_guid);

    };

    class QueryUploadPicProbabilityResponsePacket : public ResponseServerPacket
    {
    public:
        typedef boost::shared_ptr<QueryUploadPicProbabilityResponsePacket> p;

        static p ParseFromBinary(Buffer buffer);
        static p CreateErrorPacket(UINT transaction_id, UINT32 error_code);
        static p CreatePacket(UINT32 transaction_id, FLOAT upload_pic_probability);

    public:
        static const UINT8 ACTION = 0x22;
        FLOAT UploadPicProbability() const { if (ErrorCode() != 0) return 0; assert(response_index_query_uploadpicprobability_); return response_index_query_uploadpicprobability_->UploadPicProbability; }

    private:
        static bool Check(Buffer buffer);
        RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET *response_index_query_uploadpicprobability_;

    protected:
        QueryUploadPicProbabilityResponsePacket(Buffer buffer);
        QueryUploadPicProbabilityResponsePacket(UINT32 transaction_id, UINT32 error_code);
        QueryUploadPicProbabilityResponsePacket(UINT32 transaction_id, FLOAT upload_pic_probability);

    };

}
