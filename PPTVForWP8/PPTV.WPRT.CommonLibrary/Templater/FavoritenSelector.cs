using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTVData.Entity.Cloud;

    public class FavoritenSelector : DataTemplateSelector
    {
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.Tap += FavoritenSelector_Tap;
        }

        void FavoritenSelector_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var item = DataContext as CloudDataInfo ;
            if (item != null)
            { 
                var frame = Application.Current.RootVisual as Microsoft.Phone.Controls.TransitionFrame;
                if (frame != null)
                {
                    Utils.ChannelUtils.DACPageType = PPTVData.Entity.DACPageType.FAV;
                    frame.Navigate(new Uri(string.Format("/Pages/DetailPage.xaml?id={0}", item.Id), UriKind.Relative));
                }
            }
        }

        public override System.Windows.DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
        {
            return Application.Current.Resources["FavoritenTemplate"] as DataTemplate;
        }
    }
}
