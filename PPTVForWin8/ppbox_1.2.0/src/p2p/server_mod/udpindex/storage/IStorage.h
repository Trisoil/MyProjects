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
		// �������֮��,���ɵ�RidInfo ��Ϣ
		virtual void OnDownloadComplete(string url, RidInfo rid_info);
		// AddSubPiece �� ��һ��Block ���� У�� �ɹ�
		virtual void OnMakeBlockSucced(u_int block_info);
		// AddSubPiece �� ��һ��Block ���� У�� ʧ��
		virtual void OnMakeBlockFailed(u_int block_info);
	};

	interface IInstance;

	interface IDownloadDriver
	{
		typedef boost::shared_ptr<IDownloadDriver> p;

		// �������֮��,���ɵ�RidInfo ��Ϣ
		virtual void OnNoticeChangeResource(boost::shared_ptr<IInstance> instance_old, boost::shared_ptr<IInstance> instance_new) = 0;

		// ֪ͨ DownloadDriver  rid ���޵���
		virtual void OnNoticeRIDChange() = 0;

		// ֪ͨ DownloadDriver  url ��Ϣ�����˱仯
		virtual void OnNoticeUrlChange() = 0;

		// ֪ͨ DownloadDriver �������
		virtual void OnNoticeDownloadComplete() = 0;

		// ֪ͨ DownloadDriver һ��Block����ɹ�
		virtual void OnNoticeMakeBlockSucced(u_int block_info) = 0;

		// ֪ͨ DownloadDriver һ��Block����ʧ��
		virtual void OnNoticeMakeBlockFailed(u_int block_info) = 0;

		// ֪ͨ DownloadDriver contenthashУ��ɹ�
		virtual void OnNoticeContentHashSucced(string url, MD5 content_md5, size_t content_bytes, size_t file_length) = 0;
	};

    struct MetaData;
	interface IInstance
	{
		typedef boost::shared_ptr<IInstance> p;
		
		// ����
		virtual BlockMap::p GetBlockMap() = 0;
		// ���ĳ��block�ѱ��������򽫸�block����upload_driver������...
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

		//��start_piece_index֮�����һƬδ���ص�piece
		virtual bool GetNextPieceForDownload(const PieceInfo &start_piece_index, PieceInfoEx& piece_for_download) = 0;
		virtual bool GetNextPieceForDownload(size_t start_position, PieceInfoEx& piece_for_download) = 0;
		//��sub_subpiece_index֮�����һƬδ���ص�subpiece
		virtual bool GetNextSubPieceForDownload(const SubPieceInfo &sub_subpiece_index, SubPieceInfo& subpiece_for_download) = 0;
		virtual bool GetNextSubPieceForDownload(size_t start_position, SubPieceInfo& subpiece_for_download) = 0;
		// ����
		//��õ�ǰInstance��RID
		virtual RID GetRID() = 0;
        virtual void GetRidInfo(RidInfo &rid_info) = 0;
		//��õ�ǰInstance��url_s  �����ж��
		virtual void GetUrls(set<UrlInfo>& url_s) = 0;
        virtual UrlInfo GetOriginalUrl() = 0;

		//����ļ�����
		virtual void SetFileLength(size_t file_length) = 0;
		virtual size_t GetFileLength() = 0;
        virtual size_t GetDownloadBytes() = 0;
		// Block����md5У��ĵ�λ
		virtual size_t GetBlockSize() = 0;
		virtual size_t GetBlockCount() = 0;
		//�ж��Ƿ�����
		virtual bool   IsComplete() = 0;

		// ����������
		virtual void AttachDownloadDriver(IDownloadDriver::p download_driver) = 0;
		virtual void DettachDownloadDriver(IDownloadDriver::p download_driver) = 0;

		//��contentmd5У�鲢�ҷ�������
		virtual bool DoMakeContentMd5AndQuery(Buffer content_buffer) = 0;

		// ����instance�ϴ���subpiece�������ϴ�����ͳ��
		virtual void WeUploadSubPiece(size_t num)=0;

		// ��ȡinstance����Դ��ֵ�������ļ���̭
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
	
		virtual void Start(	BOOL	bUseDisk,		        //�Ƿ�ʹ�ô���, �����TRUE,��ʹ�ô���; �����FALSE,���ڴ�
							ULONGLONG		ullDiskLimit,	//ʹ�ô�������
							wstring			DiskPathName	//����ʹ��·��
							) = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;

		//���ݴ���Instance����ʱû��RID�Լ��ļ����ȵ���Ϣ
		virtual IInstance::p CreateInstance(const UrlInfo& url_info, bool is_force = false) = 0;
		//ͨ��RID����ѯInstance
		virtual IInstance::p GetInstanceByRID(const RID& rid) = 0;
		//��URL���ResourceInfo,�����ļ���С����Ϣ
		virtual void AttachRidByUrl(const string& url, const RidInfo& rid, MD5 content_md5, size_t content_bytes, int flag) = 0;
		//������µĹ��ڸ�Rid��Url
		virtual void AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s) = 0;
		//��ȡ��ǰ����ЩRID��Ϣ
		virtual void GetLocalResources(set<RID>& rid_S_) = 0;
		//��ö�Ӧ��mod_number,group_count��RID��Դ���� RID % group_count == mod_number
		virtual void GetLocalResources(set<RID>& rid_s, u_int mod_number, u_int group_count) = 0;
		//ɾ��url_info����Ӧ����Ϣ
		virtual void RemoveUrlInfo(const UrlInfo& url_info) = 0;
		//���ݲ�ѯContent��Ϣ���ص�ERRORCODE�����Ϣ
		virtual void AttachContentStatusByUrl(const string& url, bool is_need_to_add) = 0;
        //������µĹ��ڸ�url���ļ���
        virtual void AttachFilenameByUrl(const string& url, const tstring& filename) = 0;
        //����ܹ�ʹ���˵Ĵ��̿ռ�
        virtual __int64 GetUsedDiskSpace() = 0;

		virtual ~IStorage() {}
	};
}
