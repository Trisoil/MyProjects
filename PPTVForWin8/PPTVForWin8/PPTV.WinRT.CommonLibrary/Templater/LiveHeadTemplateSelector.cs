using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTV.WinRT.CommonLibrary.Templater
{
    using PPTVData;
    using PPTVData.Entity.Live;

    using PPTV.WinRT.CommonLibrary.DataModel;

    public class LiveHeadTemplateSelector : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as LiveListGroup;
            if (dataItem != null)
            {
                if (dataItem.GroupName != LiveType.Satellite.CreateString()
                    && dataItem.GroupName != LiveType.LocalTV.CreateString())
                {
                    return Application.Current.Resources["CommonBoxGroupHeadTemplate"] as DataTemplate;
                }
            }

            return Application.Current.Resources["GroupHeadTemplate"] as DataTemplate;
        }
    }
}
