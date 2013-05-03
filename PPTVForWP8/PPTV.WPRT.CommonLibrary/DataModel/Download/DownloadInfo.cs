using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.DataModel.Download
{
    public class DownloadInfo : BindableBase
    {
        public DownloadInfo()
        {
            DownloadTipInfo = new DownloadTipInfo();
        }

        public int TypeId { get; set; }

        public int ParentId { get; set; }

        public string ParentName { get; set; }

        public int ProgramIndex { get; set; }

        public int ChannelId { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }

        public string DownloadUri { get; set; }

        public string LocalFileName { get; set; }

        public string RequestId { get; set; }

        public DownloadState DownloadState
        {
            get { return DownloadTipInfo.DownloadState; }
            set { DownloadTipInfo.DownloadState = value; this.OnPropertyChanged("DownloadTipInfo"); }
        }

        public long TotalBytesReceive
        {
            get { return DownloadTipInfo.TotalBytesReceive; }
            set { DownloadTipInfo.TotalBytesReceive = value; this.OnPropertyChanged("DownloadTipInfo"); }
        }

        public long BytesReceived
        {
            get { return DownloadTipInfo.BytesReceived; }
            set { DownloadTipInfo.BytesReceived = value; this.OnPropertyChanged("DownloadTipInfo"); }
        }

        private double _downloadPercent;
        public double DownloadPercent
        {
            get { return _downloadPercent; }
            set { this.SetProperty(ref _downloadPercent, value); }
        }

        private DownloadTipInfo _downloadTipInfo;
        public DownloadTipInfo DownloadTipInfo
        {
            get { return _downloadTipInfo; }
            set { this.SetProperty(ref _downloadTipInfo, value); }
        }

    }

    public class DownloadTipInfo
    {
        public DownloadState DownloadState { get; set; }

        public long TotalBytesReceive { get; set; }

        public long BytesReceived { get; set; }
    }
}
