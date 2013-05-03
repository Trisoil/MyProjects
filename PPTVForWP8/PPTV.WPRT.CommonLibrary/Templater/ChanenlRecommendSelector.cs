using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTV.WPRT.CommonLibrary.DataModel;

    public class ChanenlRecommendSelector : DataTemplateSelector
    {
        public override DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
        {
            var dataItem = item as ChannelRecommendListItem;
            if (dataItem != null)
            {
                if (dataItem.Index == -1)
                    return Application.Current.Resources["ChannelRecommendTemplate"] as DataTemplate;
                if (dataItem.Index == -2)
                    return Application.Current.Resources["EmptyTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["ChannelRecommendMoreTemplate"] as DataTemplate;
        }
    }
}
