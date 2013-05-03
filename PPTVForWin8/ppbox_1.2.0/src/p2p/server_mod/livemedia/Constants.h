//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_CONSTANTS_H
#define LIVE_MEDIA_CONSTANTS_H

namespace live_media
{
    class Constants
    {
    public:
        const static string ConfigurationFile;
        const static string P2pSettingsSection;
        const static string ExtraSettingsSection;

        class P2pSettings
        {
        public:
            const static string ListenPort;
            const static string FetchDomain;
            const static string MaxUploadSpeed;
            const static string ResponseSendersCount;
            const static string ReceiverQueueSize;
            const static string OpenChannelOnDemand;
            const static string ChannelObsoleteTimeInSeconds;
            const static string MaxUploadSpeedPerChannel;
            const static string MaxKeepConnectionsPerChannel;

            //cache expiration
            const static string MaxBitmapTimeInSeconds;
            const static string MinBlockDataTimeInSeconds;
            const static string LastBlockVisitTimeInSeconds;

            //session management;
            const static string MaxSessionCount;
            const static string MaxSessionIdleTimeInSeconds;
            const static string MaxCachedTransactions;
            const static string UploadSessionTimeLimitInSesond;

            //download
            const static string MaxConcurrentDownloadTasks;
            const static string DownloadTaskQueueSize;
            const static string DownloadTimeOutInSeconds;
        };

        class ExtraSettings
        {
        public:
            const static string ConfigurationId;
        };

        class ChannelSettings
        {
        public:
            const static string ChannelIdentifier;
            const static string MaxUploadSpeed;
            const static string MaxSessionCount;
            const static string FetchBaseUrl;
            const static string ChannelStepTime;
        };
    };
}

#endif //LIVE_MEDIA_CONSTANTS_H
