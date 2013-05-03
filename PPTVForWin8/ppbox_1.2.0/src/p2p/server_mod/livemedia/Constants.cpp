//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "Constants.h"

namespace live_media
{
    const string Constants::ConfigurationFile = "live_media_server.conf";
    const string Constants::P2pSettingsSection = "p2p";
    const string Constants::ExtraSettingsSection = "extra";

    const string Constants::P2pSettings::ListenPort = Constants::P2pSettingsSection + "." + "listen_port";
    const string Constants::P2pSettings::FetchDomain = Constants::P2pSettingsSection + "." + "fetch_domain";
    const string Constants::P2pSettings::MaxUploadSpeed = Constants::P2pSettingsSection + "." + "global_max_upload_speed";
    const string Constants::P2pSettings::ResponseSendersCount = Constants::P2pSettingsSection + "." + "response_senders_count";
    const string Constants::P2pSettings::ReceiverQueueSize = Constants::P2pSettingsSection + "." + "receiver_queue_size";
    const string Constants::P2pSettings::OpenChannelOnDemand = Constants::P2pSettingsSection + "." + "open_channel_on_demand";
    const string Constants::P2pSettings::ChannelObsoleteTimeInSeconds = Constants::P2pSettingsSection + "." + "channel_obsolete_time_in_seconds";
    const string Constants::P2pSettings::MaxUploadSpeedPerChannel = Constants::P2pSettingsSection + "." + "max_upload_speed_per_channel";
    const string Constants::P2pSettings::MaxKeepConnectionsPerChannel = Constants::P2pSettingsSection + "." + "max_keep_connections_per_channel";

    //cache expiration
    const string Constants::P2pSettings::MaxBitmapTimeInSeconds = Constants::P2pSettingsSection + "." + "max_keep_bit_map_time_in_second";
    const string Constants::P2pSettings::MinBlockDataTimeInSeconds = Constants::P2pSettingsSection + "." + "min_block_data_time_in_second";
    const string Constants::P2pSettings::LastBlockVisitTimeInSeconds = Constants::P2pSettingsSection + "." + "last_block_visit_time_in_second";

    //session management
    const string Constants::P2pSettings::MaxSessionCount = Constants::P2pSettingsSection + "." + "global_max_keep_connections";
    const string Constants::P2pSettings::MaxSessionIdleTimeInSeconds = Constants::P2pSettingsSection + "." + "session_keep_time";
    const string Constants::P2pSettings::MaxCachedTransactions = Constants::P2pSettingsSection + "." + "max_transaction_id_num";
    const string Constants::P2pSettings::UploadSessionTimeLimitInSesond = Constants::P2pSettingsSection + "." + "upload_session_time_limit_in_second";  

    //download
    const string Constants::P2pSettings::MaxConcurrentDownloadTasks = Constants::P2pSettingsSection + "." + "max_concurrent_download_tasks";
    const string Constants::P2pSettings::DownloadTaskQueueSize = Constants::P2pSettingsSection + "." + "download_task_queue_size";
    const string Constants::P2pSettings::DownloadTimeOutInSeconds = Constants::P2pSettingsSection + "." + "download_timeout_in_second";

    const string Constants::ExtraSettings::ConfigurationId = Constants::ExtraSettingsSection + "." + "config_id";

    const string Constants::ChannelSettings::ChannelIdentifier = "channel_guid";
    const string Constants::ChannelSettings::MaxUploadSpeed = "max_upload_speed";
    const string Constants::ChannelSettings::MaxSessionCount = "max_keep_connections";
    const string Constants::ChannelSettings::FetchBaseUrl = "fetch_base_url";
    const string Constants::ChannelSettings::ChannelStepTime = "channel_step_time";
}
