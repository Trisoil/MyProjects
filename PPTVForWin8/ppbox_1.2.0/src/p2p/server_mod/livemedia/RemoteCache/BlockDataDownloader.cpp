//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "BlockDataDownloader.h"
#include "DownloadResultHandler.h"
#include "HttpClientWithTimeoutSupport.h"
#include "Configuration.h"
#include "network/Uri.h"

namespace live_media
{
    BlockDataDownloader::BlockDataDownloader(boost::shared_ptr<boost::asio::io_service> io_service)
        :io_service_(io_service)
    {
    }

    void BlockDataDownloader::Start()
    {
        time_manager_.reset(new framework::timer::AsioTimerManager(*io_service_, boost::posix_time::seconds(1)));
        time_manager_->start();

        LOG4CPLUS_INFO(Loggers::RemoteCache(), "BlockDataDownloader is started");
    }

    void BlockDataDownloader::Stop()
    {
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "BlockDataDownloader is stopped.");
    }

    void BlockDataDownloader::AsyncDownload(const channel_id & channel_identifier, size_t block_index, size_t timeout_in_seconds, const ChannelConfiguration& channel_config, boost::shared_ptr<IDownloadResultHandler> download_result_handler)
    {
        LOG4CPLUS_INFO(Loggers::RemoteCache(),"Downloading block " << block_index << " for channel " << channel_identifier);

        network::Uri uri(channel_config.channel_fetch_base_url);
        std::ostringstream stream;

        string fetch_path = uri.getpath();
        stream<<fetch_path;
        if (fetch_path.find_last_of('/') != fetch_path.length() - 1)
        {
            stream<<"/";
        }
        
        stream <<block_index<<".block";

        const size_t HttpVersion11 = 0x101;
        util::protocol::HttpRequestHead head;
        head.method = util::protocol::HttpRequestHead::get;
        head.path = stream.str();
        head.version = HttpVersion11;
        head["Accept"] = "{*/*}";
        head.host = uri.gethost();

        head.connection = util::protocol::http_filed::Connection::close;

        AsyncDownloadActually(head, timeout_in_seconds*1000, boost::bind(&BlockDataDownloader::OnResourceDownloaded, shared_from_this(), _1, _2, channel_identifier, block_index, download_result_handler));
    }
    
    void BlockDataDownloader::AsyncDownloadActually(const util::protocol::HttpRequestHead& request_header, boost::uint32_t time_out_in_milliseconds, DownloadCallBack callback)
    {
        boost::shared_ptr<util::protocol::HttpClientWithTimeoutSupport> http_client(new util::protocol::HttpClientWithTimeoutSupport(*io_service_, time_manager_));
        boost::system::error_code error;

        if (request_header.host)
        {
            std::string host = request_header.host.get();
            http_client->bind_host(request_header.host.get(), error);
            if (error)
            {
                LOG4CPLUS_ERROR(Loggers::RemoteCache(), "Http bind_host failed when downloading " << request_header.path << ", error type: " <<error);
                boost::asio::streambuf tmp_streambuf;
                callback(error, tmp_streambuf);
                return;
            }
        }

        http_client->set_time_out(time_out_in_milliseconds, error);

        if (error)
        {
            LOG4CPLUS_ERROR(Loggers::RemoteCache(), "Failed to set timeout when downloading " << request_header.path << ", error type: " << error);
            HandleFetchResult(http_client, error, callback);
        }
        else
        {
            http_client->async_fetch_result(request_header, boost::bind(&BlockDataDownloader::HandleFetchResult, shared_from_this(), http_client, _1, callback));
        }
    }

    void BlockDataDownloader::HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & err, DownloadCallBack callback)
    {
        if (err)
        {
            boost::asio::streambuf tmp_streambuf;
            callback(err,tmp_streambuf);
        } 
        else
        {
            callback(err, http_client->get_response().data());
        }
        
        boost::system::error_code error;
        http_client->close(error);
    }

    void BlockDataDownloader::OnResourceDownloaded(
        const boost::system::error_code & err, 
        boost::asio::streambuf & response_data, 
        const channel_id& channel_identifier, 
        size_t block_index, 
        boost::shared_ptr<IDownloadResultHandler> download_result_handler)
    {
        boost::shared_ptr<BlockData> block_data;
        if (!err)
        {
            const char* response_data_buffer = boost::asio::buffer_cast<const char*>(response_data.data());
            block_data.reset(new BlockData(response_data_buffer, response_data.size()));
        }

        download_result_handler->HandleDownloadResult(err, block_data);
    }
}