//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "SuperNodeService.h"
#include "framework/configure/Config.h"
#include "AsioServiceRunner.h"
#include "UdpServer/SessionManager.h"
#include "UdpServer/UdpServerManager.h"
#include "UdpServer/UdpServerListenHandler.h"
#include "RemoteCache.h"
#include "LocalDiskCache.h"
#include "RemoteCache/ContentServerResourceManager.h"
#include "MemoryCache.h"
#include "ServiceHealthMonitor.h"
#include "ConfigManager/ConfigManager.h"
#include "ConfigManager/Config.h"
#include "BlockDataAllocator.h"


namespace super_node
{
    SuperNodeService::SuperNodeService() : config_exists_(false)
    {
    }

    bool SuperNodeService::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "SuperNodeService::Start()");

        assert(!main_service_runner_);
        assert(!secondary_service_runner_);

        main_service_runner_.reset(new AsioServiceRunner("main IO service"));
        main_io_service_ = main_service_runner_->Start();

        secondary_service_runner_.reset(new AsioServiceRunner("secondary IO service"));
        secondary_io_service_ = secondary_service_runner_->Start();

        if (boost::filesystem::exists(ConfigManager::AllConfigFileName))
        {
            config_exists_ = true;
            config_manager_.reset(new ConfigManager(main_io_service_, boost::bind(&SuperNodeService::ConfigUpdateListener, shared_from_this())));
            config_manager_->Start();
            return StartModules();
        }

        config_manager_.reset(new ConfigManager(main_io_service_, boost::bind(&SuperNodeService::ConfigUpdateListener, shared_from_this())));
        config_manager_->Start();
        return true;
    }

    bool SuperNodeService::StartModules()
    {
        udp_server_config_.LoadConfig();
        remote_cache_config_.LoadConfig();
        session_manager_config_.LoadConfig();
        BlockDataAllocator::Instance().LoadConfig();

        ServiceHealthMonitor::Instance()->Start(main_io_service_);
        content_server_resource_manager_.reset(new ContentServerResourceManager(secondary_io_service_, remote_cache_config_));

        server_list_asker_.reset(
            new ServerListAsker(
                remote_cache_config_.ask_for_server_list_interval_, 
                remote_cache_config_.place_identifier_, 
                secondary_io_service_, 
                boost::bind(
                    &ContentServerResourceManager::DoUpdateContentServers, 
                    content_server_resource_manager_, _1)));

        content_server_resource_manager_->SetServerListAsker(server_list_asker_);
        remote_cache_.reset(new RemoteCache(secondary_io_service_, remote_cache_config_, content_server_resource_manager_));
        remote_cache_->Start();

        local_disk_cache_.reset(new LocalDiskCache(remote_cache_, main_io_service_));
        local_disk_cache_->Start();

        memory_cache_.reset(new MemoryCache(local_disk_cache_, main_io_service_));
        memory_cache_->Start();

        session_manager_.reset(new SessionManager(main_io_service_, session_manager_config_));
        udp_server_listener_.reset(new UdpServerListenHandler(memory_cache_, session_manager_, udp_server_config_.response_senders_count_));
        udp_server_manager_.reset(new UdpServerManager(main_io_service_, udp_server_listener_));
        udp_server_listener_->Start(udp_server_manager_);
        session_manager_->Start();
        if (false == udp_server_manager_->Start(udp_server_config_))
        {
            std::cout<<"Failed to start UDP server. Please check if another instance of SuperNodeService is already running."<<std::endl;
            LOG4CPLUS_ERROR(Loggers::UdpServer(), "Failed to start UDP server. Please check if another instance of SuperNodeService is already running.");
            OperationsLogging::Log(OperationsLogEvents::ErrorStartingUdpServer, Error);
            return false;
        }

        std::cout<<"The service is started."<<std::endl;

        return true;
    }

    void SuperNodeService::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "SuperNodeService::Stop()");

        ServiceHealthMonitor::Instance()->Stop();

        if (udp_server_listener_)
        {
            main_io_service_->post(bind(&UdpServerListenHandler::Stop, udp_server_listener_));
            udp_server_listener_.reset();
        }

        if (udp_server_manager_)
        {
            main_io_service_->post(bind(&UdpServerManager::Stop, udp_server_manager_));
            udp_server_manager_.reset();
        }

        if (local_disk_cache_)
        {
            main_io_service_->post(bind(&LocalDiskCache::Stop, local_disk_cache_));
            local_disk_cache_.reset();
        }

        if (remote_cache_)
        {
            secondary_io_service_->post(bind(&RemoteCache::Stop, remote_cache_));
            remote_cache_.reset();
        }

        if (memory_cache_)
        {
            main_io_service_->post(bind(&MemoryCache::Stop, memory_cache_));
            memory_cache_.reset();
        }

        if (config_manager_)
        {
            main_io_service_->post(bind(&ConfigManager::Stop, config_manager_));
            config_manager_.reset();
        }

        if (session_manager_)
        {
            main_io_service_->post(bind(&SessionManager::Stop, session_manager_));
            session_manager_.reset();
        }

        const size_t TimeToWaitInSeconds = 5;
#ifdef BOOST_WINDOWS_API
        Sleep(TimeToWaitInSeconds*1000);
#else
        sleep(TimeToWaitInSeconds);
#endif

        main_service_runner_->Stop();
        secondary_service_runner_->Stop();

        main_service_runner_.reset();
        secondary_service_runner_.reset();
    }

    void SuperNodeService::ConfigUpdateListener()
    {
        if (config_exists_)
        {
            UpdateConfig();
        }
        else
        {
            StartModules();
            config_exists_ = true;
        }
    }

    void SuperNodeService::UpdateConfig()
    {
        if (udp_server_manager_)
        {
            udp_server_manager_->UpdateConfig();
        }
        if (memory_cache_)
        {
            memory_cache_->UpdateConfig();
        }
        if (local_disk_cache_)
        {
            local_disk_cache_->UpdateConfig();
        }
        if (remote_cache_)
        {
            secondary_io_service_->post(boost::bind(&RemoteCache::UpdateConfig, remote_cache_));
        }
        if (session_manager_)
        {
            session_manager_->UpdateConfig();
        }
    }

    void SuperNodeService::LoadConfig()
    {
        if (!boost::filesystem::exists(ConfigManager::AllConfigFileName))
        {
            std::cout << ConfigManager::AllConfigFileName << " does not exist." << std::endl;
            return;
        }

        if (config_manager_->IsConfigUpdatedAutomatically())
        {
            std::cout << "WARNING: The service will load the new config right now." << std::endl
                << "But the config will be updated automatically later(in 0 to 600 seconds), "
                << "and then the service will use the new config." << std::endl;
        }

        UpdateConfig();
    }
}
