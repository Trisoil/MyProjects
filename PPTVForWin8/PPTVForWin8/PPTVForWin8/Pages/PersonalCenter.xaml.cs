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
using Windows.UI.Popups;

// The Item Detail Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234232

namespace PPTVForWin8.Pages
{
    using PPTVData.Factory;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.DataModel.Download;
    using PPTV.WinRT.CommonLibrary.ViewModel.Download;

    /// <summary>
    /// A page that displays details for a single item within a group while allowing gestures to
    /// flip through other items belonging to the same group.
    /// </summary>
    public sealed partial class PersonalCenter : PPTVForWin8.Common.LayoutAwarePage
    {
        static bool _isDownloading = true;
        int _colNum;

        public PersonalCenter()
        {
            this.InitializeComponent();

            _colNum = ScreenUtils.CreateColNum(160, 291);

            if (_isDownloading)
                toggleDownloading_Click(this, null);
            else
                toggleDownloaded_Click(this, null);

            if (PersonalFactory.Instance.Logined)
            {
                LayoutContent.Visibility = Visibility.Visible;
                loginButton.Visibility = Visibility.Collapsed;
                LayoutContent.DataContext = PersonalFactory.Instance.DataInfos[0].UserStateInfo;
            }
            else
            {
                loginButton.Visibility = Visibility.Visible;
                LayoutContent.Visibility = Visibility.Collapsed;
            }

            DownPathSelected();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            DownloadViewModel.Instance.DownloadingCountChannge += ChangeDownloadingElement;
            CommonUtils.DownloadSelectAction += DownPathSelected;
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            DownloadViewModel.Instance.DownloadingCountChannge -= ChangeDownloadingElement;
            CommonUtils.DownloadSelectAction -= DownPathSelected;
            if (e.NavigationMode == NavigationMode.Back)
                _isDownloading = true;
        }

        private async void DownPathSelected()
        {
            var downFolder = await DownloadViewModel.GetDownloadFolder();
            if (downFolder != null)
            {
                var path = downFolder.Path;
                if (string.IsNullOrEmpty(path))
                {
                    var folder = downFolder as Windows.Storage.StorageFolder;
                    if (folder != null)
                        path = string.Format("{0}\\{1}", folder.DisplayType, folder.DisplayName);
                }
                var result = string.Empty;
                if (path.Length <= 21)
                    result = path;
                else
                    result = string.Format("{0}...{1}", path.Substring(0, 10), path.Substring(path.Length - 11, 11));
                downPathText.Text = result;
            }
        }

        /// <summary>
        /// 登录
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void loginButton_Tapped(object sender, TappedRoutedEventArgs e)
        {
            Utils.CommonUtils.ShowUserLogin((ss, ee) =>
            {
                FirstPage.LoginSucess_Handler();
                LayoutContent.Visibility = Visibility.Visible;
                loginButton.Visibility = Visibility.Collapsed;
                LayoutContent.DataContext = PersonalFactory.Instance.DataInfos[0].UserStateInfo;
            });
        }

        /// <summary>
        /// 注销登录
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnLoginOut_Click(object sender, RoutedEventArgs e)
        {
            CommonUtils.ShowMessageConfirm(
                string.Format(Utils.Constants.LogginOut, PersonalFactory.Instance.DataInfos[0].UserStateInfo.UserName),
                () =>
                {
                    PersonalFactory.Instance.LoginOut();
                    loginButton.Visibility = Visibility.Visible;
                    LayoutContent.Visibility = Visibility.Collapsed;
                });
        }

        private void downloadMore_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var item = sender as StackPanel;
            if (item != null)
            {
                if (item.Name == "downloadingMore")
                {
                    this.Frame.Navigate(typeof(DownloadingPage));
                }
                else
                {
                    this.Frame.Navigate(typeof(DownloadedPage));
                }
            }
        }

        private void toggleDownloading_Click(object sender, RoutedEventArgs e)
        {
            _isDownloading = true;
            toggleDownloading.IsChecked = true;
            toggleDownloaded.IsChecked = false;
            pauseAllAppBar.Visibility = Visibility.Visible;
            ChangeDownloadingElement();
            downloadedListView.SelectedIndex = -1;
        }

        private void toggleDownloaded_Click(object sender, RoutedEventArgs e)
        {
            _isDownloading = false;
            toggleDownloading.IsChecked = false;
            toggleDownloaded.IsChecked = true;
            pauseAllAppBar.Visibility = Visibility.Collapsed;
            ChangeDownloadedElement();
            downloadingListView.SelectedIndex = -1;
        }

