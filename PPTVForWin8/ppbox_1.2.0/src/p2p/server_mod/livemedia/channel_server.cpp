//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"

#include "channel_server.h"
#include "channel_manager.h"
#include "AsioServiceRunner.h"
#include "CacheManager.h"
#include "RemoteCache.h"
#include "Constants.h"
#include "GlobalUploadHandler.h"
#include "RequestHandler.h"
#include <framework/network/Interface.h>
#include <bootstrap/BootstrapPacket.h>
#include "SessionManagement/SessionManager.h"
#include "SessionManagement/ChannelSessionManager.h"
#include <iosfwd>
#include <fstream>
#include <boost/program_options.hpp>

namespace live_media
{
    ChannelServer::ChannelServer() : event_loop_thread_(0), asio_timer_manager_(0), last_reload_config_time_(0)
    , force_exit_(false)
    {
        subpiece_request_relative_to_max_hits_statistics_.reset(new HitsStatistics("subpiece_request_relative_to_max_hits_statistics", -120, 600, 30));
        announce_request_relative_to_max_hits_statistics_.reset(new HitsStatistics("announce_request_relative_to_max_hits_statistics", -120, 600, 30));
        this->server_startup_time_ = time(0);
    }

    ChannelServer::~ChannelServer()
    {
    }

    bool ChannelServer::LoadConfig(bool& config_updated)
    {
        config_updated = false;
        std::ifstream input_file(Constants::ConfigurationFile.c_str());
        if (!input_file.is_open())
        {
            LOG4CPLUS_WARN(Loggers::Service(), "Failed to open configuration file.");
            return false;
        }

        std::ostringstream config_text_stream;
        while(input_file)
        {
            std::string line_text;
            getline(input_file, line_text);
            config_text_stream<<line_text<<std::endl;
        }
        
        // 动态产生 MY_ID
        if (this->server_config_.my_id.is_empty()) {
            this->server_config_.my_id.generate();
        }

        const string config_text = config_text_stream.str();
        
        if (config_text == last_configuration_text_)
        {
            LOG4CPLUS_WARN(Loggers::Service(), "No configuration change detected. Skip parsing");
            return true;
        }

        config_updated = true;

        try
        {
            if (!LoadGlobalConfiguration(config_text))
            {
                return false;
            }
            
            if (!this->LoadChannelsConfig(config_text))
            {
                return false;
            }

            last_configuration_text_ = config_text;
            return true;
        }
        catch (boost::program_options::error & e)
        {
            LOG4CPLUS_INFO(Loggers::Service(), "An error occurred while parsing configuration, error:"<<e.what());
        }

        return false;
    }

