#pragma once

/*******************************************************************************
*   Instance.h
*   class Instance, friend class Storage
*	��¼��Դ��̬��Ϣ
*	        ֧�ִ洢�ļ����ӳٴ������ӳ�ɾ����
*			�ļ��Ĺ�����ҪStorage�ļ��й���
*******************************************************************************/

#include "framework/timer/Timer.h"
#include "framework/timer/TimeCounter.h"
#include "storage/PendingGetSubPieceManager.h"
#include "storage/StatisticTools.h"

namespace storage
{
// 	struct HitRate
// 	{
// 		UINT64 from_mem;
// 		UINT64 from_disk;
// 		HitRate() : from_mem(0), from_disk(0) {}
// 		float GetHitRate() { size_t r = (from_mem+from_disk)==0 ? 0:float(from_mem)/(from_mem+from_disk)*100;return r; }
// 	};

    struct MetaData
    {
        UINT32 Duration;        // total duration of file in seconds
        UINT32 FileLength;      // file length in bytes
        UINT32 VideoDataRate;   // bytes per second
        UINT16 Width;           // width
        UINT16 Height;          // height
        string FileFormat;      // lower case file extension

        // constructor
        MetaData() 
        {
            Duration = 0;
            FileLength = 0;
            Width = 0;
            Height = 0;
            FileFormat = "none";
        }
    };

	class Instance 
		: public boost::noncopyable
		, public boost::enable_shared_from_this<Instance>
		, public framework::timer::ITimerListener
		, public  IInstance
	{
	public:
		friend class Resource;
		typedef boost::shared_ptr<Instance> p;
	public:
		friend class Storage;

		// ����һ��ӵ�и�url��instance
		static Instance::p Create(UrlInfo url_info);

		// ����FileResourceInfo��Resource_p����һ���µ�instance
		static Instance::p Open(FileResourceInfo r_f,Resource::p resource_p);

		~Instance() { Stop(); }

	private:
		Instance();
		// ��ͣ
		virtual void Start();

		virtual void Stop();

		// �رձ�instance��֪ͨstorage���ͷſռ�
		virtual void Remove();

		virtual bool IsRunning();

	protected:
		Buffer ToBuffer();

		// msʲô��û��
		void  Merge(Instance::p);

		// ����instance�ϲ���new_instance�У���֪ͨdownload_driver��Ȼ��ɾ����instance
		void  BeMergedTo(Instance::p);

		// �ϲ���Ȼ��֪ͨStorageɾ����instance
		void  MergeResourceTo();

	public:
		virtual bool Rename(const tstring& newname);

		// ���ĳ��subpiece����block�ѱ��������򽫸�subpiece����upload_driver
		// ���򣬼����block��MD5ֵ��Ȼ���ٵ��ñ�����
		virtual void AsyncGetSubPiece(const SubPieceInfo& subpiece_info, 
			const boost::asio::ip::udp::endpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener);

		// ���ĳ��block�ѱ��������򽫸�block����upload_driver������...
		virtual void AsyncGetBlock(size_t block_index, IUploadListener::p listener);

		// ��pending_subpiece_manager���ļ��ж�ȡĳ��subpiece����player_listener
		virtual void AsyncGetSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener);
		
		// ��pending_subpiece_manager���ļ��ж�ȡĳ��subpiece����player_listener
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, IPlayerListener::p listener) ;
		
		// ��pending_subpiece_manager��������ȡmax_count��subpicec����֪ͨPlayerListener
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, u_int max_count, IPlayerListener::p listener) ;
		
		// ��pending_subpiece_manager�л��ļ����ҵ�ĳ��subpiece��Ȼ��merge��merge_to_instance_p
		virtual void MergeSubPiece(const SubPieceInfo& subpiece_info,Instance::p merge_to_instance_p);

		// ��subpiece��ӵ�׼��д��Ķ��У�����Ƿ��д���ļ������block, �ϴ���, (���merge_to_instance_p)
		virtual void OnMergeSubPieceSuccess(const SubPieceInfo& subpiece_info,Buffer buffer);

		virtual bool GetNextPieceForDownload(const PieceInfo &start_piece_index, PieceInfoEx& piece_for_download);
		virtual bool GetNextPieceForDownload(size_t start_position, PieceInfoEx& piece_for_download) ;
		virtual bool GetNextSubPieceForDownload(const SubPieceInfo &sub_subpiece_index, SubPieceInfo& subpiece_for_download);
		virtual bool GetNextSubPieceForDownload(size_t start_position, SubPieceInfo& subpiece_for_download);
	
		// ��subpiece��ӵ�׼��д��Ķ��У�������Ƿ��д���ļ��������block, �ϴ��Ȳ���
		virtual void AsyncAddSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer);

		// ��poolin�е�����subpiece�Ƶ�poolout�У�����Storage�߳�д���ļ�
		// ��д����ɺ���MainThread��poolout��ɾ����subpiece
		virtual	void WriteToResource();

		// ��poolin�е�ĳ��blockǨ�Ƶ�poolout�У�Ȼ����Storage�߳�д���ļ�
		// ��д����ɺ���MainThread��poolout��ɾ��
		virtual void WriteBlockToResource(size_t block_index);

		virtual void MemNeedWriteToResource(const set<size_t>& block_need_write);

		// ����file_length�����ļ���Դ
		virtual void SetFileLength(size_t file_length);

		// ����instance�ϴ���һ��subpiece�������ϴ�����ͳ��
		virtual void WeUploadSubPiece(size_t num);

		// �����Դ����Ϊ��(�������)�������rid_info������Դ�����������������ļ���Դ
		virtual bool SetRidInfo(const RidInfo& rid_info, MD5 content_md5, size_t content_bytes) ;

		virtual void SetContentNeedToAdd(bool is_need_to_add) {content_need_to_add_ = is_need_to_add;}

		virtual int GetRidOriginFlag() const { return flag_rid_origin_; }

		virtual void SetRidOriginFlag(int flag) { flag_rid_origin_ = flag; }

