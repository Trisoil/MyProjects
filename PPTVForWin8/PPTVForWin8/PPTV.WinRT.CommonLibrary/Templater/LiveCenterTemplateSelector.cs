using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Templater
{
    using PPTV.WinRT.CommonLibrary.DataModel;

    public class LiveCenterTemplateSelector : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as LiveListItem;
            if (dataItem != null)
            {
                if (dataItem.ChannelInfo.Id <= 0)
                    return Application.Current.Resources["LiveCenterNoResultTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["LiveCenterTemplate"] as DataTemplate;
        }
    }
}
