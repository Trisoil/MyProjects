#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <map>
#include <string>

#include "framework/mswin/ShareMemory.h"

#include "SpeedInfoStatistic.h"
#include "StatisticStructs.h"
#include "HttpDownloaderStatistic.h"

using namespace std;
using namespace framework::mswin;

namespace statistic
{
	class DownloadDriverStatistic
		: public boost::noncopyable
		, public boost::enable_shared_from_this<DownloadDriverStatistic>
	{
	public:

		typedef boost::shared_ptr<DownloadDriverStatistic> p;

		static p Create(int id);

	public:

		void Start(u_int max_http_downloader_count);

		void Stop();

		void Clear();

		bool IsRunning() const;

		void OnShareMemoryTimer(u_int times);

	public:

		//////////////////////////////////////////////////////////////////////////
		// Operations

		HttpDownloaderStatistic::p AttachHttpDownloaderStatistic(const string& url);

		bool DetachHttpDownloaderStatistic(const string& url);

        bool DetachHttpDownloaderStatistic(const HttpDownloaderStatistic::p http_downloader_statistic);

        bool DetachAllHttpDownloaderStatistic();

		//////////////////////////////////////////////////////////////////////////
		// Download Driver Statistic Info

		DOWNLOADDRIVER_STATISTIC_INFO GetDownloadDriverStatisticInfo();

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void SubmitDownloadedBytes(u_int downloaded_bytes);

		void SubmitUploadedBytes(u_int uploaded_bytes);

		SPEED_INFO GetSpeedInfo();

		//////////////////////////////////////////////////////////////////////////
		// Url Info

		void SetOriginalUrl(const string& original_url);

		void SetOriginalReferUrl(const string& original_refer_url);

		//////////////////////////////////////////////////////////////////////////
		// Shared Memory

		string GetSharedMemoryName();

		u_int GetSharedMemorySize();

		//////////////////////////////////////////////////////////////////////////
		// Misc

		u_int GetDownloadDriverID() const;

		u_int GetMaxHttpDownloaderCount() const;

        //////////////////////////////////////////////////////////////////////////
        // Resource Info

        void SetResourceID(const RID& rid);

        RID GetResourceID();

        void SetFileLength(u_int file_length);

        u_int GetFileLength();

        void SetBlockSize(u_int block_size);

        u_int GetBlockSize();

        void SetBlockCount(u_short block_count);

        u_short GetBlockCount();

        //////////////////////////////////////////////////////////////////////////
        // HTTP Data Bytes

        void SubmitHttpDataBytes(u_int http_data_bytes);

        void SetLocalDataBytes(u_int local_data_bytes);

        u_int GetTotalHttpDataBytes() { return DownloadDriverStatisticInfo().TotalHttpDataBytes; }

        //////////////////////////////////////////////////////////////////////////

        // HTTP Max Download Speed (历史最大瞬时速度)
        u_int GetHttpDownloadMaxSpeed();

        // HTTP 历史平均速度
        u_int GetHttpDownloadAvgSpeed();

	private:

		//////////////////////////////////////////////////////////////////////////
		// Speed Info & HTTP Downloader Info

		void UpdateSpeedInfo();

		void UpdateHttpDownloaderInfo();

		DOWNLOADDRIVER_STATISTIC_INFO& DownloadDriverStatisticInfo();

        //////////////////////////////////////////////////////////////////////////
        // Shared Memory

        bool CreateSharedMemory();

	private:

		typedef map<string, HttpDownloaderStatistic::p> HttpDownloaderStatisticMap;

	private:

		bool is_running_;

        string original_url_;

        string original_refer_url_;

		SpeedInfoStatistic::p speed_info_;

		DOWNLOADDRIVER_STATISTIC_INFO_EX download_driver_statistic_info_;

		u_int download_driver_id_;

		HttpDownloaderStatisticMap http_downloader_statistic_map_;

		ShareMemory shared_memory_;

        u_int http_download_max_speed_;

	private:

        DownloadDriverStatistic() : http_download_max_speed_(0) {}

		DownloadDriverStatistic(u_int id);
	};
}
