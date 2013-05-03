//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_OPERATIONS_LOGGING_H
#define SUPER_NODE_OPERATIONS_LOGGING_H

namespace super_node
{
    class OperationsLogEvents
    {
    public:
        const static string ReceivedSignalToReloadConfig;
        const static string ReceivedSignalToStopService;
        const static string ReloadingConfiguration;
        
        const static string ErrorParsingCommandLineArguments;
        const static string ErrorEnumeratingDiskResources;
        const static string ErrorStartingUdpServer;
        const static string ErrorDownloadingResourceList;

        const static string WarningNoResourceOnContentServer;
        const static string WarningNoContentServer;
        const static string WarningTooManyConsecutiveDiskReadFailures;
        const static string WarningTooManyConsecutiveDiskWriteFailures;
        const static string WarningFailedToQueryDiskAvailableSpace;

        const static string DownloadResourceSucceeded;
        const static string DownloadResourceFailed;
    };

    enum OperationsLogLevel
    {
        Informational,
        Warning,
        Error
    };

    class OperationsLogging
    {
    public:
        static void Log(const string& log_event, const OperationsLogLevel log_level = Informational)
        {
            if (log_level == Informational)
            {
                LOG4CPLUS_INFO(Loggers::Operations(), log_event);
                return;
            }

            if (log_level == Warning)
            {
                LOG4CPLUS_WARN(Loggers::Operations(), log_event);
                return;
            }

            assert(log_level == Error);
            
            LOG4CPLUS_ERROR(Loggers::Operations(), log_event);
        }

        static void Log(const string& log_event, const string& param, const OperationsLogLevel log_level = Informational)
        {
            if (log_level == Informational)
            {
                LOG4CPLUS_INFO(Loggers::Operations(), log_event << " " << param);
                return;
            }

            if (log_level == Warning)
            {
                LOG4CPLUS_WARN(Loggers::Operations(), log_event << " " << param);
                return;
            }

            assert(log_level == Error);

            LOG4CPLUS_ERROR(Loggers::Operations(), log_event << " " << param);
        }

        static void Log(const string& log_event, const string& param1, const string& param2, const OperationsLogLevel log_level = Informational)
        {
            if (log_level == Informational)
            {
                LOG4CPLUS_INFO(Loggers::Operations(), log_event << " " << param1 << "|" <<param2);
                return;
            }

            if (log_level == Warning)
            {
                LOG4CPLUS_WARN(Loggers::Operations(), log_event << " " << param1 << "|" <<param2);
                return;
            }

            assert(log_level == Error);

            LOG4CPLUS_ERROR(Loggers::Operations(), log_event << " " << param1 << "|" <<param2);
        }

        static void LogDownloadSuccess(const string& server_address, const string& resource_name, size_t segment_index, size_t block_index, size_t num_of_blocks)
        {
            LOG4CPLUS_INFO(Loggers::Operations(), OperationsLogEvents::DownloadResourceSucceeded << " " << server_address << "|" << resource_name << "|" << segment_index << "|" << block_index << "|" <<num_of_blocks);
        }

        static void LogDownloadFailure(const boost::system::error_code & error, const string& server_address, const string& resource_name, size_t segment_index, size_t block_index, size_t num_of_blocks)
        {
            LOG4CPLUS_ERROR(Loggers::Operations(), OperationsLogEvents::DownloadResourceFailed << " " <<error<< "|" << server_address << "|" << resource_name << "|" << segment_index << "|" << block_index << "|" <<num_of_blocks);
        }

        static void LogDownloadResourceListFailure(const boost::system::error_code & error, const string& server_address)
        {
            LOG4CPLUS_ERROR(Loggers::Operations(), OperationsLogEvents::ErrorDownloadingResourceList << " " <<error<< "|" << server_address);
        }
    };
}

#endif //SUPER_NODE_OPERATIONS_LOGGING_H