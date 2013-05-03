using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace PPTV.WinRT.CommonLibrary.Controls
{
    public class CustomListView : ListView
    {
        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            var sv = this.GetTemplateChild("ScrollViewer") as UIElement;
            if (sv != null)
                sv.AddHandler(UIElement.PointerWheelChangedEvent, new PointerEventHandler(OnPointerWheelChanged), true);
        }

        private void OnPointerWheelChanged(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = false;
        }
    }
}
