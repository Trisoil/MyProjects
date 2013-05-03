#include "stdafx.h"
#include "statistic/P2PDownloaderStatistic.h"

namespace statistic
{
	P2PDownloaderStatistic::P2PDownloaderStatistic(const RID& rid)
		: is_running_(false)
		, resource_id_(rid)
        , speed_info_(SpeedInfoStatistic::Create())
	{
	}

	P2PDownloaderStatistic::p P2PDownloaderStatistic::Create(const RID& rid)
	{
		return p(new P2PDownloaderStatistic(rid));
	}

	void P2PDownloaderStatistic::Start(u_int max_p2p_connection_count)
	{
        STAT_DEBUG("P2PDownloaderStatistic::Start [IN], max_p2p_conn_count: " << max_p2p_connection_count);
		if (is_running_ == true)
        {
            STAT_WARN("P2PDownloaderStatistic is running, return.");
			return ;
        }

		Clear();

		p2p_downloader_statistic_info_ = P2PDOWNLOADER_STATISTIC_INFO_EX(max_p2p_connection_count);

		P2PDownloaderStatisticInfo().ResourceID = resource_id_;

		speed_info_->Start();

		if (!CreateSharedMemory())
		{
			STAT_ERROR("Create P2PDownloaderStatistic Shared Memory Failed.");
			return ;
		}

		is_running_ = true;
        STAT_DEBUG("P2PDownloaderStatistic::Start [OUT]");
	}

	void P2PDownloaderStatistic::Stop()
	{
        STAT_DEBUG("P2PDownloaderStatistic::Stop [IN]");
		if (is_running_ == false)
        {
            STAT_DEBUG("P2PDownloaderStatistic is not running. Return.");
			return ;
        }

		speed_info_->Stop();

		Clear();

		shared_memory_.Close();

		is_running_ = false;
        STAT_DEBUG("P2PDownloaderStatistic::Stop [OUT]");
	}
	
	void P2PDownloaderStatistic::Clear()
	{
		speed_info_->Clear();
		p2p_downloader_statistic_info_.Clear();
        DetachAllPeerConnectionStatistic();
	}
	
	inline bool P2PDownloaderStatistic::IsRunning() const
	{
		return is_running_;
	}

	void P2PDownloaderStatistic::OnShareMemoryTimer(u_int times)
	{
        STAT_DEBUG("P2PDownloaderStatistic::OnShareMemoryTimer [IN], times: " << times);

		if (is_running_ == false)
        {
            STAT_WARN("P2PDownloaderStatistic is not running. Return.");
			return ;
        }

        UpdateRate();
		UpdateSpeedInfo();
		UpdatePeerConnectionInfo();

		P2PDOWNLOADER_STATISTIC_INFO& info = P2PDownloaderStatisticInfo();

		// copy
		memcpy(shared_memory_.GetView(), &info, GetSharedMemorySize());
        STAT_DEBUG("Copied P2PDOWNLOADER_STATISTIC_INFO into Shared Memory: " << GetSharedMemoryName());

        STAT_DEBUG("P2PDownloaderStatistic::OnShareMemoryTimer [OUT]");
	}

	//////////////////////////////////////////////////////////////////////////
	// Attach & Detach

	PeerConnectionStatistic::p P2PDownloaderStatistic::AttachPeerConnectionStatistic(const Guid& peer_id)
	{
        STAT_DEBUG("P2PDownloaderStatistic::AttachPeerConnectionStatistic [IN], peer guid: " << peer_id);

		PeerConnectionStatistic::p peer_connection_info_;

		if (is_running_ == false)
        {
            STAT_WARN("PeerConnectionStatistic is not running. Return null.");
		    return peer_connection_info_;
        }

		// 判断个数
		if (peer_connection_statistic_map_.size() == GetMaxP2PConnectionCount())
		{
			STAT_WARN("Peer Connection Map is Full, size: " << GetMaxP2PConnectionCount() << ". Return null.");
			return peer_connection_info_;
		}

		PeerConnectionStatisticMap::iterator it = peer_connection_statistic_map_.find(peer_id);

		// 存在,返回空
		if (it != peer_connection_statistic_map_.end())
        {
            STAT_WARN("Peer " << peer_id << " already exists. Return null.");
			return peer_connection_info_;
        }

		// create and insert
		peer_connection_info_ = PeerConnectionStatistic::Create(peer_id);
		peer_connection_statistic_map_[peer_id] = peer_connection_info_;

		// start
		peer_connection_info_->Start();
        STAT_DEBUG("Peer Connection Statistic " << peer_id << " is created and started.");

        STAT_DEBUG("P2PDownloaderStatistic::AttachPeerConnectionStatistic [OUT]");
		return peer_connection_info_;
	}