// -----------------------------------------------------------------
//  һϵ�л�ȡ������Ϣ�ĺ���
	public:
		virtual RID	    GetRID();
		virtual size_t  GetBlockSize() ;
		virtual size_t  GetBlockCount();
		virtual size_t  GetFileLength();  //�ȼ���GetResourceLength()
		virtual size_t  GetResourceLength(); 
		virtual size_t  GetDiskFileSize(); 
		virtual tstring GetResourceName();
		virtual size_t  GetDownloadBytes();
		virtual float	GetInstanceValue();

		virtual size_t  GetDDNum() { return download_driver_s_.size();}
		virtual int		GetStatus() { return instance_state_; }
		virtual size_t	GetRepeatDownloadSubPieceCount() { return add_repeat_subiece_count_; }

		virtual bool    HasPiece(const PieceInfo& piece_info) ;
		virtual bool    HasPiece(size_t start_postion) ;
		virtual bool    HasSubPiece(const SubPieceInfo& subpiece_info) ;
		virtual bool    IsComplete();
		virtual bool    CanRemove();
		virtual void	SetPureDownloadMode(bool b_mode) ;
		virtual bool	IsPureDownloadMode();
        virtual bool    HasRID() const { return !!memory_resource_desc_p_; }
	
		virtual void    GetUrls(set<UrlInfo>& url_s);
        virtual UrlInfo GetOriginalUrl();
		virtual void	GetRidInfo(RidInfo &rid_info);
		virtual bool	GetFileResourceInfo(FileResourceInfo &r_info);

		virtual BlockMap::p GetBlockMap();

        virtual MetaData& GetMetaData() { return meta_data_; }

        // ��ȡMetaData
        virtual bool ParseMetaData(Buffer buffer);

	protected:
		virtual void GetUrls(vector<UrlInfo>& url_s);
