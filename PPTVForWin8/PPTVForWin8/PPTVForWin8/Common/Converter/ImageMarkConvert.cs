using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    public sealed class ImageMarkConvert : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
                double mark = -1d;
                if (double.TryParse(value.ToString(), out mark))
                {
                    return "ms-appx:///Images/" + (int)mark / 2 + ".png";
                }
                return "ms-appx:///Images/0.png";            
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
