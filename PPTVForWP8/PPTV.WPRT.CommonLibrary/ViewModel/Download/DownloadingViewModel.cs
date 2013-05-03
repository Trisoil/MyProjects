using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Download
{
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.DataModel.Download;

    public class DownloadingViewModel : FileFactoryBase<DownloadingViewModel, DownloadInfo>
    {
        public override string FileName
        {
            get { return "Downloading.db"; }
        }
    }
}
