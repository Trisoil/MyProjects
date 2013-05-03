//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskTask.h"
#include "SmallFileLocalDisk.h"

namespace super_node
{
    ErrorCode DiskNoOperationTask::Execute(boost::shared_ptr<SmallFileLocalDisk> disk)
    {
        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Running DiskNoOperationTask::Execute for disk "<<disk->GetDiskPath());
        return ErrorCodes::Success;
    }

    void DiskNoOperationTask::Cancel()
    {
        LOG4CPLUS_INFO(Loggers::LocalDisk(), "Running DiskNoOperationTask::Cancel");
    }
}
