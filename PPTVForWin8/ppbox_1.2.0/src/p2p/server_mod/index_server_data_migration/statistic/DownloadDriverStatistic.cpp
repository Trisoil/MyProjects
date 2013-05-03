#include "stdafx.h"
#include "DownloadDriverStatistic.h"
#include "SpeedInfoStatistic.h"
#include "base/func.h"

using namespace base;

namespace statistic
{
	DownloadDriverStatistic::DownloadDriverStatistic(u_int id)
		: is_running_(false)
		, download_driver_id_(id)
		, speed_info_(SpeedInfoStatistic::Create())
        , http_download_max_speed_(0)
	{
	}

	DownloadDriverStatistic::p DownloadDriverStatistic::Create(int id)
	{
		return p(new DownloadDriverStatistic(id));
	}

	void DownloadDriverStatistic::Start(u_int max_http_downloader_count)
	{
        STAT_DEBUG("DownloadDriverStatistic::Start [IN]");
		if ( is_running_ == true )
        {
            STAT_WARN("DownloadDriverStatistic is running, return.");
			return ;
        }

		Clear();

		download_driver_statistic_info_ = DOWNLOADDRIVER_STATISTIC_INFO_EX(max_http_downloader_count);
        STAT_DEBUG("    Max Http Downloader Count: " << max_http_downloader_count);

		DownloadDriverStatisticInfo().DownloadDriverID = download_driver_id_;
        SetOriginalUrl(original_url_);
        SetOriginalReferUrl(original_refer_url_);
        STAT_DEBUG("    Download Driver ID: " << download_driver_id_);

		speed_info_->Start();

		if (CreateSharedMemory() == false)
		{
			STAT_ERROR("Shared Memory Creation Failed: DownloadDriverStatistic");
			return ;
		}

		is_running_ = true;
        STAT_DEBUG("DownloadDriverStatistic::Start [OUT]");
	}

	void DownloadDriverStatistic::Stop()
	{
        STAT_DEBUG("DownloadDriverStatistic::Stop [IN]");
		if ( is_running_ == false )
        {
            STAT_WARN("DownloadDriverStatistic is not running, return.");
			return ;
        }

		speed_info_->Stop();
        STAT_DEBUG("   Speed Info Stopped.");

        Clear();

		shared_memory_.Close();
        STAT_DEBUG("   Shared Memory Closed.");

		is_running_ = false;
        STAT_DEBUG("DownloadDriverStatistic::Stop [OUT]");
	}

	void DownloadDriverStatistic::Clear()
	{
		speed_info_->Clear();
		download_driver_statistic_info_.Clear();
        DetachAllHttpDownloaderStatistic();
        STAT_DEBUG("DownloadDriverStatistic::Clear");
	}

	bool DownloadDriverStatistic::IsRunning() const
	{
		return is_running_;
	}

	void DownloadDriverStatistic::OnShareMemoryTimer(u_int times)
	{
        STAT_DEBUG("DownloadDriverStatistic::OnShareMemoryTimer [IN], times: " << times);
		if (is_running_ == false)
        {
            STAT_WARN("DownloadDriverStatistic is not running, return.");
			return ;
        }

		// Update
		UpdateSpeedInfo();
		UpdateHttpDownloaderInfo();

		// write to memory
		DOWNLOADDRIVER_STATISTIC_INFO& info = DownloadDriverStatisticInfo();
		memcpy(shared_memory_.GetView(), &info, GetSharedMemorySize());

        STAT_DEBUG("Write DOWNLOADDRIVER_STATISTIC_INFO into SharedMemory: " << info);

        STAT_DEBUG("DownloadDriverStatistic::OnShareMemoryTimer [OUT]");
	}

	//////////////////////////////////////////////////////////////////////////
	// Operations

	HttpDownloaderStatistic::p DownloadDriverStatistic::AttachHttpDownloaderStatistic(const string& url)
	{
        STAT_DEBUG("DownloadDriverStatistic::AttachHttpDownloaderStatistic [IN], Url: " << url);

        HttpDownloaderStatistic::p http_downloader_;

		if (is_running_ == false)
        {
            STAT_DEBUG("HttpDownloaderStatistic is not running, return null.");
			return http_downloader_;
        }

		// 判断MaxCount
		if (http_downloader_statistic_map_.size() == GetMaxHttpDownloaderCount())
		{
			STAT_WARN("HTTP Downloader Statistic Map is full (" << GetMaxHttpDownloaderCount() << ").");
			return http_downloader_;
		}

		// 存在, 返回空
		if (http_downloader_statistic_map_.find(url) != http_downloader_statistic_map_.end())
		{
			STAT_WARN("Url exists: " << url);
			return http_downloader_statistic_map_[url];
		}

		// 创建并开始
		http_downloader_ = HttpDownloaderStatistic::Create(url, shared_from_this());
		http_downloader_statistic_map_[ url ] = http_downloader_;

		http_downloader_->Start();
        STAT_DEBUG("Created and started httpdownloader with url: " << url);

        STAT_DEBUG("DownloadDriverStatistic::AttachHttpDownloaderStatistic [OUT]");
		return http_downloader_;
	}