    bool ChannelServer::LoadGlobalConfiguration(const string& config_text) 
    {
        std::istringstream config_stream(config_text);

        boost::uint32_t listen_port = server_config::DefaultListenPort;
        std::string fetch_domain;
        size_t global_max_upload_speed = server_config::DefaultGlobalMaxUploadSpeed;
        size_t response_senders_count = server_config::DefaultResponseSendersCount;
        size_t receiver_queue_size = server_config::DefaultReceiverQueueSize;

        size_t open_channel_on_demand = 1;
        size_t channel_obsolete_time_in_seconds = 30 * 60;
        size_t max_upload_speed_per_channel = 1024000000;
        size_t max_keep_connections_per_channel = 100000;

        boost::uint32_t max_keep_bitmap_time_in_seconds = CacheExpirationConfiguration::DefaultBlockCacheObsoleteLimitInSeconds;
        boost::uint32_t min_keep_block_data_in_seconds = CacheExpirationConfiguration::DefaultBlockDataObsoleteLimitInSeconds;
        boost::uint32_t last_block_visit_time_in_seconds = CacheExpirationConfiguration::DefaultLastVisitLimitInSeconds;

       
        boost::uint32_t global_max_keep_connections = SessionManagementConfiguration::DefaultMaxSessionNum;
        boost::uint32_t session_keep_time = SessionManagementConfiguration::DefaultSessionKeepTime;
        boost::uint32_t max_transaction_id_num = SessionManagementConfiguration::DefaultMaxTransactionIdNum;
        boost::uint32_t upload_session_time_limit_in_second = SessionManagementConfiguration::DefaultUploadSessionTimeLimitInSecond;

        boost::uint32_t max_concurrent_download_tasks    = DownloadConfiguration::DefaultMaxConcurrentDownloadTasks;
        boost::uint32_t download_task_queue_size = DownloadConfiguration::DefaultDownloadTaskQueueSize;
        boost::uint32_t download_timeout_in_seconds = DownloadConfiguration::DefaultDownloadTimeOutInSeconds;

        std::string configuration_id;

        namespace po = boost::program_options;
        po::options_description config_desc;
        config_desc.add_options()
            //[p2p]
            (Constants::P2pSettings::ListenPort.c_str(), po::value<boost::uint32_t>()->default_value(listen_port))
            (Constants::P2pSettings::FetchDomain.c_str(), po::value<std::string>()->default_value(fetch_domain))
            (Constants::P2pSettings::MaxUploadSpeed.c_str(), po::value<size_t>()->default_value(global_max_upload_speed))
            (Constants::P2pSettings::OpenChannelOnDemand.c_str(), po::value<size_t>()->default_value(open_channel_on_demand))
            (Constants::P2pSettings::ChannelObsoleteTimeInSeconds.c_str(), po::value<size_t>()->default_value(channel_obsolete_time_in_seconds))
            (Constants::P2pSettings::MaxUploadSpeedPerChannel.c_str(), po::value<size_t>()->default_value(max_upload_speed_per_channel))
            (Constants::P2pSettings::MaxKeepConnectionsPerChannel.c_str(), po::value<size_t>()->default_value(max_keep_connections_per_channel))
            (Constants::P2pSettings::ResponseSendersCount.c_str(), po::value<size_t>()->default_value(response_senders_count))
            (Constants::P2pSettings::ReceiverQueueSize.c_str(), po::value<size_t>()->default_value(receiver_queue_size))
            (Constants::P2pSettings::MaxBitmapTimeInSeconds.c_str(), po::value<boost::uint32_t>()->default_value(max_keep_bitmap_time_in_seconds))
            (Constants::P2pSettings::MinBlockDataTimeInSeconds.c_str(), po::value<boost::uint32_t>()->default_value(min_keep_block_data_in_seconds))
            (Constants::P2pSettings::LastBlockVisitTimeInSeconds.c_str(), po::value<boost::uint32_t>()->default_value(last_block_visit_time_in_seconds))
            (Constants::P2pSettings::MaxSessionCount.c_str(), po::value<boost::uint32_t>()->default_value(global_max_keep_connections))
            (Constants::P2pSettings::MaxSessionIdleTimeInSeconds.c_str(), po::value<boost::uint32_t>()->default_value(session_keep_time))
            (Constants::P2pSettings::MaxCachedTransactions.c_str(), po::value<boost::uint32_t>()->default_value(max_transaction_id_num))
            (Constants::P2pSettings::UploadSessionTimeLimitInSesond.c_str(), po::value<boost::uint32_t>()->default_value(upload_session_time_limit_in_second))
            (Constants::P2pSettings::MaxConcurrentDownloadTasks.c_str(), po::value<boost::uint32_t>()->default_value(max_concurrent_download_tasks))
            (Constants::P2pSettings::DownloadTaskQueueSize.c_str(), po::value<boost::uint32_t>()->default_value(download_task_queue_size))
            (Constants::P2pSettings::DownloadTimeOutInSeconds.c_str(), po::value<boost::uint32_t>()->default_value(download_timeout_in_seconds))
            //[extra]
            (Constants::ExtraSettings::ConfigurationId.c_str(), po::value<std::string>()->default_value(configuration_id))
            ;

        po::variables_map vm;
        po::store(po::parse_config_file(config_stream, config_desc, true), vm);
        po::notify(vm);

        listen_port = vm[Constants::P2pSettings::ListenPort].as<boost::uint32_t>();
        fetch_domain = vm[Constants::P2pSettings::FetchDomain].as<std::string>();
        global_max_upload_speed = vm[Constants::P2pSettings::MaxUploadSpeed].as<size_t>();
        open_channel_on_demand = vm[Constants::P2pSettings::OpenChannelOnDemand].as<size_t>();
        channel_obsolete_time_in_seconds = vm[Constants::P2pSettings::ChannelObsoleteTimeInSeconds].as<size_t>();
        max_upload_speed_per_channel = vm[Constants::P2pSettings::MaxUploadSpeedPerChannel].as<size_t>();
        max_keep_connections_per_channel = vm[Constants::P2pSettings::MaxKeepConnectionsPerChannel].as<size_t>();
        response_senders_count = vm[Constants::P2pSettings::ResponseSendersCount].as<size_t>();
        receiver_queue_size = vm[Constants::P2pSettings::ReceiverQueueSize].as<size_t>();
        
        max_keep_bitmap_time_in_seconds = vm[Constants::P2pSettings::MaxBitmapTimeInSeconds].as<boost::uint32_t>();
        min_keep_block_data_in_seconds = vm[Constants::P2pSettings::MinBlockDataTimeInSeconds].as<boost::uint32_t>();
        last_block_visit_time_in_seconds = vm[Constants::P2pSettings::LastBlockVisitTimeInSeconds].as<boost::uint32_t>();

        global_max_keep_connections = vm[Constants::P2pSettings::MaxSessionCount].as<boost::uint32_t>();
        session_keep_time = vm[Constants::P2pSettings::MaxSessionIdleTimeInSeconds].as<boost::uint32_t>();
        max_transaction_id_num = vm[Constants::P2pSettings::MaxCachedTransactions].as<boost::uint32_t>();
        upload_session_time_limit_in_second = vm[Constants::P2pSettings::UploadSessionTimeLimitInSesond].as<boost::uint32_t>();

        configuration_id = vm[Constants::ExtraSettings::ConfigurationId].as<std::string>();

        this->server_config_.config_id = configuration_id;
        this->server_config_.p2p_listen_port = listen_port;
        this->server_config_.fetch_domain = fetch_domain.empty() ?  std::string("doghole") : fetch_domain;
        this->server_config_.global_max_upload_speed = global_max_upload_speed;
        this->server_config_.open_channel_on_demand = open_channel_on_demand != 0;
        this->server_config_.channel_obsolete_time_in_seconds = channel_obsolete_time_in_seconds;
        this->server_config_.max_upload_speed_per_channel = max_upload_speed_per_channel;
        this->server_config_.max_keep_connections_per_channel = max_keep_connections_per_channel;
        this->server_config_.response_senders_count = response_senders_count;
        this->server_config_.receiver_queue_size = receiver_queue_size;

        this->server_config_.cache_expiration = CacheExpirationConfiguration(
            min_keep_block_data_in_seconds,
            max_keep_bitmap_time_in_seconds,
            last_block_visit_time_in_seconds);

        this->server_config_.session_management = SessionManagementConfiguration(
            global_max_keep_connections, 
            session_keep_time, 
            max_transaction_id_num,
            upload_session_time_limit_in_second);

        this->server_config_.download = DownloadConfiguration(
            max_concurrent_download_tasks, 
            download_task_queue_size, 
            download_timeout_in_seconds);

        return true;
    }