	bool P2PDownloaderStatistic::DetachPeerConnectionStatistic(const Guid& peer_id)
	{
        STAT_DEBUG("P2PDownloaderStatistic::DetachPeerConnectionStatistic [IN], peer guid: " << peer_id);

		if (is_running_ == false)
        {
            STAT_WARN("P2PDownloaderStatistic is not running. Return false.");
			return false;
        }

		PeerConnectionStatisticMap::iterator it = peer_connection_statistic_map_.find(peer_id);

		// 不存在, 返回
		if (it == peer_connection_statistic_map_.end())
        {
            STAT_WARN("Peer " << peer_id << " does not exist. Return false.");
			return false;
        }

		it->second->Stop();
		peer_connection_statistic_map_.erase(it);
        STAT_DEBUG("Peer Connection " << peer_id << " is stopped and erased.");

        STAT_DEBUG("P2PDownloaderStatistic::DetachPeerConnectionStatistic [OUT]");
		return true;
	}

    bool P2PDownloaderStatistic::DetachPeerConnectionStatistic(const PeerConnectionStatistic::p peer_connection_statistic)
    {
        return DetachPeerConnectionStatistic(peer_connection_statistic->GetPeerGuid());
    }

    bool P2PDownloaderStatistic::DetachAllPeerConnectionStatistic()
    {
        for (PeerConnectionStatisticMap::iterator it = peer_connection_statistic_map_.begin();
            it != peer_connection_statistic_map_.end(); it++)
        {
            it->second->Stop();
        }
        // clear
        peer_connection_statistic_map_.clear();

        return true;
    }

	//////////////////////////////////////////////////////////////////////////
	// Misc

	inline P2PDOWNLOADER_STATISTIC_INFO& P2PDownloaderStatistic::P2PDownloaderStatisticInfo()
	{
		return p2p_downloader_statistic_info_.Ref();
	}

	u_int P2PDownloaderStatistic::GetMaxP2PConnectionCount() const
	{
		return p2p_downloader_statistic_info_.MaxTailElementCount();
	}

	RID P2PDownloaderStatistic::GetResourceID() const
	{
		return resource_id_;
	}

	//////////////////////////////////////////////////////////////////////////
	// Speed Info

