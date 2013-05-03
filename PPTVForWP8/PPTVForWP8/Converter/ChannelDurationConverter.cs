using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace PPTVForWP8.Converter
{
    public class ChannelDurationConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            var duration = System.Convert.ToDouble(value);
            var timespan = TimeSpan.FromMinutes(duration);
            var reuslt = timespan.Hours == 0 ?
                string.Format("{0}分钟", timespan.Minutes) :
                string.Format("{0}小时{1}分钟", timespan.Hours, timespan.Minutes);
            return reuslt;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
