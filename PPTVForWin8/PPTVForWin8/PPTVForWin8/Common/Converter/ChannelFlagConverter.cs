using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Data;

namespace PPTVForWin8.Common
{
    using PPTVData.Entity;

    public class ChannelFlagConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            var channelFlag = System.Convert.ToInt32(value);
            var result = string.Empty;

            if ((channelFlag & (int)ChannelFlagType.Blue) == (int)ChannelFlagType.Blue)
                result = "超清";
            else if ((channelFlag & (int)ChannelFlagType.High) == (int)ChannelFlagType.High)
                result = "高清";

            return result;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
