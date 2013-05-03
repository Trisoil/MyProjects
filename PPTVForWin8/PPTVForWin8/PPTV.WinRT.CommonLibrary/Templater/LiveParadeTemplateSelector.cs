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

    public class LiveParadeTemplateSelector  : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as LiveParadeInfo;
            if (dataItem != null)
            {
                if (dataItem.Index == 0)
                    return Application.Current.Resources["LivePlayParadeTemplate"] as DataTemplate;
                else if (dataItem.Index % 2 == 0)
                    return Application.Current.Resources["LiveBackParadeTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["LiveParadeTemplate"] as DataTemplate;
        }
    }
}
