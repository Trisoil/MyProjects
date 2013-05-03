//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_CONFIG_MANAGER_H
#define SUPER_NODE_CONFIG_MANAGER_H

namespace super_node
{
    typedef boost::function<void()> ConfigUpdateCallback;

    class ConfigManager : public boost::enable_shared_from_this<ConfigManager>
    {
        class ConfigManagerSettings
        {
        public:
            ConfigManagerSettings() : download_interval_(600) {}

            ConfigManagerSettings(
                const std::string & common_config_url, 
                const std::string & private_config_url, 
                const std::string & server_list_url, 
                boost::uint32_t download_interval);

            void LoadFromConfig();

            const std::string& GetCommonConfigUrl() const { return common_config_url_; }
            const std::string& GetPrivateConfigUrl() const { return private_config_url_; }
            const std::string& GetServerListUrl() const { return server_list_url_; }
            boost::uint32_t GetDownloadInterval() const { return download_interval_; }

        private:
            std::string common_config_url_;
            std::string private_config_url_;
            std::string server_list_url_;
            boost::uint32_t download_interval_;
        };

    public:
        ConfigManager(boost::shared_ptr<boost::asio::io_service> io_service, ConfigUpdateCallback callback);
        void Start();
        void Start(const ConfigManagerSettings & settings);
        void Stop();
        bool IsConfigUpdatedAutomatically();

    private:
        std::string GetConfigName(boost::uint8_t config_type) const;
        void OnTimerElapsed();
        void DownloadConfig();
        void OnConfigDownloaded(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code & error_code);

    private:
        enum ConfigTypeEnum
        {
            COMMON,
            PRIVATE,
            ServerList
        };

        struct DownloadTask
        {
            DownloadTask(const std::string & download_url, boost::uint8_t config_type)
            {
                download_url_ = download_url;
                config_type_ = config_type;
            }
            std::string download_url_;
            boost::uint8_t config_type_;
        };

        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        ConfigManagerSettings settings_;
        std::list<DownloadTask> download_tasks_;
        std::map<std::string, std::map<std::string, std::string> > config_;

        ConfigUpdateCallback callback_;

    public:
        static const std::string CommonConfigFileName;
        static const std::string PrivateConfigFileName;
        static const std::string AllConfigFileName;
        static const std::string ServerListFileName;
    };
}

#endif  // SUPER_NODE_CONFIG_MANAGER_H
