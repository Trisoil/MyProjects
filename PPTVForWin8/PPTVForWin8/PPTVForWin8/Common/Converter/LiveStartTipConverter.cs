using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    using PPTVData.Entity.Live;

    public class LiveStartTipConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var dataItem = value as LiveChannelInfo;
            if (dataItem != null)
            {
                var now = DateTime.Now;
                if (dataItem.StartTime <= now
                    && dataItem.EndTime >= now)
                    return "ms-appx:///Images/liveing.png";
            }
            return "ms-appx:///Images/liveuning.png";
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
