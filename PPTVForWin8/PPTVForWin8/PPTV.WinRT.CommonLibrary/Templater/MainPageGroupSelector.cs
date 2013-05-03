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

    public class MainPageGroupSelector : GroupStyleSelector
    {
        protected override GroupStyle SelectGroupStyleCore(object group, uint level)
        {
            GroupStyle style = Application.Current.Resources["MainPageGroupStyle"] as GroupStyle;
            var dataItem = group as Windows.UI.Xaml.Data.ICollectionViewGroup;
            if (dataItem != null)
            {
                var obj = dataItem.Group as ChannelRecommendGroup;
                if (obj != null)
                {
                    if (obj.GroupName == "频道")
                        style = Application.Current.Resources["MainPageChannelGroupStyle"] as GroupStyle;
                    else
                        style = Application.Current.Resources["MainPageGroupStyle"] as GroupStyle;
                }
            }
            return style;
        }
    }
}
