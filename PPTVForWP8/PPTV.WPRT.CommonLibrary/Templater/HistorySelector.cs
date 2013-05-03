using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTVData.Entity.Cloud;

    using PPTV.WPRT.CommonLibrary.ViewModel.Play;

    public class HistorySelector : DataTemplateSelector
    {
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.Tap += HistorySelector_Tap;
        }

        void HistorySelector_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var item = DataContext as CloudDataInfo;
            if (item != null)
            { 
                var frame = Application.Current.RootVisual as Microsoft.Phone.Controls.TransitionFrame;
                if (frame != null)
                {
                    Utils.ChannelUtils.DACPageType = PPTVData.Entity.DACPageType.History;
                    frame.Navigate(new Uri(string.Format("/Pages/PlayPage.xaml?id={0}&index={1}&playtype={2}", item.Id, item.ProgramIndex, (int)PlayType.Vod), UriKind.Relative));
                }
            }
        }

        public override System.Windows.DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
        {
            return Application.Current.Resources["HistoryListTemplate"] as DataTemplate;
        }
    }
}
