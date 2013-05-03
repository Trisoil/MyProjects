using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.ApplicationModel.Search;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Factory;
    using PPTVForWin8.Pages;

    using PPTV.WinRT.CommonLibrary.ViewModel.Download;

    public sealed partial class TopTipControl : UserControl
    {
        public TopTipControl()
        {
            this.InitializeComponent();

            UpdatgeLoginTip(PersonalFactory.Instance.Logined);
            UpdateDownloadTip();

            PersonalFactory.Instance.LoginChangeAction += UpdatgeLoginTip;
            DownloadViewModel.Instance.DownloadingCountChannge += UpdateDownloadTip;
        }

        public bool IsShowSearch
        {
            set {
                if (!value)
                    searchImage.Visibility = Visibility.Collapsed;
            }
        }

        public bool IsShowLogin
        {
            set {
                if (!value)
                {
                    loginImage.Visibility = Visibility.Collapsed;
                    downloadingTip.Visibility = Visibility.Collapsed;
                }
            }
        }

        private void searchImage_Tapped(object sender, TappedRoutedEventArgs e)
        {
            SearchPane.GetForCurrentView().Show(string.Empty);
        }

        private void loginImage_Tapped(object sender, TappedRoutedEventArgs e)
        {
            Utils.CommonUtils.GetCurrentFrame().Navigate(typeof(PersonalCenter));
        }

        private void UpdatgeLoginTip(bool isLogin)
        {
            if (isLogin)
                loginImage.Source = Utils.Constants.LoginedImage;
            else
                loginImage.Source = Utils.Constants.LoginImage;
        }

        private void UpdateDownloadTip()
        {
            var count = DownloadViewModel.Instance.DownloadingItems.Count;
            if (count <= 0)
            {
                downloadingTip.Visibility = Visibility.Collapsed;
            }
            else
            {
                downloadingTip.Visibility = Visibility.Visible;
                downloadingCount.Text = count.ToString();
            }
        }
    }
}