    bool ChannelServer::LoadChannelsConfig(const std::string& config_text)
    {
        this->server_config_.channels.clear();

        const size_t MaxChannels = 300;
        for(size_t i = 1; i <= MaxChannels; ++i)
        {
            std::stringstream oss;
            oss << "live" << i <<".";
            const std::string prefix = oss.str();

            std::string channel_identifier, fetch_base_url;
            boost::uint32_t channel_step_time = 5;
            boost::uint32_t max_session_count = 0;
            boost::uint32_t max_upload_speed = 0;

            namespace po = boost::program_options;
            po::options_description config_desc;
            config_desc.add_options()
                //[liveN]
                ((prefix + Constants::ChannelSettings::ChannelIdentifier).c_str(), po::value<std::string>()->default_value(channel_identifier))
                ((prefix + Constants::ChannelSettings::ChannelStepTime).c_str(), po::value<boost::uint32_t>()->default_value(channel_step_time))
                ((prefix + Constants::ChannelSettings::FetchBaseUrl).c_str(), po::value<std::string>()->default_value(fetch_base_url))
                ((prefix + Constants::ChannelSettings::MaxSessionCount).c_str(), po::value<boost::uint32_t>()->default_value(max_session_count))
                ((prefix + Constants::ChannelSettings::MaxUploadSpeed).c_str(), po::value<boost::uint32_t>()->default_value(max_upload_speed))
                ;

            po::variables_map vm;
            bool parse_succeeded(false);
            try
            {
                std::istringstream config_stream(config_text);
                po::store(po::parse_config_file(config_stream, config_desc, true), vm);
                po::notify(vm);
                parse_succeeded = true;
            }
            catch (boost::program_options::error & e)
            {
                LOG4CPLUS_INFO(Loggers::Service(), "Failed to parse configuration for channel #"<<i<<", reason:"<<e.what());
            }

            if (!parse_succeeded)
            {
                return false;
            }

            channel_identifier = vm[prefix + Constants::ChannelSettings::ChannelIdentifier].as<std::string>();
            channel_step_time = vm[prefix + Constants::ChannelSettings::ChannelStepTime].as<boost::uint32_t>();
            fetch_base_url = vm[prefix + Constants::ChannelSettings::FetchBaseUrl].as<std::string>();
            max_session_count = vm[prefix + Constants::ChannelSettings::MaxSessionCount].as<boost::uint32_t>();
            max_upload_speed = vm[prefix + Constants::ChannelSettings::MaxUploadSpeed].as<boost::uint32_t>();

            ChannelConfiguration c;
            c.guid.from_string(channel_identifier);
            c.max_upload_speed = max_upload_speed;
            c.max_upload_in_a_while = max_session_count;
            c.max_keep_connections = max_session_count;
            c.channel_fetch_base_url = fetch_base_url;
            c.channel_step_time = channel_step_time;

            if(!c.guid.is_empty())
            {
                this->server_config_.channels.push_back(c);
            }
        }

        return true;
    }

