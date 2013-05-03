using System;
using System.Net;
using System.Windows.Data;
namespace PPTVForWP7
{
    public class ChannelTitleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string str = (String)value;
            if (str == null)
                return "";
            if (str.Length <= 5)
                return value;
            else
                return str.Substring(0, 5) + "...";
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            //string str = (String)value;
            //if (str.Length <= 7)
            //    return value;
            //else
            //    return str.Substring(0, 7);
            return value;
        }
    }
}
