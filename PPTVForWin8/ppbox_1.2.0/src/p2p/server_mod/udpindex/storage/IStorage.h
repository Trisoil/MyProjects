#pragma once
#include "base/BlockMap.h"
#include "protocal/PeerPacket.h"


namespace storage
{
	interface IUploadListener
	{
		typedef boost::shared_ptr<IUploadListener> p;
		virtual void OnAsyncGetSubPieceSucced(RID& rid, SubPieceInfo& subpiece_info, boost::asio::ip::udp::endpoint& end_point, Buffer& buffer, RequestSubPiecePacket::p packet) = 0;
		virtual void OnAsyncGetSubPieceFailed(RID& rid, SubPieceInfo& subpiece_info, boost::asio::ip::udp::endpoint& end_point, int failed_code, RequestSubPiecePacket::p packet) = 0;
		virtual void OnAsyncGetBlockSucced(RID& rid, UINT block_index, Buffer& buffer) = 0;
		virtual void OnAsyncGetBlockFailed(RID& rid, UINT block_index, int failed_code) = 0;
	};

	interface IPlayerListener
	{
		typedef boost::shared_ptr<IPlayerListener> p;
		virtual void OnAsyncGetSubPieceSucced(size_t start_position, Buffer& buffer) = 0;
		virtual void OnAsyncGetSubPieceFailed(size_t start_position, int failed_code) = 0;
		virtual void OnAsyncGetBufferSucced(size_t start_position, Buffer& buffer) = 0;
	};

	interface IInstanceListener
	{
		typedef boost::shared_ptr<IInstanceListener> p;
		// 下载完成之后,生成的RidInfo 消息
		virtual void OnDownloadComplete(string url, RidInfo rid_info);
		// AddSubPiece 后 满一个Block 并且 校验 成功
		virtual void OnMakeBlockSucced(u_int block_info);
		// AddSubPiece 后 满一个Block 并且 校验 失败
		virtual void OnMakeBlockFailed(u_int block_info);
	};

	interface IInstance;

	interface IDownloadDriver
	{
		typedef boost::shared_ptr<IDownloadDriver> p;

		// 下载完成之后,生成的RidInfo 消息
		virtual void OnNoticeChangeResource(boost::shared_ptr<IInstance> instance_old, boost::shared_ptr<IInstance> instance_new) = 0;

		// 通知 DownloadDriver  rid 从无到有
		virtual void OnNoticeRIDChange() = 0;

		// 通知 DownloadDriver  url 信息发生了变化
		virtual void OnNoticeUrlChange() = 0;

		// 通知 DownloadDriver 下载完成
		virtual void OnNoticeDownloadComplete() = 0;

		// 通知 DownloadDriver 一个Block检验成功
		virtual void OnNoticeMakeBlockSucced(u_int block_info) = 0;

		// 通知 DownloadDriver 一个Block检验失败
		virtual void OnNoticeMakeBlockFailed(u_int block_info) = 0;

		// 通知 DownloadDriver contenthash校验成功
		virtual void OnNoticeContentHashSucced(string url, MD5 content_md5, size_t content_bytes, size_t file_length) = 0;
	};

    struct MetaData;
	interface IInstance
	{
		typedef boost::shared_ptr<IInstance> p;
		
