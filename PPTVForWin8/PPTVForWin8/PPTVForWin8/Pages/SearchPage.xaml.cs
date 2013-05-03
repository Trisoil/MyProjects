using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Collections.ObjectModel;

using Windows.UI;
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

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTVData;
    using PPTVData.Utils;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Vod;
    using PPTVData.Factory.Vod;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary;
    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class SearchPage : PPTVForWin8.Common.LayoutAwarePage
    {
        ChannelSearchFactory _channelFactory;
        HotSearchFactory _hotSearchFactory;

        ChannelSearchViewModel<ChannelSearchListInfo, HotSearchInfo> _searchViewModel;
        ScrollViewer _listViewScrollviewer;
        ScrollViewer _snapListViewScrollViewer;

        string _searchQuery;
        int _currentPage = 0;
        int _maxPage = 2;
        int _requestNum = 45;
        int _typeId = 0;

        public SearchPage()
        {
            this.InitializeComponent();
            this.Loaded += load_Handler;
            snapSearchListView.LayoutUpdated += load_Handler;
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            _searchViewModel = new ChannelSearchViewModel<ChannelSearchListInfo, HotSearchInfo>();
            _searchViewModel.Items = new ObservableCollection<ChannelSearchListInfo>();
            _searchViewModel.Navs = new ObservableCollection<TextBlock>();
            this.DefaultViewModel["Items"] = _searchViewModel.Items;
            searchCatalogComboBox.ItemsSource = _searchViewModel.Navs;

            _channelFactory = new ChannelSearchFactory();
            _channelFactory.HttpSucessHandler += channelFactory_HttpSucessed;
            _channelFactory.HttpFailorTimeOut = channelFactory_HttpFailorTimeOut;
            _hotSearchFactory = new HotSearchFactory();
            _hotSearchFactory.HttpSucessHandler += hotSearchFactory_HttpSucessed;

            _requestNum = ScreenUtils.CreateRequestNum(370, 160, 251);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back
                && e.Parameter != null)
            {
                _searchQuery = e.Parameter.ToString();
                pageTitle.Text = string.Format("搜索 \"{0}\"", _searchQuery);
                ClearCache(true);
                RequestHotSearch();

                if (!string.IsNullOrEmpty(_searchQuery))
                    RequestDatas();
                else
                    NoResult();
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            _channelFactory.Cancel();
            _hotSearchFactory.Cancel();
            if (e.NavigationMode == NavigationMode.Back)
                _searchViewModel.Items.PerformanceClear();
        }

        void load_Handler(object sender, object e)
        {
            if (_listViewScrollviewer == null)
            {
                _listViewScrollviewer = DataCommonUtils.FindChildOfType<ScrollViewer>(searchListView);
                if (_listViewScrollviewer != null)
                    _listViewScrollviewer.ViewChanged += listViewScrollviewer_ViewChanged;
            }
            if (_snapListViewScrollViewer == null)
            {
                _snapListViewScrollViewer = DataCommonUtils.FindChildOfType<ScrollViewer>(snapSearchListView);
                if (_snapListViewScrollViewer != null)
                    _snapListViewScrollViewer.ViewChanged += snapListViewScrollViewer_ViewChanged;
            }
        }

        protected override void GoBack(object sender, RoutedEventArgs e)
        {
            while (this.Frame != null
                && this.Frame.CanGoBack
                && this.Frame.Content.GetType() == typeof(SearchPage))
                this.Frame.GoBack();
        }

        private void RequestDatas()
        {
            if (++_currentPage <= _maxPage
                && loadingTip.Visibility == Visibility.Collapsed)
            {
                loadingTip.Visibility = Visibility.Visible;
                _channelFactory.DownLoadDatas(_requestNum, _currentPage, _searchQuery, _typeId);
            }
        }

        private void RequestHotSearch()
        {
            if (_searchViewModel.HotNames == null
                || _searchViewModel.HotNames.Count == 0)
            {
                _hotSearchFactory.DownLoadDatas();
            }
        }

        private void ClearCache(bool isClearNav)
        {
            _currentPage = 0;
            _maxPage = 2;
            _searchViewModel.Items.PerformanceClear();
            if (isClearNav)
            {
                searchCatalogComboBox.Visibility = Visibility.Collapsed;
                noResultGrid.Visibility = Visibility.Collapsed;
                _searchViewModel.Navs.Clear();
                _typeId = 0;
            }
            if (_listViewScrollviewer != null)
            {
                _listViewScrollviewer.ScrollToHorizontalOffset(0);
            }
            if (_snapListViewScrollViewer != null)
            {
                _snapListViewScrollViewer.ScrollToVerticalOffset(0);
            }
        }

        #region HttpHandler

        private void channelFactory_HttpSucessed(object sender, HttpFactoryArgs<ChannelSearchInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;

            if (e.Result.Lists.Count > 0)
            {
                searchCatalogComboBox.Visibility = Visibility.Visible;
                noResultGrid.Visibility = Visibility.Collapsed;
                _maxPage = e.Result.OutlineInfo.PageCount;
                foreach (var channelInfo in e.Result.Lists)
                {
                    _searchViewModel.Items.Add(channelInfo);
                }
                if (_searchViewModel.Navs.Count == 0)
                {
                    var totalCount = 0;
                    foreach (var nav in e.Result.Navs)
                    {
                        totalCount += nav.Count;
                        _searchViewModel.Navs.Add(CommonUtils.CreateComoboxItem(string.Format("{0}({1})", nav.Name, nav.Count), nav.Id));
                    }
                    _searchViewModel.Navs.Insert(0, CommonUtils.CreateComoboxItem(string.Format("{0}({1})", "全部", totalCount), 0));
                    searchCatalogComboBox.SelectedIndex = 0;
                }
            }
            else
            {
                NoResult();
            }
        }

        private void hotSearchFactory_HttpSucessed(object sender, HttpFactoryArgs<List<HotSearchInfo>> e)
        {
            _searchViewModel.HotNames = e.Result;
            movieHotSearch.ItemsSource = _searchViewModel.HotNames.Where(v => v.Type == ChannelTypeFactory.Instance.GetChannelTypeId("电影"));
            tvHotSearch.ItemsSource = _searchViewModel.HotNames.Where(v => v.Type == ChannelTypeFactory.Instance.GetChannelTypeId("电视剧"));
            cartoonHotSearch.ItemsSource = _searchViewModel.HotNames.Where(v => v.Type == ChannelTypeFactory.Instance.GetChannelTypeId("动漫"));
            amuseHotSearch.ItemsSource = _searchViewModel.HotNames.Where(v => v.Type == ChannelTypeFactory.Instance.GetChannelTypeId("综艺"));
        }

        private void channelFactory_HttpFailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Constants.HttpFailTipText, RequestDatas, "刷新");
        }

        #endregion

        private void NoResult()
        {
            noResultGrid.Visibility = Visibility.Visible;
            searchCatalogComboBox.Visibility = Visibility.Collapsed;
        }

        private void listViewScrollviewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_listViewScrollviewer != null
                && searchListView.Items.Count > 0
                && _listViewScrollviewer.HorizontalOffset >= _listViewScrollviewer.ScrollableWidth)
            {
                RequestDatas();
            }
        }

        private void snapListViewScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_snapListViewScrollViewer != null
                && snapSearchListView.Items.Count > 0
                && _snapListViewScrollViewer.VerticalOffset >= _snapListViewScrollViewer.ScrollableHeight)
            {
                RequestDatas();
            }
        }

        private void xSearchCatalogComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var dataItem = searchCatalogComboBox.SelectedItem as TextBlock;
            if (dataItem != null)
            {
                var typeId = (int)dataItem.Tag;
                if (typeId != _typeId)
                {
                    _typeId = typeId;
                    ClearCache(false);
                    RequestDatas();
                }
            }
        }

        private void searchListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelSearchListInfo;
            if (dataItem != null)
                CommonUtils.DACNavagate(this.Frame, DACPageType.SEARCH, typeof(DetailPage), dataItem.Vid);
        }

        private void hotSearch_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as HotSearchInfo;
            if (dataItem != null)
                SearchPane.GetForCurrentView().Show(dataItem.Name);
        }
    }
}