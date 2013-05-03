//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ConfigManager.h"
#include "Config.h"
#include <framework/configure/Config.h>

namespace super_node
{
    const std::string ConfigManager::CommonConfigFileName = "CommonConfig.conf";
    const std::string ConfigManager::PrivateConfigFileName = "PrivateConfig.conf";
    const std::string ConfigManager::AllConfigFileName = "AllConfig.conf";
    const std::string ConfigManager::ServerListFileName = "ServerList.txt";

    ConfigManager::ConfigManager(boost::shared_ptr<boost::asio::io_service> io_service, ConfigUpdateCallback callback)
        : timer_(new boost::asio::deadline_timer(*io_service)), io_service_(io_service), callback_(callback)
    {
    }

    void ConfigManager::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::ConfigManager(), "ConfigManager::Start()");
        ConfigManagerSettings settings;
        settings.LoadFromConfig();
        Start(settings);
        LOG4CPLUS_INFO(Loggers::ConfigManager(), "ConfigManager started.");
    }

    void ConfigManager::Start(const ConfigManagerSettings & settings)
    {
        settings_ = settings;
        Config::ReadConfig(AllConfigFileName, config_);
        OnTimerElapsed();
    }

    void ConfigManager::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::ConfigManager(), "ConfigManager::Stop()");
        LOG4CPLUS_INFO(Loggers::ConfigManager(), "ConfigManager stoped.");

        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            timer_.reset();
        }
    }

    void ConfigManager::OnTimerElapsed()
    {
        if (download_tasks_.empty())
        {
            if (!settings_.GetCommonConfigUrl().empty())
            {
                download_tasks_.push_back(DownloadTask(settings_.GetCommonConfigUrl(), COMMON));
            }
            else
            {
                LOG4CPLUS_INFO(Loggers::ConfigManager(), "Common config file url is empty.");
            }

            if (!settings_.GetPrivateConfigUrl().empty())
            {
                download_tasks_.push_back(DownloadTask(settings_.GetPrivateConfigUrl(), PRIVATE));
            }
            else
            {
                LOG4CPLUS_INFO(Loggers::ConfigManager(), "Private config file url is empty.");
            }

            if (!settings_.GetServerListUrl().empty())
            {
                download_tasks_.push_back(DownloadTask(settings_.GetServerListUrl(), ServerList));
            }
            else
            {
                LOG4CPLUS_INFO(Loggers::ConfigManager(), "ServerList URL is empty.");
            }

            if (!download_tasks_.empty())
            {
                DownloadConfig();
            }
        }

        if (timer_)
        {
            timer_->expires_from_now(boost::posix_time::seconds(settings_.GetDownloadInterval()));
            timer_->async_wait(boost::bind(&ConfigManager::OnTimerElapsed, shared_from_this()));
        }
    }

    ConfigManager::ConfigManagerSettings::ConfigManagerSettings(
        const std::string & common_config_url, 
        const std::string & private_config_url,
        const std::string & server_list_url,
        boost::uint32_t download_interval)
        : common_config_url_(common_config_url), private_config_url_(private_config_url), 
        server_list_url_(server_list_url), download_interval_(download_interval)
    {

    }

    void ConfigManager::ConfigManagerSettings::LoadFromConfig()
    {
        framework::configure::Config conf("SuperNodeService.conf");
        conf.register_module("Config")
            << CONFIG_PARAM_NAME_RDONLY("common_config_url", common_config_url_)
            << CONFIG_PARAM_NAME_RDONLY("private_config_url", private_config_url_)
            << CONFIG_PARAM_NAME_RDONLY("server_list_url", server_list_url_)
            << CONFIG_PARAM_NAME_RDONLY("download_interval_in_second", download_interval_);
    }

    void ConfigManager::DownloadConfig()
    {
        boost::shared_ptr<util::protocol::HttpClient> http_client(new util::protocol::HttpClient(*io_service_));

        assert(!download_tasks_.empty());

        LOG4CPLUS_INFO(Loggers::ConfigManager(), "ConfigManager:DownloadConfig - downloading " << download_tasks_.begin()->download_url_);
        http_client->async_fetch(download_tasks_.begin()->download_url_, boost::bind(&ConfigManager::OnConfigDownloaded, shared_from_this(), http_client, _1));
    }

    std::string ConfigManager::GetConfigName(boost::uint8_t config_type) const
    {
        switch(config_type)
        {
        case COMMON:
            return CommonConfigFileName;
        case PRIVATE:
            return PrivateConfigFileName;
        case ServerList:
            return ServerListFileName;
        default:
            assert(false);
        }

        return "";
    }

    void ConfigManager::OnConfigDownloaded(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & error_code)
    {
        assert(!download_tasks_.empty());
        boost::uint8_t config_type = download_tasks_.begin()->config_type_;
        download_tasks_.pop_front();
        if (error_code)
        {
            LOG4CPLUS_ERROR(Loggers::ConfigManager(), "Download " << GetConfigName(config_type) << " error for " << error_code.message());
        }
        else
        {
            std::string config_filename = GetConfigName(config_type);

            LOG4CPLUS_INFO(Loggers::ConfigManager(), config_filename << " downloaded.");

            boost::asio::streambuf & data = http_client->response().data();
            boost::asio::const_buffer respond_data = data.data();
            const char * config_content = boost::asio::buffer_cast<const char *>(respond_data);
            int size = boost::asio::buffer_size(respond_data);

            std::ofstream config_file(config_filename.c_str(), std::ios_base::out|std::ios_base::binary);
            if (config_file)
            {
                config_file.write(config_content, size);
                LOG4CPLUS_INFO(Loggers::ConfigManager(), "Updated "<<config_filename);
            }
            else
            {
                LOG4CPLUS_ERROR(Loggers::ConfigManager(), "open " << config_filename << " failed.");
            }
        }

        if (download_tasks_.empty())
        {
            std::map<std::string, std::map<std::string, std::string> > new_config;
            if (Config::MergeConfig(PrivateConfigFileName, CommonConfigFileName, new_config) && new_config != config_)
            {
                LOG4CPLUS_INFO(Loggers::ConfigManager(), "Config has been changed.");

                config_ = new_config;
                Config::WriteConfig(AllConfigFileName, new_config);
                if (callback_)
                {
                    callback_();
                }
            }
        }
        else
        {
            DownloadConfig();
        }
    }

    bool ConfigManager::IsConfigUpdatedAutomatically()
    {
        return !settings_.GetCommonConfigUrl().empty() || 
            !settings_.GetPrivateConfigUrl().empty() || 
            !settings_.GetServerListUrl().empty();
    }
}
