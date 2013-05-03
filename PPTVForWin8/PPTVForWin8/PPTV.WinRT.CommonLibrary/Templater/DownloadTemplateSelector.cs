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

    public class DownloadTemplateSelector : DataTemplateSelector
    {
        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            var dataItem = item as ProgramInfo;
            if (dataItem != null)
            {
                var title = 0;
                if (!int.TryParse(dataItem.Title, out title))
                    return Application.Current.Resources["DownloadAmuseTemplate"] as DataTemplate;
            }

            return Application.Current.Resources["DownloadListTemplate"] as DataTemplate;
        }
    }
}
