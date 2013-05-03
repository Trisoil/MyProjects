//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"

namespace super_node
{
    const ErrorCode ErrorCodes::Success(0);

    //generic error
    const ErrorCode ErrorCodes::ResourceNotFound(0x001001);
    const ErrorCode ErrorCodes::UnknownError(0x001002);
    const ErrorCode ErrorCodes::ServiceBusy(0x001003);
    const ErrorCode ErrorCodes::ServiceStopped(0x001004);

    //disk access error
    const ErrorCode ErrorCodes::DiskWriteFailure(0x002001);
    const ErrorCode ErrorCodes::DiskReadFailure(0x002002);
    const ErrorCode ErrorCodes::DiskReadWriteFailure(0x002003);


    const ErrorCode ErrorCodes::OutOfMemory(0x002004);

    //remote cache specific error 
    const ErrorCode ErrorCodes::DownloadFailure(0x003001);
}
