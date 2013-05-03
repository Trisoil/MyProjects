//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskTask.h"

namespace super_node
{
    ErrorCode DiskTask::Execute()
    {
        ErrorCode error_code = Execute(local_disk_);
        return error_code;
    }
}