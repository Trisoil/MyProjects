//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskTask.h"
#include "SmallFileLocalDisk.h"

namespace super_node
{
    ErrorCode DiskDeletionTask::Execute(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Deleting " << resources_.size() <<" resources from disk"<<disk->GetDiskPath());

        ErrorCode error_code = ErrorCodes::Success;
        for(size_t i = 0; i < resources_.size(); ++i)
        {
            disk->RemoveResource(resources_[i]);
            boost::filesystem::path resource_folder_path = disk->GetResourceFolderPath(resources_[i]);

            try
            {
                if (boost::filesystem::exists(resource_folder_path))
                {
                    boost::filesystem::remove_all(resource_folder_path);
                }
            }
            catch (const boost::filesystem::filesystem_error& error)
            {
                LOG4CPLUS_ERROR(Loggers::LocalDisk(), "An error occurred while trying to delete " << resource_folder_path << ". Error:" << error.what());
                error_code = ErrorCodes::DiskWriteFailure;
            }
        }

        callback_(error_code, resources_);

        return error_code;
    }

    void DiskDeletionTask::Cancel()
    {
        LOG4CPLUS_INFO(Loggers::LocalDisk(), "A task to delete " << resources_.size() <<" resources is cancelled.");
        if (callback_)
        {
            callback_(ErrorCodes::ServiceBusy, resources_);
        }
    }
}
