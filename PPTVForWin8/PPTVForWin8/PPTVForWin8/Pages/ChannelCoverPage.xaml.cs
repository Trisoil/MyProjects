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
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class ChannelCoverPage : PPTVForWin8.Common.LayoutAwarePage
    {
        CoverNavFactory _coverNavFactory;
        CoverListFactory _coverChannelFactory;

        ChannelCoverViewModel _coverViewModel;
        List<RecommandNav> _coverNavs;
        int _coverNavIndex = -1;

        public ChannelCoverPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            _coverViewModel = new ChannelCoverViewModel();
            _coverViewModel.Groups = new ObservableCollection<ChannelCoverGroup>();
            this.DefaultViewModel["Items"] = _coverViewModel.Groups;

            semanticZoomOutGridView.ItemsSource = coverViewSource.View.CollectionGroups;

            _coverNavFactory = new CoverNavFactory();
            _coverNavFactory.HttpSucessHandler += coverNav_Successed;
            _coverNavFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _coverChannelFactory = new CoverListFactory();
            _coverChannelFactory.HttpSucessHandler += coverFactory_GetCoverInfosSuccess;
            _coverChannelFactory.HttpFailorTimeOut += http_FailorTimeOut;

            RequestDatas();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            _coverNavFactory.Cancel();
            _coverChannelFactory.Cancel();
        }

        void RequestDatas()
        {
            loadingTip.Visibility = Visibility.Visible;
            _coverViewModel.Groups.PerformanceClear();
            if (_coverNavs != null) _coverNavs.Clear();
            _coverNavIndex = -1;
            
            _coverNavFactory.DownLoadDatas();
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
            group.ItemsGroup = new ObservableCollection<ChannelCoverItem>();
            group.GroupName = _coverNavs[_coverNavIndex].Name;
            _coverViewModel.Groups.Add(group);

            foreach (var cover in args.Result)
            {
                group.ItemsGroup.Add(new ChannelCoverItem()
                { 
                    ChannelId = cover.Vid,
                    Title = cover.Title,
                    ImageUri = cover.ImageUri,
                    Note = cover.Note
                });
            }
            getCoverList();
        }

        void getCoverList()
        {
            _coverNavIndex++;
            if (_coverNavIndex < _coverNavs.Count)
            {
                _coverChannelFactory.DownLoadDatas(_coverNavs[_coverNavIndex].Navid);
            }
        }

        void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Constants.VodPageFail, RequestDatas, "刷新");
        }

        #endregion

        private void channelCoverGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelCoverItem;
            if (dataItem != null)
            {
                CommonUtils.DACNavagate(this.Frame, DACPageType.COVER, typeof(DetailPage), dataItem.ChannelId);
            }
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            RequestDatas();
            BottomAppBar.IsOpen = false;
        }
    }
}
