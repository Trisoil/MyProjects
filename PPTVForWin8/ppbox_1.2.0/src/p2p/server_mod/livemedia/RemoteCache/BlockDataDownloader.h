//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef LIVE_MEDIA_BLOCK_DATA_DOWNLOADER_H
#define LIVE_MEDIA_BLOCK_DATA_DOWNLOADER_H

#include <framework/timer/AsioTimerManager.h>
#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpClient.h>

namespace live_media
{   
    class IDownloadResultHandler;
    struct ChannelConfiguration;

    typedef boost::function<
        void (const boost::system::error_code&, boost::asio::streambuf &)
            >DownloadCallBack;

    class IBlockDataDownloader
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void AsyncDownload(const channel_id & channel_identifier, size_t block_index, size_t timeout_in_seconds, const ChannelConfiguration& channel_config, boost::shared_ptr<IDownloadResultHandler> download_result_handler) = 0;
        virtual ~IBlockDataDownloader(){}
    };

    class BlockDataDownloader
        :public boost::enable_shared_from_this<BlockDataDownloader>,
        public IBlockDataDownloader
    {
    public:
        BlockDataDownloader(boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();

        void AsyncDownload(const channel_id & channel_identifier, size_t block_index, size_t timeout_in_seconds, const ChannelConfiguration& channel_config, boost::shared_ptr<IDownloadResultHandler> download_result_handler);

    private:
        void AsyncDownloadActually(const util::protocol::HttpRequestHead& request_header, boost::uint32_t time_out_in_milliseconds, DownloadCallBack callback);
        void HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & err, DownloadCallBack callback);
        void OnResourceDownloaded(const boost::system::error_code & err, boost::asio::streambuf & response_data, const channel_id& channel_identifier, size_t block_index, boost::shared_ptr<IDownloadResultHandler> download_result_handler);

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<framework::timer::AsioTimerManager> time_manager_;
    };
}

#endif //LIVE_MEDIA_BLOCK_DATA_DOWNLOADER_H