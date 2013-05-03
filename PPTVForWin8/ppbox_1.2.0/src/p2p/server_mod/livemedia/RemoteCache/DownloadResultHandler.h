//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _DOWNLOAD_RESULT_HANDLER_H_
#define _DOWNLOAD_RESULT_HANDLER_H_

namespace live_media
{
    class BlockData;
    class IDownloadResultHandler
    {
    public:
        virtual ~IDownloadResultHandler(){}
        virtual void HandleDownloadResult(const boost::system::error_code& err, boost::shared_ptr<BlockData> blocks) = 0;
    };
}
#endif