#pragma once

/*******************************************************************************
//
//  Resource.h
//  class Resource
//  ����subpiece��д��Ͷ���, ���д��麯����ʵ�ּ�class FileResource
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
		// ���� ����
		Resource(
			size_t file_length,					 // �ļ���С
			tstring file_name,					 // �����ļ���
			boost::shared_ptr<Instance> inst_p,  // ������instance
			size_t init_size
			);
		Resource(
			ResourceDescriptor::p resource_desc_p,  // �������ļ����� 
			tstring file_name,						// �����ļ���
			boost::shared_ptr<Instance> inst_p,		// ������instance
			size_t actual_size						// ռ�ÿռ�
			);

	public:
		// ��ĳ��instance
		void BindInstance(boost::shared_ptr<Instance> instance_p); 

		boost::shared_ptr<Instance> GetInstance()  { return instance_p_; }

		// ��ȿ�����ǰ��ResourceDescriptor
		ResourceDescriptor::p CloneResourceDescriptor();

		// �����ļ���
		virtual tstring GetLocalFileName() ;

		// ��һ��block��subpieceд���ļ����������block��MD5ֵ��Ȼ�󽻸�Instance,��ȡ��Ҫ�ϴ���block���ϴ�
		virtual void ThreadPendingHashBlock(size_t block_index, std::map<SubPieceInfo,Buffer> block_pending_buffer_set);

		// ��һ��block��subpieceд���ļ���������MD5ֵ
		virtual void ThreadWriteBlock(size_t block_index, std::map<SubPieceInfo,Buffer> block_pending_buffer_set);

		// ��subpieceд���ļ���������content��MD5ֵ��Ȼ�󽻸�Instance
		virtual void ThreadPendingHashContent(std::map<SubPieceInfo,Buffer> block_pending_buffer_set, size_t content_bytes);

		// ������Դ��Ϣ��cfg�ļ���
		virtual void SecSaveResourceFileInfo();

		// ���ļ��ж�ȡָ����subpiece��Ȼ�󽻸�IUploadListener
		virtual void ThreadSecReadSubPiece(const RID& rid,const SubPieceInfo& subpiece_info, 
			const REndpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener);

		// ���ļ��ж�ȡָ����block��Ȼ�󽻸�IUploadListener
		virtual void ThreadReadBlockForUpload(const RID& rid,const size_t block_index,
			IUploadListener::p listener, bool need_hash);

		// ���ļ��ж�ȡָ����subpiece��Ȼ�󽻸�IPlayerListener
		virtual void ThreadGetSubPieceForPlay(const SubPieceInfo subpiece_info, IPlayerListener::p listener);

		// ��ĳ��subpiece����ȡ������n��������subpiece��Ȼ�󽻸�player_listener
		virtual void ThreadReadBufferForPlay(const SubPieceInfo subpiece_info, size_t n, IPlayerListener::p listener);

		// ���ļ��ж�ȡָ����subpiece��Ȼ�󽻸�Instance merge
		virtual void ThreadMergeSubPieceToInstance(const SubPieceInfo subpiece_info, 
			boost::shared_ptr<Instance> merge_to_instance_p);

		// ��һ��subpieceд���ļ���Ȼ����Instance��poolout_pending_subpiece_info��ɾ����subpiece
		virtual void ThreadSecWriteSubPiece(SubPieceInfo subpiece_info, Buffer buf);

		virtual void ThreadWriteBuffer(set<SubPieceInfo>& splist, Buffer buf);

		// ������֪ͨinstance��ɾ��ĳ��block��֪ͨinstance��������Դ��Ϣ
		virtual void RemoveBlock(size_t index);

		virtual void NeedSaveCfg(bool need = true) { need_saveinfo_to_disk_ = true; }

	// ------------------------------------------------------------------------------------
	// ������ʵ��
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
		ResourceDescriptor::p file_resource_desc_p_; // �������ļ�����
		size_t actual_size_;			// �ɹ��캯����ֵ�����������иı�
		bool need_saveinfo_to_disk_;	// �����Ƿ�������Ϣ���浽cfg�ļ���
		tstring file_name_;				// �����ļ���
		boost::shared_ptr<Instance> instance_p_;	// ������instance
		volatile  bool is_running_;  // ��ֵ���ܻᱻ�����̱߳��ı�
	}; // class Resource

} // namespace storage


