using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace PPTVForWP8.Converter
{
    using PPTV.WPRT.CommonLibrary.DataModel.Download;

    public class DownloadStateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var dataItem = value as DownloadTipInfo;
            if (dataItem != null)
            {
                switch (dataItem.DownloadState)
                {
                    case DownloadState.Downloading:
                    case DownloadState.WaitingForWiFi:
                    case DownloadState.WaitingForExternalPower:
                        return "/Images/downing.png";
                    case DownloadState.Pause:
                        return "/Images/downpause.png";
                    case DownloadState.Await:
                        return "/Images/downawait.png";
                }
            }
            return "ms-appx:///Images/downpause.png";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
