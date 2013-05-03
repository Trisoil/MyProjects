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

    public class ChannelTypeTemplateSelector : DataTemplateSelector
    {
        protected override Windows.UI.Xaml.DataTemplate SelectTemplateCore(object item, Windows.UI.Xaml.DependencyObject container)
        {
            var dataItem = item as ChannelTypeItem;
            if (dataItem != null)
            {
                if (dataItem.TypeId <= -20)
                    return Application.Current.Resources["localMainPageChannelTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["MainPageChannelTemplate"] as DataTemplate;
        }
    }
}
