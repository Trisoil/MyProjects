//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DISK_STATISTICS_H
#define SUPER_NODE_DISK_STATISTICS_H

namespace super_node
{
    class IOStatistics;
    class QueueStatistics;
    class DiskResource;

    class DiskStatistics
        : public count_object_allocate<DiskStatistics>
    {
    public:
        DiskStatistics(const string disk_path);

        size_t GetFreeSpacePercentage() const;
        size_t GetDiskSpaceInBytes() const;

        void UpdateDiskAvailableSpace();
        void UpdateDiskAvailableSpace(boost::uintmax_t free_space_in_bytes, boost::uintmax_t disk_space_in_bytes)
        {
            free_space_in_bytes_ = free_space_in_bytes; 
            disk_space_in_bytes_ = disk_space_in_bytes;
        }

        boost::shared_ptr<IOStatistics> GetDiskReadStatistics() const { return disk_read_statistics_; }
        boost::shared_ptr<IOStatistics> GetDiskWriteStatistics() const { return disk_write_statistics_; }
        boost::shared_ptr<QueueStatistics> GetQueueStatistics() const { return queue_statistics_; }

    private:
        string disk_path_;
        boost::uintmax_t free_space_in_bytes_;
        boost::uintmax_t disk_space_in_bytes_;

        boost::shared_ptr<IOStatistics> disk_read_statistics_;
        boost::shared_ptr<IOStatistics> disk_write_statistics_;
        boost::shared_ptr<QueueStatistics> queue_statistics_;
    };
}

#endif //SUPER_NODE_DISK_STATISTICS_H
