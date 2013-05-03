#pragma once

/*******************************************************************************
//
//  PendingSubpieceManager.h
//  class PendingSubPieceManager, ά������<SubPieceInfo, Buffer>��map
//
*******************************************************************************/

#include <boost/thread.hpp>
#ifndef BOOST_HAS_THREADS
#error invalid thread config
#endif
#include <boost/detail/lightweight_mutex.hpp>

#include "Resource.h"

namespace storage
{
	using boost::detail::lightweight_mutex;
//	using boost::detail::lightweight_mutex::scoped_lock;

	class PendingSubPieceManager
		:public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<PendingSubPieceManager> p;

	private:
		PendingSubPieceManager()
		{
			IniFile ini;
			ini.SetSection(TEXT("Storage"));
			pending_queue_length_ = ini.GetInteger(TEXT("PendingQueueLength"),4*1024);
		}

	public:
		static p Create()
		{
			return p(new PendingSubPieceManager());
		}

		// ��ĳ��subpiece���뵽poolin_pending_subpiece_info
		void PushToInPool(SubPieceInfo in,Buffer buf);

		// ��poolin_pending_subpiece_info�в���ĳ��subpiece�����poolin�в����ڣ�����poolout�в���
		bool Find(SubPieceInfo in);

		// ��poolin_pending_subpiece_info�в���ĳ��subpiece������������Ӧ��buffer
		// ���poolin�в����ڣ�����poolout�в��ҡ�������Ҳ������򷵻ؿ�buffer
		Buffer Get(SubPieceInfo in);

		Buffer GetForPlay(SubPieceInfo in);

		// ��poolin_pending_subpiece_info��ɾ��ĳ��subpiece�����poolin�в����ڣ����poolout��ɾ��
		bool Delete(SubPieceInfo in);

		// ��poolin_pending_subpiece_info��ɾ��block_index��subpiece
		void RemoveBlock(size_t block_index);

		// ��poolin_pending_subpiece_info��ɾ��block_index��subpiece�������뵽block_buffer_set
		// Ȼ�󣬽�block_buffer_set���뵽poolout_pending_subpiece_info
		void MoveBlockBufferToOutPool( u_int block_index,map<SubPieceInfo,Buffer> &block_buffer_set);

		// ��poolin_pending_subpiece_info�ĵ�һ��Ԫ���Ƶ�poolout_pending_subpiece_info
		bool MoveToOutPool(SubPieceInfo &in,Buffer &subpiece_buffer);

		// ��poolout_pending_subpiece_info��ɾ��ĳ��subpiece
		void Remove(const SubPieceInfo &in);

		void Remove(size_t block_index);

		bool Empty() { return poolin_pending_subpiece_info_.empty(); }

		set<size_t> GetNeedWriteBlock();

		set<size_t> GetAllBlock();

		SubPieceInfo GetPlaySubPieceInfo() { return play_subpiece_info_; }

	protected:
	private:
		typedef std::map<SubPieceInfo,Buffer> PoolPendingSubPieceInfo;
		PoolPendingSubPieceInfo poolin_pending_subpiece_info_;
		PoolPendingSubPieceInfo poolout_pending_subpiece_info_;
		size_t pending_queue_length_;
		SubPieceInfo play_subpiece_info_;

	}; // class PendingSubPieceManager

} // namespace storage