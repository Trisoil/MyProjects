//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"

namespace live_media
{
    const ErrorCode ErrorCodes::Success(0);

    //generic error
    const ErrorCode ErrorCodes::ResourceNotFound(0x001001);
    const ErrorCode ErrorCodes::UnknownError(0x001002);
    const ErrorCode ErrorCodes::ServiceBusy(0x001003);
    const ErrorCode ErrorCodes::ServiceStopped(0x001004);

    //remote cache specific error 
    const ErrorCode ErrorCodes::DownloadFailure(0x003001);
}