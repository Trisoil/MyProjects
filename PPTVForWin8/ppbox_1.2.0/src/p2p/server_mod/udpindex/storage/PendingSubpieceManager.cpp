/*******************************************************************************
//
//  PendingSubpieceManager.cpp
//  
//
*******************************************************************************/

#include "stdafx.h"
#include "storage_base.h"
#include "base\blockmap.h"
#include "PendingSubpieceManager.h"

namespace storage
{
	// ��ĳ��subpiece���뵽poolin_pending_subpiece_info
	void PendingSubPieceManager::PushToInPool(SubPieceInfo in,Buffer buf)
	{
		poolin_pending_subpiece_info_.insert(make_pair(in,buf));
	}

	// ��poolin_pending_subpiece_info�в���ĳ��subpiece�����poolin�в����ڣ�����poolout�в���
	bool PendingSubPieceManager::Find(SubPieceInfo in)
	{
		bool ret = poolin_pending_subpiece_info_.find(in)!=poolin_pending_subpiece_info_.end();
		if(ret)
		{
			return true;
		}
		return poolout_pending_subpiece_info_.find(in)!=poolout_pending_subpiece_info_.end();

	}

	// ��poolin_pending_subpiece_info�в���ĳ��subpiece������������Ӧ��buffer
	// ���poolin�в����ڣ�����poolout�в��ҡ�
	Buffer PendingSubPieceManager::Get(SubPieceInfo in)
	{
		std::map<SubPieceInfo,Buffer>::const_iterator it = poolin_pending_subpiece_info_.find(in);
		if(it!= poolin_pending_subpiece_info_.end())
		{
			return it->second;
		}
		it = poolout_pending_subpiece_info_.find(in);
		if(it!= poolout_pending_subpiece_info_.end())
		{
			return it->second;
		}
		Buffer n(0);
		return n;
	}

	Buffer PendingSubPieceManager::GetForPlay(SubPieceInfo in)
	{
		play_subpiece_info_ = in;
		return Get(in);
	}

	// ��poolin_pending_subpiece_info��ɾ��ĳ��subpiece�����poolin�в����ڣ����poolout��ɾ��
	bool PendingSubPieceManager::Delete(SubPieceInfo in)
	{
		if(poolin_pending_subpiece_info_.erase(in)==1)
		{
			return true;
		}
		return poolout_pending_subpiece_info_.erase(in)==1;
	}

	// ��poolin_pending_subpiece_info��ɾ��block_index��subpiece
	void PendingSubPieceManager::RemoveBlock(size_t block_index)
	{
		SubPieceInfo subpiece_info;
		subpiece_info.block_index_ = block_index;
		subpiece_info.subpiece_index_ = 0;
		// <TODO>???????????????????????????????????????
		while(subpiece_info.block_index_!=block_index)
		{
			poolin_pending_subpiece_info_.erase(subpiece_info);
			PoolPendingSubPieceInfo::iterator it = poolin_pending_subpiece_info_.lower_bound(subpiece_info);
			subpiece_info = it->first;
		}
	}

	// ��poolin_pending_subpiece_info��ɾ��block_index��subpiece�������뵽block_buffer_set
	// Ȼ�󣬽�block_buffer_set���뵽poolout_pending_subpiece_info
	void PendingSubPieceManager::MoveBlockBufferToOutPool( u_int block_index,
		map<SubPieceInfo,Buffer> &block_buffer_set)
	{
		SubPieceInfo subpiece_info;
		subpiece_info.block_index_ = block_index;
		subpiece_info.subpiece_index_ = 0;
		PoolPendingSubPieceInfo::iterator it = poolin_pending_subpiece_info_.lower_bound(subpiece_info);
		while(it!=poolin_pending_subpiece_info_.end())
		{
			subpiece_info = it->first;
			if(subpiece_info.block_index_!=block_index)
			{
				break;
			}
			block_buffer_set.insert(make_pair(subpiece_info,it->second));
			size_t b_erase = poolin_pending_subpiece_info_.erase(subpiece_info);
			assert(b_erase==1);
			it = poolin_pending_subpiece_info_.lower_bound(subpiece_info);
		}
		poolout_pending_subpiece_info_.insert(block_buffer_set.begin(),block_buffer_set.end());
	}

	// ��poolin_pending_subpiece_info�ĵ�һ��Ԫ���Ƶ�poolout_pending_subpiece_info
	bool PendingSubPieceManager::MoveToOutPool(SubPieceInfo &in,Buffer &subpiece_buffer)
	{
		std::map<SubPieceInfo,Buffer>::iterator it = poolin_pending_subpiece_info_.begin();
		if(it==poolin_pending_subpiece_info_.end())
		{
			return false;
		}
		in = it->first;
		subpiece_buffer = it->second;
		assert(subpiece_buffer.length_!=0);
		poolout_pending_subpiece_info_.insert(make_pair(in,subpiece_buffer));
		size_t dc = poolin_pending_subpiece_info_.erase(in);
		assert(dc==1);
		return true;
	}		

	// ��poolout_pending_subpiece_info��ɾ��ĳ��subpiece
	void PendingSubPieceManager::Remove(const SubPieceInfo &in)
	{
		size_t dc = poolout_pending_subpiece_info_.erase(in);
		assert(dc==1);
	}

	// ��poolout_pending_subpiece_info��ɾ��ĳ��block
	void PendingSubPieceManager::Remove( size_t block_index )
	{
		for (map<SubPieceInfo, Buffer>::iterator it = poolout_pending_subpiece_info_.begin();
			it != poolout_pending_subpiece_info_.end();)
		{
			if (it->first.block_index_ == block_index)
			{
				poolout_pending_subpiece_info_.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	set<size_t> PendingSubPieceManager::GetNeedWriteBlock()
	{
		set<size_t> need_write_block;
		for (map<SubPieceInfo, Buffer>::iterator it = poolin_pending_subpiece_info_.begin();
			it != poolin_pending_subpiece_info_.end(); ++it)
		{
			if (it->first.block_index_ < play_subpiece_info_.block_index_)
			{
				need_write_block.insert(it->first.block_index_);
			}
		}
		if (poolin_pending_subpiece_info_.size() > pending_queue_length_)
		{
			need_write_block.insert(poolin_pending_subpiece_info_.begin()->first.block_index_);
		}
		return need_write_block;
	}

	set<size_t> PendingSubPieceManager::GetAllBlock()
	{
		set<size_t> need_write_block;
		for (map<SubPieceInfo, Buffer>::iterator it = poolin_pending_subpiece_info_.begin();
			it != poolin_pending_subpiece_info_.end(); ++it)
		{
			need_write_block.insert(it->first.block_index_);
		}
		return need_write_block;
	}
}