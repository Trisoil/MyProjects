//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "OperationsLogging.h"

namespace super_node
{
    const string OperationsLogEvents::ReceivedSignalToReloadConfig = "ReceivedSignalToReloadConfig";
    const string OperationsLogEvents::ReceivedSignalToStopService = "ReceivedSignalToStopService";
    const string OperationsLogEvents::ReloadingConfiguration = "ReloadingConfiguration";

    const string OperationsLogEvents::ErrorParsingCommandLineArguments = "ErrorParsingCommandLineArguments";
    const string OperationsLogEvents::ErrorEnumeratingDiskResources = "ErrorEnumeratingDiskResources";
    const string OperationsLogEvents::ErrorStartingUdpServer = "ErrorStartingUdpServer";
    const string OperationsLogEvents::ErrorDownloadingResourceList = "ErrorDownloadingResourceList";

    const string OperationsLogEvents::WarningNoResourceOnContentServer = "WarningNoResourceOnContentServer";
    const string OperationsLogEvents::WarningNoContentServer = "WarningNoContentServer";
    const string OperationsLogEvents::WarningTooManyConsecutiveDiskReadFailures = "WarningTooManyConsecutiveDiskReadFailures";
    const string OperationsLogEvents::WarningTooManyConsecutiveDiskWriteFailures = "WarningTooManyConsecutiveDiskWriteFailures";
    const string OperationsLogEvents::WarningFailedToQueryDiskAvailableSpace = "WarningFailedToQueryDiskAvailableSpace";

    const string OperationsLogEvents::DownloadResourceSucceeded = "DownloadResourceSucceeded";
    const string OperationsLogEvents::DownloadResourceFailed = "DownloadResourceFailed";
}