	void P2PDownloaderStatistic::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		speed_info_->SubmitDownloadedBytes(downloaded_bytes);
	}

	void P2PDownloaderStatistic::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		speed_info_->SubmitUploadedBytes(uploaded_bytes);
	}

	SPEED_INFO P2PDownloaderStatistic::GetSpeedInfo()
	{
		UpdateSpeedInfo();
		return P2PDownloaderStatisticInfo().SpeedInfo;
	}

    u_int P2PDownloaderStatistic::GetElapsedTimeInMilliSeconds()
    {
        return speed_info_->GetElapsedTimeInMilliSeconds();
    }

	void P2PDownloaderStatistic::UpdateSpeedInfo()
	{
		P2PDownloaderStatisticInfo().SpeedInfo = speed_info_->GetSpeedInfo();
	}

	//////////////////////////////////////////////////////////////////////////
	// Peer Connection Info

	void P2PDownloaderStatistic::UpdatePeerConnectionInfo()
	{
        STAT_DEBUG("P2PDownloaderStatistic::UpdatePeerConnectionInfo [IN]");

		P2PDownloaderStatisticInfo().PeerCount = peer_connection_statistic_map_.size();

        STAT_DEBUG("Peer Count: " << P2PDownloaderStatisticInfo().PeerCount);

		//assert(P2PDownloaderStatisticInfo().PeerCount <= GetMaxP2PConnectionCount());
        if (P2PDownloaderStatisticInfo().PeerCount > GetMaxP2PConnectionCount())
        {
            STAT_WARN("Peer Count exceeds max allowed count: " << GetMaxP2PConnectionCount() << ", Reset PeerCount to that value.");
            P2PDownloaderStatisticInfo().PeerCount = GetMaxP2PConnectionCount();
        }

		PeerConnectionStatisticMap::iterator it = peer_connection_statistic_map_.begin();
		for (u_int i = 0; it != peer_connection_statistic_map_.end(); it++, i++)
		{
			P2PDownloaderStatisticInfo().P2PConnections[i] = it->second->GetPeerConnectionInfo();
		}

        STAT_DEBUG("P2PDownloaderStatistic::UpdatePeerConnectionInfo [OUT]");
	}

	//////////////////////////////////////////////////////////////////////////
	// Resource Info

	void P2PDownloaderStatistic::SetFileLength(u_int file_length)
	{
		P2PDownloaderStatisticInfo().FileLength = file_length;
	}

	void P2PDownloaderStatistic::SetBlockNum(u_short block_num)
	{
		P2PDownloaderStatisticInfo().BlockNum = block_num;
	}

	void P2PDownloaderStatistic::SetBlockSize(u_short block_size)
	{
		P2PDownloaderStatisticInfo().BlockSize = block_size;
	}

	//////////////////////////////////////////////////////////////////////////
	// Ip Pool Info

	void P2PDownloaderStatistic::SetIpPoolPeerCount(u_short ip_pool_peer_count)
	{
		P2PDownloaderStatisticInfo().IpPoolPeerCount = ip_pool_peer_count;
	}

	void P2PDownloaderStatistic::SetExchangingPeerCount(UINT8 exchanging_peer_count)
	{
		P2PDownloaderStatisticInfo().ExchangingPeerCount = exchanging_peer_count;
	}

	void P2PDownloaderStatistic::SetConnectingPeerCount(UINT8 connecting_peer_count)
	{
		P2PDownloaderStatisticInfo().ConnectingPeerCount = connecting_peer_count;
	}

    void P2PDownloaderStatistic::SetFullBlockPeerCount(UINT16 full_block_peer_count)
    {
        P2PDownloaderStatisticInfo().FullBlockPeerCount = full_block_peer_count;
    }

	//////////////////////////////////////////////////////////////////////////
	// Algorithm Info

	void P2PDownloaderStatistic::SetTotalWindowSize(u_short total_window_size)
	{
		P2PDownloaderStatisticInfo().TotalWindowSize = total_window_size;
	}

	u_short P2PDownloaderStatistic::GetTotalAssignedSubPieceCount()
	{
		return P2PDownloaderStatisticInfo().TotalAssignedSubPieceCount;
	}

    void P2PDownloaderStatistic::SubmitAssignedSubPieceCount(u_short assigned_sub_piece_count)
    {
        P2PDownloaderStatisticInfo().TotalAssignedSubPieceCount = assigned_sub_piece_count;
    }

	void P2PDownloaderStatistic::SubmitUnusedSubPieceCount(u_short unused_sub_piece_count)
	{
		P2PDownloaderStatisticInfo().TotalUnusedSubPieceCount += unused_sub_piece_count;
	}

	void P2PDownloaderStatistic::SubmitRecievedSubPieceCount(u_short received_sub_piece_count)
	{
		P2PDownloaderStatisticInfo().TotalRecievedSubPieceCount += received_sub_piece_count;
	}

	void P2PDownloaderStatistic::SubmitRequestSubPieceCount(u_short request_sub_piece_count)
	{
		P2PDownloaderStatisticInfo().TotalRequestSubPieceCount += request_sub_piece_count;
	}

    u_short P2PDownloaderStatistic::GetSubPieceRetryRate()
    {
        UpdateRate();
		return P2PDownloaderStatisticInfo().SubPieceRetryRate;
	}

    u_short P2PDownloaderStatistic::GetUDPLostRate()
    {
        UpdateRate();
		return P2PDownloaderStatisticInfo().UDPLostRate;
	}

    void P2PDownloaderStatistic::UpdateRate()
    {
        P2PDownloaderStatisticInfo().SubPieceRetryRate = (P2PDownloaderStatisticInfo().TotalRecievedSubPieceCount == 0) ? 0 :
            (100.0 * P2PDownloaderStatisticInfo().TotalUnusedSubPieceCount / P2PDownloaderStatisticInfo().TotalRecievedSubPieceCount + 0.5);

        P2PDownloaderStatisticInfo().UDPLostRate = (P2PDownloaderStatisticInfo().TotalRequestSubPieceCount == 0) ? 0 :
            (100.0 * (P2PDownloaderStatisticInfo().TotalRequestSubPieceCount - P2PDownloaderStatisticInfo().TotalRecievedSubPieceCount) / P2PDownloaderStatisticInfo().TotalRequestSubPieceCount + 0.5);
    }

    //////////////////////////////////////////////////////////////////////////
    // P2P Data Bytes

    void P2PDownloaderStatistic::SubmitP2PDataBytes(u_int p2p_data_bytes)
    {
        P2PDownloaderStatisticInfo().TotalP2PDataBytes += p2p_data_bytes;
    }

    void P2PDownloaderStatistic::ClearP2PDataBytes()
    {
        P2PDownloaderStatisticInfo().TotalP2PDataBytes = 0;
    }

    u_int P2PDownloaderStatistic::GetTotalP2PDataBytes()
    {
        return P2PDownloaderStatisticInfo().TotalP2PDataBytes;
    }

	//////////////////////////////////////////////////////////////////////////
	// Shared Memory
	bool P2PDownloaderStatistic::CreateSharedMemory()
	{
        STAT_DEBUG("Creating SharedMemory: [" << GetSharedMemoryName() << "] size: " << GetSharedMemorySize() << " Bytes.");

		shared_memory_.Create(GetSharedMemoryName().c_str(), GetSharedMemorySize());

        return shared_memory_.IsMapped();
	}

	string P2PDownloaderStatistic::GetSharedMemoryName()
	{
		return CreateP2PDownloaderModuleSharedMemoryName(GetCurrentProcessID(), GetResourceID());
	}

	u_int P2PDownloaderStatistic::GetSharedMemorySize()
	{
		return p2p_downloader_statistic_info_.Size();
	}
}
