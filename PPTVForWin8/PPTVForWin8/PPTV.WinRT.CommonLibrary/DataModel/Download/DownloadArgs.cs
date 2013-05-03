using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel.Download
{
    public class DownloadArgs : EventArgs
    {
        public DownloadArgs(string message)
        {
            Message = message;
        }

        public string Message { get; set; }
    }
}
