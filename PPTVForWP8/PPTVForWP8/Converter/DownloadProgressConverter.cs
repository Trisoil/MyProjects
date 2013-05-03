using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace PPTVForWP8.Converter
{
    using PPTV.WPRT.CommonLibrary.DataModel.Download;

    public class DownloadProgressConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var dataItem = value as DownloadTipInfo;
            if (dataItem != null)
            {
                switch (dataItem.DownloadState)
                {
                    case DownloadState.Downloading:
                        return string.Format("下载中 {0}MB/{1}MB", (dataItem.BytesReceived / 1024.0 / 1024.0).ToString("0.00"), (dataItem.TotalBytesReceive / 1024.0 / 1024.0).ToString("0.00"));
                    case DownloadState.Await:
                        return string.Format("等待下载 {0}MB/{1}MB", (dataItem.BytesReceived / 1024.0 / 1024.0).ToString("0.00"), (dataItem.TotalBytesReceive / 1024.0 / 1024.0).ToString("0.00"));
                    case DownloadState.Pause:
                        return string.Format("已暂停 {0}MB/{1}MB", (dataItem.BytesReceived / 1024.0 / 1024.0).ToString("0.00"), (dataItem.TotalBytesReceive / 1024.0 / 1024.0).ToString("0.00"));
                    case DownloadState.Downloaded:
                        return string.Format("{0}MB", (dataItem.TotalBytesReceive / 1024.0 / 1024.0).ToString("0.00"));
                    case DownloadState.DownloadError:
                        return "下载失败";
                    case DownloadState.PlayInfoError:
                        return "无法获取下载地址";
                    case DownloadState.WaitingForWiFi:
                        return "等待WiFi网络";
                    case DownloadState.WaitingForExternalPower:
                        return "等待外接电源";
                    default:
                        return "等待下载";
                }
            }
            return string.Empty;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
