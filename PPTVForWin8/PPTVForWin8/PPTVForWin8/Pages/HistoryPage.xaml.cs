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
using Windows.UI.StartScreen;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTVData.Entity;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;

    using PPTV.WinRT.CommonLibrary.ViewModel;
    using PPTV.WinRT.CommonLibrary.Utils;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class HistoryPage : PPTVForWin8.Common.LayoutAwarePage
    {
        public HistoryPage()
        {
            this.InitializeComponent();

            this.DefaultViewModel["Items"] = LocalRecentFactory.Instance.DataInfos;
            historyListView.SelectedIndex = -1;
        }

        private void historyListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var recent = e.ClickedItem as CloudDataInfo;
            if (recent != null)
            {
                var info = new PlayInfoHelp(recent.Id, recent.ProgramIndex);
                Utils.CommonUtils.DACNavagate(this.Frame, PPTVData.Entity.DACPageType.History,
                    typeof(PlayPage), info);
            }
        }

        private void historyListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (historyListView.SelectedItems.Count > 0)
            {
                BottomAppBar.IsSticky = true;
                BottomAppBar.IsOpen = true;
                deleteSingleAppBar.Visibility = Visibility.Visible;
                pinToStartAppBar.Visibility = Visibility.Visible;
            }
            else
            {
                BottomAppBar.IsSticky = false;
                BottomAppBar.IsOpen = false;
                deleteSingleAppBar.Visibility = Visibility.Collapsed;
                pinToStartAppBar.Visibility = Visibility.Collapsed;
            }
        }

        private void RemoveSingle_Click(object sender, RoutedEventArgs e)
        {
            foreach(var dataItem in historyListView.SelectedItems)
            {
                var data = dataItem as CloudDataInfo;
                if (data != null)
                    LocalRecentFactory.Instance.DeleteRecord(data.Id);
            }
            BottomAppBar.IsOpen = false;
        }

        private void RemoveAll_Click(object sender, RoutedEventArgs e)
        {
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.ClearConfirm, LocalRecentFactory.Instance.RemoveAll);
        }

        private async void PineToStart_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in historyListView.SelectedItems)
            {
                await TileUtils.Pin_Start(dataItem, true, Utils.Constants.LogoUri);
            }
            historyListView.SelectedIndex = -1;
        }
    }
}
