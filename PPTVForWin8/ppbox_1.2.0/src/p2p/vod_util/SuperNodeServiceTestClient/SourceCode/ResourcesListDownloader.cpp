//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourcesListDownloader.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iomanip>

namespace super_node_test
{
    string Date(const boost::gregorian::date& resource_date)
    {
        std::ostringstream date_string_stream;
        date_string_stream<<resource_date.year()<<std::setfill('0')<<std::setw(2)<<resource_date.month().as_number()<<std::setw(2)<<resource_date.day();
        return date_string_stream.str();
    }

    string MovieResourceName(const boost::gregorian::date& resource_date)
    {
        std::ostringstream movie_resource_name;
        movie_resource_name<<"Movie-"<<Date(resource_date)<<".txt";
        return movie_resource_name.str();
    }

    void ResourceListDownloader::Start(GetResourceListCallback callback)
    {
        callback_ = callback;
        TryAnotherDay();
    }

    void ResourceListDownloader::TryAnotherDay()
    {
        boost::gregorian::date resource_date = today_ - boost::gregorian::date_duration(days_before_);

        string movie_resource_name = MovieResourceName(resource_date);
        std::ifstream resource_file(movie_resource_name.c_str());
        if (resource_file)
        {
            //resource list for current date already exists, not going to download
            std::cout<<"Found existing resource list for "<<Date(resource_date)<<std::endl;

            resource_file.close();
            callback_(movie_resource_name);
            return;
        }

        std::ostringstream url_builder;
        url_builder<<"http://60.28.216.145:8090/analog/hot"<<Date(resource_date)<<".txt";

        std::cout<<"Trying to download resource list for "<<Date(resource_date)<<std::endl;
        DoAsyncDownload(url_builder.str(), bind(&ResourceListDownloader::OnResourceListDownloaded, shared_from_this(), movie_resource_name, _1, _2));
    }

    void ResourceListDownloader::OnResourceListDownloaded(const std::string& movie_resource_name,const boost::system::error_code& err, boost::asio::streambuf& response_data)
    {
        if (err)
        {
            if (++days_before_ > 5)
            {
                std::cout<<"Failing all attempts. Giving up..."<<std::endl;
                callback_("");
            }
            else
            {
                TryAnotherDay();
            }

            return;
        }

        std::ofstream out_stream(movie_resource_name.c_str());
        std::copy(std::istreambuf_iterator<char>(&response_data), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(out_stream));
        callback_(movie_resource_name);
    }

    void ResourceListDownloader::DoAsyncDownload(const string& request_path, DownloadCallback callback)
    {
        boost::shared_ptr<util::protocol::HttpClient> http_client(new util::protocol::HttpClient(*io_service_));

        boost::system::error_code error;

        http_client->bind_host(server_address_, "8090", error);

        if (error)
        {
            callback(error, boost::asio::streambuf());
            return;
        }

        util::protocol::HttpRequestHead head;
        head.method = util::protocol::HttpRequestHead::get;
        head.path = request_path;
        head["Accept"] = "{*/*}";
        //framework does not support comma (,) separated value
        //head["If-Modified-Since"] = "{Sat, 13 Aug 2011 22:56:45 GMT}";
        head.host = server_address_;
        head.connection = util::protocol::http_filed::Connection::close;

        http_client->async_fetch(head, boost::bind(&ResourceListDownloader::HandleFetchResult, shared_from_this(), http_client, _1, callback));
    }

    void ResourceListDownloader::HandleFetchResult(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code& err,DownloadCallback callback)
    {
        if (err)
        {
            callback(err, boost::asio::streambuf());
        }
        else
        {
            callback(err, http_client->get_response().data());
        }

        http_client->close();
    }
}