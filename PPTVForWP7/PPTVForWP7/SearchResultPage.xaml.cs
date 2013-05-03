using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;
using Microsoft.Phone.Controls;
using System.Windows.Threading;

namespace PPTVForWP7
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVForWP7.Utils;
    using PPTVForWP7.Controls;

    public partial class SearchResultPage : PhoneApplicationPage
    {
        private VodSmartSearchFactory _searchFactory = null;

        static SearchResultPage()
        {
            TiltEffect.TiltableItems.Add(typeof(SearchResultItem));
        }

        public SearchResultPage()
        {
            InitializeComponent();
            _searchFactory = new VodSmartSearchFactory();
            _searchFactory.HttpSucessHandler += OnGetChannelsBySearchSucceed;
            _searchFactory.HttpFailHandler += OnGetChannelsBySearchFailed;
            _searchFactory.HttpTimeOutHandler += OnGetChannelsBySearchTimeout;
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            var searchText = NavigationContext.QueryString["searchText"];
            _searchFactory.DownLoadDatas(searchText, 1, 50);
            SearchTitle.Text = searchText;
        }

        private void OnGetChannelsBySearchSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            var result = args.Result.Channels;
            ResultCount.Text = String.Format("共找到{0}条结果", result.Count);
            xSearchResult.ItemsSource = result;
        }

        private void OnGetChannelsBySearchFailed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            CommonUtils.HttpTimeOutWarn();
        }

        private void OnGetChannelsBySearchTimeout(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            CommonUtils.HttpTimeOutWarn();
        }

        private void OnResultTap(object sender, System.Windows.Input.GestureEventArgs args)
        {
            int vid = (sender as SearchResultItem).Vid;
            if (vid != -1)
            {
                string page = String.Format("/DetailPage.xaml?vid={0}&programSource=4", vid);
                NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
            }
        }
    }
}