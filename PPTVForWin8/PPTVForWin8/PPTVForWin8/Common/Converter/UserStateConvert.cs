using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    public class UserStateConvert : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var vip = System.Convert.ToInt32(value);
            return vip == 0 ? "普通用户" : "VIP会员";
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
