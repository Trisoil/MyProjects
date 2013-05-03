//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef RESOURCE_LIST_DOWNLOADER_H
#define RESOURCE_LIST_DOWNLOADER_H

namespace super_node_test
{
    class ResourceListDownloader
        : public boost::enable_shared_from_this<ResourceListDownloader>
    {
        typedef boost::function<
            void (const boost::system::error_code&, boost::asio::streambuf&)
        > DownloadCallback;

    public:

        typedef boost::function<
            void (const string& resource_list)
        > GetResourceListCallback;

        ResourceListDownloader(boost::shared_ptr<boost::asio::io_service> io_service)
            : server_address_("60.28.216.145"), io_service_(io_service), started_(false)
        {
            using namespace boost::posix_time;  
            today_ = second_clock::local_time().date();
            days_before_ = 0;
        }

        void Start(GetResourceListCallback callback);
        void Stop(){}

    private:
        void TryAnotherDay();
        void DoAsyncDownload(const string& request_path, DownloadCallback callback);
        void HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code& err,DownloadCallback callback);
        void OnResourceListDownloaded(const std::string& date, const boost::system::error_code& err, boost::asio::streambuf& response_data);
    private:
        GetResourceListCallback callback_;
        boost::gregorian::date today_;
        size_t days_before_;

        string server_address_;
        bool started_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        std::multimap<size_t, string> resources_on_server_;
    };
}

#endif //RESOURCE_LIST_DOWNLOADER_H