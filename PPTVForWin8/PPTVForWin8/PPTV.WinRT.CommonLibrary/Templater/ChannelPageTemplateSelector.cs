using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Templater
{
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;

    public class ChannelPageTemplateSelector : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as ChannelListItem;
            if (dataItem != null)
            {
                if (ChannelTypeFactory.Instance.JudgeLandscapeChannel(dataItem.TypeInfo.TypeName))
                {
                    return Application.Current.Resources["ChannelListLandscapeTemplate"] as DataTemplate;
                }
            }
            return Application.Current.Resources["ChannelListTemplate"] as DataTemplate;
        }
    }
}
