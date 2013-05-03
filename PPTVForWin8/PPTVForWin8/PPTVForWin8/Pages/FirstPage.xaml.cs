using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Collections.ObjectModel;

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

// The Grouped Items Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234231

namespace PPTVForWin8.Pages
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;
    using PPTVData.Entity.WAY;
    using PPTVData.Factory.WAY;

    using PPTVForWin8.Pages;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary;
    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    /// <summary>
    /// A page that displays a grouped collection of items.
    /// </summary>
    public sealed partial class FirstPage : PPTVForWin8.Common.LayoutAwarePage
    {
        static CloudGetFactory _cloudRecentGetFactory;
        static CloudGetFactory _cloudFavolitenGetFactory;

        RecommendNavFactory _recommendNavFactory;
        RecommendListFactory _recommendChannelFactory;
        ChannelRecommendViewModel _recommendChannelViewModel;

        List<RecommandNav> _recommandNavs;
        int _recCount;
        int _currentRecIndex;
        int _currentWAY = -1;
        
        public FirstPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            ChannelTypeFactory.Instance.HttpFailorTimeOut += http_FailorTimeOut;
            this.DefaultViewModel["ChannelTypes"] = ChannelTypeFactory.Instance.SelectedViewModel;

            _recommendChannelViewModel = new ChannelRecommendViewModel();
            _recommendChannelViewModel.Groups = new ObservableCollection<ChannelRecommendGroup>();
            this.DefaultViewModel["ChannelLists"] = _recommendChannelViewModel.Groups;

            _recommendNavFactory = new RecommendNavFactory();
            _recommendNavFactory.HttpSucessHandler += recommandNav_Successed;
            _recommendNavFactory.HttpFailorTimeOut += http_FailorTimeOut;

            LocalRecentFactory.Instance.LocalChangeAction += LoadRecentDatas;
            LocalFavoritesFactory.Instance.LocalChangeAction += LoadFavDatas;

            _cloudRecentGetFactory = new CloudGetFactory();
            _cloudRecentGetFactory.HttpSucessHandler += cloudRecentGetFactory_HttpSucess;

            _cloudFavolitenGetFactory = new CloudGetFactory();
            _cloudFavolitenGetFactory.HttpSucessHandler += cloudFavolitenGetFactory_HttpSucess;

            var colCount = ScreenUtils.CreateColNum(160, 255);
            _recCount = colCount * 3;
            var channelTypeHeight = (180 * colCount) + 50;
            ChannelTypeFactory.Instance.ColNumber = channelTypeHeight / 130;
            channelTypeGridView.MaxHeight = channelTypeHeight;

            slidesShowControl.Init(http_FailorTimeOut);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (_currentWAY == -1
                || _currentWAY != WAYGetFactory.WayGetInfo.UserType)
            {
                _currentWAY = WAYGetFactory.WayGetInfo.UserType;
                if (_currentWAY == -1)
                    MainPage.InitWAY();
                RequestDatas();
            }
            slidesShowControl.StartSlidesShow();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            CloseTips();

            slidesShowControl.Cancel();
            slidesShowControl.EndSlidesShow();
            _recommendNavFactory.Cancel();
        }

        void RequestDatas()
        {
            if (loadingTip.Visibility == Visibility.Collapsed)
            {
                ShowTips();
                _recommendChannelViewModel.Groups.PerformanceClear();
                ChannelTypeFactory.Instance.Clear();

                slidesShowControl.RequestDatas();
                ChannelTypeFactory.Instance.DownloadDatas();
                _recommendNavFactory.DownLoadDatas();
                InitCloud();
            }
        }

        private void InitCloud()
        {
            LoadRecentDatas();
            LoadFavDatas();

            if (PersonalFactory.Instance.Logined)
            {
                var personalInfo = PersonalFactory.Instance.DataInfos[0];
                _cloudRecentGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Recent);
                _cloudFavolitenGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Favorites);
            }
        }

        #region HttpHandler

        void recommandNav_Successed(object sender, HttpFactoryArgs<List<RecommandNav>> args)
        {
            _recommandNavs = args.Result;

            if (_recommandNavs.Count > 0 )
            {
                _currentRecIndex = 0;
                _recommendChannelFactory = new RecommendListFactory();
                _recommendChannelFactory.HttpSucessHandler += recommandChannelFactory_GetChannelsSucceed;
                _recommendChannelFactory.HttpFailorTimeOut += http_FailorTimeOut;
                getRecommand();
            }
        }

        void recommandChannelFactory_GetChannelsSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            
            if (args.Result.Channels.Count <= 0) { _currentRecIndex++; return; }

            var group = new ChannelRecommendGroup();
            group.RecommendIndex = _currentRecIndex;
            group.GroupName = _recommandNavs[_currentRecIndex].Name;
            group.ItemsGroup = new ObservableCollection<ChannelRecommendListItem>();
            _recommendChannelViewModel.Groups.Add(group);

            for (var i = 0; i < args.Result.Channels.Count; i++)
            {
                var dataItem = new ChannelRecommendListItem()
                {
                    ChannelId = args.Result.Channels[i].Id,
                    ChannelFlag = args.Result.Channels[i].ChannelFlag,
                    ImageUri = args.Result.Channels[i].ImageUri,
                    Title = args.Result.Channels[i].Title,
                    Mark = args.Result.Channels[i].Mark
                };
                group.ItemsGroup.Add(dataItem);
            }

            _currentRecIndex++;
            if (_currentRecIndex < _recommandNavs.Count)
                getRecommand();
            else
                CloseTips();
        }

        void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            CommonUtils.ShowMessageConfirm(Constants.NotNetworkTip, navigate_Download, RequestDatas, "确定", "刷新");
        }

        void navigate_Download()
        {
            this.Frame.Navigate(typeof(Pages.DownloadedPage));
        }

        void getRecommand()
        {
            _recommendChannelFactory.DownLoadDatas(_recCount, 1, _recommandNavs[_currentRecIndex].Navid);    
        }

        void cloudRecentGetFactory_HttpSucess(object sender, HttpFactoryArgs<CloudArgs> args)
        {
            LocalRecentFactory.Instance.InsertCloudRecords(args.Result.Datas);
        }

        void cloudFavolitenGetFactory_HttpSucess(object sender, HttpFactoryArgs<CloudArgs> args)
        {
            LocalFavoritesFactory.Instance.InsertCloudRecords(args.Result.Datas);
        }

        #endregion

        #region Cloud Events

        void LoadRecentDatas()
        {
            recentList.ItemsSource = LocalRecentFactory.Instance.DataInfos.Take(_recCount / 3);
            if (recentList.Items.Count > 0)
            {
                recentList.Visibility = Visibility.Visible;
                noHistoryResult.Visibility = Visibility.Collapsed;
                gridViewItemHistory.Width = 410;
            }
            else
            {
                noHistoryResult.Visibility = Visibility.Visible;
                recentList.Visibility = Visibility.Collapsed;
                gridViewItemHistory.Width = 235;
            }
        }

        void LoadFavDatas()
        {
            favList.ItemsSource = LocalFavoritesFactory.Instance.DataInfos.Take(_recCount / 3);
            if (favList.Items.Count > 0)
            {
                favList.Visibility = Visibility.Visible;
                noFavResult.Visibility = Visibility.Collapsed;
                gridViewItemFavorite.Width = 410;
            }
            else
            {
                noFavResult.Visibility = Visibility.Visible;
                favList.Visibility = Visibility.Collapsed;
                gridViewItemFavorite.Width = 235;
            }
        }

        /// <summary>
        /// 删除单个最近观看或我的收藏
        /// </summary>
        private void RemoveSingle_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in recentList.SelectedItems)
            {
                var data = dataItem as CloudDataInfo;
                if (data != null)
                    LocalRecentFactory.Instance.DeleteRecord(data.Id);
            }

            foreach (var dataItem in favList.SelectedItems)
            {
                var data = dataItem as CloudDataInfo;
                if (data != null)
                    LocalFavoritesFactory.Instance.DeleteRecord(data.Id);
            }

            BottomAppBar.IsOpen = false;
        }

        /// <summary>
        /// 清空所有最近观看或我的收藏
        /// </summary>
        private void RemoveAll_Click(object sender, RoutedEventArgs e)
        {
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.ClearConfirm,
                () =>
                {
                    if (recentList.SelectedItems.Count > 0)
                    {
                        LocalRecentFactory.Instance.RemoveAll();
                    }
                    else if (favList.SelectedItems.Count > 0)
                    {
                        LocalFavoritesFactory.Instance.RemoveAll();
                    }
                });
        }

        private void recentList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (recentList.SelectedItems.Count > 0)
                ClearListSelector(recentList, true);
            else
                ClearListSelector(null, false);
        }

        private void favList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (favList.SelectedItems.Count > 0)
                ClearListSelector(favList, true);
            else
                ClearListSelector(null, false);
        }

        private void recentList_ItemClick(object sender, ItemClickEventArgs e)
        {
            var recent = e.ClickedItem as CloudDataInfo;
            if (recent != null)
            {
                var info = new PlayInfoHelp(recent.Id, recent.ProgramIndex);
                CommonUtils.DACNavagate(this.Frame, DACPageType.History, typeof(PlayPage), info);
            }
        }

        private void favList_ItemClick(object sender, ItemClickEventArgs e)
        {
            var fav = e.ClickedItem as CloudDataInfo;
            if (fav != null)
            {
                CommonUtils.DACNavagate(this.Frame, DACPageType.FAV, typeof(DetailPage), fav.Id);
            }
        }

        #endregion

        /// <summary>
        /// 登录成功回调
        /// </summary>
        public static void LoginSucess_Handler()
        {
            TileUtils.CreateToastNotifications("登录成功");
            PersonalFactory.Instance.DataInfos[0].IsCurrentLogin = true;
            PersonalInfo personalInfo = PersonalFactory.Instance.DataInfos[0];

            MainPage.InitWAY();
            _cloudRecentGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Recent);
            _cloudFavolitenGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Favorites);
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            RequestDatas();
            BottomAppBar.IsOpen = false;
        }

        private async void PineToStart_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in recentList.SelectedItems)
                await TileUtils.Pin_Start(dataItem, true, Utils.Constants.LogoUri);

            foreach (var dataItem in favList.SelectedItems)
                await TileUtils.Pin_Start(dataItem, false, Utils.Constants.LogoUri);

            ClearListSelector(null, true);
        }

        private void unPineToStart_Click(object sender, RoutedEventArgs e)
        {
            foreach (var dataItem in channelTypeGridView.SelectedItems)
            {
                var channelType = dataItem as ChannelTypeItem;
                if (channelType != null)
                    ChannelTypeFactory.Instance.RemoveSelecedType(channelType);
            }
            ClearListSelector(null, true);
        }

        private void itemGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelRecommendListItem;
            if (dataItem != null)
                this.Frame.Navigate(typeof(DetailPage), dataItem.ChannelId);

            var channelType = e.ClickedItem as ChannelTypeItem;
            if (channelType != null)
                PagesUtils.ChannelTypeNavagite(this, channelType);
        }

        private void HeadGrid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            FrameworkElement grid = sender as Grid;
            if (grid == null) grid = sender as StackPanel;
            if (grid != null)
            {
                var headName = grid.Tag as string;
                if (!string.IsNullOrEmpty(headName))
                {
                    switch (headName)
                    {
                        case "推荐":
                            this.Frame.Navigate(typeof(ChannelCoverPage)); break;
                        case "频道":
                            this.Frame.Navigate(typeof(ChannelTypePage)); break;
                        case "最近观看":
                            this.Frame.Navigate(typeof(HistoryPage)); break;
                        case "我的收藏":
                            this.Frame.Navigate(typeof(FavoritenPage)); break;
                        default:
                            break;
                    }
                }
                else
                {
                    var id = Convert.ToInt32(grid.Tag);
                    this.Frame.Navigate(typeof(ChannelRecommendPage), _recommandNavs[id]);
                }
            }
        }

        private void channelItemsListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (channelTypeGridView.SelectedItems.Count > 0)
                ClearListSelector(channelTypeGridView, true);
            else
                ClearListSelector(null, false);
        }

        private void ClearListSelector(Selector showSelector, bool changeSelectedIndex)
        {
            if (showSelector == null)
            {
                if (changeSelectedIndex)
                    recentList.SelectedIndex = favList.SelectedIndex = channelTypeGridView.SelectedIndex = -1;
                LeftCommands.Visibility =  unPinToStartAppBar.Visibility = Visibility.Collapsed;
                BottomAppBar.IsSticky = false;
                BottomAppBar.IsOpen = false;
            }
            else
            {
                if (showSelector == channelTypeGridView)
                {
                    if (changeSelectedIndex)
                        favList.SelectedIndex = recentList.SelectedIndex = -1;
                    LeftCommands.Visibility = Visibility.Collapsed;
                    unPinToStartAppBar.Visibility = Visibility.Visible;
                }
                else
                {
                    if (changeSelectedIndex)
                    {
                        if (recentList != showSelector)
                            recentList.SelectedIndex = -1;
                        else
                            favList.SelectedIndex = -1;
                        channelTypeGridView.SelectedIndex = -1;
                    }
                    LeftCommands.Visibility = Visibility.Visible;
                    unPinToStartAppBar.Visibility = Visibility.Collapsed;
                }
                BottomAppBar.IsSticky = true;
                BottomAppBar.IsOpen = true;
            }
        }

        private void ShowTips()
        {
            loadingTip.Visibility = Visibility.Visible;
            recommendControl.Visibility = Visibility.Collapsed;
            channelTypeControl.Visibility = Visibility.Collapsed;
            channleRecommendGridView.Visibility = Visibility.Collapsed;
            gridViewItemHistory.Visibility = Visibility.Collapsed;
            gridViewItemFavorite.Visibility = Visibility.Collapsed;
        }

        private void CloseTips()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            recommendControl.Visibility = Visibility.Visible;
            channelTypeControl.Visibility = Visibility.Visible;
            channleRecommendGridView.Visibility = Visibility.Visible;
            gridViewItemHistory.Visibility = Visibility.Visible;
            gridViewItemFavorite.Visibility = Visibility.Visible;
        }
    }
}