// -----------------------------------------------------------------
	public:
		/**`
		* @brief ����ԴRID��� ��� HttpServer	
		*/
		// ���url_info(����Ѵ��ڲ�������)����֪ͨdownload_driver
		virtual void AddUrlInfo(const vector<UrlInfo>& url_info_s) ;

		// ���url(����Ѵ��ڣ��滻����refer)����֪ͨdownload_driver
		virtual void AddUrlInfo(const UrlInfo& url_info) ;

		// ��url_info_s��ɾ��ĳ��url����֪ͨdownload_driver
		virtual void RemoveUrl(const string& url_str) ;

		// ��instance����ĳ��download_driver
		virtual void AttachDownloadDriver(IDownloadDriver::p download_driver);

		// ��instance��ɾ��ĳ��download_driver
		virtual void DettachDownloadDriver(IDownloadDriver::p download_driver);

	public:
		// ��contentд���ļ�����֪ͨdownload_driver
		virtual bool DoMakeContentMd5AndQuery(Buffer content_buffer);

		// ��ʱ��
		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		// ֪ͨDownloadDriver makeblock�ɹ���ʧ��
		virtual void OnNotifyHashBlock(size_t block_index,bool b_success);

		// ��poolout��ɾ��ĳ��subpiece
		virtual void OnWriteSubPieceFinish(SubPieceInfo subpiece_info);

		// ֪ͨStorage����Ӧ������֪ͨappmodule��AddRidUrlRequestPacket����֪ͨdownload_driver�������
		virtual void OnHashResourceFinish();

		// blockд����ϣ�����MD5ֵ������ļ��Ƿ�д����ϣ������֪ͨ����ģ���������
		// ��ȡ��Ҫ�ϴ���block���ϴ�
		virtual void OnPendingHashBlockFinish(size_t block_index,MD5 hash_val);

		virtual void OnHashBlockFinish(size_t block_index,MD5 hash_val, Buffer& buf, IUploadListener::p listener);
		
		// contentд����ϣ�֪ͨdownload_driver
		virtual void OnPendingHashContentFinish(MD5 hash_val, size_t content_bytes);
		
		// ֪ͨstorage�ر�instance���ͷ���Դ�ռ�
		virtual void OnResourceCloseFinish(Resource::p resource_p);

		// ��resource_p_�е�instanceָ�򱾶��󣬲�����Ϣд��ResourceInfo�ļ���(ResourceInfo.dat)
		// instance״̬: APPLY_RESOURCE --> HAVE_RESOURCE
		virtual void AttachResource(Resource::p resource_p);

		// ��Դ�ļ�������֪ͨstorage���ļ���Ϣд����Դ��Ϣ�ļ�
		virtual void OnDiskFileNameChange(tstring file_name) ;

		virtual void GetBlockPosition(size_t block_index,size_t &offset,u_int &length);

		virtual void GetSubPiecePosition(const SubPieceInfo &subpiec_info,size_t &offset,u_int &length);

	protected:
		// ����Դ����, pending_subpiece_manager��pending_get_subpiece_manager��ɾ��block
		// ֪ͨupload_listener��ȡsubpieceʧ�ܣ�֪ͨdownload_driver��makeblockʧ��
		virtual void OnRemoveResourceBlockFinish(size_t block_index);

	private:
		// ��Url�л�ȡ�ļ����������ȡʧ�ܣ�������һ���������ʾ�ļ���
		void ParseFileNameFromUrl(const UrlInfo &url_info,tstring &resource_name);

		// ͨ��subpiece����block��MD5ֵ��Ȼ�����ϴ��Ȳ���
		void PendingHashBlock(u_int block_index);

		// ͨ��subpiece����content��MD5ֵ
		void PendingHashContent();

		// ����url_info_s�ĵ�һ��Url������Դ�ļ����ļ�����Ȼ����Storage������Դ
		// instance״̬��NEED_RESOURCE --> APPLY_RESOURCE
		void TryCreateResource();

		

	protected:
		bool is_running_;
		bool b_pure_download_mode_;

		// instance��״̬����ʼΪAPPLY_RESOURCE
		// NEED_RESOURCE, APPLY_RESOURCE, HAVE_RESOURCE
		int  instance_state_;
		framework::timer::PeriodicTimer::p cfg_timer_; //killself and cfg_save timer! 10s
		framework::timer::PeriodicTimer::p traffic_timer_; // 1s
		framework::timer::OnceTimer::p merge_timer_; // 250ms
		framework::timer::TimeCounter delete_tc_;
		framework::timer::TimeCounter traffic_tc_;

		ResourceDescriptor::p memory_resource_desc_p_;
		PendingSubPieceManager::p pending_subpiece_manager_p_;
		PendingGetSubPieceManager::p pending_get_subpiece_req_manager_p_;

//		RidInfo instance_rid_info_;

		//����¼����url�����ܷ����ڶ��DownloadDriver,��Ӧ���start_url_info_;  
		set<IDownloadDriver::p> download_driver_s_;
		size_t dd_num_;
		// ��ʼ״̬ӦΪ"x.flv"��������Դ��ӦΪ����·���ļ�������Ϊ.tpp�ļ�
		tstring  resource_name_;
		size_t disk_file_size_;
		set<UrlInfo> url_info_s_;
		UrlInfo origanel_url_info_;
		Resource::p  resource_p_; 
		Instance::p  merge_to_instance_p_; 
		// ��Դ��block��״̬��NO_VERIFIED, HAS_BEEN_VERIFIED, BEING_VERIFIED
		std::vector<char>  block_verify_map_;
		size_t last_push_time_;
		list<size_t> traffic_list_;

	private:
		size_t    add_repeat_subiece_count_;
		SubPieceInfo  merging_pos_subpiece;

		//���ݸ�֪
		MD5 content_md5_;
		size_t content_bytes_;
		//size_t content_kb_;
		//map<size_t, Buffer> content_buf_;
        //bool is_content_buffer_full_;
        Buffer content_buffer_;

		bool content_need_to_add_;
		bool content_need_to_query_;
		HitRate hit_rate_;
		HitRate upload_hit_rate_;

        MetaData meta_data_;
		int flag_rid_origin_;
	};
}