    bool ChannelServer::InitalizeMyselfInfo()
    {
    #ifndef __APPLE__
        std::vector<framework::network::Interface> interfaces;
        ::framework::network::enum_interface(interfaces);

        unsigned long ip = 0;
        for(size_t i = 0; i < interfaces.size(); i++) {
            unsigned long tmp_ip = inet_addr(interfaces[i].addr.to_string().c_str());
            if(tmp_ip == inet_addr("127.0.0.1") || tmp_ip == 0)
                continue;
            ip = ntohl(tmp_ip);
            break;
        }
    #else   
        // 
        boost::asio::ip::address local_addr;
        boost::asio::ip::tcp::resolver resolver(*this->io_service_);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end; // End marker.
        while (iter != end)
        {       
            boost::asio::ip::tcp::endpoint ep = *iter++;
            if(ep.address().is_v4()) {
                local_addr = ep.address();
                break;
            }
        }

        unsigned long ip = inet_addr(local_addr.to_string().c_str());
    #endif

        this->myself_info_.my_info_.IP = ip;
        this->myself_info_.my_info_.UdpPort = this->server_config_.p2p_listen_port;
        this->myself_info_.my_info_.PeerVersion = protocol::PEER_VERSION;
        this->myself_info_.my_info_.DetectIP = ip;
        this->myself_info_.my_info_.DetectUdpPort = this->server_config_.p2p_listen_port;
        this->myself_info_.my_info_.StunIP = 0;
        this->myself_info_.my_info_.StunUdpPort = 0;

        this->myself_info_.my_download_info_.IsDownloading = 0;
        this->myself_info_.my_download_info_.OnlineTime = 0;
        this->myself_info_.my_download_info_.AvgDownload = 0;
        this->myself_info_.my_download_info_.NowDownload = 0;
        this->myself_info_.my_download_info_.AvgUpload = 0;
        this->myself_info_.my_download_info_.NowUpload = 0;

        return true;
    }

