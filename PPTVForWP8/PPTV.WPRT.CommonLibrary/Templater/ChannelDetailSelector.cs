using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTVData.Entity;

    public class ChannelDetailSelector : DataTemplateSelector
    {
        public override System.Windows.DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            var dataItem = item as ProgramInfo;
            if (dataItem != null)
            {
                var title = 0;
                if (!int.TryParse(dataItem.Title, out title))
                    return Application.Current.Resources["ChannelDetailAmuseTemplate"] as DataTemplate;
            }

            return Application.Current.Resources["ChannelDetailTemplate"] as DataTemplate;
        }
    }
}