        private void ChangeDownloadingElement()
        {
            this.DefaultViewModel["DownloadingItems"] = DownloadViewModel.Instance.DownloadingItems.Take(_colNum);

            downloadingPanel.Visibility = Visibility.Visible;
            downloadedPanel.Visibility = Visibility.Collapsed;
            if (downloadingListView.Items.Count > 0)
            {
                downloadingListView.Visibility = Visibility.Visible;
                downloadingMore.Visibility = Visibility.Visible;
                noDonloadingResult.Visibility = Visibility.Collapsed;
                pauseAllAppBar.Visibility = Visibility.Visible;
            }
            else
            {
                noDonloadingResult.Visibility = Visibility.Visible;
                downloadingListView.Visibility = Visibility.Collapsed;
                downloadingMore.Visibility = Visibility.Collapsed;
                pauseAllAppBar.Visibility = Visibility.Collapsed;
            }
        }

        private void ChangeDownloadedElement()
        {
            this.DefaultViewModel["DownloadedItems"] = DownloadViewModel.Instance.DownloadedItems.Take(_colNum);

            downloadedPanel.Visibility = Visibility.Visible;
            downloadingPanel.Visibility = Visibility.Collapsed;
            pauseAllAppBar.Visibility = Visibility.Collapsed;
            if (downloadedListView.Items.Count > 0)
            {
                downloadedListView.Visibility = Visibility.Visible;
                downloadedMore.Visibility = Visibility.Visible;
                noDonloadedResult.Visibility = Visibility.Collapsed;
            }
            else
            {
                noDonloadedResult.Visibility = Visibility.Visible;
                downloadedListView.Visibility = Visibility.Collapsed;
                downloadedMore.Visibility = Visibility.Collapsed;
            }
        }

        private void downloadingListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var downInfo = e.ClickedItem as DownloadInfo;
            if (downInfo != null)
            {
                DownloadViewModel.Instance.HandleDownload(downInfo);
            }
        }

        private void downloadListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (downloadingListView.SelectedItems.Count > 0
                || downloadedListView.SelectedItems.Count > 0)
            {
                BottomAppBar.IsSticky = true;
                BottomAppBar.IsOpen = true;
                deleteSingleAppBar.Visibility = Visibility.Visible;
            }
            else
            {
                BottomAppBar.IsSticky = false;
                BottomAppBar.IsOpen = false;
                deleteSingleAppBar.Visibility = Visibility.Collapsed;
            }
        }

        private void downloadSelect_Tapped(object sender, TappedRoutedEventArgs e)
        {
            CommonUtils.DownloadFolderSelect();
        }

        #region AppBar Handler

        private void downloadedListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var downInfo = e.ClickedItem as DownloadInfo;
            if (downInfo != null)
            {
                CommonUtils.DACNavagate(this.Frame, PPTVData.Entity.DACPageType.LOCALPLAY, typeof(PlayPage), downInfo);
            }
        }

        private void pauseAllAppBar_Click(object sender, RoutedEventArgs e)
        {
            //if (downloadComboBox.SelectedIndex == 0)
            if (toggleDownloading.IsChecked == true) 
                DownloadViewModel.Instance.PauseAllDownload();
        }

        private void deleteSingleAppBar_Click(object sender, RoutedEventArgs e)
        {
            if (downloadingListView.SelectedItems.Count > 0)
            {
                var list = new List<DownloadInfo>(downloadingListView.SelectedItems.Cast<DownloadInfo>());
                DownloadViewModel.Instance.DeletedDownloadings(list);
            }
            else if (downloadedListView.SelectedItems.Count > 0)
            {
                var list = new List<DownloadInfo>(downloadedListView.SelectedItems.Cast<DownloadInfo>());
                DownloadViewModel.Instance.DeletedDownloadeds(list);
            }
            
            if (downloadedListView.SelectedItems.Count > 0)
                ChangeDownloadedElement();
        }

        private void deleteAllAppBar_Click(object sender, RoutedEventArgs e)
        {
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.ClearConfirm,
            () =>
            {
                //if (downloadComboBox.SelectedIndex == 0)
                if (toggleDownloading.IsChecked == true)
                {
                    DownloadViewModel.Instance.DeletedAllDownloading();
                }
                else
                {
                    DownloadViewModel.Instance.DeletedAllDownloaded();
                    ChangeDownloadedElement();
                }
            });
        }

        #endregion
    }
}
