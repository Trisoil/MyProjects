#pragma once

/*******************************************************************************
//
//  Resource.h
//  class Resource
//  负责subpiece的写入和读出, 所有纯虚函数的实现见class FileResource
//
*******************************************************************************/

#include "ResourceDescriptor.h"
#include "CfgFile.h"
#include "framework\mainThread.h"
#include "IStorage.h"
#include "protocal/PeerPacket.h"

namespace storage
{
	class Instance;

	class Resource
		: public boost::noncopyable
		, public boost::enable_shared_from_this<Resource>
	{
	public:
		friend class Instance;
		typedef boost::shared_ptr<Resource> p;
		typedef boost::asio::ip::udp::endpoint REndpoint;

	protected:
		// 构造 启动
		Resource(
			size_t file_length,					 // 文件大小
			tstring file_name,					 // 完整文件名
			boost::shared_ptr<Instance> inst_p,  // 关联的instance
			size_t init_size
			);
		Resource(
			ResourceDescriptor::p resource_desc_p,  // 关联的文件描述 
			tstring file_name,						// 完整文件名
			boost::shared_ptr<Instance> inst_p,		// 关联的instance
			size_t actual_size						// 占用空间
			);

	public:
		// 绑定某个instance
		void BindInstance(boost::shared_ptr<Instance> instance_p); 

		boost::shared_ptr<Instance> GetInstance()  { return instance_p_; }

		// 深度拷贝当前的ResourceDescriptor
		ResourceDescriptor::p CloneResourceDescriptor();

		// 返回文件名
		virtual tstring GetLocalFileName() ;

		// 将一个block的subpiece写入文件，并计算该block的MD5值，然后交给Instance,读取需要上传的block，上传
		virtual void ThreadPendingHashBlock(size_t block_index, std::map<SubPieceInfo,Buffer> block_pending_buffer_set);

		// 将一个block的subpiece写入文件，不计算MD5值
		virtual void ThreadWriteBlock(size_t block_index, std::map<SubPieceInfo,Buffer> block_pending_buffer_set);

		// 将subpiece写入文件，并计算content的MD5值，然后交给Instance
		virtual void ThreadPendingHashContent(std::map<SubPieceInfo,Buffer> block_pending_buffer_set, size_t content_bytes);

		// 保存资源信息到cfg文件中
		virtual void SecSaveResourceFileInfo();

		// 从文件中读取指定的subpiece，然后交给IUploadListener
		virtual void ThreadSecReadSubPiece(const RID& rid,const SubPieceInfo& subpiece_info, 
			const REndpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener);

		// 从文件中读取指定的block，然后交给IUploadListener
		virtual void ThreadReadBlockForUpload(const RID& rid,const size_t block_index,
			IUploadListener::p listener, bool need_hash);

		// 从文件中读取指定的subpiece，然后交给IPlayerListener
		virtual void ThreadGetSubPieceForPlay(const SubPieceInfo subpiece_info, IPlayerListener::p listener);

		// 从某个subpiece处读取不超过n个的连续subpiece，然后交给player_listener
		virtual void ThreadReadBufferForPlay(const SubPieceInfo subpiece_info, size_t n, IPlayerListener::p listener);

		// 从文件中读取指定的subpiece，然后交给Instance merge
		virtual void ThreadMergeSubPieceToInstance(const SubPieceInfo subpiece_info, 
			boost::shared_ptr<Instance> merge_to_instance_p);

		// 将一个subpiece写入文件，然后交由Instance从poolout_pending_subpiece_info中删除该subpiece
		virtual void ThreadSecWriteSubPiece(SubPieceInfo subpiece_info, Buffer buf);

		virtual void ThreadWriteBuffer(set<SubPieceInfo>& splist, Buffer buf);

		// 改名并通知instance、删除某个block并通知instance、保存资源信息
		virtual void RemoveBlock(size_t index);

		virtual void NeedSaveCfg(bool need = true) { need_saveinfo_to_disk_ = true; }

	// ------------------------------------------------------------------------------------
	// 派生类实现
	public:
		virtual size_t	GetLocalFileSize() = 0;
		virtual size_t	FreeDiskSpace() = 0;

	protected:	
		virtual void	FlushStore() = 0;
		virtual Buffer	ReadBuffer(const size_t startpos, const size_t length) = 0;
		virtual void	WriteBuffer(const size_t startpos, const Buffer & buffer) = 0;
		virtual void	Erase(const size_t startpos, const size_t length) = 0;
		virtual bool	TryRenameToNormalFile() = 0;
		virtual bool	TryRenameToTppFile() = 0;
		virtual void	CloseResource() = 0;
		virtual void    Rename(const tstring& newname)=0;

	protected:
		ResourceDescriptor::p file_resource_desc_p_; // 关联的文件描述
		size_t actual_size_;			// 由构造函数赋值，在派生类中改变
		bool need_saveinfo_to_disk_;	// 决定是否将下载信息保存到cfg文件中
		tstring file_name_;				// 完整文件名
		boost::shared_ptr<Instance> instance_p_;	// 关联的instance
		volatile  bool is_running_;  // 该值可能会被其他线程被改变
	}; // class Resource

} // namespace storage


