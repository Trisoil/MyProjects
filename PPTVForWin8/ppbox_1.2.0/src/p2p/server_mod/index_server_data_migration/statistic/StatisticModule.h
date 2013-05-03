#pragma once

#include "base/guid.h"
#include "framework/timer/Timer.h"
#include "framework/mswin/ShareMemory.h"
#include "SpeedInfoStatistic.h"
#include "P2PDownloaderStatistic.h"
#include "DownloadDriverStatistic.h"
#include "StatisticStructs.h"
#include "base/base.h"
#include "protocal/structs.h"

#include <map>
#include <vector>

using namespace std;
using namespace protocal;
using namespace framework::mswin;
using namespace framework::timer;

namespace statistic
{
	class StatisticModule 
		: public boost::noncopyable
		, public boost::enable_shared_from_this<StatisticModule>
		, public framework::timer::ITimerListener
	{
	public:

		typedef boost::shared_ptr<StatisticModule> p;

	public:

		// 启停
		void Start(
			u_int flush_interval_in_milliseconds,   // 写入共享内存时间周期(ms)
			u_int max_http_downloader_count,        // 默认最大HttpDownloaderStatistic个数
			u_int max_p2p_connection_count          // 默认最大P2PConnectionStatistic个数
		);

		void Stop();

		// 模块
		DownloadDriverStatistic::p AttachDownloadDriverStatistic(u_int id, u_int max_http_downloader_count);

		DownloadDriverStatistic::p AttachDownloadDriverStatistic(u_int id);

		bool DetachDownloadDriverStatistic(int id);

        bool DetachDownloadDriverStatistic(const DownloadDriverStatistic::p download_driver_statistic);

        bool DetachAllDownloadDriverStatistic();

		P2PDownloaderStatistic::p AttachP2PDownloaderStatistic(const RID& rid, u_int max_p2p_connection_count);

		P2PDownloaderStatistic::p AttachP2PDownloaderStatistic(const RID& rid);

		bool DetachP2PDownaloaderStatistic(const RID& rid);

        bool DetachP2PDownaloaderStatistic(const P2PDownloaderStatistic::p p2p_downloader_statistic);

        bool DetachAllP2PDownaloaderStatistic();

		// 消息
		virtual void OnTimerElapsed(boost::shared_ptr<Timer> pointer, u_int times);

		// Timer
		void OnShareMemoryTimer(u_int times);

		// 状态
		bool IsRunning() const { return is_running_; }

	public:

		//////////////////////////////////////////////////////////////////////////
		// Statistic Info

		STASTISTIC_INFO GetStatisticInfo();

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void SubmitDownloadedBytes(u_int downloaded_bytes);

		void SubmitUploadedBytes(u_int uploaded_bytes);

		SPEED_INFO GetSpeedInfo();

        UINT32 GetMaxHttpDownloadSpeed() const;

        UINT32 GetTotalDownloadSpeed();

        //////////////////////////////////////////////////////////////////////////
        // Local Download Info

        protocal::PEER_DOWNLOAD_INFO GetLocalPeerDownloadInfo();

        protocal::PEER_DOWNLOAD_INFO GetLocalPeerDownloadInfo(const RID& rid);

		//////////////////////////////////////////////////////////////////////////
		// 设置 和 获取 IP Info

        void SetLocalPeerInfo(const CANDIDATE_PEER_INFO& local_peer_info);

        CandidatePeerInfo GetLocalPeerInfo();

        // 设置 和 获取 Local PeerInfo 中的 Local 部分

        void SetLocalPeerIp(u_int ip);

        void SetLocalPeerUdpPort(u_short udp_port);

        void SetLocalPeerTcpPort(u_short tcp_port);

        void SetLocalPeerAddress(const PEER_ADDR& peer_addr);

        PeerAddr GetLocalPeerAddress();

        // 设置 和 获取 Local PeerInfo 中的 Detect 部分

        void SetLocalDetectSocketAddress(const SOCKET_ADDR& socket_addr);

        SocketAddr GetLocalDetectSocketAddress();

        // 设置 和 获取 Local PeerInfo 中的 Stun 部分

        void SetLocalStunSocketAddress(const SOCKET_ADDR& socket_addr);

        SocketAddr GetLocalStunSocketAddress();

		void SetLocalIPs(const vector<u_long>& local_ips);

		//////////////////////////////////////////////////////////////////////////
		// Peer Info

		void SetLocalPeerVersion(u_int local_peer_version);

		void SetLocalPeerType(UINT8 local_peer_type);

		//////////////////////////////////////////////////////////////////////////
		// Tracker Server

		void SetTrackerInfo(u_int group_count, const vector<TRACKER_INFO>& tracker_infos);

		void SetIsSubmitTracker(const TRACKER_INFO& tracker_info, bool is_submit_tracker);

		void SubmitCommitRequest(const TRACKER_INFO& tracker_info);

		void SubmitCommitResponse(const TRACKER_INFO& tracker_info);

		void SubmitKeepAliveRequest(const TRACKER_INFO& tracker_info);

		void SubmitKeepAliveResponse(const TRACKER_INFO& tracker_info, u_short keep_alive_interval);

		void SubmitListRequest(const TRACKER_INFO& tracker_info);

		void SubmitListResponse(const TRACKER_INFO& tracker_info, u_int peer_count);

		void SubmitErrorCode(const TRACKER_INFO& tracker_info, UINT8 error_code);

		//////////////////////////////////////////////////////////////////////////
		// Index Server

		void SetIndexServerInfo(u_long ip, u_short port, byte type = 0);

		void SetIndexServerInfo(const SocketAddr& socket_addr, byte type = 0);

		void SubmitQueryRIDByUrlRequest();

		void SubmitQueryRIDByUrlResponse();

		void SubmitQueryHttpServersByRIDRequest();

		void SubmitQueryHttpServersByRIDResponse();

		void SubmitQueryTrackerListRequest();

		void SubmitQueryTrackerListResponse();

		void SubmitAddUrlRIDRequest();

		void SubmitAddUrlRIDResponse();

		//////////////////////////////////////////////////////////////////////////
		//停止时数据上传相关

        void SubmitP2PDownloaderDownloadBytes(u_int p2p_downloader_download_bytes);

		void SubmitOtherServerDownloadBytes(u_int other_server_download_bytes);

        //////////////////////////////////////////////////////////////////////////
		//下载中 数据下载 实时相关信息

		void SubmitTotalHttpNotOriginalDataBytes (u_int bytes);    // 实时 下载的纯数据 字节数

        void SubmitTotalP2PDataBytes(u_int bytes);  // 实时 P2P下载的纯数据 字节数

        void SubmitTotalHttpOriginalDataBytes(u_int bytes);// 实时 原生下载的纯数据 字节数

        void SubmitCompleteCount(u_int count);					// 已经完成的DD数

        u_int GetTotalDataBytes(); 

        u_short GetTotalDataBytesInMB();

        //////////////////////////////////////////////////////////////////////////
        // Upload Cache 统计

        void SetUploadCacheRequest(u_int count);

        void SetUploadCacheHit(u_int count);

        float GetUploadCacheHitRate();

        //////////////////////////////////////////////////////////////////////////
        // HttpProxyPort

        void SetHttpProxyPort(u_short port);

        //////////////////////////////////////////////////////////////////////////
        // IncomingPeersCount

        void SubmitIncomingPeer();

        UINT16 GetIncomingPeersCount();

        //////////////////////////////////////////////////////////////////////////
        // DownloadDuration

        void SubmitDownloadDurationInSec(const UINT16& download_duration_in_sec);

        UINT16 GetDownloadDurationInSec();

	private:

		void Clear();

		//////////////////////////////////////////////////////////////////////////
		// Updates

		void UpdateSpeedInfo();

		void UpdateTrackerInfo();

        void UpdateMaxHttpDownloadSpeed();

		//////////////////////////////////////////////////////////////////////////
		// Shared Memory

		bool CreateSharedMemory();

		string GetSharedMemoryName();

		u_int GetSharedMemorySize();

		//////////////////////////////////////////////////////////////////////////
		// Tracker Info

		STATISTIC_TRACKER_INFO& GetTracker(const TRACKER_INFO& tracker_info);

	private:

		static const u_int HASH_SIZE = UINT8_MAX_VALUE;

		static UINT8 HashFunc(u_int value) { return value % HASH_SIZE; }

		//////////////////////////////////////////////////////////////////////////
		// P2PDownloader RID Address

		UINT8 Address(const RID& rid); // Be careful when buffer FULL

		bool AddP2PDownloaderRID(const RID& rid);

		bool RemoveP2PDownloaderRID(const RID& rid);

		//////////////////////////////////////////////////////////////////////////
		// DownloaderDriverID Address

		UINT Address(u_int id);

		bool AddDownloadDriverID(u_int id);

		bool RemoveDownloadDriverID(u_int id);

	private: // types

		typedef map<u_int, DownloadDriverStatistic::p> DownloadDriverStatisticMap;

		typedef map<RID, P2PDownloaderStatistic::p> P2PDownloadDriverStatisticMap;

		typedef map<TRACKER_INFO, STATISTIC_TRACKER_INFO> StatisticTrackerInfoMap;

	private:

		// 模块
		DownloadDriverStatisticMap download_driver_statistic_map_;

		P2PDownloadDriverStatisticMap p2p_downloader_statistic_map_;

		StatisticTrackerInfoMap statistic_tracker_info_map_;

		STATISTIC_TRACKER_INFO* hot_tracker_; // optimization

		// 定时器
		PeriodicTimer::p share_memory_timer_;

		// 状态
		bool is_running_;

		u_int max_http_downloader_count_;

		u_int max_p2p_connection_count_;

		// 共享内存
		ShareMemory shared_memory_;

		// 统计信息
		STASTISTIC_INFO statistic_info_;

		// Speed Info
		SpeedInfoStatistic::p speed_info_;

	private:

		StatisticModule();

		static StatisticModule::p inst_;

	public:

		static StatisticModule::p Inst() { return inst_; }

	};
}
