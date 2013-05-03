#pragma once

#include <framework/logger/LoggerListRecord.h>
#include <framework/logger/LoggerFormatRecord.h>
#include <framework/logger/LoggerStreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE("Tracker");

/*
#define TRACKER_TYPE "tracker"

#define TRACK_INFO(message) LOG(__INFO, TRACKER_TYPE, message)
#define TRACK_EVENT(message) LOG(__EVENT, TRACKER_TYPE, message)
#define TRACK_DEBUG(message) LOG(__DEBUG, TRACKER_TYPE, message)
#define TRACK_WARN(message) LOG(__WARN, TRACKER_TYPE, message)
#define TRACK_ERROR(message) LOG(__ERROR, TRACKER_TYPE, message)
*/

//#define TRACK_INFO(message) LOG_S( ::framework::logger::Logger::kLevelInfor, message)
//#define TRACK_EVENT(message) LOG_S(::framework::logger::Logger::kLevelEvent, message)
//#define TRACK_DEBUG(message) LOG_S(::framework::logger::Logger::kLevelDebug, message)
//#define TRACK_WARN(message) LOG_S(::framework::logger::Logger::kLevelAlarm, message)
#define TRACK_ERROR(message) LOG_S(::framework::logger::Logger::kLevelError, message)

#define TRACK_INFO(message) 
#define TRACK_EVENT(message) 
#define TRACK_DEBUG(message) 
#define TRACK_WARN(message) 
//#define TRACK_ERROR(message) 

namespace udptracker
{
	inline std::vector<std::string> ConvertIPs(const std::vector<boost::uint32_t>& ips)
    {
        std::vector<std::string> ipstr;
        for (size_t i = 0; i < ips.size(); i++)
            ipstr.push_back(IpPortToUdpEndpoint(ips[i], 0).address().to_string());
        return ipstr;
    }
}
