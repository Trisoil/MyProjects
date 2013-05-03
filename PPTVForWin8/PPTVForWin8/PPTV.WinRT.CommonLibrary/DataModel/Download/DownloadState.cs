using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel.Download
{
    public enum DownloadState
    {
        Downloading = 0,
        Await,
        Pause,
        Downloaded,
        PlayInfoError,
        DownloadError,
    }
}
