using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace PPTV.WPRT.CommonLibrary.Templater
{
    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel.Channel;

    public class ChannelTypeSelector : DataTemplateSelector
    {
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            this.Tap += ChannelTypeSelector_Tap;
        }

        void ChannelTypeSelector_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            var item = DataContext as ChannelTypeItem;
            if (item != null)
            {
                var path = ChannelTypeFactory.Instance.GetLocalPath(item.TypeId);
                if (string.IsNullOrEmpty(path))
                    path = string.Format("/Pages/ChannelPage.xaml?id={0}&name={1}", item.TypeId, item.TypeName);
                var frame = Application.Current.RootVisual as Microsoft.Phone.Controls.TransitionFrame;
                if (frame != null)
                    frame.Navigate(new Uri(path, UriKind.Relative));
            }
        }

        public override System.Windows.DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
        {
            var dataItem = item as ChannelTypeItem;
            if (dataItem != null)
            {
                if (dataItem.TypeId <= -20)
                    return Application.Current.Resources["ChannelLocalTypeTemplate"] as DataTemplate;
            }
            return Application.Current.Resources["ChannelTypeTemplate"] as DataTemplate;
        }
    }
}
