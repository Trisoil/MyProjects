//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _DOWNLOAD_PROGRESS_LISTENER_H_
#define _DOWNLOAD_PROGRESS_LISTENER_H_

namespace super_node
{
    class DownloadTask;
    class IDownloadProgressListener
    {
    public:
        virtual ~IDownloadProgressListener(){};
        virtual void OnDownloadTaskCompleted(DownloadTask& download_task, bool succeed) = 0;
    };
}
#endif