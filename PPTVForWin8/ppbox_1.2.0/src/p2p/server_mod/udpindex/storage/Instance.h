#pragma once

/*******************************************************************************
*   Instance.h
*   class Instance, friend class Storage
*	记录资源动态信息
*	        支持存储文件的延迟创立和延迟删除！
*			文件的管理需要Storage的集中管理
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

		// 创建一个拥有该url的instance
		static Instance::p Create(UrlInfo url_info);

		// 根据FileResourceInfo和Resource_p创建一个新的instance
		static Instance::p Open(FileResourceInfo r_f,Resource::p resource_p);

		~Instance() { Stop(); }

	private:
		Instance();
		// 起停
		virtual void Start();

		virtual void Stop();

		// 关闭本instance并通知storage，释放空间
		virtual void Remove();

		virtual bool IsRunning();

	protected:
		Buffer ToBuffer();

		// ms什么都没做
		void  Merge(Instance::p);

		// 将本instance合并到new_instance中，并通知download_driver，然后删除本instance
		void  BeMergedTo(Instance::p);

		// 合并，然后通知Storage删除本instance
		void  MergeResourceTo();

	public:
		virtual bool Rename(const tstring& newname);

		// 如果某个subpiece所在block已被鉴定，则将该subpiece交给upload_driver
		// 否则，计算该block的MD5值，然后再调用本函数
		virtual void AsyncGetSubPiece(const SubPieceInfo& subpiece_info, 
			const boost::asio::ip::udp::endpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener);

		// 如果某个block已被鉴定，则将该block交给upload_driver，否则...
		virtual void AsyncGetBlock(size_t block_index, IUploadListener::p listener);

		// 从pending_subpiece_manager或文件中读取某个subpiece交给player_listener
		virtual void AsyncGetSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener);
		
		// 从pending_subpiece_manager或文件中读取某个subpiece交给player_listener
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, IPlayerListener::p listener) ;
		
		// 从pending_subpiece_manager中连续读取max_count个subpicec，并通知PlayerListener
		virtual void AsyncGetSubPieceForPlay(size_t start_postion, u_int max_count, IPlayerListener::p listener) ;
		
		// 从pending_subpiece_manager中或文件中找到某个subpiece，然后merge到merge_to_instance_p
		virtual void MergeSubPiece(const SubPieceInfo& subpiece_info,Instance::p merge_to_instance_p);

		// 将subpiece添加到准备写入的队列，检查是否该写入文件，检查block, 上传等, (针对merge_to_instance_p)
		virtual void OnMergeSubPieceSuccess(const SubPieceInfo& subpiece_info,Buffer buffer);

		virtual bool GetNextPieceForDownload(const PieceInfo &start_piece_index, PieceInfoEx& piece_for_download);
		virtual bool GetNextPieceForDownload(size_t start_position, PieceInfoEx& piece_for_download) ;
		virtual bool GetNextSubPieceForDownload(const SubPieceInfo &sub_subpiece_index, SubPieceInfo& subpiece_for_download);
		virtual bool GetNextSubPieceForDownload(size_t start_position, SubPieceInfo& subpiece_for_download);
	
		// 将subpiece添加到准备写入的队列，并检查是否该写入文件，并检查block, 上传等操作
		virtual void AsyncAddSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer);

		// 将poolin中的所有subpiece移到poolout中，并由Storage线程写入文件
		// 待写入完成后交由MainThread从poolout中删除该subpiece
		virtual	void WriteToResource();

		// 将poolin中的某个block迁移到poolout中，然后由Storage线程写入文件
		// 待写入完成后交由MainThread从poolout中删除
		virtual void WriteBlockToResource(size_t block_index);

		virtual void MemNeedWriteToResource(const set<size_t>& block_need_write);

		// 根据file_length创建文件资源
		virtual void SetFileLength(size_t file_length);

		// 告诉instance上传了一个subpiece，用于上传流量统计
		virtual void WeUploadSubPiece(size_t num);

		// 如果资源描述为空(正常情况)，则根据rid_info创建资源描述符，进而创建文件资源
		virtual bool SetRidInfo(const RidInfo& rid_info, MD5 content_md5, size_t content_bytes) ;

		virtual void SetContentNeedToAdd(bool is_need_to_add) {content_need_to_add_ = is_need_to_add;}

		virtual int GetRidOriginFlag() const { return flag_rid_origin_; }

		virtual void SetRidOriginFlag(int flag) { flag_rid_origin_ = flag; }

// -----------------------------------------------------------------
//  一系列获取属性信息的函数
	public:
		virtual RID	    GetRID();
		virtual size_t  GetBlockSize() ;
		virtual size_t  GetBlockCount();
		virtual size_t  GetFileLength();  //等价于GetResourceLength()
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

        // 提取MetaData
        virtual bool ParseMetaData(Buffer buffer);

	protected:
		virtual void GetUrls(vector<UrlInfo>& url_s);
// -----------------------------------------------------------------
	public:
		/**`
		* @brief 给资源RID添加 多个 HttpServer	
		*/
		// 添加url_info(如果已存在不做操作)，并通知download_driver
		virtual void AddUrlInfo(const vector<UrlInfo>& url_info_s) ;

		// 添加url(如果已存在，替换本地refer)，并通知download_driver
		virtual void AddUrlInfo(const UrlInfo& url_info) ;

		// 从url_info_s中删除某个url，并通知download_driver
		virtual void RemoveUrl(const string& url_str) ;

		// 向instance加入某个download_driver
		virtual void AttachDownloadDriver(IDownloadDriver::p download_driver);

		// 从instance中删掉某个download_driver
		virtual void DettachDownloadDriver(IDownloadDriver::p download_driver);

	public:
		// 将content写入文件，并通知download_driver
		virtual bool DoMakeContentMd5AndQuery(Buffer content_buffer);

		// 定时器
		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		// 通知DownloadDriver makeblock成功或失败
		virtual void OnNotifyHashBlock(size_t block_index,bool b_success);

		// 从poolout中删除某个subpiece
		virtual void OnWriteSubPieceFinish(SubPieceInfo subpiece_info);

		// 通知Storage做相应操作，通知appmodule发AddRidUrlRequestPacket包，通知download_driver下载完毕
		virtual void OnHashResourceFinish();

		// block写入完毕，赋予MD5值，检查文件是否写入完毕，完毕则通知其他模块下载完毕
		// 读取需要上传的block，上传
		virtual void OnPendingHashBlockFinish(size_t block_index,MD5 hash_val);

		virtual void OnHashBlockFinish(size_t block_index,MD5 hash_val, Buffer& buf, IUploadListener::p listener);
		
		// content写入完毕，通知download_driver
		virtual void OnPendingHashContentFinish(MD5 hash_val, size_t content_bytes);
		
		// 通知storage关闭instance，释放资源空间
		virtual void OnResourceCloseFinish(Resource::p resource_p);

		// 将resource_p_中的instance指向本对象，并将信息写入ResourceInfo文件中(ResourceInfo.dat)
		// instance状态: APPLY_RESOURCE --> HAVE_RESOURCE
		virtual void AttachResource(Resource::p resource_p);

		// 资源文件改名，通知storage将文件信息写入资源信息文件
		virtual void OnDiskFileNameChange(tstring file_name) ;

		virtual void GetBlockPosition(size_t block_index,size_t &offset,u_int &length);

		virtual void GetSubPiecePosition(const SubPieceInfo &subpiec_info,size_t &offset,u_int &length);

	protected:
		// 从资源描述, pending_subpiece_manager和pending_get_subpiece_manager中删除block
		// 通知upload_listener获取subpiece失败，通知download_driver，makeblock失败
		virtual void OnRemoveResourceBlockFinish(size_t block_index);

	private:
		// 从Url中获取文件名，如果获取失败，则生成一串随机数表示文件名
		void ParseFileNameFromUrl(const UrlInfo &url_info,tstring &resource_name);

		// 通过subpiece计算block的MD5值，然后检查上传等操作
		void PendingHashBlock(u_int block_index);

		// 通过subpiece计算content的MD5值
		void PendingHashContent();

		// 根据url_info_s的第一个Url生成资源文件的文件名，然后向Storage申请资源
		// instance状态：NEED_RESOURCE --> APPLY_RESOURCE
		void TryCreateResource();

		

	protected:
		bool is_running_;
		bool b_pure_download_mode_;

		// instance的状态，初始为APPLY_RESOURCE
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

		//不记录启动url，可能服务于多个DownloadDriver,对应多个start_url_info_;  
		set<IDownloadDriver::p> download_driver_s_;
		size_t dd_num_;
		// 初始状态应为"x.flv"，创建资源后应为完整路径文件名，且为.tpp文件
		tstring  resource_name_;
		size_t disk_file_size_;
		set<UrlInfo> url_info_s_;
		UrlInfo origanel_url_info_;
		Resource::p  resource_p_; 
		Instance::p  merge_to_instance_p_; 
		// 资源中block的状态：NO_VERIFIED, HAS_BEEN_VERIFIED, BEING_VERIFIED
		std::vector<char>  block_verify_map_;
		size_t last_push_time_;
		list<size_t> traffic_list_;

	private:
		size_t    add_repeat_subiece_count_;
		SubPieceInfo  merging_pos_subpiece;

		//内容感知
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
