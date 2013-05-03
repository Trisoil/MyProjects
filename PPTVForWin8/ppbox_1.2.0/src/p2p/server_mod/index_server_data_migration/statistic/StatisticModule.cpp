#include "stdafx.h"
#include "StatisticModule.h"
#include "DownloadDriverStatistic.h"
#include "P2PDownloaderStatistic.h"

namespace statistic
{
	StatisticModule::p StatisticModule::inst_(new StatisticModule());

	StatisticModule::StatisticModule()
		: is_running_(false)
        , speed_info_(SpeedInfoStatistic::Create())
	{
	}

	void StatisticModule::Start(
		u_int flush_interval_in_milliseconds,
		u_int max_http_downloader_count, 
		u_int max_p2p_connection_count)
	{
		STAT_DEBUG("StatisticModule::Start [IN]");
		STAT_EVENT("StatisticModule is starting.");

		if( is_running_ == true ) 
		{
			STAT_WARN("StatisticModule::Start when module is running.");
			return;
		}

		// 清理
		Clear();

		max_http_downloader_count_ = max_http_downloader_count;

		max_p2p_connection_count_ = max_p2p_connection_count;

		STAT_DEBUG(" Max HTTP Downloader Count: " << max_http_downloader_count);
		STAT_DEBUG(" Max P2P Connection Count: " << max_p2p_connection_count);

		if (CreateSharedMemory() == false)
		{
			STAT_ERROR("Create Shared Memory Failed");
			return ;
		}

        speed_info_->Start();

 		// 创建定时器
 		share_memory_timer_ = PeriodicTimer::create(flush_interval_in_milliseconds, shared_from_this());
 		STAT_DEBUG("Timer Created, Interval: " << flush_interval_in_milliseconds << "(ms)");
 
 		// 启动定时器 share_memory_timer_
 		share_memory_timer_->Start();

		is_running_ = true;

		STAT_EVENT("StatisticModule starts successfully.");
		STAT_DEBUG("StatisticModule::Start [OUT]");
	}

	void StatisticModule::Stop()
	{
		STAT_INFO("StatisticModule::Stop [IN]");

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule::Stop when module is not running.");
			return;
		}

  		// 停止定时器 share_memory_timer_
   		if (share_memory_timer_) 
   		{
   			share_memory_timer_->Stop();
   			share_memory_timer_.reset();
   		}
  		
		STAT_DEBUG("Timer stopped.");

        // 清理
        Clear();

		shared_memory_.Close();

		is_running_ = false;

