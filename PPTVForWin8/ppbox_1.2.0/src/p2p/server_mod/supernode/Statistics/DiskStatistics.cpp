//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskStatistics.h"
#include "IOStatistics.h"
#include "QueueStatistics.h"

namespace super_node
{
    DiskStatistics::DiskStatistics(const string disk_path)
        : disk_path_(disk_path)
    {
        disk_read_statistics_.reset(new IOStatistics());
        disk_write_statistics_.reset(new IOStatistics());
        queue_statistics_.reset(new QueueStatistics());

        disk_space_in_bytes_ = 0;
        free_space_in_bytes_ = 0;

        UpdateDiskAvailableSpace();
    }

    void DiskStatistics::UpdateDiskAvailableSpace()
    {
        try
        {
            boost::filesystem::space_info disk_space = boost::filesystem::space(boost::filesystem::path(disk_path_));
            disk_space_in_bytes_ = disk_space.capacity;
            free_space_in_bytes_ = disk_space.available;
        }
        catch(const boost::filesystem::filesystem_error& e)
        {
            OperationsLogging::Log(OperationsLogEvents::WarningFailedToQueryDiskAvailableSpace, this->disk_path_, Warning);
            LOG4CPLUS_WARN(Loggers::LocalDisk(), "An error occurred while trying to get disk available space for disk" << disk_path_ <<". Error:" << e.what());
        }
    }

    size_t DiskStatistics::GetDiskSpaceInBytes() const
    {
        return static_cast<size_t>(disk_space_in_bytes_);
    }

    size_t DiskStatistics::GetFreeSpacePercentage() const
    {
        if (disk_space_in_bytes_ == 0 || free_space_in_bytes_ >= disk_space_in_bytes_)
        {
            return 0;
        }

        return static_cast<size_t>(100*free_space_in_bytes_/disk_space_in_bytes_);
    }
}