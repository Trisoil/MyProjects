using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    public class ChannelMarkConvert : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var mark = System.Convert.ToDouble(value);
            return mark.ToString("0.0");
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
