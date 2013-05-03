#include "stdafx.h"
#include "base/func.h"
#include "HttpDownloaderStatistic.h"
#include "StatisticUtil.h"
#include "StatisticStructs.h"
#include "SpeedInfoStatistic.h"
#include "DownloadDriverStatistic.h"

namespace statistic
{
    HttpDownloaderStatistic::HttpDownloaderStatistic(const string& url, DownloadDriverStatistic::p download_driver_statistic)
		: url_(url)
		, is_running_(false)
		, speed_info_(SpeedInfoStatistic::Create())
        , download_driver_statistic_(download_driver_statistic)
	{
	}

    HttpDownloaderStatistic::p HttpDownloaderStatistic::Create(const string& url, DownloadDriverStatistic::p download_driver_statistic)
	{
		return p(new HttpDownloaderStatistic(url, download_driver_statistic));
	}

	void HttpDownloaderStatistic::Start()
	{
        STAT_DEBUG("HttpDownloaderStatistic::Start [IN]");
		if (is_running_ == true)
        {
            STAT_DEBUG("HttpDownloaderStatistic is running. Return.");
			return ;
        }

		Clear();

        SetUrl(url_);

		speed_info_->Start();
        STAT_DEBUG("Started Speed Info.");

		is_running_ = true;
        STAT_DEBUG("HttpDownloaderStatistic::Start [OUT]");
	}

	void HttpDownloaderStatistic::Stop()
	{
        STAT_DEBUG("HttpDownloaderStatistic::Stop [IN]");
		if (is_running_ == false)
        {
            STAT_DEBUG("HttpDownloaderStatistic is not running. Return.");
			return ;
        }

		speed_info_->Stop();
        STAT_DEBUG("Stopped Speed Info.");

		Clear();

		is_running_ = false;
        STAT_DEBUG("HttpDownloaderStatistic::Stop [OUT]");
	}

	void HttpDownloaderStatistic::Clear()
	{
		speed_info_->Clear();
		http_downloader_info_.Clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// HTTP Downloader Info

	HTTP_DOWNLOADER_INFO HttpDownloaderStatistic::GetHttpDownloaderInfo()
	{
		UpdateSpeedInfo();
		return http_downloader_info_;
	}

	//////////////////////////////////////////////////////////////////////////
	// Speed Info

	SPEED_INFO HttpDownloaderStatistic::GetSpeedInfo()
	{
		UpdateSpeedInfo();
		return http_downloader_info_.SpeedInfo;
	}

	void HttpDownloaderStatistic::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		speed_info_->SubmitDownloadedBytes(downloaded_bytes);
        if (download_driver_statistic_)
        {
            download_driver_statistic_->SubmitDownloadedBytes(downloaded_bytes);
        }
        else
        {
            STAT_WARN("Download Driver is NULL, HttpDownloaderStatisis: " << url_);
        }
        STAT_DEBUG("HttpDownloaderStatistic::SubmitDownloadedBytes " << downloaded_bytes << " Bytes.");
	}