		// 操作
		virtual BlockMap::p GetBlockMap() = 0;
		// 如果某个block已被鉴定，则将该block交给upload_driver，否则...
		virtual void AsyncGetBlock(size_t block_index, IUploadListener::p listener)=0;
		virtual void AsyncGetSubPiece(const SubPieceInfo& subpiece_info, const boost::asio::ip::udp::endpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener) = 0;
		virtual void AsyncAddSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer) = 0;
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, u_int max_count, IPlayerListener::p listener) = 0;
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, IPlayerListener::p listener)  = 0;
		virtual void AsyncGetSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener) = 0;
		virtual bool HasPiece(const PieceInfo& piece_info) = 0;
		virtual bool HasPiece(size_t start_postion) = 0;
		virtual bool HasSubPiece(const SubPieceInfo& subpiece_info) = 0;
        virtual bool HasRID() const = 0;

		//从start_piece_index之后的下一片未下载的piece
		virtual bool GetNextPieceForDownload(const PieceInfo &start_piece_index, PieceInfoEx& piece_for_download) = 0;
		virtual bool GetNextPieceForDownload(size_t start_position, PieceInfoEx& piece_for_download) = 0;
		//从sub_subpiece_index之后的下一片未下载的subpiece
		virtual bool GetNextSubPieceForDownload(const SubPieceInfo &sub_subpiece_index, SubPieceInfo& subpiece_for_download) = 0;
		virtual bool GetNextSubPieceForDownload(size_t start_position, SubPieceInfo& subpiece_for_download) = 0;
		// 属性
		//获得当前Instance的RID
		virtual RID GetRID() = 0;
        virtual void GetRidInfo(RidInfo &rid_info) = 0;
		//获得当前Instance的url_s  可能有多个
		virtual void GetUrls(set<UrlInfo>& url_s) = 0;
        virtual UrlInfo GetOriginalUrl() = 0;

		//获得文件长度
		virtual void SetFileLength(size_t file_length) = 0;
		virtual size_t GetFileLength() = 0;
        virtual size_t GetDownloadBytes() = 0;
		// Block是做md5校验的单位
		virtual size_t GetBlockSize() = 0;
		virtual size_t GetBlockCount() = 0;
		//判断是否下完
		virtual bool   IsComplete() = 0;

		// 关联类属性
		virtual void AttachDownloadDriver(IDownloadDriver::p download_driver) = 0;
		virtual void DettachDownloadDriver(IDownloadDriver::p download_driver) = 0;

		//做contentmd5校验并且发送请求
		virtual bool DoMakeContentMd5AndQuery(Buffer content_buffer) = 0;

		// 告诉instance上传了subpiece，用于上传流量统计
		virtual void WeUploadSubPiece(size_t num)=0;

		// 获取instance的资源价值，用于文件淘汰
		virtual float GetInstanceValue()=0;

		virtual void GetBlockPosition(size_t block_index,size_t &offset,u_int &length)=0;
		virtual void GetSubPiecePosition(const SubPieceInfo &subpiec_info,size_t &offset,u_int &length)=0;
        virtual bool ParseMetaData(Buffer buffer) = 0;
        virtual MetaData& GetMetaData() = 0;
		virtual int GetRidOriginFlag() const = 0;
		virtual void SetRidOriginFlag(int flag)=0;

	};

	interface IStorage
	{
		typedef boost::shared_ptr<IStorage> p;
	
		virtual void Start(	BOOL	bUseDisk,		        //是否使用磁盘, 如果是TRUE,则使用磁盘; 如果是FALSE,则纯内存
							ULONGLONG		ullDiskLimit,	//使用磁盘上限
							wstring			DiskPathName	//磁盘使用路径
							) = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;

		//根据创建Instance，此时没有RID以及文件长度等信息
		virtual IInstance::p CreateInstance(const UrlInfo& url_info, bool is_force = false) = 0;
		//通过RID来查询Instance
		virtual IInstance::p GetInstanceByRID(const RID& rid) = 0;
		//该URL获得ResourceInfo,包括文件大小等信息
		virtual void AttachRidByUrl(const string& url, const RidInfo& rid, MD5 content_md5, size_t content_bytes, int flag) = 0;
		//获得了新的关于该Rid的Url
		virtual void AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s) = 0;
		//获取当前有哪些RID信息
		virtual void GetLocalResources(set<RID>& rid_S_) = 0;
		//获得对应的mod_number,group_count的RID资源，即 RID % group_count == mod_number
		virtual void GetLocalResources(set<RID>& rid_s, u_int mod_number, u_int group_count) = 0;
		//删除url_info所对应的信息
		virtual void RemoveUrlInfo(const UrlInfo& url_info) = 0;
		//根据查询Content信息返回的ERRORCODE获得信息
		virtual void AttachContentStatusByUrl(const string& url, bool is_need_to_add) = 0;
        //获得了新的关于该url的文件名
        virtual void AttachFilenameByUrl(const string& url, const tstring& filename) = 0;
        //获得总共使用了的磁盘空间
        virtual __int64 GetUsedDiskSpace() = 0;

		virtual ~IStorage() {}
	};
}
