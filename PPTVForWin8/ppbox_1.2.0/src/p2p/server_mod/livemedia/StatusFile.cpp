//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "pre.h"
#include "StatusFile.h"
#include "AsioServiceRunner.h"
#include <fstream>

namespace live_media
{
    void StatusFile::Start()
    {
        service_runner_.reset(new AsioServiceRunner("status_report"));
        io_service_ = service_runner_->Start();
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&StatusFile::DoReportStatus, shared_from_this()));
    }

    void StatusFile::DoReportStatus()
    {
        ServiceStatusDetector::Instance().TouchServices();

        boost::mutex::scoped_lock lock(mutex_);

        std::vector<std::string > channels_string;

        for (std::map<std::string, ChannelStatus>::iterator iter = channel_map_.begin();
            iter != channel_map_.end(); iter++) 
        {
            std::ostringstream channel_oss;
            channel_oss << "\t<channel id=\"";
            channel_oss << iter->first
                << "\">\n"
                << "\t\t<status upload_speed=\'" << iter->second.upload_speed_<< "\'"
                << " user_count=\'" << iter->second.user_count_ << "\'"
                << " uploading_user_count=\'" << iter->second.upload_user_count_<< "\'"
                << " begin_time=\'" << iter->second.begin_time_ << "\'"
                << " end_time=\'" << iter->second.end_time_ << "\'"
                << " block_cache_num=\'" << iter->second.block_cache_num_ << "\'"
                << " block_data_num=\'" << iter->second.block_data_num_ << "\'"
                << " />\n\t</channel>\n";

            channels_string.push_back(channel_oss.str());
        }

        std::ostringstream oss;
        oss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<live_status version=\"" << LIVE_MEDIA_SERVER_VERSION << "\""
            << " config_id=\"" << global_channel_status_.configuration_id_ << "\" >\n"
            << "<global_status>\n\t<status listen_port=\'"<< global_channel_status_.listen_port_
            << "\' user_count=\'" << global_channel_status_.user_count_
            << "\' uploading_user_count=\'" << global_channel_status_.upload_user_count_
            << "\' upload_speed=\'" << global_channel_status_.upload_speed_
            << "\' block_cache_num=\'" << global_channel_status_.block_cache_num_
            << "\' block_data_num=\'" << global_channel_status_.block_data_num_
            << "\' services_queue_status=\'"<< ServiceStatusDetector::Instance().GetServicesStatus()
            << "\' />\n</global_status>\n"
            << "<channel_status count=\'" << channel_map_.size() << "\' >\n";

        for(size_t i = 0; i < channels_string.size(); i++) 
        {
            oss << channels_string[i];
        }

        oss << "</channel_status>\n</live_status>\n";

        std::ofstream ofs("status.xml.tmp", std::ios::trunc | std::ios::out);
        ofs << oss.str();
        ofs.close();

        StatisticsReporter reporter;
        reporter.OpenFile();
        for (std::map<std::string, GroupStatus>::iterator group_iter = groups_map_.begin();
            group_iter != groups_map_.end(); group_iter++)
        {
            for (std::map<std::string, boost::uint32_t>::iterator iter = group_iter->second.begin();
                iter != group_iter->second.end(); iter++)
            {
                reporter.AddStatus(group_iter->first, iter->first, iter->second);
            }
        }

        reporter.SaveFile();
        remove("status.xml");
        rename("status.xml.tmp", "status.xml");

        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&StatusFile::DoReportStatus, shared_from_this()));
    }

    void StatusFile::AddGroupStatus(std::string group_name, const std::map<std::string, boost::uint32_t> & status)
    {
        boost::mutex::scoped_lock lock(mutex_);
        groups_map_[group_name] = status;
    }

    void StatusFile::UpdateChannelStatus(const GlobalStatus & global_status, const std::map<std::string, ChannelStatus> & channels_status)
    {
        boost::mutex::scoped_lock lock(mutex_);
        channel_map_.clear();
        global_channel_status_ = global_status;
        channel_map_ = channels_status;
    }
}