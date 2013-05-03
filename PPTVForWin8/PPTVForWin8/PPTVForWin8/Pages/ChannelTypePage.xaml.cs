using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class ChannelTypePage : PPTVForWin8.Common.LayoutAwarePage
    {
        public ChannelTypePage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;
            this.DefaultViewModel["Items"] = ChannelTypeFactory.Instance.AllViewModel.Groups;

            channelPreviewGridView.SelectedIndex = -1;
        }

        private void PineToStart_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in channelPreviewGridView.SelectedItems)
            {
                var channelType = dataItem as ChannelTypeItem;
                if (channelType != null)
                {
                    ChannelTypeFactory.Instance.AddSelectedType(channelType);
                    TileUtils.CreateToastNotifications(string.Format("\"{0}\"被固定到首页", channelType.TypeName));
                }
            }
            channelPreviewGridView.SelectedIndex = -1;
        }

        private void unPineToStart_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in channelPreviewGridView.SelectedItems)
            {
                var channelType = dataItem as ChannelTypeItem;
                if (channelType != null)
                    ChannelTypeFactory.Instance.RemoveSelecedType(channelType);
            }
            channelPreviewGridView.SelectedIndex = -1;
        }

        private void channelPreviewGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelTypeItem;
            if (dataItem != null)
                Utils.PagesUtils.ChannelTypeNavagite(this, dataItem);
        }

        private void channelPreviewGridView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (channelPreviewGridView.SelectedItems.Count > 0)
            {
                BottomAppBar.IsSticky = true;
                BottomAppBar.IsOpen = true;

                var dataItem = channelPreviewGridView.SelectedItems[0] as ChannelTypeItem;
                if (dataItem != null)
                {
                    if (ChannelTypeFactory.Instance.SelectedViewModel.Contains(dataItem))
                    {
                        unPinToStartAppBar.Visibility = Visibility.Visible;
                        pinToStartAppBar.Visibility = Visibility.Collapsed;
                    }
                    else
                    {
                        pinToStartAppBar.Visibility = Visibility.Visible;
                        unPinToStartAppBar.Visibility = Visibility.Collapsed;
                    }
                }
            }
            else
            {
                BottomAppBar.IsSticky = false;
                BottomAppBar.IsOpen = false;
                pinToStartAppBar.Visibility = Visibility.Collapsed;
                unPinToStartAppBar.Visibility = Visibility.Collapsed;
            }
        }
    }
}
