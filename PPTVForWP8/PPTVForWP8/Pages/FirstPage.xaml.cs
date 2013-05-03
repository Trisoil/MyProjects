using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.Cloud;

    using PPTVForWP8.Common;
    using PPTVForWP8.Controls;

    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel;
    using PPTV.WPRT.CommonLibrary.ViewModel.Channel;
    using PPTV.WPRT.CommonLibrary.Utils;

    public partial class FirstPage : PhoneApplicationPage
    {
        static CloudGetFactory _cloudRecentGetFactory;
        static CloudGetFactory _cloudFavolitenGetFactory;

        RecommendNavFactory _recommendNavFactory;
        RecommendListFactory _recommendChannelFactory;
        ChannelRecommendViewModel _recommendChannelViewModel;

        ApplicationBarIconButton _refreshBar;
        ApplicationBarIconButton _selectBar;
        ApplicationBarIconButton _unPinBar;
        Dictionary<object, PivotCallbacks> _callbacks;

        public static List<RecommandNav> RecommandNavs;

        int _recCount;
        int _currentRecIndex;

        public FirstPage()
        {
            InitializeComponent();

            _callbacks = new Dictionary<object, PivotCallbacks>(4);
            _callbacks[recommendPivotItem] = new PivotCallbacks
            {
                Init = RecommendPivotItemBarInit,
                OnActivated = RecommendPivotItemBarActive
            };
            _callbacks[typePivotItem] = new PivotCallbacks
            {
                Init = TypePivotItemInit,
                OnActivated = TypePivotItemActive,
                OnBackKeyPress = TypePivotItemBackPress
            };
            _callbacks[personPivotItem] = new PivotCallbacks 
            {
                OnActivated = personPivotItemActive
            };

            foreach (var callbacks in _callbacks.Values)
            {
                if (callbacks.Init != null)
                {
                    callbacks.Init();
                }
            }

            TiltEffect.TiltableItems.Add(typeof(SlidesShowControl));
            TiltEffect.TiltableItems.Add(typeof(LoginControl));

            _recCount = 9;
            _recommendChannelViewModel = new ChannelRecommendViewModel();

            _recommendNavFactory = new RecommendNavFactory();
            _recommendNavFactory.HttpSucessHandler += recommandNav_Successed;
            _recommendNavFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _cloudRecentGetFactory = new CloudGetFactory();
            _cloudRecentGetFactory.HttpSucessHandler += cloudRecentGetFactory_HttpSucess;

            _cloudFavolitenGetFactory = new CloudGetFactory();
            _cloudFavolitenGetFactory.HttpSucessHandler += cloudFavolitenGetFactory_HttpSucess;

            recommendList.ItemsSource = _recommendChannelViewModel;
            typeList.ItemsSource = ChannelTypeFactory.Instance.SelectedViewModel;
            personalList.ItemsSource = ChannelTypeFactory.Instance.LocalTypes;
            ChannelTypeFactory.Instance.HttpFailorTimeOut += http_FailorTimeOut;
            LoginPage.LoginSucessHandler = LoginSucess_Handler;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                RequestDatas();
            }
            slidesShowControl.StartSlidesShow();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            slidesShowControl.Cancel();
            slidesShowControl.EndSlidesShow();
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(firstPivot.SelectedItem, out callbacks) && (callbacks.OnBackKeyPress != null))
            {
                callbacks.OnBackKeyPress(e);
            }
            if (e.Cancel) return;
            if (MessageBox.Show("确定退出吗?", "提示", MessageBoxButton.OKCancel) != MessageBoxResult.OK)
            {
                e.Cancel = true;
            }
            else
            {
                DACFactory.Instance.Quit();
            }
        }

        void RequestDatas()
        {
            loadingTip.Visibility = Visibility.Visible;
            _recommendChannelViewModel.PerformanceClear();
            ChannelTypeFactory.Instance.Clear();

            slidesShowControl.RequestDatas();
            ChannelTypeFactory.Instance.DownloadDatas();
            _recommendNavFactory.DownLoadDatas();
            InitCloud();
        }

        private void InitCloud()
        {
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
            RecommandNavs = args.Result.Take(5).ToList();

            if (RecommandNavs.Count > 0)
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

            var count = args.Result.Channels.Count;
            if (count < _recCount)
                count = Convert.ToInt32(Math.Ceiling(count / 3.0) * 3);
            for (var i = 0; i < count; i++)
            {
                ChannelRecommendListItem dataItem = null;
                if (i < args.Result.Channels.Count)
                    dataItem = new ChannelRecommendListItem()
                    {
                        Index = i == 0 ? _currentRecIndex + 1 : -1,
                        GruopName = RecommandNavs[_currentRecIndex].Name,
                        GroupCount = args.Result.ChannelCount,
                        ChannelId = args.Result.Channels[i].Id,
                        ChannelFlag = args.Result.Channels[i].ChannelFlag,
                        ImageUri = args.Result.Channels[i].ImageUri,
                        Title = args.Result.Channels[i].Title,
                        Mark = args.Result.Channels[i].Mark
                    };
                else
                    dataItem = new ChannelRecommendListItem { Index = -2 };
                _recommendChannelViewModel.Add(dataItem);
            }

            _currentRecIndex++;
            if (_currentRecIndex < RecommandNavs.Count)
                getRecommand();
            else
                loadingTip.Visibility = Visibility.Collapsed;
        }

        void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.HttpFailTipText, RequestDatas);
        }

        void getRecommand()
        {
            _recommendChannelFactory.DownLoadDatas(_recCount, 1, RecommandNavs[_currentRecIndex].Navid);
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

        #region AppBar

        void RecommendPivotItemBarInit()
        {
            _refreshBar = new ApplicationBarIconButton();
            _refreshBar.IconUri = new Uri("/Images/AppBar/refresh.png", UriKind.Relative);
            _refreshBar.Text = "刷新";
            _refreshBar.Click += ApplicationBarRefresh_Click;
        }

        void RecommendPivotItemBarActive()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            ApplicationBar.Mode = ApplicationBarMode.Minimized;
            ApplicationBar.Buttons.Add(_refreshBar);
        }

        void TypePivotItemInit()
        {
            _selectBar = new ApplicationBarIconButton();
            _selectBar.IconUri = new Uri("/Images/AppBar/select.png", UriKind.Relative);
            _selectBar.Text = "选择";
            _selectBar.Click += selectBar_Click;

            _unPinBar = new ApplicationBarIconButton();
            _unPinBar.IconUri = new Uri("/Images/AppBar/unpin.png", UriKind.Relative);
            _unPinBar.Text = "取消固定";
            _unPinBar.Click += unPinBar_Click;
        }

        void TypePivotItemActive()
        {
            ApplicationBar.Mode = ApplicationBarMode.Default;
            if (typeList.IsSelectionEnabled)
            {
                typeList.EnforceIsSelectionEnabled = false;
            }
            else
            {
                SetUpTypePivotItemBar();
            }
        }

        void SetUpTypePivotItemBar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (typeList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_unPinBar);
                UpdateTypePivotItemBar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateTypePivotItemBar()
        {
            _unPinBar.IsEnabled = typeList.SelectedItems != null && typeList.SelectedItems.Count > 0;
        }

        void TypePivotItemBackPress(CancelEventArgs e)
        {
            if (typeList.IsSelectionEnabled)
            {
                typeList.EnforceIsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        void personPivotItemActive()
        {
            ApplicationBar.Mode = ApplicationBarMode.Minimized;
            Utils.CommonUtils.ClearApplicationBar(this);
        }

        #endregion

        private void LoginSucess_Handler()
        {
            TileUtils.CreateBasicToast("登录成功");
            loginControl.UpdateState();
            MainPage.InitWAY();

            PersonalInfo personalInfo = PersonalFactory.Instance.DataInfos[0];
            _cloudRecentGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Recent);
            _cloudFavolitenGetFactory.DownLoadDatas(personalInfo.UserStateInfo.UserName, CloudType.Favorites);
        }

        private void Pivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(firstPivot.SelectedItem, out callbacks) && (callbacks.OnActivated != null))
            {
                callbacks.OnActivated();
            }
        }

        private void channelRecommand_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Pages/ChannelRecommendPage.xaml?index=0", UriKind.Relative));
        }

        private void channelTtype_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/Pages/ChannelTypePage.xaml", UriKind.Relative));
        }

        private void recommendList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = recommendList.SelectedItem as ChannelRecommendListItem;
            if (item != null)
            {
                if (item.Index == -1)
                    Utils.CommonUtils.DACNavagate(DACPageType.RECOMMEND, string.Format("/Pages/DetailPage.xaml?id={0}", item.ChannelId));
                else
                    NavigationService.Navigate(new Uri(string.Format("/Pages/ChannelRecommendPage.xaml?index={0}", item.Index), UriKind.Relative));
            }
            recommendList.SelectedItem = null;
        }

        private void typeList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateTypePivotItemBar();
        }

        private void typeList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpTypePivotItemBar();
        }

        private void ApplicationBarRefresh_Click(object sender, EventArgs e)
        {
            RequestDatas();
        }

        private void selectBar_Click(object sender, EventArgs e)
        {
            typeList.EnforceIsSelectionEnabled = true;
        }

        void unPinBar_Click(object sender, EventArgs e)
        {
            ChannelTypeFactory.Instance.RemoveSelectedTypes(typeList.SelectedItems);
        }

        private void personalList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = personalList.SelectedItem as ChannelTypeItem;
            if (item != null)
            {
                var path = ChannelTypeFactory.Instance.GetLocalPath(item.TypeId);
                if (!string.IsNullOrEmpty(path))
                    NavigationService.Navigate(new Uri(path, UriKind.Relative));
            }
            personalList.SelectedItem = null;
        }

        private void AboutMenuItem_Click(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/Pages/AboutPage.xaml", UriKind.Relative));
        }
    }
}