    void ChannelServer::RunService()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "channel_server::RunService()");

        if (!StartService())
        {
            this->force_exit_ = true;
            this->ShutDown();
            return;
        }

        std::cout<<"server started successfully"<<std::endl;

        this->EventLoop();
    }

    bool ChannelServer::StartService()
    {
        try 
        {
            bool config_updated(false);
            if (!this->LoadConfig(config_updated))
            {
                LOG4CPLUS_WARN(Loggers::Service(), "Failed to load configuration, and the service could not be started.");
                std::cout<<"Could not start service due to failure to load configuration."<<std::endl;
                return false;
            }
        }
        catch (...) 
        {
            LOG4CPLUS_ERROR(Loggers::Service(), "An unknown error occurred while trying to load configuration.");
            std::cout<<"Could not start service. An unknown error occurred while trying to load configuration."<<std::endl;
            return false;
        }

        if(false == this->InitalizeMyselfInfo()) 
        {
            printf("out of memory\n");
            return false;
        }

        assert(!this->asio_timer_manager_);
        this->asio_timer_manager_ = new framework::timer::AsioTimerManager(*this->io_service_, boost::posix_time::seconds(1));
        this->asio_timer_manager_->start();

        this->speed_statistics_.reset(new SpeedStatistics(io_service_, "server_speed_statistics"));
        this->speed_statistics_->StartTimer();

        // 启动P2P的UDP服务
        this->udp_server_.reset(new protocol::UdpServer(*this->io_service_, shared_from_this()));

        // 监听端口
        if(!this->udp_server_->Listen(this->server_config_.p2p_listen_port)) 
        {
            std::cout<<"Failed to listen on port:"<<this->server_config_.p2p_listen_port<<std::endl;
            LOG4CPLUS_ERROR(Loggers::Operations(), "FailedToListenOnConfiguredPort|"<<this->server_config_.p2p_listen_port);
            return false;
        }

        {
            send_packet_statistics_.reset(new PacketStatistics("packet_send", io_service_));
            receive_packet_statistics_.reset(new PacketStatistics("packet_receive", io_service_));
            receive_packet_statistics_->AddCounter(RequestTypes::Connect);
            receive_packet_statistics_->AddCounter(RequestTypes::RequestAnnounce);
            receive_packet_statistics_->AddCounter(RequestTypes::Announce);
            receive_packet_statistics_->AddCounter(RequestTypes::RequestSubPiece);
            receive_packet_statistics_->AddCounter(RequestTypes::CloseSession);
            send_packet_statistics_->AddCounter(ResponseTypes::Connect);
            send_packet_statistics_->AddCounter(ResponseTypes::Announce);
            send_packet_statistics_->AddCounter(ResponseTypes::Subpiece);

            send_packet_statistics_->StartTimer();
            receive_packet_statistics_->StartTimer();

            protocol::register_live_peer_packet(*this->udp_server_);
            protocol::register_peer_packet(*this->udp_server_);
            //protocol::register_bootstrap_packet(*this->udp_server_);
            //protocol::register_index_packet(*this->udp_server_);
            protocol::register_tracker_packet(*this->udp_server_);

            this->udp_server_->Recv(server_config_.receiver_queue_size);
        }

        {
            // 创建 tracker manager
            TrackerManager::asio_timer_manager_ = this->asio_timer_manager_;
            TrackerClient::peer_guid_ = this->server_config_.my_id;
            TrackerClient::udp_server_ = this->udp_server_;
            this->tracker_manager_ = TrackerManager::Inst();
            if (0 == this->tracker_manager_) {
                printf("out of memory\n");
                return false;
            }

            this->tracker_manager_->Start(".");
        }

        {
            // 创建 IndexManager
            p2sp::IndexManager::asio_timer_manager_ = this->asio_timer_manager_;
            p2sp::IndexManager::udp_server_ = this->udp_server_.get();
            p2sp::IndexManager::io_service_ = this->io_service_.get();
            p2sp::IndexManager::peer_guid_ = this->server_config_.my_id;
            this->index_manager_ = p2sp::IndexManager::CreateInst(*this->io_service_);
            if (0 == this->index_manager_) {
                printf("out of memory\n");
                return false;
            }

            this->index_manager_->Start("ppvabs.pplive.com", 6400);
        }

        {
            this->channel_manager_.reset(new ChannelManager(this));
            remote_cache_runner_.reset(new AsioServiceRunner("remote_cache"));
            boost::shared_ptr<boost::asio::io_service> downloader_io_service = remote_cache_runner_->Start();
            remote_cache_.reset(new RemoteCache(downloader_io_service, channel_manager_, server_config_.download));
            remote_cache_->Start();

            // 初始化 channel 列表
            if(!this->InitalizeChannels()) {
                return false;
            }

            request_handler_.reset(new RequestHandler(channel_manager_, session_manager_));

            global_upload_handler_.reset(new GlobalUploadHandler(shared_from_this()));
            global_upload_handler_->Start();

            TrackerClient::client_resource_ids_.clear();

            std::vector<IChannelPointer> chns = this->channel_manager_->GetChannels();
            for(size_t i = 0; i < chns.size(); i++) 
            {
                TrackerClient::client_resource_ids_.insert(chns[i]->GetChannelId());
            }
        }

        boost::shared_ptr<ExpirationStrategy> strategy(new ExpirationStrategy());
        strategy->SetStrategy(server_config_.cache_expiration);
        this->cache_manager_.reset(new CacheManager(channel_manager_, io_service_));
        cache_manager_->SetCacheExpirationStrategy(strategy);
        cache_manager_->Start();

        return true;
    }

    void ChannelServer::OnTimer()
    {
        if (global_upload_handler_)
        {
            global_upload_handler_->OnTimer();
        }
    }

    void ChannelServer::Run()
    {
        this->io_service_.reset(new boost::asio::io_service());

        // 启动后台的推线程
        this->event_loop_thread_ = new boost::thread(boost::bind(&ChannelServer::RunService, this));

        ServiceStatusDetector::Instance().AddService("main", io_service_);

        // 等待用户输入交互数据
        interact_with_input();
    }

    void ChannelServer::ShutDown()
    {
        if (this->io_service_) {
            this->io_service_->stop();
        }

        if (request_handler_)
        {
            request_handler_->Stop();
            request_handler_.reset();
        }

        if (this->channel_manager_) {
            this->channel_manager_->Uninitalize();
        }

        if (this->tracker_manager_) {
            this->tracker_manager_->Stop();
            this->tracker_manager_.reset((TrackerManager *)(0));
        }

        if (this->udp_server_) {
            this->udp_server_->Close();
            this->udp_server_.reset();
        }
   
        if (remote_cache_)
        {
            remote_cache_->Stop();
            remote_cache_.reset();
        }

        if (remote_cache_runner_)
        {
            remote_cache_runner_->Stop();
            remote_cache_runner_.reset();
        }

        global_upload_handler_.reset();

        this->speed_statistics_.reset();
        this->channel_manager_.reset();
        delete this->asio_timer_manager_;
        this->io_service_.reset();
        delete this->event_loop_thread_;

        if (this->force_exit_) {
            exit(-1);
        }
    }

    int ChannelServer::EventLoop()
    {
        if (!this->io_service_) {
            return -1;
        }

        this->io_service_->run();

        return 0;
    }

    bool ChannelServer::InitalizeChannels()
    {
        boost::shared_ptr<IChannelSessionManager> channel_session_manager(new ChannelSessionManager(channel_manager_));
        session_manager_.reset(new SessionManager(io_service(), server_config_.session_management, channel_session_manager));
        session_manager_->Start();

        if (!this->channel_manager_->Initalize()) {
            return false;
        }

        return true;
    }

    const myself_info & ChannelServer::GetMyselfInfo()
    {
        // 更新数据
        this->myself_info_.my_download_info_.OnlineTime = time(0) - this->server_startup_time_;

        return this->myself_info_;
    }

    void ChannelServer::OnUdpRecv(protocol::Packet const & packet)
    {
        switch(packet.PacketAction)
        {
        case protocol::QueryRidByUrlPacket::Action:
        case protocol::QueryRidByContentPacket::Action:
        case protocol::QueryHttpServerByRidPacket::Action:
        case protocol::QueryLiveTrackerListPacket::Action:
        case protocol::AddRidUrlPacket::Action:
        case protocol::QueryStunServerListPacket::Action:
        case protocol::QueryIndexServerListPacket::Action:
        case protocol::QueryKeyWordListPacket::Action:
        case protocol::QueryUploadPicProbabilityPacket::Action:
            //        case protocol::QueryPushTaskPacket:
        case protocol::QueryNotifyListPacket::Action:
            // 抛向 IndexManager 处理
            this->index_manager_->OnUdpRecv((const protocol::ServerPacket &)packet);
            return;
        case protocol::ListPacket::Action:
        case protocol::ReportPacket::Action:
        case protocol::QueryPeerCountPacket::Action:
            // 查看是否是 tracker 的包
            this->tracker_manager_->OnUdpRecv((const protocol::ServerPacket &) packet);
            return;

        case protocol::CloseSessionPacket::Action:
            request_handler_->HandleCloseSession(packet.end_point);
            return;

        case protocol::PeerInfoPacket::Action:
            request_handler_->HandlePeerHeartBeat(packet.end_point, packet.transaction_id_, protocol::PeerInfoPacket::Action);
            return;
        };
      
        // 换成 common packet
        protocol::CommonPeerPacket & common_packet = (protocol::CommonPeerPacket &)packet;
        if (common_packet.resource_id_.is_empty()) {
            return;
        }

        switch (packet.PacketAction) {
            case protocol::ConnectPacket::Action:
                //
                {
                    boost::shared_ptr<protocol::ConnectPacket> copyed_packet(new protocol::ConnectPacket((protocol::ConnectPacket&)packet));
                    request_handler_->HandleConnectRequest(copyed_packet);
                }
                break;
            case protocol::LiveRequestAnnouncePacket::Action:
                {
                    boost::shared_ptr<protocol::LiveRequestAnnouncePacket> copyed_packet(new protocol::LiveRequestAnnouncePacket((protocol::LiveRequestAnnouncePacket&)packet));
                    request_handler_->HandleAnnounceRequest(copyed_packet);
                }

                break;
            case protocol::LiveAnnouncePacket::Action:
                {
                    request_handler_->HandlePeerHeartBeat(packet.end_point, packet.transaction_id_, protocol::LiveAnnouncePacket::Action);
                    break;
                }
            case protocol::LiveRequestSubPiecePacket::Action:
                {
                    boost::shared_ptr<protocol::LiveRequestSubPiecePacket> copyed_packet(new protocol::LiveRequestSubPiecePacket((protocol::LiveRequestSubPiecePacket&)packet));
                    receive_packet_statistics_->AddRequestSubpieceNumData(copyed_packet->request_sub_piece_count_);
                    request_handler_->HandleSubPieceRequest(copyed_packet);
                    break;
                }
            case protocol::LiveSubPiecePacket::Action:
                {
                    request_handler_->HandlePeerHeartBeat(packet.end_point, packet.transaction_id_, protocol::LiveSubPiecePacket::Action);
                    LOG4CPLUS_WARN(Loggers::Operations(), "Receive LiveSubpiecePacket!");
                    break;
                }
            default:
                // unknown packet
                LOG4CPLUS_INFO(Loggers::Service(), "Received unknown packet type:"<<packet.PacketAction);
                return;
        }
    }

    void ChannelServer::ReloadServerConfig()
    {
        // 一段时间之后重新载入配置
        if(time(0) - this->last_reload_config_time_ < 10) {
            return;
        }

        this->last_reload_config_time_ = time(0);

        // 先保存原本的配置
        server_config old_config = this->server_config_;

        bool config_updated(false);
        if (!this->LoadConfig(config_updated))
        {
            LOG4CPLUS_WARN(Loggers::Operations(), "IgnoredNewConfigDueToLoadFailure");
            LOG4CPLUS_WARN(Loggers::Service(), "Failed to load latest configuration, and will fall back to use previous configuration settings.");
            this->server_config_ = old_config;
            return;
        }

        if (!config_updated)
        {
            return;
        }

        LOG4CPLUS_INFO(Loggers::Service(), "Applying new configuration settings.");

        this->global_upload_handler_->SetMaxUploadSpeed(server_config_.global_max_upload_speed);
        session_manager_->UpdateConfiguration(server_config_.session_management);
        boost::shared_ptr<ExpirationStrategy> expiration_strategy(new ExpirationStrategy());
        expiration_strategy->SetStrategy(server_config_.cache_expiration);
        cache_manager_->SetCacheExpirationStrategy(expiration_strategy);

        if (server_config_.open_channel_on_demand)
            return;

        // 比较 channels 的变化
        for (size_t i = 0; i < old_config.channels.size(); ++i) 
        {
            // 
            ChannelConfiguration & old_c = old_config.channels[i];
            bool found = false;
            for (size_t j = 0 ; j < this->server_config_.channels.size() ; j++) {
                if (old_c.guid == this->server_config_.channels[j].guid) {
                    // 找到了
                    found = true;
                    ChannelPointer cha = this->channel_manager_->GetChannel(old_c.guid);
                    if (!cha) {
                        // 
                        continue;
                    }

                    // 应用新的配置到 channel 上面
                    cha->SetChannelConfig(this->server_config_.channels[j]);
                    break;
                }
            }

            if (!found) {
                // 没有找到,这个频道没有了
                this->channel_manager_->RemoveChannel(old_c.guid);
            }
        }

        // 查找新添加的 channel
        for (size_t i = 0 ; i < this->server_config_.channels.size() ; i++) {
            // 
            ChannelConfiguration & new_c = this->server_config_.channels[i];
            ChannelPointer cha = this->channel_manager_->GetChannel(new_c.guid);
            if (!cha) {
                // 新增的频道
                this->channel_manager_->AddChannel(new_c);
            }
        }
    }

    static void print_help_msg()
    {
        std::cout	<< "" << std::endl
            << "Command:" << std::endl
            << "exit" << std::endl
            << "list_bs" << std::endl
            << "list_trackers" << std::endl
            << "help" << std::endl
            << "version" << std::endl;
    }

    void ChannelServer::interact_with_input()
    {
        while (true) 
        {
            std::string command;
            std::cin >> command;

            if (!io_service_)
            {
                break;
            }

            if (command == "exit") 
            {
                std::cout<<"Waiting for the main IO service thread to exit..."<<std::endl;

                this->io_service_->post(boost::bind(&ChannelServer::ShutDown, shared_from_this()));

                const size_t MaxSecondsToWait = 10;
                this->event_loop_thread_->timed_join(posix_time::seconds(MaxSecondsToWait));
                break;
            }

            if (command == "list_trackers") 
            {
                this->io_service_->post(boost::bind(&ChannelServer::list_trackers, shared_from_this()));
                continue;
            }
            
            if (command == "list_bs") 
            {
                this->io_service_->post(boost::bind(&ChannelServer::list_bs, shared_from_this()));
                continue;
            }
            
            if (command == "version") 
            {
                printf("version: %s\n", LIVE_MEDIA_SERVER_VERSION);
                continue;
            }
            
            print_help_msg();
        }
    }

    void ChannelServer::list_trackers()
    {
        this->tracker_manager_->list_trackers();
    }

    void ChannelServer::list_bs()
    {
        std::cout << "current bs server addr: " << this->index_manager_->server_list_endpoint_ << std::endl;
    }

    void ChannelServer::AddSendPacketStatistics(boost::uint8_t action, const channel_id& channel_identifier, size_t packet_size)
    {
        io_service_->post(bind(&ChannelServer::HandleAddSendPacketStatistics, shared_from_this(), action, channel_identifier, packet_size));
    }

    void ChannelServer::HandleAddSendPacketStatistics(boost::uint8_t action, const channel_id& channel_identifier, size_t packet_size)
    {
        //Performance Note:
        //每次发包都post到主io_service运行线程来统计的逻辑，是目前主io_service运行线程中相对比较消耗CPU (涉及到statistics内部map以及channels的lookup)的。
        //如果需要进一步优化主处理线程的性能, 可考虑在sender thread pool中各自统计到一定量之后(比如每1000个请求或每10ms一次)
        //再post到主io_service运行线程来汇集统计
        switch(action)
        {
        case protocol::ConnectPacket::Action:
            ++(send_packet_statistics_)->GetStatistics(ResponseTypes::Connect);
            break;
        case protocol::LiveAnnouncePacket::Action:
            ++(send_packet_statistics_)->GetStatistics(ResponseTypes::Announce);
            break;
        case protocol::LiveSubPiecePacket::Action:
            ++(send_packet_statistics_)->GetStatistics(ResponseTypes::Subpiece);
            break;
        }

        speed_statistics_->AddData(packet_size);
        ChannelPointer target_channel = channel_manager_->GetChannel(channel_identifier);
        if (target_channel)
        {
            target_channel->speed_statistics_->AddData(packet_size);
        }
    }
}
