//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ContentServer.h"
#include "DownloadResultHandler.h"
#include "key_generator.h"
#include "ResourceChangeListener.h"
#include "HttpClientWithTimeoutSupport.h"

namespace super_node
{
    const std::string ContentServer::request_resource_list_path = "/MD5.txt";
    const std::string RemoteCacheConfig::DefaultPlaceIdentifier = "1000";

    ContentServer::ContentServer(std::string server_address, const RemoteCacheConfig& resource_cache_config, boost::shared_ptr<IResourceChangeListener> resource_change_listener, boost::shared_ptr<boost::asio::io_service> io_service)
        :server_address_(server_address), resource_change_listener_(resource_change_listener), io_service_(io_service)
    {
        UpdateConfig(resource_cache_config);
    }

    void ContentServer::Start()
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        time_manager_.reset(new framework::timer::AsioTimerManager(*io_service_, boost::posix_time::seconds(1)));
        time_manager_->start();

        DownloadResourceList();
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "Server " << server_address_ << " is available");
    }

    void ContentServer::Stop()
    {
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "server " << server_address_ << " is stopped.");

        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            if (cancel_error)
            {
                LOG4CPLUS_WARN(super_node::Loggers::RemoteCache(), "Failed to cancel timer. Error code:" << cancel_error);
            }
            timer_.reset();
        }
    }

    void ContentServer::DownloadResourceList()
    {
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "Downloading "<<request_resource_list_path<<" from "<<server_address_);
        DownloadCallBack func = boost::bind(&ContentServer::OnResourceListDownloaded, shared_from_this(), _1, _2);

        util::protocol::HttpRequestHead head;
        head.method = util::protocol::HttpRequestHead::get;
        head.path = request_resource_list_path;
        head["Accept"] = "{*/*}";
        head.host = server_address_;
        head.connection = util::protocol::http_filed::Connection::close;

        io_service_->post(boost::bind(&ContentServer::AsyncDownloadActually, shared_from_this(),
            head, resource_list_download_timeout_in_seconds_*1000, func));

        if (timer_)
        {
            timer_->expires_from_now(boost::posix_time::minutes(ask_for_resource_list_interval_) + boost::posix_time::seconds((rand() % 10) * 10));
            timer_->async_wait(boost::bind(&ContentServer::DownloadResourceList, shared_from_this()));
        }
    }

    //this piece of code comes from peer (UrlCodec::Encode)
    static string UrlEncode(const string& url)
    {
        const static char hex_map[] = "0123456789ABCDEF";
        string encoded_url;
        for (boost::uint32_t i = 0; i < url.length(); ++i)
        {
            char c = url[i];
            // is alpha
            if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
            {
                encoded_url.push_back(c);
            }
            else
            {
                encoded_url.push_back('%');
                encoded_url.push_back(hex_map[(c & 0xFF) >> 4]);
                encoded_url.push_back(hex_map[c & 0x0F]);
            }
        }

        return encoded_url;
    }

    template<typename CharType> 
    class NoThousandsSeparator 
        : public std::numpunct<CharType> 
    {
    }; 

    void ContentServer::AsyncDownload(const std::string & resource_name , size_t segment_index, size_t start_block_index, size_t num_of_blocks, boost::shared_ptr<IDownloadResultHandler> download_result_handler)
    {
        assert(num_of_blocks > 0);

        LOG4CPLUS_INFO(Loggers::RemoteCache(),"Downloading " << resource_name << "[" << segment_index << "]" << " from " << server_address_ );
        string key = KeyGenerator::GenKey();
        std::ostringstream stream;
        stream<<"/"<<segment_index<<"/"<<UrlEncode(resource_name)<<"?key="<< key;

        const size_t HttpVersion11 = 0x101;
        util::protocol::HttpRequestHead head;
        head.method = util::protocol::HttpRequestHead::get;
        head.path = stream.str();
        head.version = HttpVersion11;
        head["Accept"] = "{*/*}";
        head.host = server_address_;
        //the range is inclusive, and the offset starts with 0
        size_t range_start = start_block_index*BlockData::MaxBlockSize;
        size_t range_end = range_start + num_of_blocks*BlockData::MaxBlockSize  - 1;

        //Note: there's an issue with the built-in head.range support from framework
        //head.range = util::protocol::http_filed::Range(range_start, range_end+1);
        //the resulting request field uses commas (',') as thousand separator in the range value, 
        //and it's not accepted as valid request by the server.
        //Also note that, if we were to use the built-in range support from framework, 
        //range_end would be EXCLUSIVE, as framework would do the minus one

        std::ostringstream bytes_range;
        //locale will take care of destroying the pointer to the numpunct
        bytes_range.imbue(std::locale(bytes_range.getloc(), new NoThousandsSeparator<char>())); 
        bytes_range<<"{bytes="<<range_start<<"-"<<range_end<<"}";
        head["range"] = bytes_range.str();
        head.connection = util::protocol::http_filed::Connection::close;

        AsyncDownloadActually(head, resource_download_timeout_in_seconds_*1000, boost::bind(&ContentServer::OnResourceDownloaded, shared_from_this(), _1, _2, resource_name, segment_index, start_block_index, num_of_blocks, download_result_handler));
    }
    
    void ContentServer::AsyncDownloadActually(const util::protocol::HttpRequestHead& request_header, boost::uint32_t time_out_in_milliseconds, DownloadCallBack callback)
    {
        boost::shared_ptr<util::protocol::HttpClientWithTimeoutSupport> http_client(new util::protocol::HttpClientWithTimeoutSupport(*io_service_, time_manager_));
        boost::system::error_code error;
        http_client->bind_host(server_address_, error);
        if (error)
        {
            LOG4CPLUS_ERROR(Loggers::RemoteCache(), "Http bind_host failed when downloading " << request_header.path << " from " << server_address_<< " error type: " <<error);
            boost::asio::streambuf tmp_streambuf;
            callback(error, tmp_streambuf);
            return;
        }

        http_client->set_time_out(time_out_in_milliseconds, error);

        if (error)
        {
            LOG4CPLUS_ERROR(Loggers::RemoteCache(), "Failed to set timeout when downloading " << request_header.path << " from " << server_address_<< " error type: " << error);
            HandleFetchResult(http_client, error, callback);
        }
        else
        {
            http_client->async_fetch_result(request_header, boost::bind(&ContentServer::HandleFetchResult, shared_from_this(), http_client, _1, callback));
        }
    }

    void ContentServer::HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & err, DownloadCallBack callback)
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
        
        http_client->close();
    }

    void ContentServer::OnResourceDownloaded(
        const boost::system::error_code & err, 
        boost::asio::streambuf & response_data, 
        const string& resource_name, 
        size_t segment_index, 
        size_t start_block_index, 
        size_t num_of_blocks, 
        boost::shared_ptr<IDownloadResultHandler> download_result_handler)
    {
        std::vector<boost::shared_ptr<BlockData> > blocks;
        if (!err)
        {
            OperationsLogging::LogDownloadSuccess(server_address_, resource_name, segment_index, start_block_index, num_of_blocks);
            const char* response_data_buffer = boost::asio::buffer_cast<const char*>(response_data.data());

            std::vector<char> response_content(response_data_buffer, response_data_buffer + response_data.size());

            size_t offset = 0;
            while(offset < response_content.size())
            {
                std::vector<char>::const_iterator copy_begin_pos = response_content.begin() + offset;
                std::vector<char>::const_iterator copy_end_pos = response_content.end();
                if (offset + BlockData::MaxBlockSize < response_content.size())
                {
                    copy_end_pos = copy_begin_pos + BlockData::MaxBlockSize;
                }

                boost::shared_ptr<BlockData> block(new BlockData(copy_begin_pos, copy_end_pos));
                blocks.push_back(block);

                offset += BlockData::MaxBlockSize;
            }
        }
        else
        {
            OperationsLogging::LogDownloadFailure(err, server_address_, resource_name, segment_index, start_block_index, num_of_blocks);
        }

        std::map<size_t, boost::shared_ptr<BlockData> > result;

        if (!err)
        {
            assert(blocks.size() <= num_of_blocks);
            for(size_t i = 0; i < num_of_blocks; ++i)
            {
                if (i < blocks.size() && blocks[i])
                {
                    result[i + start_block_index] = blocks[i];
                }
                else
                {
                    result[i + start_block_index] = boost::shared_ptr<BlockData>();
                }
            }
        }

        download_result_handler->HandleDownloadResult(err, result);
    }

    void ContentServer::OnResourceListDownloaded(const boost::system::error_code & err, boost::asio::streambuf & response_data)
    {
        if (!err)
        {
            LOG4CPLUS_INFO(Loggers::RemoteCache(), "Downloaded "<<request_resource_list_path<<" from "<<server_address_);

            std::istream stream(&response_data);
            std::set<std::string> new_resources;
            std::set<std::string> add_resources;
            std::set<std::string> remove_resources;
            ParseResourceList(stream, new_resources);
            GenerateAddAndRemoveResource(resource_on_server_, new_resources, add_resources, remove_resources);
            if (resource_change_listener_ && (add_resources.size() > 0 || remove_resources.size() > 0))
            {
                resource_change_listener_->OnResourceChanged(add_resources, remove_resources, shared_from_this());
                resource_on_server_ = new_resources;
            }
        }
        else
        {
            OperationsLogging::LogDownloadResourceListFailure(err, server_address_);
        }

        if (resource_on_server_.size() == 0)
        {
            LOG4CPLUS_WARN(Loggers::RemoteCache(), "No resource is available on CDN "<<server_address_);
            OperationsLogging::Log(OperationsLogEvents::WarningNoResourceOnContentServer, server_address_, Warning);
        }
    }

    void ContentServer::ParseResourceList(std::istream & stream, std::set<std::string> & new_resources)
    {
        const int MaxLineSize = 1024;
        char line_text[MaxLineSize];

        while (stream)
        {
            if (!stream.getline(line_text, MaxLineSize))
            {
                break;
            }


            string resource_name(line_text);
            string::size_type delimiter_pos = resource_name.find('|');

            if (delimiter_pos != string::npos)
            {
                resource_name = resource_name.substr(0, delimiter_pos);

                if (new_resources.find(resource_name) == new_resources.end())
                {
                    new_resources.insert(resource_name);
                }
            }
            else if (resource_name != "end")
            {
                assert(false);
                std::cout << "--->invalid line in resource list: " << resource_name << std::endl;
            }

        }
    }

    void ContentServer::GenerateAddAndRemoveResource(const std::set<std::string> & old_resources, const std::set<std::string> & new_resources, std::set<std::string> & add_resources, std::set<std::string> &remove_resources)
    {
        add_resources = new_resources;
        remove_resources = old_resources;

        if (old_resources.size() > 0)
        {
            for(std::set<string>::const_iterator iter_from_old = old_resources.begin();
                iter_from_old != old_resources.end();
                ++iter_from_old)
            {
                if (add_resources.find(*iter_from_old) != add_resources.end())
                {
                    remove_resources.erase(*iter_from_old);
                    add_resources.erase(*iter_from_old);
                }
            }
        }
    }

    void ContentServer::UpdateConfig(const RemoteCacheConfig& config)
    {
        ask_for_resource_list_interval_ = config.ask_for_resource_list_interval_;
        resource_list_download_timeout_in_seconds_ = config.resource_list_download_timeout_in_seconds_;
        resource_download_timeout_in_seconds_ = config.resource_download_timeout_in_seconds_;
    }
}
