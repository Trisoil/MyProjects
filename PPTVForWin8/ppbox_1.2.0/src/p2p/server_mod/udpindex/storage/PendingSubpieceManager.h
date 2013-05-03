#pragma once

/*******************************************************************************
//
//  PendingSubpieceManager.h
//  class PendingSubPieceManager, 维护两个<SubPieceInfo, Buffer>的map
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

		// 将某个subpiece插入到poolin_pending_subpiece_info
		void PushToInPool(SubPieceInfo in,Buffer buf);

		// 在poolin_pending_subpiece_info中查找某个subpiece，如果poolin中不存在，则在poolout中查找
		bool Find(SubPieceInfo in);

		// 在poolin_pending_subpiece_info中查找某个subpiece，并返回所对应的buffer
		// 如果poolin中不存在，则在poolout中查找。如果都找不到，则返回空buffer
		Buffer Get(SubPieceInfo in);

		Buffer GetForPlay(SubPieceInfo in);

		// 从poolin_pending_subpiece_info中删除某个subpiece，如果poolin中不存在，则从poolout中删除
		bool Delete(SubPieceInfo in);

		// 从poolin_pending_subpiece_info中删除block_index的subpiece
		void RemoveBlock(size_t block_index);

		// 从poolin_pending_subpiece_info中删除block_index的subpiece，并插入到block_buffer_set
		// 然后，将block_buffer_set插入到poolout_pending_subpiece_info
		void MoveBlockBufferToOutPool( u_int block_index,map<SubPieceInfo,Buffer> &block_buffer_set);

		// 将poolin_pending_subpiece_info的第一个元素移到poolout_pending_subpiece_info
		bool MoveToOutPool(SubPieceInfo &in,Buffer &subpiece_buffer);

		// 从poolout_pending_subpiece_info中删除某个subpiece
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