#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "base/BlockMap.h"
#include "StatisticStructs.h"
#include "SpeedInfoStatistic.h"

namespace statistic
{
	class PeerConnectionStatistic
		: public boost::noncopyable
		, public boost::enable_shared_from_this<PeerConnectionStatistic>
	{
	public:

		typedef boost::shared_ptr<PeerConnectionStatistic> p;

		static p Create(const Guid& peer_id);

	public:

		void Start();

		void Stop();

		void Clear();

		bool IsRunning() const;

	public:

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void SubmitDownloadedBytes(u_int downloaded_bytes);

		void SubmitUploadedBytes(u_int uploaded_bytes);

		SPEED_INFO GetSpeedInfo();

		//////////////////////////////////////////////////////////////////////////
		// Peer Connection Info

		P2P_CONNECTION_INFO GetPeerConnectionInfo();

		//////////////////////////////////////////////////////////////////////////
		// Misc

		void SetPeerVersion(u_int peer_version);

		void SetPeerType(UINT8 peer_type);

		void SetCandidatePeerInfo(const CANDIDATE_PEER_INFO& peer_info);

		void SetPeerDownloadInfo(const PEER_DOWNLOAD_INFO& peer_download_info);

		void SetBitmap(Buffer bitmap);

		void SetBitmap(BlockMap::p block_map);

		void SetWindowSize(UINT8 window_size);

		void SetAssignedSubPieceCount(UINT8 assigned_subpiece_count);

		void SetElapsedTime(u_short elapsed_time); // TODO: how?

		void SubmitRTT(u_short rtt);

        void SetAverageDeltaTime(u_long avg_delt_time);

        void SetSortedValue(u_long sorted_value);

        u_short GetAverageRTT();

        u_int GetTotalRTTCount();

        //////////////////////////////////////////////////////////////////////////
        // Peer Guid

        Guid GetPeerGuid() const;

	private:

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void UpdateSpeedInfo();

	private:

		bool is_running_;

		P2P_CONNECTION_INFO peer_connection_info_;

		SpeedInfoStatistic::p speed_info_;

		Guid peer_guid_;

	private:

		PeerConnectionStatistic() {}

		PeerConnectionStatistic(const Guid& peer_id);

	};

}
