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
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;
    using PPTV.WinRT.CommonLibrary.Utils;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class ChannelRecommendPage : PPTVForWin8.Common.LayoutAwarePage
    {
        int _recId;
        int _currentPage;
        int _maxPage = 2;
        int _requestNum;
        int _groupNum = 6;

        RecommendListFactory _recommendChannelFactory;
        ChannelRecommendViewModel _recommendViewModel;

        ScrollViewer _gridViewScrollViewer;
        ScrollViewer _snapGridViewScrollViewer;

        public ChannelRecommendPage()
        {
            this.InitializeComponent();
            this.Loaded += ChannelRecommendPage_Loaded;
            recommendListListView.LayoutUpdated += ChannelRecommendPage_Loaded;
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            _recommendChannelFactory = new RecommendListFactory();
            _recommendChannelFactory.HttpSucessHandler += recommendChannelFactory_GetChannelsSucceed;
            _recommendChannelFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _recommendViewModel = new ChannelRecommendViewModel();
            _recommendViewModel.Groups = new ObservableCollection<ChannelRecommendGroup>();
            this.DefaultViewModel["RecommendListSource"] = _recommendViewModel.Groups;

            _groupNum = ScreenUtils.CreateColNum(160) * 2;
            _requestNum = ScreenUtils.CreateGroupRequestNum(372, 160, 2);
        }

        void ChannelRecommendPage_Loaded(object sender, object e)
        {
            if (_gridViewScrollViewer == null)
            {
                _gridViewScrollViewer = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollViewer>(recommendListGridView);
                if (_gridViewScrollViewer != null)
                    _gridViewScrollViewer.ViewChanged += _gridViewScrollViewer_ViewChanged;
            }
            if (_snapGridViewScrollViewer == null)
            {
                _snapGridViewScrollViewer = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollViewer>(recommendListListView);
                if (_snapGridViewScrollViewer != null)
                    _snapGridViewScrollViewer.ViewChanged += snapGridViewScrollViewer_ViewChanged;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                var para = e.Parameter as RecommandNav;
                if (para != null)
                {
                    _recId = Convert.ToInt32(para.Navid);
                    pageTitle.Text = para.Name;
                    LoadDatas();
                }
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;

            _recommendChannelFactory.Cancel();
        }

        private void LoadDatas()
        {
            loadingTip.Visibility = Visibility.Collapsed;

            if (_gridViewScrollViewer != null)
                _gridViewScrollViewer.ScrollToHorizontalOffset(0);
            if (_snapGridViewScrollViewer != null)
                _snapGridViewScrollViewer.ScrollToVerticalOffset(0);

            _currentPage = 0;
            _maxPage = 2;

            _recommendViewModel.Groups.PerformanceClear();
            _recommendChannelFactory.Cancel();

            RequestDatas();
        }

        void RequestDatas()
        {
            if (++_currentPage <= _maxPage
                && loadingTip.Visibility == Visibility.Collapsed)
            {
                _recommendChannelFactory.DownLoadDatas(_requestNum, _currentPage, _recId);
                loadingTip.Visibility = Visibility.Visible;
            }
        }

        #region HttpHandler

        void recommendChannelFactory_GetChannelsSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            _maxPage = e.Result.PageCount;

            ChannelRecommendGroup group = null;
            for (int i = 0; i < e.Result.Channels.Count; i++)
            {
                if (i % _groupNum == 0)
                {
                    group = new ChannelRecommendGroup();
                    group.ItemsGroup = new ObservableCollection<ChannelRecommendListItem>();
                    _recommendViewModel.Groups.Add(group);
                }
                group.ItemsGroup.Add(new ChannelRecommendListItem
                {
                    ChannelId = e.Result.Channels[i].Id,
                    ChannelFlag = e.Result.Channels[i].ChannelFlag,
                    Mark = e.Result.Channels[i].Mark,
                    ImageUri = e.Result.Channels[i].ImageUri,
                    Title = e.Result.Channels[i].Title,
                    Act = e.Result.Channels[i].Act,
                    Duration = e.Result.Channels[i].Duration
                });
            }
        }

        void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.VodPageFail, RequestDatas, "刷新");
        }

        #endregion

        private void _gridViewScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_gridViewScrollViewer != null
                && recommendListGridView.Items.Count > 0
                && _gridViewScrollViewer.HorizontalOffset >= _gridViewScrollViewer.ScrollableWidth)
            {
                RequestDatas();
            }
        }

        private void snapGridViewScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_snapGridViewScrollViewer != null
                && recommendListListView.Items.Count > 0
                && _snapGridViewScrollViewer.VerticalOffset >= _snapGridViewScrollViewer.ScrollableHeight)
            {
                RequestDatas();
            }
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            LoadDatas();
            BottomAppBar.IsOpen = false;
        }

        private void recommendList_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelRecommendListItem;
            if (dataItem != null)
            {
                Utils.CommonUtils.DACNavagate(this.Frame, PPTVData.Entity.DACPageType.RECOMMEND, typeof(DetailPage), dataItem.ChannelId);
            }
        }
    }
}
