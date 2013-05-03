using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Templater
{
    using PPTVData.Entity;

    public class ChannelDetailTemplateSelector : DataTemplateSelector
    {
        protected override Windows.UI.Xaml.DataTemplate SelectTemplateCore(object item, Windows.UI.Xaml.DependencyObject container)
        {
            var dataItem = item as ProgramInfo;
            if (dataItem != null)
            {
                var title = 0;
                if (!int.TryParse(dataItem.Title, out title))
                    return Application.Current.Resources["ChannelDetailAmuseTemplate"] as DataTemplate;
            }

            return Application.Current.Resources["ChannelDetailListTemplate"] as DataTemplate;
        }
    }
}
