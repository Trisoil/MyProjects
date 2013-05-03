#include "stdafx.h"
#include "statistic/PeerConnectionStatistic.h"

namespace statistic
{
	PeerConnectionStatistic::PeerConnectionStatistic(const Guid& peer_id)
		: is_running_(false)
		, peer_guid_(peer_id)
		, speed_info_(SpeedInfoStatistic::Create())
	{
	}

	PeerConnectionStatistic::p PeerConnectionStatistic::Create(const Guid& peer_id)
	{
		return p(new PeerConnectionStatistic(peer_id));
	}

	void PeerConnectionStatistic::Start()
	{
		STAT_DEBUG("PeerConnectionStatistic::Start [IN]");

		if (is_running_ == true)
		{
			STAT_WARN("PeerConnectionStatistic is running, return.");
			return ;
		}

		Clear();
		
		peer_connection_info_.PeerGuid = peer_guid_;
		STAT_DEBUG("    PeerGuid: " << peer_guid_);
		
		speed_info_->Start();

		is_running_ = true;
		STAT_DEBUG("PeerConnectionStatistic::Start [OUT]");
	}

	void PeerConnectionStatistic::Stop()
	{
		STAT_DEBUG("PeerConnectionStatistic::Stop [IN]");
		if (is_running_ == false)
		{
			STAT_WARN("PeerConnectionStatistic is not running, return.");
			return ;
		}

		speed_info_->Stop();

		Clear();

		is_running_ = false;
		STAT_DEBUG("PeerConnectionStatistic::Stop [OUT]");
	}

	void PeerConnectionStatistic::Clear()
	{
		speed_info_->Clear();
		peer_connection_info_.Clear();
	}

	inline bool PeerConnectionStatistic::IsRunning() const
	{
		return is_running_;
	}

	//////////////////////////////////////////////////////////////////////////
	// Speed Info

	void PeerConnectionStatistic::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		speed_info_->SubmitDownloadedBytes(downloaded_bytes);
	}

	void PeerConnectionStatistic::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		speed_info_->SubmitUploadedBytes(uploaded_bytes);
	}

	SPEED_INFO PeerConnectionStatistic::GetSpeedInfo()
	{
		UpdateSpeedInfo();
		return peer_connection_info_.SpeedInfo;
	}

	void PeerConnectionStatistic::UpdateSpeedInfo()
	{
		peer_connection_info_.SpeedInfo = speed_info_->GetSpeedInfo();
	}

	//////////////////////////////////////////////////////////////////////////
	// Peer Connection Info

	P2P_CONNECTION_INFO PeerConnectionStatistic::GetPeerConnectionInfo()
	{
		UpdateSpeedInfo();
		return peer_connection_info_;
	}

	//////////////////////////////////////////////////////////////////////////
	// Misc

	void PeerConnectionStatistic::SetPeerVersion(u_int peer_version)
	{
		peer_connection_info_.PeerVersion = peer_version;
	}

	void PeerConnectionStatistic::SetPeerType(UINT8 peer_type)
	{
		peer_connection_info_.PeerType = peer_type;
	}

	void PeerConnectionStatistic::SetCandidatePeerInfo(const CANDIDATE_PEER_INFO& peer_info)
	{
		peer_connection_info_.PeerInfo = peer_info;
	}

	void PeerConnectionStatistic::SetPeerDownloadInfo(const PEER_DOWNLOAD_INFO& peer_download_info)
	{
		peer_connection_info_.PeerDownloadInfo = peer_download_info;
	}

	void PeerConnectionStatistic::SetBitmap(Buffer bitmap)
	{
		assert(bitmap.length_ <= BITMAP_SIZE);
        memcpy(peer_connection_info_.BitMap, bitmap.data_.get(), min(BITMAP_SIZE, bitmap.length_));
	}

    void PeerConnectionStatistic::SetBitmap(BlockMap::p block_map)
    {
        SetBitmap(block_map->MakeToBinary());
    }

	void PeerConnectionStatistic::SetWindowSize(UINT8 window_size)
	{
		peer_connection_info_.WindowSize = window_size;
	}

	void PeerConnectionStatistic::SetAssignedSubPieceCount(UINT8 assigned_subpiece_count)
	{
		peer_connection_info_.AssignedSubPieceCount = assigned_subpiece_count;
	}

	void PeerConnectionStatistic::SubmitRTT(u_short rtt)
	{
		peer_connection_info_.RTT_Total += rtt;
		peer_connection_info_.RTT_Count++;
        peer_connection_info_.RTT_Average = 
            (peer_connection_info_.RTT_Count == 0 ? 0 : peer_connection_info_.RTT_Total / peer_connection_info_.RTT_Count);
		peer_connection_info_.RTT_Max = max(peer_connection_info_.RTT_Max, rtt);
	}

    u_short PeerConnectionStatistic::GetAverageRTT()
    {
        return peer_connection_info_.RTT_Average;
    }

    u_int PeerConnectionStatistic::GetTotalRTTCount()
    {
        return peer_connection_info_.RTT_Count;
    }

    void PeerConnectionStatistic::SetElapsedTime(u_short elapsed_time)
    {
        peer_connection_info_.ElapseTime = elapsed_time;
    }

    void PeerConnectionStatistic::SetAverageDeltaTime(u_long avg_delt_time)
    {
        peer_connection_info_.AverageDeltaTime = avg_delt_time;
    }

    void PeerConnectionStatistic::SetSortedValue(u_long sorted_value)
    {
        peer_connection_info_.SortedValue = sorted_value;
    }

    //////////////////////////////////////////////////////////////////////////
    // Peer Guid

    Guid PeerConnectionStatistic::GetPeerGuid() const
    {
        return peer_guid_;
    }
}
