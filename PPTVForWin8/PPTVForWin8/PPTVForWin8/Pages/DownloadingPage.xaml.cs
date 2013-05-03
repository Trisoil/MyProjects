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

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTV.WinRT.CommonLibrary.DataModel.Download;
    using PPTV.WinRT.CommonLibrary.ViewModel.Download;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class DownloadingPage : PPTVForWin8.Common.LayoutAwarePage
    {
        public DownloadingPage()
        {
            this.InitializeComponent();

            this.DefaultViewModel["Items"] = DownloadViewModel.Instance.DownloadingItems;
        }

        private void pauseAllAppBar_Click(object sender, RoutedEventArgs e)
        {
            DownloadViewModel.Instance.PauseAllDownload();
        }

        private void deleteSingleAppBar_Click(object sender, RoutedEventArgs e)
        {
            var list = new List<DownloadInfo>(downloadingGridView.SelectedItems.Cast<DownloadInfo>());
            DownloadViewModel.Instance.DeletedDownloadings(list);
        }

        private void deleteAllAppBar_Click(object sender, RoutedEventArgs e)
        {
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.ClearConfirm, DownloadViewModel.Instance.DeletedAllDownloading);
        }

        private void downloadingGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var downInfo = e.ClickedItem as DownloadInfo;
            if (downInfo != null)
            {
                DownloadViewModel.Instance.HandleDownload(downInfo);
            }
        }

        private void downloadingListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (downloadingGridView.SelectedItems.Count > 0)
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
    }
}
