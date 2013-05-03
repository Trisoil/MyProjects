//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef CHANNEL_SERVER_H
#define CHANNEL_SERVER_H

#include <bootstrap/BootstrapPacket.h>
#include <stun/IndexPacket.h>

#include <tracker/TrackerClient.h>
#include <tracker/TrackerGroup.h>
#include <tracker/TrackerManager.h>

#include "bootstrap/IndexManager.h"

#include "channel.h"
#include "packet_statistics.h"
#include "MemoryCache.h"
#include "CacheManager.h"
#include "Configuration.h"

namespace live_media
{
    class Channel;
    class ChannelManager;
    class ChannelCache;

    class IRemoteCache;
    class AsioServiceRunner;
    class GlobalUploadHandler;
    class RequestHandler;

    struct myself_info
    {
        protocol::CandidatePeerInfo my_info_;
        protocol::PEER_DOWNLOAD_INFO my_download_info_;
    };

    class ChannelServer 
        : public protocol::IUdpServerListener , 
        public boost::enable_shared_from_this<ChannelServer>
    {
        // 
    public:
        ChannelServer();
        ~ChannelServer();

        void Run();
        void ShutDown();

        server_config & GetServerConfig()
        {
            return this->server_config_;
        }

        boost::shared_ptr<boost::asio::io_service> io_service()
        {
            return this->io_service_;
        }

        boost::shared_ptr<IRemoteCache> GetRemoteCache() const
        {            
            return remote_cache_;
        }        

        const peer_id & GetMyId()
        {
            return this->server_config_.my_id;
        }

        const myself_info & GetMyselfInfo();

        boost::shared_ptr<boost::asio::io_service> GetIoService()
        {
            return io_service_;
        }

        boost::shared_ptr<SpeedStatistics> speed_statistics_;

        boost::shared_ptr<HitsStatistics> subpiece_request_relative_to_max_hits_statistics_;
        boost::shared_ptr<HitsStatistics> announce_request_relative_to_max_hits_statistics_;

        void ReloadServerConfig();

        template<typename T>
        void send_packet(const T & packet, boost::uint8_t packet_action)
        {
            AddSendPacketStatistics(packet_action, packet.resource_id_, packet.length());
            this->udp_server_->send_packet(packet, packet.protocol_version_);
        }

        boost::shared_ptr<SessionManager> GetSessionManager() const
        {
            return session_manager_;
        }

        void OnTimer();

    private:
        int EventLoop();

        void interact_with_input();

        virtual void OnUdpRecv( protocol::Packet const & packet );

        void RunService();

    private:

        bool StartService();

        bool LoadConfig(bool& config_updated);

        bool LoadGlobalConfiguration(const string& config_text);

        bool LoadChannelsConfig(const std::string& config_text);

        bool InitalizeChannels();

        bool InitalizeMyselfInfo();

        void AddSendPacketStatistics(boost::uint8_t action, const channel_id& channel_identifier, size_t packet_size);

        void HandleAddSendPacketStatistics(boost::uint8_t action, const channel_id& channel_identifier, size_t packet_size);

        // 
        server_config server_config_;

        // ios
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::thread * event_loop_thread_;
        boost::shared_ptr<ChannelManager> channel_manager_;
        myself_info myself_info_;
        boost::shared_ptr<RequestHandler> request_handler_;
        boost::shared_ptr<SessionManager> session_manager_;
        boost::shared_ptr<live_media::CacheManager> cache_manager_;
        boost::shared_ptr<protocol::UdpServer> udp_server_;
        boost::shared_ptr<AsioServiceRunner> remote_cache_runner_;
        boost::shared_ptr<IRemoteCache> remote_cache_;
        boost::shared_ptr<GlobalUploadHandler> global_upload_handler_;
        std::string last_configuration_text_;
public:
        TrackerManager::p tracker_manager_;
        p2sp::IndexManager::p index_manager_;

        framework::timer::AsioTimerManager * asio_timer_manager_;

        time_t last_reload_config_time_;
        boost::shared_ptr<PacketStatistics> receive_packet_statistics_;
        boost::shared_ptr<PacketStatistics> send_packet_statistics_;

    private:
        void list_trackers();
        void list_bs();

    public:
        time_t server_startup_time_;

        bool force_exit_;
    };
}

#endif //CHANNEL_SERVER_H
