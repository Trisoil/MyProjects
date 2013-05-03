using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    public class CloudDeviceConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            int data = System.Convert.ToInt32(value);
            return Utils.CloudUtils.CloudDevice(data);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
