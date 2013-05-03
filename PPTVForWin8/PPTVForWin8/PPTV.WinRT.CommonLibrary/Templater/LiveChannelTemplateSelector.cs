using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Templater
{
    using PPTVData.Entity.Live;
    using PPTVData.Factory.Live;

    using PPTV.WinRT.CommonLibrary.DataModel;

    public class LiveChannelTemplateSelector : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as LiveListItem;
            if (dataItem != null)
            {
                if (dataItem.ChannelInfo.Id <= 0)
                    return Application.Current.Resources["LiveCenterNoResultTemplate"] as DataTemplate;
                else if (LiveCenterFactoryBase.IsDirectPlay(dataItem.ChannelInfo.LiveType))
                    return Application.Current.Resources["SnapLiveCenterTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["SnapLiveListTemplate"] as DataTemplate;
        }
    }
}
