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
    using PPTV.WPRT.CommonLibrary.ViewModel.Play;

    public class DownloadedSelector : DataTemplateSelector
    {
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.Tap += DownloadedSelector_Tap;
        }

        void DownloadedSelector_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var item = DataContext as DownloadInfo;
            if (item != null)
            {
                var frame = Application.Current.RootVisual as Microsoft.Phone.Controls.TransitionFrame;
                if (frame != null)
                {
                    var path = string.Format("/Pages/PlayPage.xaml?id={0}&playtype={1}", item.ChannelId, (int)PlayType.Download);
                    frame.Navigate(new Uri(path, UriKind.Relative));
                }
            }
        }

        public override System.Windows.DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            return Application.Current.Resources["DownloadedListTemplate"] as DataTemplate;
        }
    }
}
