using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.ApplicationSettings;

namespace PPTVForWin8.Common
{
    public class SettingsFlyout
    {
        private static Popup popup;

        public Action CloseAction;

        public void ShowFlyout(UserControl control, int width, bool isLightDismiss)
        {
            if (popup == null)
            {
                popup = new Popup();
                popup.Closed += popup_Closed;

                popup.ChildTransitions = new TransitionCollection();
                popup.ChildTransitions.Add(new PaneThemeTransition()
                {
                    Edge = (SettingsPane.Edge == SettingsEdgeLocation.Right) ?
                                   EdgeTransitionLocation.Right :
                                   EdgeTransitionLocation.Left
                });
            }
            Window.Current.Activated += Current_Activated;

            popup.Width = width;
            popup.Height = Window.Current.Bounds.Height;
            popup.IsLightDismissEnabled = isLightDismiss;

            control.Width = width;
            control.Height = Window.Current.Bounds.Height;

            popup.Child = control;
            popup.SetValue(Canvas.LeftProperty, SettingsPane.Edge == SettingsEdgeLocation.Right ? (Window.Current.Bounds.Width - width) : 0);
            popup.SetValue(Canvas.TopProperty, 0);
            popup.IsOpen = true;
        }

        void Current_Activated(object sender, Windows.UI.Core.WindowActivatedEventArgs e)
        {
            if (e.WindowActivationState == Windows.UI.Core.CoreWindowActivationState.Deactivated)
            {
                popup.IsOpen = false;
                if (CloseAction != null)
                    CloseAction();
            }
        }

        void popup_Closed(object sender, object e)
        {
            Window.Current.Activated -= Current_Activated;
        }

    }
}
