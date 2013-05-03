#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <map>

#include "VarSizeStruct.h"
#include "StatisticStructs.h"
#include "SpeedInfoStatistic.h"
#include "PeerConnectionStatistic.h"
#include "base/guid.h"

#include "framework/mswin/ShareMemory.h"

using namespace std;
using namespace framework::mswin;

namespace statistic
{
	class P2PDownloaderStatistic
		: public boost::noncopyable
		, public boost::enable_shared_from_this<P2PDownloaderStatistic>
	{
	public:

		typedef boost::shared_ptr<P2PDownloaderStatistic> p;

		static p Create(const RID& rid);

	public:

		void Start(u_int max_p2p_connection_count);

		void Stop();

		void Clear();

		bool IsRunning() const;

		void OnShareMemoryTimer(u_int times);

	public:

		//////////////////////////////////////////////////////////////////////////
		// Attach & Detach

		PeerConnectionStatistic::p AttachPeerConnectionStatistic(const Guid& peer_id);

		bool DetachPeerConnectionStatistic(const Guid& peer_id);

        bool DetachPeerConnectionStatistic(const PeerConnectionStatistic::p peer_connection_statistic);

        bool DetachAllPeerConnectionStatistic();

    public:

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void SubmitDownloadedBytes(u_int downloaded_bytes);

		void SubmitUploadedBytes(u_int uploaded_bytes);

		SPEED_INFO GetSpeedInfo();

        u_int GetElapsedTimeInMilliSeconds();

		//////////////////////////////////////////////////////////////////////////
		// Resource Info

		void SetFileLength(u_int file_length);

		void SetBlockNum(u_short block_num);

		void SetBlockSize(u_short block_size);

		//////////////////////////////////////////////////////////////////////////
		// IP Pool Info

		void SetIpPoolPeerCount(u_short ip_pool_peer_count);

		void SetExchangingPeerCount(UINT8 exchanging_peer_count);

		void SetConnectingPeerCount(UINT8 connecting_peer_count);

        void SetFullBlockPeerCount(UINT16 full_block_peer_count);

		//////////////////////////////////////////////////////////////////////////
		// Algorithm Info

		void SetTotalWindowSize(u_short total_window_size);

        void SubmitAssignedSubPieceCount(u_short assigned_sub_piece_count);

		void SubmitUnusedSubPieceCount(u_short unused_sub_piece_count);

		void SubmitRecievedSubPieceCount(u_short received_sub_piece_count);

		void SubmitRequestSubPieceCount(u_short request_sub_piece_count);

        u_short GetTotalAssignedSubPieceCount();

        u_short GetSubPieceRetryRate();

        u_short GetUDPLostRate();

		//////////////////////////////////////////////////////////////////////////
		// Misc

		u_int GetMaxP2PConnectionCount() const;

		RID GetResourceID() const;

        //////////////////////////////////////////////////////////////////////////
        // P2P Data Bytes

        void SubmitP2PDataBytes(u_int p2p_data_bytes);

        void ClearP2PDataBytes();

        u_int GetTotalP2PDataBytes();

	private:

		//////////////////////////////////////////////////////////////////////////
		// Updates

		void UpdateSpeedInfo();

		void UpdatePeerConnectionInfo();

        void UpdateRate();

		//////////////////////////////////////////////////////////////////////////
		// Shared Memory

		bool CreateSharedMemory();

		string GetSharedMemoryName();

		u_int GetSharedMemorySize();

		//////////////////////////////////////////////////////////////////////////
		// Misc

		void ClearMaps();

		P2PDOWNLOADER_STATISTIC_INFO& P2PDownloaderStatisticInfo();

	private:

		typedef map<Guid, PeerConnectionStatistic::p> PeerConnectionStatisticMap;

	private:

		bool is_running_;

		P2PDOWNLOADER_STATISTIC_INFO_EX p2p_downloader_statistic_info_;

		SpeedInfoStatistic::p speed_info_;

		RID resource_id_;

		ShareMemory shared_memory_;

		PeerConnectionStatisticMap peer_connection_statistic_map_;

	private:

		P2PDownloaderStatistic() {}

		P2PDownloaderStatistic(const RID& rid);

	};

}
