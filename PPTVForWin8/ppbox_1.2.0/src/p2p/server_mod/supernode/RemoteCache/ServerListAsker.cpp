//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "ServerListAsker.h"

namespace super_node
{
    ServerListAsker::ServerListAsker(boost::uint32_t interval, const std::string & place_identifier, boost::shared_ptr<boost::asio::io_service> io_service, DownloadServerListCallBack callback)
        :interval_(interval), place_identifier_(place_identifier), io_service_(io_service), callback_(callback)
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
    }

    void ServerListAsker::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "ServerListAsker::Start()");
        io_service_->post(bind(&ServerListAsker::DoAskForServerList, shared_from_this()));
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "ServerListAsker Started.");
    }

    void ServerListAsker::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "ServerListAsker::Stop()");
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "ServerListAsker Stopped.");
    }

    void ServerListAsker::DoAskForServerList()
    {
        try
        {
            std::vector<std::string> servers;
            std::ifstream in("ServerList.txt");
            if (!in)
            {
                LOG4CPLUS_ERROR(Loggers::RemoteCache(), "ServerList.txt does not exist.");
            }
            else
            {
                while (!in.eof())
                {
                    std::string line_text;
                    std::string ip, place_name;
                    boost::uint32_t server_number;
                    std::string place_identifier;
                    std::getline(in, line_text);
                    std::istringstream iss(line_text, std::istringstream::in);
                    iss >> place_identifier >> place_name >> server_number >> ip;
                    if (iss && place_identifier  == place_identifier_)
                    {
                        servers.push_back(ip);
                    }
                }

                in.close();
                std::sort(servers.begin(), servers.end());
                callback_(servers);
            }
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &)
        {
            LOG4CPLUS_ERROR(Loggers::RemoteCache(), "RemoteCache::AskForServerList(): boost::filesystem::exist() throw an exception.");
        }

        timer_->expires_from_now(boost::posix_time::minutes(interval_));
        timer_->async_wait(boost::bind(&ServerListAsker::DoAskForServerList, shared_from_this()));
    }

    void ServerListAsker::UpdateConfig(boost::uint32_t interval, const std::string & place_identifier)
    {
        if (interval_ != interval || place_identifier_ != place_identifier)
        {
            interval_ = interval;
            place_identifier_ = place_identifier;
            io_service_->post(bind(&ServerListAsker::DoAskForServerList, shared_from_this()));
        }
    }
}