	bool DownloadDriverStatistic::DetachHttpDownloaderStatistic(const string& url)
	{
        STAT_DEBUG("DownloadDriverStatistic::DetachHttpDownloaderStatistic [IN], Url: " << url);

		if (is_running_ == false)
        {
            STAT_WARN("DownloadDriverStatistic is not running. return false.");
			return false;
        }

		// 不存在, 返回
		HttpDownloaderStatisticMap::iterator it = http_downloader_statistic_map_.find(url);
		if (it == http_downloader_statistic_map_.end())
        {
            STAT_WARN("Return false. Can not find given url: " << url);
			return false;
        }

		assert(it->second);

		it->second->Stop();
		http_downloader_statistic_map_.erase(it);
        STAT_DEBUG("Stopped HttpDownloader Statistic, and erased from map.");

        STAT_DEBUG("DownloadDriverStatistic::DetachHttpDownloaderStatistic [OUT]");
		return true;
	}

    bool DownloadDriverStatistic::DetachHttpDownloaderStatistic(const HttpDownloaderStatistic::p http_downloader_statistic)
    {
        return DetachHttpDownloaderStatistic(http_downloader_statistic->GetUrl());
    }

    bool DownloadDriverStatistic::DetachAllHttpDownloaderStatistic()
    {
        for (HttpDownloaderStatisticMap::iterator it = http_downloader_statistic_map_.begin();
            it != http_downloader_statistic_map_.end(); it++)
        {
            it->second->Stop();
        }
        // clear
        http_downloader_statistic_map_.clear();

        return true;
    }

	//////////////////////////////////////////////////////////////////////////
	// Url Info

	void DownloadDriverStatistic::SetOriginalUrl(const string& original_url)
	{
        original_url_ = original_url;
		SetTruncUrl(original_url, DownloadDriverStatisticInfo().OriginalUrl);
        STAT_DEBUG("DownloadDriverStatistic::SetOriginalReferUrl, Original Url: " << original_url);
	}

	void DownloadDriverStatistic::SetOriginalReferUrl(const string& original_refer_url)
	{
        original_refer_url_ = original_refer_url;
		SetTruncUrl(original_refer_url, DownloadDriverStatisticInfo().OriginalReferUrl);
        STAT_DEBUG("DownloadDriverStatistic::SetOriginalReferUrl, Original Refer Url: " << original_refer_url);
	}

	//////////////////////////////////////////////////////////////////////////
	// Misc

	u_int DownloadDriverStatistic::GetDownloadDriverID() const
	{
		return download_driver_id_;
	}

	u_int DownloadDriverStatistic::GetMaxHttpDownloaderCount() const
	{
		return download_driver_statistic_info_.MaxTailElementCount();
	}

    //////////////////////////////////////////////////////////////////////////
    // Resource Info

    void DownloadDriverStatistic::SetResourceID(const RID& rid)
    {
        DownloadDriverStatisticInfo().ResourceID = rid;
    }

    RID DownloadDriverStatistic::GetResourceID()
    {
        return DownloadDriverStatisticInfo().ResourceID;
    }

    void DownloadDriverStatistic::SetFileLength(u_int file_length)
    {
        DownloadDriverStatisticInfo().FileLength = file_length;
    }

    u_int DownloadDriverStatistic::GetFileLength()
    {
        return DownloadDriverStatisticInfo().FileLength;
    }

    void DownloadDriverStatistic::SetBlockSize(u_int block_size)
    {
        DownloadDriverStatisticInfo().BloskSize = block_size;
    }

    u_int DownloadDriverStatistic::GetBlockSize()
    {
        return DownloadDriverStatisticInfo().BloskSize;
    }

    void DownloadDriverStatistic::SetBlockCount(u_short block_count)
    {
        DownloadDriverStatisticInfo().BlockCount = block_count;
    }

    u_short DownloadDriverStatistic::GetBlockCount()
    {
        return DownloadDriverStatisticInfo().BlockCount;
    }

