using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace PPTVForWP8.Converter
{
    using PPTVData.Entity.Cloud;

    public class ChannelWatchPositionConvert : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            int pos = 0;
            int programIndex = -1;
            var dataItem = value as CloudDurationInfo;
            if (dataItem != null)
            {
                pos = dataItem.Pos;
                programIndex = dataItem.ProgramIndex;

                var position = TimeSpan.FromSeconds(pos);
                var result = position.Hours == 0 ?
                    string.Format("{0}分{1}秒", position.Minutes, position.Seconds) :
                    string.Format("{0}小时{1}分{2}秒", position.Hours, position.Minutes, position.Seconds);

                return programIndex == -1 ?
                            string.Format("上次观看到 {0}", result) : string.Format("上次观看到 第{0}集 {1}", programIndex + 1, result);
            }
            return string.Empty;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
