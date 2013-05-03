using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.ViewModel.Download
{
    using PPTVData.Factory;

    using PPTV.WinRT.CommonLibrary.DataModel.Download;

    internal class DownloadingViewModel : FileFactoryBase<DownloadingViewModel, DownloadInfo>
    {
        public override string FileName
        {
            get { return "Downloading.db"; }
        }
    }
}
