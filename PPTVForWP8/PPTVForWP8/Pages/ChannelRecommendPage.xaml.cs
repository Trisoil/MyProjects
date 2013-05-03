using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Navigation;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel;

    public partial class ChannelRecommendPage : PhoneApplicationPage
    {
        CoverNavFactory _coverNavFactory;
        CoverListFactory _coverChannelFactory;
        RecommendListFactory _recommendChannelFactory;

        ChannelCoverViewModel _coverViewModel;
        ChannelRecommendViewModel _recommendChannelViewModel;
        List<RecommandNav> _coverNavs;
        ScrollBar _listScrollBar;

        int _pivotIndex;
        int _currentPage;
        int _maxPage = 2;
        int _requestNum = 8;
        int _coverNavIndex = -1;

        public ChannelRecommendPage()
        {
            InitializeComponent();
            this.Loaded += ChannelRecommendPage_Loaded;

            _coverNavFactory = new CoverNavFactory();
            _coverNavFactory.HttpSucessHandler += coverNav_Successed;
            _coverNavFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _recommendChannelFactory = new RecommendListFactory();
            _recommendChannelFactory.HttpSucessHandler += recommendChannelFactory_GetChannelsSucceed;
            _recommendChannelFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _coverViewModel = new ChannelCoverViewModel();
            _recommendChannelViewModel = new ChannelRecommendViewModel();

            coverList.ItemsSource = _coverViewModel;

            var navs = FirstPage.RecommandNavs;
            if (navs != null)
            {
                foreach (var nav in navs)
                {
                    var item = new PivotItem();
                    var head = new TextBlock();
                    head.Text = nav.Name;
                    head.Tag = nav.Navid;
                    head.Style = App.Current.Resources["PivotMasterTitleStyle"] as Style;
                    var content = new LongListSelector();
                    content.Style = App.Current.Resources["PivotListStyle"] as Style;
                    content.ItemTemplate = App.Current.Resources["CoverRecommendTemplate"] as DataTemplate;
                    content.ItemsSource = _recommendChannelViewModel;
                    content.SelectionChanged += recommend_SelectionChanged;
                    TiltEffect.SetIsTiltEnabled(content, true);
                    item.Header = head;
                    item.Content = content;
                    recommendPivot.Items.Add(item);
                }
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                _pivotIndex = Convert.ToInt32(NavigationContext.QueryString["index"]);

                recommendPivot.SelectedIndex = _pivotIndex;
                RequestDatas();

                recommendPivot.SelectionChanged += recommendPivot_SelectionChanged;
            }
        }

        void ChannelRecommendPage_Loaded(object sender, RoutedEventArgs e)
        {
            LoadRecommendPage();
        }

        void RequestDatas()
        {
            if (_pivotIndex == 0)
            {
                loadingTip.Visibility = Visibility.Visible;

                _coverNavIndex = -1;
                _coverViewModel.PerformanceClear();
                _coverNavFactory.DownLoadDatas();
            }
            else
            {
                _currentPage = 0;
                _maxPage = 2;
                _recommendChannelViewModel.PerformanceClear();

                loadingTip.Visibility = Visibility.Collapsed;
                LoadRecommendPage();
                LoadRecommendDatas();
            }
        }

        void LoadRecommendDatas()
        {
            if (loadingTip.Visibility == Visibility.Collapsed
                && ++_currentPage <= _maxPage)
            {
                _recommendChannelFactory.DownLoadDatas(_requestNum, _currentPage, FirstPage.RecommandNavs[_pivotIndex - 1].Navid);
                loadingTip.Visibility = Visibility.Visible;
            }
        }

        void LoadRecommendPage()
        {
            if (recommendPivot.SelectedIndex == 0) return;

            if (_listScrollBar != null)
            {
                _listScrollBar.ValueChanged -= _listScrollBar_ValueChanged;
                _listScrollBar = null;
            }

            var item = recommendPivot.SelectedItem as PivotItem;
            if (item != null)
            {
                var list = item.Content as LongListSelector;
                if (list != null)
                {
                    _listScrollBar = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollBar>(list);
                    if (_listScrollBar != null)
                        _listScrollBar.ValueChanged += _listScrollBar_ValueChanged;
                }
            }
        }

        #region HttpHandler

        void coverNav_Successed(object sender, HttpFactoryArgs<List<RecommandNav>> args)
        {
            _coverNavs = args.Result;
            getCoverList();
        }

        void coverFactory_GetCoverInfosSuccess(object sender, HttpFactoryArgs<List<CoverInfo>> args)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            if (args.Result.Count <= 0) return;

            var group = new ChannelCoverGroup();
            group.GroupName = _coverNavs[_coverNavIndex].Name;
            _coverViewModel.Add(group);

            foreach (var cover in args.Result)
            {
                group.Add(new ChannelCoverItem()
                {
                    ChannelId = cover.Vid,
                    Title = cover.Title,
                    ImageUri = cover.ImageUri,
                    Note = cover.Note
                });
            }
            getCoverList();
        }

        void recommendChannelFactory_GetChannelsSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            _maxPage = e.Result.PageCount;

            for (int i = 0; i < e.Result.Channels.Count; i++)
            {
                _recommendChannelViewModel.Add(new ChannelRecommendListItem
                {
                    ChannelId = e.Result.Channels[i].Id,
                    ChannelFlag = e.Result.Channels[i].ChannelFlag,
                    Mark = e.Result.Channels[i].Mark,
                    ImageUri = e.Result.Channels[i].ImageUri,
                    Title = e.Result.Channels[i].Title,
                    //Act = e.Result.Channels[i].Act,
                    Duration = e.Result.Channels[i].Duration
                });
            }
        }

        void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.HttpFailTipText, RequestDatas);
        }

        #endregion

        void getCoverList()
        {
            _coverNavIndex++;
            if (_coverNavIndex < _coverNavs.Count)
            {
                if (_coverChannelFactory == null)
                {
                    _coverChannelFactory = new CoverListFactory();
                    _coverChannelFactory.HttpSucessHandler += coverFactory_GetCoverInfosSuccess;
                    _coverChannelFactory.HttpFailorTimeOut += http_FailorTimeOut;
                }
                _coverChannelFactory.DownLoadDatas(_coverNavs[_coverNavIndex].Navid);
            }
        }

        void _listScrollBar_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_listScrollBar != null
                && e.NewValue > e.OldValue
                && e.NewValue + 100 >= _listScrollBar.Maximum)
            {
                LoadRecommendDatas();
            }
        }

        private void recommendPivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            _pivotIndex = recommendPivot.SelectedIndex;
            RequestDatas();
        }

        private void ApplicationBarRefresh_Click(object sender, EventArgs e)
        {
            RequestDatas();
        }

        private void coverList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = coverList.SelectedItem as ChannelCoverItem;
            if (item != null)
                Utils.CommonUtils.DACNavagate(DACPageType.COVER, string.Format("/Pages/DetailPage.xaml?id={0}", item.ChannelId));
            coverList.SelectedItem = null;
        }

        private void recommend_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var list = sender as LongListSelector;
            if (list != null)
            {
                var item = list.SelectedItem as ChannelRecommendListItem;
                if (item != null)
                    Utils.CommonUtils.DACNavagate(DACPageType.RECOMMEND, string.Format("/Pages/DetailPage.xaml?id={0}", item.ChannelId));
                list.SelectedItem = null; 
            }
        }
    }
}