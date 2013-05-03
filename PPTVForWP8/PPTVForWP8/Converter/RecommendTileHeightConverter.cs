using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace PPTVForWP8.Converter
{
    public class RecommendTileHeightConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double baseHeight = 182;

            double multiplier;

            if (parameter == null || double.TryParse(parameter.ToString(), out multiplier) == false)
            {
                multiplier = 1;
            }

            return baseHeight * multiplier;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
