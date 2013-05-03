using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Data;

namespace PPTVForWP7
{

    public class LongChannelTitleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string str = (String)value;
            if (str == null)
                return "";
            if (str.Length <= 8)
                return value;
            else
                return str.Substring(0, 8) + "...";
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