	void HttpDownloaderStatistic::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		speed_info_->SubmitUploadedBytes(uploaded_bytes);
        if (download_driver_statistic_)
        {
            download_driver_statistic_->SubmitUploadedBytes(uploaded_bytes);
        }
        else
        {
            STAT_WARN("Download Driver is NULL, HttpDownloaderStatisis: " << url_);
        }
        STAT_DEBUG("HttpDownloaderStatistic::SubmitUploadedBytes " << uploaded_bytes << " Bytes.");
	}

	void HttpDownloaderStatistic::UpdateSpeedInfo()
	{
		http_downloader_info_.SpeedInfo = speed_info_->GetSpeedInfo();
	}

	//////////////////////////////////////////////////////////////////////////
	// Events

	void HttpDownloaderStatistic::SubmitHttpConnected()
	{
		http_downloader_info_.LastConnectedTime = GetTickCountInMilliSecond();
        STAT_DEBUG("HttpDownloaderStatistic::SubmitHttpConnected " << http_downloader_info_.LastConnectedTime);
	}

	void HttpDownloaderStatistic::SubmitRequestPiece()
	{
		http_downloader_info_.LastRequestPieceTime = GetTickCountInMilliSecond();
        STAT_DEBUG("HttpDownloaderStatistic::SubmitRequestPiece " << http_downloader_info_.LastRequestPieceTime);
	}

	void HttpDownloaderStatistic::SubmitRetry()
	{
		http_downloader_info_.RetryCount++;
        STAT_DEBUG("HttpDownloaderStatistic::SubmitRetry count: " << http_downloader_info_.RetryCount);
	}

    void HttpDownloaderStatistic::ClearRetry()
    {
        http_downloader_info_.RetryCount = 0;
        STAT_DEBUG("HttpDownloaderStatistic::ClearRetry count: " << http_downloader_info_.RetryCount);
    }

	//////////////////////////////////////////////////////////////////////////
	// Status

	void HttpDownloaderStatistic::SetSupportRange(bool is_support_range)
	{
		http_downloader_info_.IsSupportRange = is_support_range;
        STAT_DEBUG("HttpDownloaderStatistic::SetSupportRange " << is_support_range);
	}

	void HttpDownloaderStatistic::SetHttpStatusCode(u_int http_status_code)
	{
		http_downloader_info_.LastHttpStatusCode = http_status_code;
        STAT_DEBUG("HttpDownloaderStatistic::SetHttpStatusCode " << http_status_code);
	}

	//////////////////////////////////////////////////////////////////////////
	// Url

	void HttpDownloaderStatistic::SetUrl(const string& url)
	{
		url_ = url;
		SetTruncUrl(url_, http_downloader_info_.Url);
	}

	string HttpDownloaderStatistic::GetUrl() const
	{
		return url_;
	}

	void HttpDownloaderStatistic::SetReferUrl(const string& refer_url)
	{
		refer_url_ = refer_url;
		SetTruncUrl(refer_url_, http_downloader_info_.ReferUrl);
	}

	string HttpDownloaderStatistic::GetReferUrl() const
	{
		return refer_url_;
	}

	void HttpDownloaderStatistic::SetRedirectUrl(const string& redirect_url)
	{
		redirect_url_ = redirect_url;
		SetTruncUrl(redirect_url_, http_downloader_info_.RedirectUrl);
	}

	string HttpDownloaderStatistic::GetRedirectUrl() const
	{
		return redirect_url_;
	}

	//////////////////////////////////////////////////////////////////////////
	// PieceInfo

	void HttpDownloaderStatistic::SetDownloadingPieceInfo(const PIECE_INFO_EX& downloading_piece_info)
	{
		http_downloader_info_.DownloadingPieceEx = downloading_piece_info;
        STAT_DEBUG("HttpDownloaderStatistic::SetDownloadingPieceInfo [" << downloading_piece_info << "].");
	}

	void HttpDownloaderStatistic::SetStartPieceInfo(const PIECE_INFO_EX& start_piece_info)
	{
		http_downloader_info_.StartPieceEx = start_piece_info;
        STAT_DEBUG("HttpDownloaderStatistic::SetStartPieceInfo [" << start_piece_info << "].");
	}

	void HttpDownloaderStatistic::SetDownloadingPieceInfo(
		u_short BlockIndex, u_short PieceIndexInBlock, u_short SubPieceIndexInPiece)
	{
		SetDownloadingPieceInfo( PIECE_INFO_EX(BlockIndex, PieceIndexInBlock, SubPieceIndexInPiece) );
	}

	void HttpDownloaderStatistic::SetStartPieceInfo(
		u_short BlockIndex, u_short PieceIndexInBlock, u_short SubPieceIndexInPiece)
	{
		SetStartPieceInfo( PIECE_INFO_EX(BlockIndex, PieceIndexInBlock, SubPieceIndexInPiece) );
	}

    //////////////////////////////////////////////////////////////////////////
    // death

    void HttpDownloaderStatistic::SetIsDeath(bool is_death)
    {
        http_downloader_info_.IsDeath = is_death;
    }

    bool HttpDownloaderStatistic::IsDeath() const
    {
        return http_downloader_info_.IsDeath;
    }

}
