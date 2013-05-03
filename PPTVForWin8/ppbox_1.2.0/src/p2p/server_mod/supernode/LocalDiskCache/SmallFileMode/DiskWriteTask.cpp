//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskTask.h"
#include "SmallFileLocalDisk.h"
#include "Statistics/DiskStatistics.h"
#include "Statistics/IOStatistics.h"

namespace super_node
{
    ErrorCode DiskWriteTask::Execute(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        framework::timer::TimeCounter time_counter;

        ErrorCode error_code = ErrorCodes::UnknownError;
        try
        {
            error_code = ExecuteDiskWrite(disk);
        }
        catch (const boost::filesystem::filesystem_error&)
        {
            error_code = ErrorCodes::DiskWriteFailure;
        }

        if (error_code == ErrorCodes::Success)
        {
            boost::uint32_t elapsed_ticks = time_counter.elapse();

            size_t bytes_written = 0;

            for(std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks_.begin();
                iter != blocks_.end();
                ++iter)
            {
                size_t block_index = iter->first;
                boost::shared_ptr<BlockData> block_data = iter->second;
                if (block_data)
                {
                    bytes_written += block_data->Size();
                }
            }

            disk_write_statistics_->Add(bytes_written, elapsed_ticks);
        }
        else
        {
            disk_write_statistics_->AddFailure();
        }

        callback_(error_code, this->resource_identifier_);

        return error_code;
    }

    ErrorCode DiskWriteTask::ExecuteDiskWrite(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        boost::filesystem::path resource_folder_path = disk->GetResourceFolderPath(resource_identifier_);

        LOG4CPLUS_TRACE(Loggers::LocalDisk(), "DiskWriteTask::Execute(): Writing "<<resource_identifier_.GetResourceName()<<"("<<blocks_.size()<<" blocks) into disk " << disk->GetDiskPath());

        if (boost::filesystem::exists(resource_folder_path))
        {
            LOG4CPLUS_TRACE(Loggers::LocalDisk(), "DiskWriteTask::Execute():  resource directory "<<resource_folder_path<<" already exists.");
        }
        else if (!boost::filesystem::create_directories(resource_folder_path))
        {
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), "DiskWriteTask::Execute(): Failed to create directory "<<resource_folder_path);
            return ErrorCodes::DiskWriteFailure;
        }

        size_t resource_size = 0;

        assert(blocks_.size() > 0);
        if (blocks_.size() == 0)
        {
            LOG4CPLUS_WARN(Loggers::LocalDisk(), "DiskWriteTask::Execute(): An attempt to write empty blocks to "<<resource_folder_path<<" is rejected.");
            return ErrorCodes::UnknownError;
        }

        for(std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks_.begin();
            iter != blocks_.end();
            ++iter)
        {
            size_t block_index = iter->first;
            boost::shared_ptr<BlockData> block_data = iter->second;

            if (block_data)
            {
                boost::filesystem::path block_file_path = disk->GetResourceBlockFilePath(resource_identifier_, block_index);
                LOG4CPLUS_TRACE(Loggers::LocalDisk(), "DiskWriteTask::Execute(): Writing block file "<<block_file_path);
                if (block_data->WriteFile(block_file_path))
                {
                    resource_size += block_data->Size();
                }
                else
                {
                    LOG4CPLUS_ERROR(Loggers::LocalDisk(), "DiskWriteTask::Execute(): Failed to write "<<block_file_path);
                }
            }
        }

        disk->UpdateResource(resource_identifier_);
        return ErrorCodes::Success;
    }

    void DiskWriteTask::Cancel()
    {
        if (callback_)
        {
            callback_(ErrorCodes::ServiceBusy, this->resource_identifier_);
        }
    }
}
