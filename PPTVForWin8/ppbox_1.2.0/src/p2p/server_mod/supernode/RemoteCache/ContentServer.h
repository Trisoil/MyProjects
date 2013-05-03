//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _SN_CONTENT_SERVER_H_
#define _SN_CONTENT_SERVER_H_

#include <framework/timer/AsioTimerManager.h>
#include "SuperNodeServiceStruct.h"

namespace super_node
{   
    class IDownloadResultHandler;
    class IResourceChangeListener;

    typedef boost::function<
        void (const boost::system::error_code&, boost::asio::streambuf &)
            >DownloadCallBack;

    class IContentServer
    {
    public:
        virtual void Start() = 0;
        virtual void AsyncDownload(const std::string & resource_name , size_t segment_index, size_t start_block_index, size_t num_of_blocks, boost::shared_ptr<IDownloadResultHandler> download_result_handler) = 0;
        virtual ~IContentServer(){}
    };

    class ContentServer
        :public boost::enable_shared_from_this<ContentServer>,
        public IContentServer
    {
    public:
        ContentServer(std::string server_address, const RemoteCacheConfig& resource_cache_config, boost::shared_ptr<IResourceChangeListener> resource_change_listener, boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();
        const std::string & GetServerAddress() const { return server_address_; }       
        void AsyncDownload(const std::string & resource_name , size_t segment_index, size_t start_block_index, size_t num_of_blocks, boost::shared_ptr<IDownloadResultHandler> download_result_handler);
        bool operator == (const ContentServer & rhs)
        {
            return server_address_ == rhs.server_address_;
        }

        bool operator != (const ContentServer & rhs)
        {
            return server_address_ != rhs.server_address_;
        }

        void UpdateConfig(const RemoteCacheConfig& config);

    private:
        void AsyncDownloadActually(const util::protocol::HttpRequestHead& request_header, boost::uint32_t time_out_in_milliseconds, DownloadCallBack callback);
        void HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & err, DownloadCallBack callback);
        void OnResourceDownloaded(const boost::system::error_code & err, boost::asio::streambuf & buf, const string& resource_name, size_t segment_index, size_t start_block_index, size_t num_of_blocks, boost::shared_ptr<IDownloadResultHandler> download_result_handler);
        void OnResourceListDownloaded(const boost::system::error_code & err, boost::asio::streambuf & buf);

    private:
        static void ParseResourceList(std::istream & stream, std::set<std::string> & new_resources);
        static void GenerateAddAndRemoveResource(const std::set<std::string> & old_resources, const std::set<std::string> & new_resources, std::set<std::string> & add_resources, std::set<std::string> & remove_resources);
        void DownloadResourceList();

    private:
        std::string server_address_;
        std::set<string> resource_on_server_;
        boost::shared_ptr<IResourceChangeListener> resource_change_listener_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        boost::shared_ptr<framework::timer::AsioTimerManager> time_manager_;
        static const std::string request_resource_list_path;
        size_t ask_for_resource_list_interval_;
        size_t resource_list_download_timeout_in_seconds_;
        size_t resource_download_timeout_in_seconds_;
    };
}

#endif