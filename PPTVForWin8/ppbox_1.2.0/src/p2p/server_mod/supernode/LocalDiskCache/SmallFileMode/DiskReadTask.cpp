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
    ErrorCode DiskReadTask::Execute(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        framework::timer::TimeCounter time_counter;

        boost::shared_ptr<BlockData> block_data;
        boost::filesystem::path block_file_path = disk->GetResourceBlockFilePath(resource_identifier_, block_id_);

        ErrorCode error_code = ErrorCodes::ResourceNotFound;
        try
        {
            LOG4CPLUS_TRACE(Loggers::LocalDisk(), "DiskReadTask::Execute(): reading "<<resource_identifier_.GetResourceName()<<"["<<block_id_<<"] from disk " << disk->GetDiskPath());
            block_data.reset(new BlockData(block_file_path));
            if (block_data->Size() > 0)
            {
                error_code = ErrorCodes::Success;
            }
            else
            {
                block_data.reset();
            }
        }
        catch(const boost::filesystem::filesystem_error&)
        {
            error_code = ErrorCodes::DiskReadFailure;
        }

        if (error_code == ErrorCodes::Success && block_data)
        {
            boost::uint32_t elapsed_ticks = time_counter.elapse();
            disk_read_statistics_->Add(block_data->Size(), elapsed_ticks);
        }
        else
        {
            disk_read_statistics_->AddFailure();
            LOG4CPLUS_WARN(Loggers::LocalDisk(), "DiskReadTask::Execute(): failed to read block("<<resource_identifier_.GetResourceName()<<","<<block_id_<<") from disk " << disk->GetDiskPath());
        }

        if (callback_)
        {
            callback_(error_code, block_data);
        }

        return error_code;
    }

    void DiskReadTask::Cancel()
    {
        LOG4CPLUS_TRACE(Loggers::LocalDisk(), "A task to read " << resource_identifier_.GetResourceName() <<" is cancelled.");
        if (callback_)
        {
            callback_(ErrorCodes::ServiceBusy, boost::shared_ptr<BlockData>());
        }
    }
}