	//////////////////////////////////////////////////////////////////////////
	// Speed Info & HTTP Downloader Info

	void DownloadDriverStatistic::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		speed_info_->SubmitDownloadedBytes(downloaded_bytes);
	}

	void DownloadDriverStatistic::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		speed_info_->SubmitUploadedBytes(uploaded_bytes);
	}

	SPEED_INFO DownloadDriverStatistic::GetSpeedInfo()
	{
		UpdateSpeedInfo();
		return DownloadDriverStatisticInfo().SpeedInfo;
	}

	void DownloadDriverStatistic::UpdateSpeedInfo()
	{
        STAT_DEBUG("DownloadDriverStatistic::UpdateSpeedInfo");
		if (is_running_ == false)
        {
            STAT_WARN("DownloadDriverStatistic Module is not running, return.");
			return ;
        }

		DownloadDriverStatisticInfo().SpeedInfo = speed_info_->GetSpeedInfo();
	}

	void DownloadDriverStatistic::UpdateHttpDownloaderInfo()
	{
        STAT_DEBUG("DownloadDriverStatistic::UpdateHttpDownloaderInfo [IN]");
		if (is_running_ == false)
        {
            STAT_WARN("DownloadDriverStatistic module is running, return.");
			return ;
        }

		DownloadDriverStatisticInfo().HttpDownloaderCount = http_downloader_statistic_map_.size();
        STAT_DEBUG("Http Downloader Count: " << http_downloader_statistic_map_.size());

		assert(DownloadDriverStatisticInfo().HttpDownloaderCount <= GetMaxHttpDownloaderCount());

		HttpDownloaderStatisticMap::iterator it = http_downloader_statistic_map_.begin();
		for (u_int i = 0; it != http_downloader_statistic_map_.end(); it++, i++)
		{
			assert(it->second);
			DownloadDriverStatisticInfo().HttpDownloaders[i] = it->second->GetHttpDownloaderInfo();
		}
        STAT_DEBUG("DownloadDriverStatistic::UpdateHttpDownloaderInfo [OUT]");
	}

	DOWNLOADDRIVER_STATISTIC_INFO& DownloadDriverStatistic::DownloadDriverStatisticInfo()
	{
		return download_driver_statistic_info_.Ref();
	}

    //////////////////////////////////////////////////////////////////////////
    // HTTP Data Bytes

    void DownloadDriverStatistic::SubmitHttpDataBytes(u_int http_data_bytes)
    {
        DownloadDriverStatisticInfo().TotalHttpDataBytes += http_data_bytes;
    }

    void DownloadDriverStatistic::SetLocalDataBytes(u_int local_data_bytes)
    {
        DownloadDriverStatisticInfo().TotalLocalDataBytes = local_data_bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    // HTTP Max Download Speed

    u_int DownloadDriverStatistic::GetHttpDownloadMaxSpeed()
    {
        STL_FOR_EACH_CONST(HttpDownloaderStatisticMap, http_downloader_statistic_map_, iter)
        {
            HttpDownloaderStatistic::p statistic = iter->second;
            u_int now_speed = statistic->GetSpeedInfo().NowDownloadSpeed;
            if (now_speed > http_download_max_speed_) 
            {
                http_download_max_speed_ = now_speed;
            }
        }
        return http_download_max_speed_;
    }

    u_int DownloadDriverStatistic::GetHttpDownloadAvgSpeed()
    {
        u_int http_avg = 0;
        STL_FOR_EACH_CONST(HttpDownloaderStatisticMap, http_downloader_statistic_map_, iter)
        {
            HttpDownloaderStatistic::p statistic = iter->second;
            u_int avg_speed = statistic->GetSpeedInfo().AvgDownloadSpeed;
            //!
            http_avg += avg_speed;
        }
        return http_avg;
    }

	//////////////////////////////////////////////////////////////////////////
	// Shared Memory
	bool DownloadDriverStatistic::CreateSharedMemory()
	{
        STAT_DEBUG("DownloadDriverStatistic::CreateSharedMemory: [" << GetSharedMemoryName().c_str() << "] size:" << GetSharedMemorySize() << " Bytes.");
		shared_memory_.Create(GetSharedMemoryName().c_str(), GetSharedMemorySize());
		return shared_memory_.IsMapped();
	}

	string DownloadDriverStatistic::GetSharedMemoryName()
	{
		return CreateDownloadDriverModuleSharedMemoryName(GetCurrentProcessID(), GetDownloadDriverID());
	}

	u_int DownloadDriverStatistic::GetSharedMemorySize()
	{
		return download_driver_statistic_info_.Size();
	}

}
