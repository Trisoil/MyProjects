using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTV.WPRT.CommonLibrary.DataModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Download;

    public class DownloadingSelector : DataTemplateSelector
    {
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.Tap += DownloadingSelector_Tap;
        }

        void DownloadingSelector_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var item = DataContext as DownloadInfo;
            if (item != null)
            {
                DownloadViewModel.Instance.HandleDownload(item);
            }
        }

        public override System.Windows.DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            return Application.Current.Resources["DownloadingListTemplate"] as DataTemplate;
        }
    }
}
