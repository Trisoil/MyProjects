using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    using PPTV.WinRT.CommonLibrary.DataModel.Download;

    public class DownloadStateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var dataItem = value as DownloadTipInfo;
            if (dataItem != null)
            {
                switch (dataItem.DownloadState)
                {
                    case DownloadState.Downloading:
                        return "ms-appx:///Images/downing.png";
                    case DownloadState.Pause:
                        return "ms-appx:///Images/downpause.png";
                    case DownloadState.Await:
                        return "ms-appx:///Images/downawait.png";
                }
            }
            return "ms-appx:///Images/downpause.png";
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return null;
        }
    }
}
