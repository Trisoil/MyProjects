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

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTVData;
    using PPTVData.Factory;
    using PPTVData.Entity.Live;
    using PPTVData.Factory.Live;

    using PPTVForWin8.Utils;
    using PPTVForWin8.Common;
    using PPTVForWin8.Controls;

    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;
    using PPTV.WinRT.CommonLibrary.Utils;

    public sealed partial class LiveChannelPage : PPTVForWin8.Common.LayoutAwarePage
    {
        int _groupIndex = 0;
        int _colCount = 4;
        bool _isClickMore;

        static bool _satelliteMore;
        static bool _localtvMore;
        static double _scrollOffset;

        LiveListFactory _listFactory;
        LiveCenterSportsFactory _sportsFactory;
        LiveCenterGameFacotry _gamesFactory;

        LiveListViewModel _viewModel;
        LiveDetailControl _liveDetailControl;
        SettingsFlyout _settingsFlyout;
        ObservableCollection<LiveListGroup> _liveCenterViewModel;

        public LiveChannelPage()
        {
            this.InitializeComponent();

            _listFactory = new LiveListFactory();
            _listFactory.HttpSucessHandler += listFactory_HttpSucessed;
            _listFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _sportsFactory = new LiveCenterSportsFactory();
            _sportsFactory.HttpSucessHandler += liveCenter_HttpSucessed;
            _sportsFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _gamesFactory = new LiveCenterGameFacotry();
            _gamesFactory.HttpSucessHandler += liveCenter_HttpSucessed;
            _gamesFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _viewModel = new LiveListViewModel();
            _viewModel.Groups = new ObservableCollection<LiveListGroup>();
            this.DefaultViewModel["LiveChannels"] = _viewModel.Groups;

            _liveCenterViewModel = new ObservableCollection<LiveListGroup>();
            this.DefaultViewModel["LiveCenterSource"] = _liveCenterViewModel;

            _colCount = ScreenUtils.CreateColNum(119, 260);
            var gridHeight = 129 * _colCount + 30;
            satelliteGridView.MaxHeight = gridHeight;
            localtvGridView.MaxHeight = gridHeight;
            livecenterGridView.MaxHeight = gridHeight + 60;

            RequestDatas();
        }

        public LiveListViewModel ViewModel
        {
            get { return _viewModel; }
        }

        public Visibility LoadingTipVisibility
        {
            get { return loadingTip.Visibility; }
            set { loadingTip.Visibility = value; }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            liveDetailClose();
            if (e.NavigationMode == NavigationMode.Back)
            {
                _scrollOffset = 0;
                _satelliteMore = _localtvMore = false;
            }
            else
            {
                _scrollOffset = rootScroolViewer.HorizontalOffset;
            }
            _listFactory.Cancel();
            _sportsFactory.Cancel();
            _gamesFactory.Cancel();
        }

        void RequestDatas()
        {
            ShowTips();
            _isClickMore = false;
            _groupIndex = 0;
            _liveCenterViewModel.Clear();
            _viewModel.Groups.PerformanceClear();

            if (_satelliteMore)
            {
                _listFactory.DownLoadDatas((int)LiveType.Satellite, 200, 1);
                satelliteMore.Visibility = Visibility.Collapsed;
            }
            else
            {
                _listFactory.DownLoadDatas((int)LiveType.Satellite, _colCount * 2, 1);
                satelliteMore.Visibility = Visibility.Visible;
            }

            if (_liveDetailControl != null)
                _liveDetailControl.RequestDatas();
        }

        #region HttpHandler

        void listFactory_HttpSucessed(object sender, HttpFactoryArgs<LiveArgs> e)
        {
            LiveListGroup group = null;
            IEnumerable<LiveChannelInfo> list = e.Result.Channels;
            if (_isClickMore)
            {
                CloseTips();
                group = _viewModel.Groups[_groupIndex];
                list = list.Skip(_colCount * 2);
            }
            else
            {
                group = CreateGroup(e.Result.GroupName);
            }

            if (_groupIndex == 0)
                this.DefaultViewModel["SatelliteSource"] = group.ItemsGroup;
            else
                this.DefaultViewModel["LocalTVSource"] = group.ItemsGroup;

            CreateItems(group, list);
            if (!_isClickMore)
                GetLiveList();
        }

        void liveCenter_HttpSucessed(object sender, HttpFactoryArgs<LiveArgs> e)
        {
            var group = CreateGroup(e.Result.GroupName);
            _liveCenterViewModel.Add(group);
            CreateItems(group, e.Result.Channels);

            if (_groupIndex == 3
                && _scrollOffset != 0)
            {
                rootScroolViewer.ScrollToHorizontalOffset(_scrollOffset);
                _scrollOffset = 0;
            }

            CloseTips();
            GetLiveList();
        }

        void GetLiveList()
        {
            _groupIndex++;

            if (_groupIndex == 1)
            {
                if (_localtvMore)
                {
                    _listFactory.DownLoadDatas((int)LiveType.LocalTV, 200, 1);
                    localtvMore.Visibility = Visibility.Collapsed;
                }
                else
                {
                    _listFactory.DownLoadDatas((int)LiveType.LocalTV, _colCount * 2, 1);
                    localtvMore.Visibility = Visibility.Visible;
                }
            }
            else if (_groupIndex == 2)
                _sportsFactory.DownLoadDatas(DateTime.Now.ToString("yyyy-MM-dd"));
            else if (_groupIndex == 3)
                _gamesFactory.DownLoadDatas(DateTime.Now.ToString("yyyy-MM-dd"));
        }

        LiveListGroup CreateGroup(string groupName)
        {
            var group = new LiveListGroup();
            group.GroupName = groupName;
            group.ItemsGroup = new ObservableCollection<LiveListItem>();
            _viewModel.Groups.Add(group);
            return group;
        }

        void CreateItems(LiveListGroup group, IEnumerable<LiveChannelInfo> items)
        {
            bool hasReuslt = false;
            foreach (var live in items)
            {
                hasReuslt = true;
                var item = new LiveListItem() { ChannelInfo = live };
                group.ItemsGroup.Add(item);
            }
            if (!hasReuslt)
                group.ItemsGroup.Add(new LiveListItem() { ChannelInfo = new LiveChannelInfo(LiveType.Sports) { Id = 0, Title = string.Format("暂无{0}节目", group.GroupName), StartTime = DateTime.Now.AddDays(1) } });
        }

        public void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Constants.HttpFailTipText, RequestDatas, "刷新");
        }

        #endregion

        private void liveChannelGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as LiveListItem;
            if (item == null) return;

            if (LiveCenterFactoryBase.IsDirectPlay(item.ChannelInfo.LiveType))
            {
                if (IsStart(item))
                    this.Frame.Navigate(typeof(PlayPage), item);
            }
            else
            {
                if (_settingsFlyout == null)
                {
                    _settingsFlyout = new SettingsFlyout();
                    _settingsFlyout.CloseAction += liveDetailClose;
                    _liveDetailControl = new LiveDetailControl();
                    _liveDetailControl.CloseAction += liveDetailClose;
                    _liveDetailControl.HttpTimeOutOrFail += http_FailorTimeOut;
                    _settingsFlyout.ShowFlyout(_liveDetailControl, 510, false);

                    emptyGrid.Width = 510;
                }

                _liveDetailControl.SetSource(item);
            }
        }

        private void snapliveChannel_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as LiveListItem;
            if (item != null)
            {
                if (IsStart(item))
                    this.Frame.Navigate(typeof(PlayPage), item);
            }
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            RequestDatas();
            BottomAppBar.IsOpen = false;
        }

        private void satelliteMore_Tapped(object sender, TappedRoutedEventArgs e)
        {
            _groupIndex = 0;
            if (loadingTip.Visibility == Visibility.Collapsed)
            {
                _isClickMore = true;
                _satelliteMore = true;
                loadingTip.Visibility = Visibility.Visible;
                satelliteMore.Visibility = Visibility.Collapsed;
                _listFactory.DownLoadDatas((int)LiveType.Satellite, 200, 1);
            }
        }

        private void localtvMore_Tapped(object sender, TappedRoutedEventArgs e)
        {
            _groupIndex = 1;
            if (loadingTip.Visibility == Visibility.Collapsed)
            {
                _isClickMore = true;
                _localtvMore = true;
                loadingTip.Visibility = Visibility.Visible;
                localtvMore.Visibility = Visibility.Collapsed;
                _listFactory.DownLoadDatas((int)LiveType.LocalTV, 200, 1);
            }
        }

        private bool IsStart(LiveListItem item)
        {
            if (LiveCenterFactoryBase.IsDirectPlay(item.ChannelInfo.LiveType))
            {
                if (!(item.ChannelInfo.StartTime <= DateTime.Now
                    && item.ChannelInfo.EndTime >= DateTime.Now))
                {
                    liveDetailClose();
                    return false;
                }
            }
            return true;
        }

        private void liveDetailClose()
        {
            if (_settingsFlyout != null)
            {
                var popup = _liveDetailControl.Parent as Popup;
                if (popup != null)
                    popup.IsOpen = false;
                _settingsFlyout = null;
                _liveDetailControl = null;

                emptyGrid.Width = 47;
            }
        }

        private void ShowTips()
        {
            loadingTip.Visibility = Visibility.Visible;
            satelliteControl.Visibility = Visibility.Collapsed;
            localtvControl.Visibility = Visibility.Collapsed;
            satelliteMore.Visibility = Visibility.Collapsed;
            localtvMore.Visibility = Visibility.Collapsed;
        }

        private void CloseTips()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            satelliteControl.Visibility = Visibility.Visible;
            localtvControl.Visibility = Visibility.Visible;
        }

        private void rootGrid_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            liveDetailClose();
        }
    }
}
