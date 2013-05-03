using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Controls;

namespace PPTVForWin8.Utils
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.Cloud;

    using PPTVForWin8.Pages;
    using PPTVForWin8.Controls;

    public static class CommonUtils
    {
        private static bool _isShowDialog;
        private static bool _isDownloadSelecting;

        public static Action DownloadSelectAction;

        /// <summary>
        /// 弹出提示框
        /// </summary>
        /// <param name="message"></param>
        public async static void ShowMessage(string message, string title = ":(")
        {
            if (_isDownloadSelecting || _isShowDialog) return;

            var dialog = new MessageDialog(message, title);
            _isShowDialog = true;
            await dialog.ShowAsync();
            _isShowDialog = false;
        }

        /// <summary>
        /// 弹出是否确定提示框
        /// </summary>
        /// <param name="message"></param>
        /// <param name="handler"></param>
        /// <returns></returns>
        public static async void ShowMessageConfirm(string message, Action handler, string tipContent = "确定")
        {
            if (_isDownloadSelecting || _isShowDialog) return;

            var dialog = new MessageDialog(message, "提示");
            dialog.Commands.Add(new UICommand()
            {
                Id = tipContent,
                Label = tipContent,
                Invoked = null
            });
            dialog.Commands.Add(new UICommand()
            {
                Id = "取消",
                Label = "取消",
                Invoked = null
            });
            _isShowDialog = true;
            var cmd = await dialog.ShowAsync();
            _isShowDialog = false;
            if (cmd.Id.ToString() == tipContent)
                handler();
        }

        public static async void ShowMessageConfirm(string message,
            Action handler1, Action handler2, string tip1, string tip2)
        {
            if (_isDownloadSelecting || _isShowDialog) return;

            var dialog = new MessageDialog(message, "提示");
            dialog.Commands.Add(new UICommand()
            {
                Id = tip1,
                Label = tip1,
                Invoked = null
            });
            dialog.Commands.Add(new UICommand()
            {
                Id = tip2,
                Label = tip2,
                Invoked = null
            });
            _isShowDialog = true;
            var cmd = await dialog.ShowAsync();
            _isShowDialog = false;
            if (cmd.Id.ToString() == tip1)
                handler1();
            else
                handler2();
        }

        /// <summary>
        /// 显示登录对话框
        /// </summary>
        public static void ShowUserLogin(EventHandler<RoutedEventArgs> loginSucessHandler)
        {
            var loginWin = new UserLogin();
            loginWin.LoginSucessHandler += loginSucessHandler;
            loginWin.Width = Window.Current.CoreWindow.Bounds.Width;
            loginWin.Height = Window.Current.CoreWindow.Bounds.Height;
            loginWin.Popup.Child = loginWin;
            loginWin.Popup.IsOpen = true;
        }

        public static Frame GetCurrentFrame()
        {
            var rootFrame = Window.Current.Content as Frame;
            if (rootFrame != null)
            {
                var mainPage = rootFrame.Content as MainPage;
                if (mainPage != null)
                {
                    return mainPage.MainPageFrame;
                }
            }
            return new Frame();
        }

        /// <summary>
        /// 返回当前页面的实例
        /// </summary>
        /// <returns></returns>
        public static Page GetCurrentPage()
        {
            var rootFrame = GetCurrentFrame();
            if (rootFrame != null)
                return rootFrame.Content as Page;
            return null;
        }

        public static void DACNavagate(Frame frame, DACPageType dacType, Type Page, object paras)
        {
            PlayPage.DACPageType = dacType;
            frame.Navigate(Page, paras);
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
            Storyboard.SetTargetProperty(timeline, path);
        }

        public static BitmapImage GetImage(string imageUrl)
        {
            if (!string.IsNullOrEmpty(imageUrl))
                return new BitmapImage(new Uri(imageUrl, UriKind.RelativeOrAbsolute));
            return null;
        }

        public static TextBlock CreateComoboxItem(string text, object tag)
        {
            var textBlock = new TextBlock();
            textBlock.FontSize = 40;
            textBlock.Foreground = App.Current.Resources["PPTVBlueColor"] as SolidColorBrush;
            textBlock.Text = text;
            textBlock.Tag = tag;
            return textBlock;
        }

        public static async void DownloadFolderSelect()
        {
            var folderPicker = new Windows.Storage.Pickers.FolderPicker();
            folderPicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.Desktop;
            folderPicker.FileTypeFilter.Add(".mp4");
            _isDownloadSelecting = true;
            var folder = await folderPicker.PickSingleFolderAsync();
            _isDownloadSelecting = false;
            if (folder != null)
            {
                Windows.Storage.AccessCache.StorageApplicationPermissions.FutureAccessList.AddOrReplace(PPTV.WinRT.CommonLibrary.ViewModel.Download.DownloadBase.DownloadFolderToken, folder);
                if (DownloadSelectAction != null)
                    DownloadSelectAction();
            }
        }
    }
}
