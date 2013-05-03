using System;
using System.Net;
using System.Security;
using System.Windows.Data;

namespace PPTVForWP7
{
    public class TimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            //string str = (String)value;
            //int count = System.Convert.ToInt32(str);
            return value + "分钟";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return value;
        }
    }
}
