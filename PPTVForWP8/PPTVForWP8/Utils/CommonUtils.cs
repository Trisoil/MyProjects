using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Navigation;
using System.Windows.Media.Imaging;

using Microsoft.Phone.Controls;

namespace PPTVForWP8.Utils
{
    using PPTVData.Entity;
    using PPTVForWP8.Pages;
    using PPTV.WPRT.CommonLibrary.Utils;

    public static class CommonUtils
    {
        public static void ShowMessageConfirm(string message, Action handler)
        {
            if (MessageBox.Show(message, "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                handler();
            }
        }

        public static PhoneApplicationPage GetCurrentPage()
        {
            var rootFrame = App.RootFrame;
            if (rootFrame != null)
                return rootFrame.Content as PhoneApplicationPage;
            return null;
        }

        public static void Navagate(string uri)
        { 
            var page = GetCurrentPage();
            if (page != null)
            {
                page.NavigationService.Navigate(new Uri(uri, UriKind.Relative));
            }
        }

        public static void DACNavagate(DACPageType dacType, string uri)
        {
            var page = GetCurrentPage();
            if (page != null)
            {
                ChannelUtils.DACPageType = dacType;
                page.NavigationService.Navigate(new Uri(uri, UriKind.Relative));
            }
        }

        public static void ClearApplicationBar(PhoneApplicationPage page)
        {
            while (page.ApplicationBar.Buttons.Count > 0)
            {
                page.ApplicationBar.Buttons.RemoveAt(0);
            }
        }

        public static Color GetColor(string colorName)
        {
            if (colorName.StartsWith("#"))
                colorName = colorName.Replace("#", string.Empty);
            int v = int.Parse(colorName, System.Globalization.NumberStyles.HexNumber);
            return new Color()
            {
                A = Convert.ToByte((v >> 24) & 255),
                R = Convert.ToByte((v >> 16) & 255),
                G = Convert.ToByte((v >> 8) & 255),
                B = Convert.ToByte((v >> 0) & 255)
            };
        }
        
        public static BitmapImage GetImage(string imageUrl)
        {
            if (!string.IsNullOrEmpty(imageUrl))
                return new BitmapImage(new Uri(imageUrl, UriKind.RelativeOrAbsolute));
            return null;
        }

        public static Storyboard GenerateStoryboardByTwoHPoint(UIElement uc, double originalPoint, double currentPoint, TimeSpan timeSpan, bool hasOriginalPoint = true, bool hasEffect = false)
        {
            var daukf = new DoubleAnimationUsingKeyFrames();
            if (hasOriginalPoint)
            {
                if (hasEffect)
                    daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = new TimeSpan(0, 0, 0), Value = originalPoint, EasingFunction = new BackEase() { Amplitude = 0.65 } });
                else
                    daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = new TimeSpan(0, 0, 0), Value = originalPoint, EasingFunction = new QuadraticEase() { EasingMode = EasingMode.EaseInOut } });
            }
            if (hasEffect)
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = timeSpan, Value = currentPoint, EasingFunction = new BackEase() { Amplitude = 0.65 } });
            else
                daukf.KeyFrames.Add(new EasingDoubleKeyFrame() { KeyTime = timeSpan, Value = currentPoint, EasingFunction = new QuadraticEase() { EasingMode = EasingMode.EaseInOut } });
            StoryboardBindTargetAndProperty(daukf, uc, "(UIElement.RenderTransform).(TransformGroup.Children)[0].(TranslateTransform.X)");
            Storyboard sb = new Storyboard();
            sb.Children.Add(daukf);
            return sb;
        }

        public static void StoryboardBindTargetAndProperty(Timeline timeline, DependencyObject target, string path)
        {
            Storyboard.SetTarget(timeline, target);
            Storyboard.SetTargetProperty(timeline, new PropertyPath(path));
        }
    }
}