		STAT_EVENT("StatisticModule is stopped successfully.");
		STAT_DEBUG("StatisticModule::Stop [OUT]");
	}

	DownloadDriverStatistic::p StatisticModule::AttachDownloadDriverStatistic(u_int id, u_int max_http_downloader_count)
	{
		STAT_DEBUG("StatisticModule::AttachDownloadDriverStatistic [IN]");
		STAT_DEBUG("StatisticModule::AttachDownloadDriverStatistic id: " << id << ", max_count: " << max_http_downloader_count);

		DownloadDriverStatistic::p download_driver;

		if( is_running_ == false )
		{
			STAT_WARN("StatisticModule is not running, return null.");
			return download_driver;
		}

		// 如果已经存在 则返回空
		if( download_driver_statistic_map_.find(id) != download_driver_statistic_map_.end() )
		{
			STAT_WARN("ID " << id << " exists, return null.");
			return download_driver;
		}

		// 判断最大个数
		if (statistic_info_.DownloadDriverCount == UINT8_MAX_VALUE)
		{
			STAT_WARN("Download Driver Map is Full, size: " << statistic_info_.DownloadDriverCount << ". Return null");
			return download_driver;
		}

		// 将ID加入数组, 线性开地址
		if (AddDownloadDriverID(id) == true)
        {
            // 新建一个 DownloadDriverStatistic, 然后添加到 download_driver_statistic_s_ 中
            // 返回这个新建的 DownloadDriverStatistic
            download_driver = DownloadDriverStatistic::Create(id);
            download_driver_statistic_map_[id] = download_driver;
            STAT_DEBUG("Download Driver with id " << id << " has been created.");

            // start it
            download_driver->Start(max_http_downloader_count);
            STAT_DEBUG("Download Driver with id " << id << " has been started.");

            STAT_DEBUG("StatisticModule::AttachDownloadDriverStatistic [OUT]");
        }

		return download_driver;
	}

	DownloadDriverStatistic::p StatisticModule::AttachDownloadDriverStatistic(u_int id)
	{
		return AttachDownloadDriverStatistic(id, max_http_downloader_count_);
	}

	bool StatisticModule::DetachDownloadDriverStatistic(int id)
	{
		STAT_DEBUG("StatisticModule::DetachDownloadDriverStatistic [IN]");
		STAT_EVENT("StatisticModule is detaching download driver with id " << id);

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule is not running, return false.");
			return false;
		}

		// 如果 不存该 DownloadDriverStatistic  返回false
		if( download_driver_statistic_map_.find(id) == download_driver_statistic_map_.end() )
		{
			STAT_WARN("Download Driver with id " << id << " does not exist.");
			return false;
		}

		// 将ID移出数组
		if (RemoveDownloadDriverID(id) == true)
        {
            // 停止该 DownloadDriverStatistic
            download_driver_statistic_map_[id]->Stop();
            STAT_DEBUG("Download Driver " << id << " has been stopped.");

            // 删除该 DownloadDriverStatistic
            download_driver_statistic_map_.erase(id);
            STAT_DEBUG("Download Driver " << id << " has been removed from statistic map.");

            STAT_DEBUG("StatisticModule::DetachDownloadDriverStatistic [OUT]");

            return true;
        }

        return false;
	}

    bool StatisticModule::DetachDownloadDriverStatistic(const DownloadDriverStatistic::p download_driver_statistic)
    {
        return DetachDownloadDriverStatistic(download_driver_statistic->GetDownloadDriverID());
    }

    bool StatisticModule::DetachAllDownloadDriverStatistic()
    {
        for (DownloadDriverStatisticMap::iterator it = download_driver_statistic_map_.begin(); 
            it != download_driver_statistic_map_.end(); it++)
        {
            RemoveDownloadDriverID(it->first);
            it->second->Stop();
        }
        // clear
        download_driver_statistic_map_.clear();

        return true;
    }

	P2PDownloaderStatistic::p StatisticModule::AttachP2PDownloaderStatistic(const RID& rid, u_int max_p2p_connection_count)
	{
		STAT_DEBUG("StatisticModule::AttachP2PDownloaderStatistic [IN]");
		STAT_EVENT("StatisticModule is attaching p2p downloader with RID " << rid << ", max count: " << max_p2p_connection_count);
		P2PDownloaderStatistic::p p2p_downloader_;

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule is not running, return null.");
			return p2p_downloader_;
		}

		// 如果已经存在 则返回 空
		if (p2p_downloader_statistic_map_.find(rid) != p2p_downloader_statistic_map_.end())
		{
			STAT_WARN("p2p downloader " << rid << " exists. Return null.");
			return p2p_downloader_;
		}

		// 判断最大个数
		if (statistic_info_.P2PDownloaderCount == UINT8_MAX_VALUE)
		{
			STAT_WARN("p2p downloader map is full. size: " << statistic_info_.P2PDownloaderCount << ". Return null.");
			return p2p_downloader_;
		}

		// 将RID加入数组, 线性开地址
		AddP2PDownloaderRID(rid);

		// 新建一个 P2PDownloaderStatistic::p, 然后添加到 download_driver_statistic_s_ 中
		p2p_downloader_ = P2PDownloaderStatistic::Create(rid);
		p2p_downloader_statistic_map_[rid] = p2p_downloader_;

		STAT_DEBUG("Created P2P Downloader with RID: " << rid);

		// 该 P2PDownloadeStatisticr::p -> Start() 
		p2p_downloader_->Start(max_p2p_connection_count);

		STAT_DEBUG("Started P2P Downloader: " << rid << ", count: " << max_p2p_connection_count);

		// 返回这个新建的 P2PDownloaderStatistic::p
		STAT_DEBUG("StatisticModule::AttachP2PDownloaderStatistic [OUT]");
		return p2p_downloader_;
	}

	P2PDownloaderStatistic::p StatisticModule::AttachP2PDownloaderStatistic(const RID& rid)
	{
		return AttachP2PDownloaderStatistic(rid, max_p2p_connection_count_);
	}

	bool StatisticModule::DetachP2PDownaloaderStatistic(const RID& rid)
	{
		STAT_DEBUG("StatisticModule::DetachP2PDownaloaderStatistic [IN]");
		STAT_EVENT("StatisticModule is detaching P2PDownloader: " << rid);

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule is not running, return false.");
			return false;
		}

		P2PDownloadDriverStatisticMap::iterator it = p2p_downloader_statistic_map_.find(rid);

		// 不存在, 返回
		if (it == p2p_downloader_statistic_map_.end())
		{
			STAT_WARN("P2PDownloader does not exist. Return false.");
			return false;
		}

		assert(it->second);

		it->second->Stop();
		p2p_downloader_statistic_map_.erase(it);
		STAT_DEBUG("P2PDownloader " << rid << " has been stopped and removed.");
		RemoveP2PDownloaderRID(rid);

		STAT_DEBUG("StatisticModule::DetachP2PDownaloaderStatistic [OUT]");
		return true;
	}

    bool StatisticModule::DetachP2PDownaloaderStatistic(const P2PDownloaderStatistic::p p2p_downloader_statistic)
    {
        return DetachP2PDownaloaderStatistic(p2p_downloader_statistic->GetResourceID());
    }

    bool StatisticModule::DetachAllP2PDownaloaderStatistic()
    {
        for (P2PDownloadDriverStatisticMap::iterator it = p2p_downloader_statistic_map_.begin();
            it != p2p_downloader_statistic_map_.end(); it++)
        {
            RemoveP2PDownloaderRID(it->first);
            it->second->Stop();
        }

        // clear
        p2p_downloader_statistic_map_.clear();

        return true;
    }

	void StatisticModule::OnTimerElapsed(boost::shared_ptr<Timer> pointer, u_int times)
	{
		STAT_EVENT("StatisticModule::OnTimerElapsed, times: " << times);

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule is not running, return.");
			return;
		}

		if( pointer == share_memory_timer_ )
		{
			OnShareMemoryTimer(times);
		}
		else
		{
			STAT_ERROR("Invalid Timer Pointer.");
			assert(0);
		}
	}

	void StatisticModule::OnShareMemoryTimer(u_int times)
	{
		if(times % 4 != 0)
			return;

		STAT_DEBUG("StatisticModule::OnShareMemoryTimer [IN], times: " << times);
		STAT_EVENT("StatisticModule::OnShareMemoryTimer, Writing data into shared memory.");

		if( is_running_ == false ) 
		{
			STAT_WARN("StatisticModule is not running, return.");
			return;
		}

		// 将内部变量拷贝到共享内存中
		UpdateSpeedInfo();
		UpdateTrackerInfo();
        UpdateMaxHttpDownloadSpeed();

		STAT_EVENT("Updated Statistic Information has been written to shared memory.");
		memcpy(shared_memory_.GetView(), &statistic_info_, sizeof(STASTISTIC_INFO));

		// 遍历所有的  DownloadDriverStatistic::p->OnShareMemoryTimer(times)
		STAT_DEBUG("Starting to update downloader driver statistic.");
		for (DownloadDriverStatisticMap::iterator it = download_driver_statistic_map_.begin();
			it != download_driver_statistic_map_.end(); it++)
		{
			assert(it->second);
			it->second->OnShareMemoryTimer(times);
		}
		STAT_EVENT("All Downloader Drirvers shared memory has been updated.");

		// 遍历所有的  P2PDownloaderStatistic::p->OnShareMemoryTimer(times)
		STAT_DEBUG("Starting to update p2p downloader statistic.");
		for (P2PDownloadDriverStatisticMap::iterator it = p2p_downloader_statistic_map_.begin();
			it != p2p_downloader_statistic_map_.end(); it++)
		{
			assert(it->second);
			it->second->OnShareMemoryTimer(times);
		}
		STAT_EVENT("All P2PDownloader shared memory has been updated.");

		STAT_DEBUG("StatisticModule::OnShareMemoryTimer [OUT]");
	}

	STASTISTIC_INFO StatisticModule::GetStatisticInfo()
	{
		UpdateSpeedInfo();
		return statistic_info_;
	}

	void StatisticModule::Clear()
	{
		// Maps
        DetachAllP2PDownaloaderStatistic();
        DetachAllDownloadDriverStatistic();
		statistic_tracker_info_map_.clear();

		// Statistic Info
		statistic_info_.Clear();

		// SpeedInfo
		speed_info_->Clear();

		// Misc
		max_http_downloader_count_ = 0;
		max_p2p_connection_count_ = 0;
	}

	string StatisticModule::GetSharedMemoryName()
	{
		return CreateStatisticModuleSharedMemoryName(GetCurrentProcessID());
	}

	inline u_int StatisticModule::GetSharedMemorySize()
	{
		return sizeof(STASTISTIC_INFO);
	}

	bool StatisticModule::CreateSharedMemory()
	{
		STAT_DEBUG("StatisticModule::CreateSharedMemory Creating Shared Memory.");

		shared_memory_.Close();
		shared_memory_.Create(GetSharedMemoryName().c_str(), GetSharedMemorySize());

		STAT_DEBUG("Created Shared Memory: " << ((const char*) GetSharedMemoryName().c_str()) << ", size: " << GetSharedMemorySize() << " Byte(s).");

		return shared_memory_.IsMapped();
	}

	//////////////////////////////////////////////////////////////////////////
	// Speed Info

	void StatisticModule::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		speed_info_->SubmitDownloadedBytes(downloaded_bytes);
		STAT_DEBUG("StatisticModule::SubmitDownloadedBytes added: " << downloaded_bytes);
	}

	void StatisticModule::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		speed_info_->SubmitUploadedBytes(uploaded_bytes);
		STAT_DEBUG("StatisticModule::SubmitUploadedBytes added: " << uploaded_bytes);
	}

	SPEED_INFO StatisticModule::GetSpeedInfo()
	{
		UpdateSpeedInfo();
		return statistic_info_.SpeedInfo;
	}

    UINT32 StatisticModule::GetMaxHttpDownloadSpeed() const
    {
        return statistic_info_.MaxHttpDownloadSpeed;
    }

    UINT32 StatisticModule::GetTotalDownloadSpeed()
    {
        UINT32 total_speed = GetSpeedInfo().NowDownloadSpeed;
        for (DownloadDriverStatisticMap::const_iterator it = download_driver_statistic_map_.begin(); it != download_driver_statistic_map_.end(); ++it)
        {
            DownloadDriverStatistic::p download_driver = it->second;
            if (download_driver)
            {
                total_speed += download_driver->GetSpeedInfo().NowDownloadSpeed;
            }
        }
        return total_speed;
    }

    //////////////////////////////////////////////////////////////////////////
    // Local Download Info

    protocal::PEER_DOWNLOAD_INFO StatisticModule::GetLocalPeerDownloadInfo()
    {
        protocal::PEER_DOWNLOAD_INFO local_download_info;

        if (is_running_ == false)
        {
            STAT_WARN("StatisticModule::GetLocalPeerDownloadInfo Statistic is not running. Return. ");
            return local_download_info;
        }

        UpdateSpeedInfo();

        local_download_info.OnlineTime = speed_info_->GetElapsedTimeInMilliSeconds();
        local_download_info.AvgDownload = statistic_info_.SpeedInfo.AvgDownloadSpeed;
        local_download_info.AvgUpload = statistic_info_.SpeedInfo.AvgUploadSpeed;
        local_download_info.NowDownload = statistic_info_.SpeedInfo.NowDownloadSpeed;
        local_download_info.NowUpload = statistic_info_.SpeedInfo.NowUploadSpeed;
        local_download_info.IsDownloading = TRUE;

		return local_download_info;
    }

    protocal::PEER_DOWNLOAD_INFO StatisticModule::GetLocalPeerDownloadInfo(const RID& rid)
    {
        PEER_DOWNLOAD_INFO peer_download_info;
        if (is_running_ == false)
        {
            return peer_download_info;
        }

        P2PDownloadDriverStatisticMap::iterator it = p2p_downloader_statistic_map_.find(rid);
        if (it == p2p_downloader_statistic_map_.end())
        {
            return peer_download_info;
        }

        SPEED_INFO info = it->second->GetSpeedInfo();
        peer_download_info.IsDownloading = TRUE;
        peer_download_info.AvgDownload = info.AvgDownloadSpeed;
        peer_download_info.AvgUpload = info.AvgUploadSpeed;
        peer_download_info.NowDownload = info.NowDownloadSpeed;
        peer_download_info.NowUpload = info.NowUploadSpeed;
        peer_download_info.OnlineTime = it->second->GetElapsedTimeInMilliSeconds();

        return peer_download_info;
    }

	//////////////////////////////////////////////////////////////////////////
	// Updates

	void StatisticModule::UpdateSpeedInfo()
	{
		statistic_info_.SpeedInfo = speed_info_->GetSpeedInfo();
		STAT_DEBUG("StatisticModule::UpdateSpeedInfo Speed Info has been updated.");
	}

	void StatisticModule::UpdateTrackerInfo()
	{
		STAT_DEBUG("StatisticModule::UpdateTrackerInfo [IN]");

		statistic_info_.TrackerCount = statistic_tracker_info_map_.size();
		STAT_DEBUG("Current Tracker Number: " << statistic_info_.TrackerCount);

		assert(statistic_info_.TrackerCount <= UINT8_MAX_VALUE);

		StatisticTrackerInfoMap::iterator it = statistic_tracker_info_map_.begin();
		for (u_int i = 0; it != statistic_tracker_info_map_.end(); it++, i++)
		{
			statistic_info_.TrackerInfos[i] = it->second;
		}
		STAT_DEBUG("StatisticModule::UpdateTrackerInfo [OUT]");
	}

    void StatisticModule::UpdateMaxHttpDownloadSpeed()
    {
        UINT32 max_http_download_speed = 0;
        STL_FOR_EACH_CONST(DownloadDriverStatisticMap, download_driver_statistic_map_, iter)
        {
            DownloadDriverStatistic::p dd_statistic_ = iter->second;
            if (dd_statistic_)
            {
                UINT32 http_speed = dd_statistic_->GetHttpDownloadMaxSpeed();
                if (max_http_download_speed < http_speed)
                    max_http_download_speed = http_speed;
            }
        }
        statistic_info_.MaxHttpDownloadSpeed = max_http_download_speed;
    }

	//////////////////////////////////////////////////////////////////////////
	// IP Info

	void StatisticModule::SetLocalPeerInfo(const CANDIDATE_PEER_INFO& local_peer_info)
	{
		statistic_info_.LocalPeerInfo = local_peer_info;
		STAT_DEBUG("StatisticModule::SetLocalPeerInfo [" << CandidatePeerInfo(local_peer_info) << "].");
	}
	
	CandidatePeerInfo StatisticModule::GetLocalPeerInfo()
	{
		return CandidatePeerInfo(statistic_info_.LocalPeerInfo);
	}

	void StatisticModule::SetLocalPeerAddress(const PEER_ADDR& peer_addr)
	{
		statistic_info_.LocalPeerInfo.IP = peer_addr.IP;
		statistic_info_.LocalPeerInfo.UdpPort = peer_addr.UdpPort;
		statistic_info_.LocalPeerInfo.TcpPort = peer_addr.TcpPort;
		STAT_DEBUG("StatisticModule::SetLocalPeerAddress [" << PeerAddr(peer_addr) << "].");
	}

    void StatisticModule::SetLocalPeerIp(u_int ip)
    {
        statistic_info_.LocalPeerInfo.IP = ip;
    }

    void StatisticModule::SetLocalPeerUdpPort(u_short udp_port)
    {
        statistic_info_.LocalPeerInfo.UdpPort = udp_port;
    }

    void StatisticModule::SetLocalPeerTcpPort(u_short tcp_port)
    {
        statistic_info_.LocalPeerInfo.TcpPort = tcp_port;
    }

	PeerAddr StatisticModule::GetLocalPeerAddress()
	{
		return PeerAddr(statistic_info_.LocalPeerInfo.IP, statistic_info_.LocalPeerInfo.UdpPort, statistic_info_.LocalPeerInfo.TcpPort);
	}

    void StatisticModule::SetLocalDetectSocketAddress(const SOCKET_ADDR& socket_addr)
	{
		statistic_info_.LocalPeerInfo.DetectIP = socket_addr.IP;
		statistic_info_.LocalPeerInfo.DetectUdpPort = socket_addr.Port;
		STAT_DEBUG("StatisticModule::SetLocalDetectSocketAddress [" << SocketAddr(socket_addr) << "].");
	}

	SocketAddr StatisticModule::GetLocalDetectSocketAddress()
	{
		return SocketAddr(statistic_info_.LocalPeerInfo.DetectIP, statistic_info_.LocalPeerInfo.DetectUdpPort);
	}

	void StatisticModule::SetLocalStunSocketAddress(const SOCKET_ADDR& socket_addr)
	{
		statistic_info_.LocalPeerInfo.StunIP = socket_addr.IP;
		statistic_info_.LocalPeerInfo.StunUdpPort = socket_addr.Port;
		STAT_DEBUG("StatisticModule::SetLocalStunSocketAddress [" << SocketAddr(socket_addr) << "].");
	}
	SocketAddr StatisticModule::GetLocalStunSocketAddress()
	{
		return SocketAddr(statistic_info_.LocalPeerInfo.StunIP, statistic_info_.LocalPeerInfo.StunUdpPort);
	}

	void StatisticModule::SetLocalIPs(const vector<u_long>& local_ips)
	{
		statistic_info_.LocalIpCount = min(MAX_IP_COUNT, local_ips.size());

		STAT_DEBUG("Local IP Count: " << local_ips.size() << ", Max allowed count: " << MAX_IP_COUNT);

		for (u_int i = 0; i < statistic_info_.LocalIpCount; i++)
		{
			statistic_info_.LocalIPs[i] = local_ips[i];
			STAT_DEBUG("    Set IP: " << IpPortToUdpEndpoint(local_ips[i], 0).address());
		}
		STAT_DEBUG("All " << statistic_info_.LocalIpCount << " IPs are stored.");
	}

	//////////////////////////////////////////////////////////////////////////
	// Peer Info

	void StatisticModule::SetLocalPeerVersion(u_int local_peer_version)
	{
		statistic_info_.LocalPeerVersion = local_peer_version;
		STAT_DEBUG("StatisticModule::SetLocalPeerVersion " << local_peer_version);
	}

	void StatisticModule::SetLocalPeerType(UINT8 local_peer_type)
	{
		statistic_info_.LocalPeerType = local_peer_type;
		STAT_DEBUG("StatisticModule::SetLocalPeerType " << (int)local_peer_type);
	}

	//////////////////////////////////////////////////////////////////////////
	// Tracker Server

	inline STATISTIC_TRACKER_INFO& StatisticModule::GetTracker(const TRACKER_INFO& tracker_info)
	{
		return statistic_tracker_info_map_[tracker_info];
	}

	void StatisticModule::SetTrackerInfo(u_int group_count, const vector<TRACKER_INFO>& tracker_infos)
	{
		STAT_DEBUG("StatisticModule::SetTrackerInfo [IN]");
		STAT_DEBUG("    Tracker Group Count: " << group_count << ", Trackers Number: " << tracker_infos.size());

		statistic_info_.GroupCount = group_count;
		statistic_info_.TrackerCount = min(tracker_infos.size(), UINT8_MAX_VALUE);
		STAT_DEBUG("    Statistic Tracker Count: " << statistic_info_.TrackerCount);

		statistic_tracker_info_map_.clear();
		for (u_int i = 0; i < statistic_info_.TrackerCount; i++)
		{
			const TRACKER_INFO& tracker_info = tracker_infos[i];
			STAT_DEBUG("    Add Tracker Info: " << tracker_info);
			statistic_tracker_info_map_[ tracker_info ] = STATISTIC_TRACKER_INFO(tracker_info);
		}
		STAT_DEBUG("StatisticModule::SetTrackerInfo [OUT]");
	}

	void StatisticModule::SetIsSubmitTracker(const TRACKER_INFO& tracker_info, bool is_submit_tracker)
	{
		GetTracker(tracker_info).IsSubmitTracker = is_submit_tracker;
		STAT_DEBUG("Set IsSubmitTracker: " << is_submit_tracker << " to Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitCommitRequest(const TRACKER_INFO& tracker_info)
	{
		GetTracker(tracker_info).CommitRequestCount++;
		STAT_DEBUG("Current CommitRequestCount: " << GetTracker(tracker_info).CommitRequestCount << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitCommitResponse(const TRACKER_INFO& tracker_info)
	{
		GetTracker(tracker_info).CommitResponseCount++;
		STAT_DEBUG("Current CommitResponseCount: " << GetTracker(tracker_info).CommitResponseCount << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitKeepAliveRequest(const TRACKER_INFO& tracker_info)
	{
		GetTracker(tracker_info).KeepAliveRequestCount++;
		STAT_DEBUG("Current KeepAliveRequestCount: " << GetTracker(tracker_info).KeepAliveRequestCount << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitKeepAliveResponse(const TRACKER_INFO& tracker_info, u_short keep_alive_interval)
	{
		STATISTIC_TRACKER_INFO& tracker = GetTracker(tracker_info);
		tracker.KeepAliveResponseCount++;
		tracker.KeepAliveInterval = keep_alive_interval;
		STAT_DEBUG("Current KeepAliveResponseCount: " << GetTracker(tracker_info).KeepAliveResponseCount << ", KLPInterval: " << keep_alive_interval << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitListRequest(const TRACKER_INFO& tracker_info)
	{
		GetTracker(tracker_info).ListRequestCount++;
		STAT_DEBUG("Current ListRequestCount: " << GetTracker(tracker_info).ListRequestCount << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitListResponse(const TRACKER_INFO& tracker_info, u_int peer_count)
	{
		STATISTIC_TRACKER_INFO& tracker = GetTracker(tracker_info);
		tracker.ListResponseCount++;
		tracker.LastListReturnPeerCount = peer_count;
		STAT_DEBUG("Current ListResponseCount: " << GetTracker(tracker_info).ListResponseCount << ", PeerCount: " << peer_count << " of Tracker: " << tracker_info);
	}

	void StatisticModule::SubmitErrorCode(const TRACKER_INFO& tracker_info, UINT8 error_code)
	{
		GetTracker(tracker_info).ErrorCode = error_code;
		STAT_DEBUG("Current ErrorCode: " << error_code << " of Tracker: " << tracker_info);
	}

	//////////////////////////////////////////////////////////////////////////
	// Index Server

	void StatisticModule::SetIndexServerInfo(u_long ip, u_short port, byte type)
	{
		statistic_info_.StatisticIndexInfo.IP = ip;
		statistic_info_.StatisticIndexInfo.Port = port;
		statistic_info_.StatisticIndexInfo.Type = type;
		STAT_DEBUG("Set IndexServerInfo: " << IpPortToUdpEndpoint(ip, port) << ", type[0-U;1-T]: " << type);
	}

	void StatisticModule::SetIndexServerInfo(const SocketAddr& socket_addr, byte type)
	{
		statistic_info_.StatisticIndexInfo.IP = socket_addr.IP;
		statistic_info_.StatisticIndexInfo.Port = socket_addr.Port;
		statistic_info_.StatisticIndexInfo.Type = type;
		STAT_DEBUG("Set IndexServerInfo: " << socket_addr << ", type[0-U;1-T]: " << type);
	}

	void StatisticModule::SubmitQueryRIDByUrlRequest()
	{
		statistic_info_.StatisticIndexInfo.QueryRIDByUrlRequestCount++;
		STAT_DEBUG("IndexServer, QueryRIDByUrlRequestCount: " << statistic_info_.StatisticIndexInfo.QueryRIDByUrlRequestCount);
	}

	void StatisticModule::SubmitQueryRIDByUrlResponse()
	{
		statistic_info_.StatisticIndexInfo.QueryRIDByUrlResponseCount++;
		STAT_DEBUG("IndexServer, QueryRIDByUrlResponseCount: " << statistic_info_.StatisticIndexInfo.QueryRIDByUrlResponseCount);
	}

	void StatisticModule::SubmitQueryHttpServersByRIDRequest()
	{
		statistic_info_.StatisticIndexInfo.QueryHttpServersByRIDRequestCount++;
		STAT_DEBUG("IndexServer, QueryHttpServersByRIDRequestCount: " << statistic_info_.StatisticIndexInfo.QueryHttpServersByRIDRequestCount);
	}

	void StatisticModule::SubmitQueryHttpServersByRIDResponse()
	{
		statistic_info_.StatisticIndexInfo.QueryHttpServersByRIDResponseCount++;
		STAT_DEBUG("IndexServer, QueryHttpServersByRIDResponseCount: " << statistic_info_.StatisticIndexInfo.QueryHttpServersByRIDResponseCount);
	}

	void StatisticModule::SubmitQueryTrackerListRequest()
	{
		statistic_info_.StatisticIndexInfo.QueryTrackerListRequestCount++;
		STAT_DEBUG("IndexServer, QueryTrackerListRequestCount: " << statistic_info_.StatisticIndexInfo.QueryTrackerListRequestCount);
	}

	void StatisticModule::SubmitQueryTrackerListResponse()
	{
		statistic_info_.StatisticIndexInfo.QureyTrackerListResponseCount++;
		STAT_DEBUG("IndexServer, QureyTrackerListResponseCount: " << statistic_info_.StatisticIndexInfo.QureyTrackerListResponseCount);
	}

	void StatisticModule::SubmitAddUrlRIDRequest()
	{
		statistic_info_.StatisticIndexInfo.AddUrlRIDRequestCount++;
		STAT_DEBUG("IndexServer, AddUrlRIDRequestCount: " << statistic_info_.StatisticIndexInfo.AddUrlRIDRequestCount);
	}

	void StatisticModule::SubmitAddUrlRIDResponse()
	{
		statistic_info_.StatisticIndexInfo.AddUrlRIDResponseCount++;
		STAT_DEBUG("IndexServer, AddUrlRIDResponseCount: " << statistic_info_.StatisticIndexInfo.AddUrlRIDResponseCount);
	}

	//////////////////////////////////////////////////////////////////////////
	// P2PDownloader RID Address

	UINT8 StatisticModule::Address(const RID& rid) // Be careful when buffer FULL
	{
		UINT8 pos = HashFunc(boost::hash_value(rid));
        UINT32 count = 0;
		while (statistic_info_.P2PDownloaderRIDs[pos] != GUID_NULL && count < UINT8_MAX_VALUE)
		{
			if (statistic_info_.P2PDownloaderRIDs[pos] == rid)
				return pos;
			pos = (pos + 1) % HASH_SIZE; // size 太小,不适合用平方
            ++count;
		}
		return pos;
	}

	bool StatisticModule::AddP2PDownloaderRID(const RID& rid)
	{
		UINT8 pos = Address(rid);
		if (statistic_info_.P2PDownloaderRIDs[pos] == GUID_NULL)
		{
			statistic_info_.P2PDownloaderCount++;
			statistic_info_.P2PDownloaderRIDs[pos] = rid;
			return true;
		}
		return false;
	}

	bool StatisticModule::RemoveP2PDownloaderRID(const RID& rid)
	{
        UINT8 pos = Address(rid);
		if (statistic_info_.P2PDownloaderRIDs[pos] == rid)
		{
			statistic_info_.P2PDownloaderCount--;
			statistic_info_.P2PDownloaderRIDs[pos] = GUID_NULL;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// DownloaderDriverID Address

	UINT StatisticModule::Address(u_int id)
	{
		UINT8 pos = HashFunc(id);
        UINT32 count = 0;
		while (statistic_info_.DownloadDriverIDs[pos] != 0 && count < UINT8_MAX_VALUE)
		{
			if (statistic_info_.DownloadDriverIDs[pos] == id)
				return pos;
            pos = (pos + 1) % HASH_SIZE;
            ++count;
		}
		return pos;
	}

	bool StatisticModule::AddDownloadDriverID(u_int id)
	{
		UINT8 pos = Address(id);
		if (statistic_info_.DownloadDriverIDs[pos] == 0)
		{
			statistic_info_.DownloadDriverCount++;
			statistic_info_.DownloadDriverIDs[pos] = id;
			return true;
		}
		return false;
	}

	bool StatisticModule::RemoveDownloadDriverID(u_int id)
	{
		UINT8 pos = Address(id);
		if (statistic_info_.DownloadDriverIDs[pos] == id)
		{
			statistic_info_.DownloadDriverCount--;
			statistic_info_.DownloadDriverIDs[pos] = 0;
			return true;
		}
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	//停止时数据上传相关
	void StatisticModule::SubmitP2PDownloaderDownloadBytes(u_int p2p_downloader_download_bytes)
	{
		statistic_info_.TotalP2PDownloadBytes += p2p_downloader_download_bytes;
		STAT_DEBUG("IndexServer, SubmitP2PDownloaderDownloadBytes: " << statistic_info_.TotalP2PDownloadBytes);
	}

	void StatisticModule::SubmitOtherServerDownloadBytes(u_int other_server_download_bytes)
	{
		statistic_info_.TotalOtherServerDownloadBytes += other_server_download_bytes;
		STAT_DEBUG("IndexServer, QueryRIDByUrlResponseCount: " << statistic_info_.TotalOtherServerDownloadBytes);
	}

	//////////////////////////////////////////////////////////////////////////
	//下载中 数据下载 实时相关信息
	void StatisticModule::SubmitTotalHttpNotOriginalDataBytes (u_int bytes)    // 实时 下载的纯数据 字节数
	{
		statistic_info_.TotalHttpNotOriginalDataBytes+=bytes;						// 实时 下载的纯数据 字节数
	}

	void StatisticModule::SubmitTotalP2PDataBytes(u_int bytes)  // 实时 P2P下载的纯数据 字节数
	{
		statistic_info_.TotalP2PDataBytes+=bytes;					// 实时 P2P下载的纯数据 字节数
	}

	void StatisticModule::SubmitTotalHttpOriginalDataBytes(u_int bytes)// 实时 原生下载的纯数据 字节数
	{
		statistic_info_.TotalHttpOriginalDataBytes+=bytes;				// 实时 原生下载的纯数据 字节数
	}

	void StatisticModule::SubmitCompleteCount(u_int count)					// 已经完成的DD数
	{
		statistic_info_.CompleteCount+=count;					// 已经完成的DD数
	}

    u_int StatisticModule::GetTotalDataBytes()
    {
        return statistic_info_.TotalP2PDataBytes + statistic_info_.TotalHttpOriginalDataBytes;
    }

    u_short StatisticModule::GetTotalDataBytesInMB()
    {
        return ((GetTotalDataBytes() >> 20) & 0xFFFFU);
    }

    //////////////////////////////////////////////////////////////////////////
    // Upload Cache 统计

    void StatisticModule::SetUploadCacheRequest(u_int count)
    {
        statistic_info_.TotalUploadCacheRequestCount = count;
    }

    void StatisticModule::SetUploadCacheHit(u_int count)
    {
        statistic_info_.TotalUploadCacheHitCount = count;
    }

    float StatisticModule::GetUploadCacheHitRate()
    {
        if (statistic_info_.TotalUploadCacheRequestCount == 0) return 0.0f;
        return (statistic_info_.TotalUploadCacheHitCount + 0.0f) / statistic_info_.TotalUploadCacheRequestCount;
    }

    //////////////////////////////////////////////////////////////////////////
    // HttpProxyPort

    void StatisticModule::SetHttpProxyPort(u_short port)
    {
        statistic_info_.HttpProxyPort = port;
    }

    //////////////////////////////////////////////////////////////////////////
    // IncomingPeer
    void StatisticModule::SubmitIncomingPeer()
    {
        statistic_info_.IncomingPeersCount++;
    }

    UINT16 StatisticModule::GetIncomingPeersCount()
    {
        return statistic_info_.IncomingPeersCount;
    }

    //////////////////////////////////////////////////////////////////////////
    // DownloadDurationInSec

    void StatisticModule::SubmitDownloadDurationInSec(const UINT16& download_duration_in_sec)
    {
        statistic_info_.DownloadDurationInSec += download_duration_in_sec;
    }

    UINT16 StatisticModule::GetDownloadDurationInSec()
    {
        return statistic_info_.DownloadDurationInSec;
    }

}
