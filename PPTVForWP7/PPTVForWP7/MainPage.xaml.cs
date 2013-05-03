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
using System.Windows.Threading;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.IO.IsolatedStorage;
using System.Windows.Media.Imaging;
using System.Diagnostics;
using System.Threading;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using Microsoft.Phone.Info;

namespace PPTVForWP7
{
    using PPTVData.Factory;
    using PPTVData.Entity;
    using PPTVForWP7.Controls;
    using PPTVForWP7.Utils;

    public partial class MainPage : PhoneApplicationPage
    {
        private RecommendNavFactory _recommendNavFactory;//影片导航信息
        private RecommandCoverNavFactory _recommendCoverNavFactory;//大图导航信息
        private DateTime _firstPress = DateTime.Now;
        private bool _isPageLoaded = false;
        private bool _isShowingMsgBox = false;
        RecommendNav _selectedRecommendNavChannel = new RecommendNav();
        RecommendNav _selectedRecommendNavCover = new RecommendNav();
        GetDataStatusHelp _getDataStatusHelp;
        List<RecommendNav> _recommendNavsByCovers;//大图导航

        UpgradeFactory _upgradeFactory;
        VodRecommandFactory _channelFactoryByRecommendNav;

        static MainPage()
        {
            TiltEffect.TiltableItems.Add(typeof(SearchHistoryItem));
            TiltEffect.TiltableItems.Add(typeof(StackPanel));
            //TiltEffect.TiltableItems.Add(typeof(ChannelPreviewItem));
        }

        public MainPage()
        {
            Utils.Utils.ProgramStart();
           
            InitializeComponent();
            InitVariableNEventHandle();

            if (Utils.Utils._lanchCount == 0)
            {
                GetUserInfo();
                _upgradeFactory.DownLoadDatas();
            }
            Utils.Utils._lanchCount++;

            Init();//开始下载
        }

        public T FindVisualChildByName<T>(DependencyObject parent, string name) where T : DependencyObject
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parent); i++)
            {
                var child = VisualTreeHelper.GetChild(parent, i);
                string controlName = child.GetValue(Control.NameProperty) as string;
                if (controlName == name)
                {
                    return child as T;
                }
                else
                {
                    T result = FindVisualChildByName<T>(child, name);
                    if (result != null)
                        return result;
                }
            }
            return null;
        }

        private void InitVariableNEventHandle()
        {
            //升级
            _upgradeFactory = new UpgradeFactory();

            _upgradeFactory.HttpSucessHandler += OnGetUpgradeInfoSucceeded;
            _upgradeFactory.HttpFailHandler += OnGetUpgradeInfoFailed;
            _upgradeFactory.HttpTimeOutHandler += OnGetUpgradedInfoTimeout;

            //大图的导航
            _recommendCoverNavFactory = new RecommandCoverNavFactory();
            _recommendCoverNavFactory.HttpFailHandler += _recommendCoverNavFactory_GetRecommendNavFailed;
            _recommendCoverNavFactory.HttpSucessHandler += _recommendCoverNavFactory_GetRecommendNavSucceed;
            _recommendCoverNavFactory.HttpTimeOutHandler += _recommendCoverNavFactory_GetRecommendNavTimeout;

            //影片的导航
            _recommendNavFactory = new RecommendNavFactory();
            _recommendNavFactory.HttpSucessHandler += OnGetRecommendNavSucceeded;
            _recommendNavFactory.HttpFailHandler += OnGetRecommendNavFailed;
            _recommendNavFactory.HttpTimeOutHandler += OnGetRecommendNavTimeout;

            //下载推荐的影片
            _channelFactoryByRecommendNav = new VodRecommandFactory();
            _channelFactoryByRecommendNav.HttpSucessHandler += OnGetChannelsByRecommendSucceeded;
            _channelFactoryByRecommendNav.HttpFailHandler += _channelFactoryByRecommendNav_GetChannelsByRecommendNavFailed;
            _channelFactoryByRecommendNav.HttpTimeOutHandler += _channelFactoryByRecommendNav_GetChannelsByRecommendNavTimeout;
            xCoverBar.CoverImagesDownloadCompleted += new CoversBar.DownloadedEventHandler(xCoverBar_CoverImagesDownloadCompleted);
            xCoverBar.ItemTap += new CoversBar.ImageTapEventHandler(xCoverBar_ItemTap);

            xHotSearchBar.ItemTap += new HotSearchBar.ImageTapEventHandler(xHotSearchBar_ItemTap);

            _getDataStatusHelp = new GetDataStatusHelp();
            _getDataStatusHelp.DataDownloadFailure += new GetDataStatusHelp.DownloadFailEventHandler(_getDataStatusHelp_DataDownloadFailure);

        }

        void _getDataStatusHelp_DataDownloadFailure(object sender)
        {
            MessageBox.Show(Utils.Utils.NetExceptInfo);
        }

        void xHotSearchBar_ItemTap(object sender, HotSearchInfo hotSearchInfo)
        {
            ProgramSearch.SetKeyAndSearch(hotSearchInfo.Key);
        }

        void xCoverBar_ItemTap(object sender, CoverInfo coverInfo)
        {
            int vid = coverInfo.Vid;
            string page = String.Format("/DetailPage.xaml?vid={0}&programSource=0", vid);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }

        void xCoverBar_CoverImagesDownloadCompleted(object sender)
        {
        }

        void MainPage_NavSelected(object sender, RecommendNav recommendNav)
        {
            _selectedRecommendNavChannel = recommendNav;
            Refresh();
        }

        void _recommendCoverNavFactory_GetRecommendNavTimeout(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
        }

        void _recommendCoverNavFactory_GetRecommendNavSucceed(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
            _recommendNavsByCovers = args.Result;

            if (args.Result.Count > 0)
            {
                _selectedRecommendNavCover = _recommendNavsByCovers[0];
                xCoverBar.BeginDownload(_selectedRecommendNavCover.Navid);
            }
        }

        private void OnGetRecommendNavSucceeded(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
            //获取到导航菜单后
            _getDataStatusHelp.Clear();

            if (args.Result.Count > 0)
            {
                _selectedRecommendNavChannel = args.Result[0];
                _channelFactoryByRecommendNav.DownLoadDatas(1, _selectedRecommendNavChannel.Navid, 100);
            }
        }

        void mainNavMenu_NavSelected(object sender, RecommendNav recommendNav)
        {
        }

        void _recommendCoverNavFactory_GetRecommendNavFailed(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
        }

        private void OnSearchHistoryTap(object sender, System.Windows.Input.GestureEventArgs args)
        {
            string title = (sender as SearchHistoryItem).TitleText;
            if (!string.IsNullOrEmpty(title))
            {
                OnProgramSearchInputText(ProgramSearch, title);
            }
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            //激活
            if ((App.Current as App).isReActivated)
            {
                _isPageLoaded = true;
                (App.Current as App).isReActivated = false;
            }

            SetBar();
            //InnerRoot.Opacity = 1;
            if ((App.Current as App).isFromSplashScreen)
            {
                (App.Current as App).isFromSplashScreen = false;
                NavigationService.RemoveBackEntry();
            }
            if ((App.Current as App).backToMain)
            {

                PhoneApplicationFrame frame = App.Current.RootVisual as PhoneApplicationFrame;
                int removeCount = 0;
                foreach (System.Windows.Navigation.JournalEntry entry in NavigationService.BackStack)
                {
                    removeCount++;
                }
                for (int i = 0; i < removeCount; i++)
                {
                    NavigationService.RemoveBackEntry();
                }
                (App.Current as App).backToMain = false;
                MinimizedAppBar(true);
                SetApplicationBarMenuType(AppBarMenuType.FIRSTPAGE);

            }
        }

        protected override void OnNavigatedFrom(System.Windows.Navigation.NavigationEventArgs e)
        {
            _getDataStatusHelp.Clear();
            _isPageLoaded = false;
            DetailPage detailPage = e.Content as DetailPage;
            if (detailPage != null)
            {
                string uriStr = e.Uri.OriginalString;
                string Vid = "";
                int beg = uriStr.IndexOf("vid=");
                beg += 4;
                int end = uriStr.IndexOf('&', beg);
                if (end == -1)
                    Vid = uriStr.Substring(beg, uriStr.Length - beg);
                else
                    Vid = uriStr.Substring(beg, end - beg);
            }
        }

        private void OnGetUpgradedInfoTimeout(object sender, HttpFactoryArgs<UpgradeInfo> args)
        {
            //Init();
        }

        private void OnGetUpgradeInfoFailed(object sender, HttpFactoryArgs<UpgradeInfo> args)
        {
            //Init();
        }

        private void OnGetUpgradeInfoSucceeded(object sender, HttpFactoryArgs<UpgradeInfo> args)
        {
            var upgradeInfo = args.Result;
            
            try
            {
                Version latestVersion = Version.Parse(upgradeInfo.LatestVersion);
                Version newVersion = Version.Parse(upgradeInfo.NewVersion);
                Version curVersion = Utils.Utils.GetClientVersion();
                if (curVersion == null)
                {
                    Init();
                    return;
                }
                if (curVersion < latestVersion)
                {
                    MessageBox.Show(upgradeInfo.LatestVersionInfo, "提示", MessageBoxButton.OK);
                    WebBrowser webBrowser = new WebBrowser();
                    Content = webBrowser;
                    webBrowser.HorizontalAlignment = HorizontalAlignment.Stretch;
                    webBrowser.VerticalAlignment = VerticalAlignment.Stretch;
                    webBrowser.Navigate(new Uri(upgradeInfo.Url, UriKind.RelativeOrAbsolute));
                    return;
                }
                else if (curVersion < newVersion)
                {
                    if (MessageBox.Show("检查到有更新的版本:" + newVersion.ToString() + upgradeInfo.NewVersionInfo + "现在就进行升级吗?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                    {
                        WebBrowser webBrowser = new WebBrowser();
                        Content = webBrowser;
                        webBrowser.HorizontalAlignment = HorizontalAlignment.Stretch;
                        webBrowser.VerticalAlignment = VerticalAlignment.Stretch;
                        webBrowser.Navigate(new Uri(upgradeInfo.Url, UriKind.RelativeOrAbsolute));
                        return;
                    }
                }
            }
            catch (Exception exception)
            {
                string message = exception.Message;
                Init();
                return;
            }
            //Init();
        }

        /// <summary>
        /// 开始下载数据及加载数据库
        /// </summary>
        private void Init()
        {
            //下载导航
            _recommendCoverNavFactory.DownLoadDatas();
            _recommendNavFactory.DownLoadDatas();
            xHotSearchBar.Init();
            LoadSearchDB();

            DACFactory.DACStartup(Utils.Utils.GetClientVersion());
        }

        //更新
        private void Refresh()
        {
            WaitSignal.Visibility = Visibility.Visible;
            mainNavMenu.InitVariableNEventHandle();

            if (_recommendNavsByCovers == null)
            {
                _recommendCoverNavFactory.DownLoadDatas();
                return;
            }
            foreach (RecommendNav item in _recommendNavsByCovers)
            {
                if (_selectedRecommendNavChannel.Name == item.Name)
                {
                    _selectedRecommendNavCover = item;
                    break;
                }
            }

            xCoverBar.BeginDownload(_selectedRecommendNavCover.Navid);
            _channelFactoryByRecommendNav.DownLoadDatas(1, _selectedRecommendNavChannel.Navid, 100);
        }

        void _channelFactoryByRecommendNav_GetChannelsByRecommendNavTimeout(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            WaitSignal.Visibility = Visibility.Collapsed;
            if (_isShowingMsgBox)
                return;
            _isShowingMsgBox = true;
            MessageBox.Show("获取数据超时,请检查网络状况后重试");
        }

        void _channelFactoryByRecommendNav_GetChannelsByRecommendNavFailed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
        }

        private void LoadSearchDB()
        {
            xSearchHistory.ItemsSource = SearchHistoryFactory.Instance.DataInfos;
        }

        private void OnChannelClick(object sender, RoutedEventArgs args)
        {
            int typeid = (sender as ChannelItem).TypeId;
            if (typeid == (int)ChannelTypes.VIP尊享)
            {
                if (!PPTVForWP7.Utils.Utils._isValidatedFromServer)
                {
                    if (MessageBox.Show("请先登录", "", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                    {
                        NavigationService.Navigate(new Uri("/LoginPage.xaml?VIP=1", UriKind.RelativeOrAbsolute));
                    }
                    else
                    {
                        return;
                    }
                }
            }
            string page = String.Format("/VODPage.xaml?typeid={0}&programSource=2", typeid);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }

        private void OnProgramSearchInputText(object sender, string text)
        {
            WaitSignal.Visibility = Visibility.Visible;
            SearchHistoryFactory.Instance.AddEntity(new DBHistorySearchInfo(text, DateTime.Now));
            WaitSignal.Visibility = Visibility.Collapsed;
            ProgramSearch.SearchBox.Text = string.Empty;
            NavigationService.Navigate(new Uri(string.Format("/SearchResultPage.xaml?searchText={0}", text), UriKind.Relative));
        }

        private void OnGetCoverFailed(object sender, Exception e)
        {
            MessageBox.Show("访问数据失败,请按刷新按钮刷新页面", "提示", MessageBoxButton.OK);
        }

        private void OnGetChannelsByRecommendSucceeded(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            //获取到推荐影片
            xRecommendChannelsListBox.ItemsSource = args.Result.Channels;
            WaitSignal.Visibility = Visibility.Collapsed;
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            if (!_isPageLoaded)
            {
                e.Cancel = true;
                return;
            }
            if (NavigationService.BackStack.Count<System.Windows.Navigation.JournalEntry>() == 0)
            {
                try
                {
                    var vc = Microsoft.Devices.VibrateController.Default;
                    vc.Start(TimeSpan.FromMilliseconds(100));
                    if (MessageBox.Show("确定退出吗?", "提示", MessageBoxButton.OKCancel) != MessageBoxResult.OK)
                    {
                        e.Cancel = true;
                    }
                    else
                    {
                        PPTVData.Factory.DACFactory.DACQuit(Utils.Utils.GetClientVersion(), Utils.Utils.GetAppLifeTime());
                    }
                }
                catch (Exception exception)
                {
                    string message = exception.Message;
                    e.Cancel = true;
                }
            }
        }

        private void OnGetRecommendNavFailed(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
        }

        private void OnGetRecommendNavTimeout(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
            _getDataStatusHelp.Do(Init);
        }

        private bool EmptyImagesDirectory()
        {
            try
            {
                using (IsolatedStorageFile isoStore = IsolatedStorageFile.GetUserStoreForApplication())
                {
                    if (!isoStore.DirectoryExists("/shared/transfers"))
                    {
                        isoStore.CreateDirectory("/shared/transfers");
                    }
                    else
                    {
                        var files = IsolatedStorageFile.GetUserStoreForApplication().GetFileNames("/shared/transfers/*.*");
                        foreach (string fileName in files)
                        {
                            string path = "/shared/transfers/" + fileName;
                            isoStore.DeleteFile(path);
                        }
                    }
                }
            }
            catch (Exception exception)
            {
                string errorMessage = exception.Message;
                return false;
            }
            return true;
        }

        private void CheckUserLog()
        {
            if (Utils.Utils._isValidatedFromServer)
            {
                xLogedInfoGrid.Visibility = Visibility.Visible;
                xNormalInfoStackPanel.Visibility = Visibility.Collapsed;
                xUserNameTextBlock.Text = Utils.Utils._userStateInfo.UserName;
                xDegreeTextBlock.Text = "等级:" + Utils.Utils._userStateInfo.Degree;
                if (Utils.Utils._userStateInfo.VIP == 1)
                    xVipLogo.Visibility = Visibility.Visible;
                else
                    xVipLogo.Visibility = Visibility.Collapsed;
            }
            else
            {
                xLogedInfoGrid.Visibility = Visibility.Collapsed;
                xNormalInfoStackPanel.Visibility = Visibility.Visible;
            }
        }
        MainNavMenu mainNavMenu;
        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            CheckUserLog();
            mainNavMenu = FindVisualChildByName<MainNavMenu>(xPanItemRecommend, "xMainNavMenu");
            mainNavMenu.NavSelected += new MainNavMenu.NavTypeSelectedEventHandler(MainPage_NavSelected);
            mainNavMenu.InitVariableNEventHandle();
            _isPageLoaded = true;
        }

        private void MinimizedAppBar(bool Minminized)
        {
            if (Minminized)
            {
                ApplicationBar.Mode = ApplicationBarMode.Minimized;
                ApplicationBar.Buttons.Clear();
                WaitSignal.Margin = new Thickness(0, 0, 0, 30);
            }
            else
            {
                ApplicationBar.Mode = ApplicationBarMode.Default;
                WaitSignal.Margin = new Thickness(0, 0, 0, 70);
            }
        }

        private void OnAboutClick(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/AboutPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void OnRefreshClick(object sender, EventArgs e)
        {
            _isShowingMsgBox = false;
            Refresh();
        }

        private void SetApplicationBarMenuType(AppBarMenuType type)
        {
            ApplicationBar.MenuItems.Clear();
            if (type == AppBarMenuType.FIRSTPAGE)
            {
                ApplicationBar.Buttons.Add(Utils.CommonUtils.CreateRefreshAppBar(OnRefreshClick));
            }
            ApplicationBarMenuItem menuItemAbout = new ApplicationBarMenuItem();
            menuItemAbout.Text = "关于";
            menuItemAbout.Click += OnAboutClick;
            ApplicationBar.MenuItems.Add(menuItemAbout);
        }

        private void OnLoginOffClick(object sender, EventArgs e)
        {
            Utils.Utils.LogOff();
            SetAppBarButton("登录", new Uri("/Images/appbar.logon.png", UriKind.RelativeOrAbsolute), OnLogonClick);
            CheckUserLog();
        }

        private void OnLogonClick(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/LoginPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void SetAppBarButton(string title, Uri imageUri, EventHandler clickHandler)
        {
            ApplicationBar.Buttons.Clear();
            ApplicationBarIconButton button = new ApplicationBarIconButton(imageUri);
            button.Text = title;
            button.Click += clickHandler;
            ApplicationBar.Buttons.Add(button);
        }

        private void Panorama_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SetBar();
        }

        private void SetBar()
        {
            const int FIRST = -1;
            const int RECOMMENDATION = 0;
            const int CHANNEL = 1;
            const int SEARCH = 2;
            //const int VIEWHISTORY = 3;
            const int PERSONALCENTER = 3;

            switch (this.MainPanorama.SelectedIndex)
            {
                case FIRST:
                case RECOMMENDATION:
                    MinimizedAppBar(true);
                    SetApplicationBarMenuType(AppBarMenuType.FIRSTPAGE);
                    break;
                case CHANNEL:
                    MinimizedAppBar(true);
                    SetApplicationBarMenuType(AppBarMenuType.OTHERPAGE);
                    break;
                case SEARCH:
                    //case VIEWHISTORY:
                    MinimizedAppBar(false);
                    SetApplicationBarMenuType(AppBarMenuType.OTHERPAGE);
                    SetAppBarButton("清除", new Uri("/Images/appbar.delete.rest.png", UriKind.RelativeOrAbsolute), OnClear);
                    break;
                case PERSONALCENTER:
                    MinimizedAppBar(false);
                    SetApplicationBarMenuType(AppBarMenuType.PERSONALINFOPAGE);
                    if (Utils.Utils._isValidatedFromServer)
                    {
                        SetAppBarButton("注销", new Uri("/Images/appbar.logoff.png", UriKind.RelativeOrAbsolute), OnLoginOffClick);
                    }
                    else
                        SetAppBarButton("登录", new Uri("/Images/appbar.logon.png", UriKind.RelativeOrAbsolute), OnLogonClick);
                    break;
                default:
                    break;
            }
        }

        private void OnClear(object sender, EventArgs e)
        {
            const int SEARCH = 2;
            //const int VIEWHISTORY = 3;
            //const int FAVORITE = 4;

            switch (MainPanorama.SelectedIndex)
            {
                case SEARCH:
                    OnClearSearchHistory();
                    break;
                //case VIEWHISTORY:
                //    OnClearViewHistory();
                //    break;
                //case FAVORITE:
                //OnClearFavorite();
                //break;
                default:
                    break;
            }
        }

        private void OnClearSearchHistory()
        {
            if (MessageBox.Show("删除全部历史搜索记录?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                SearchHistoryFactory.Instance.RemoveAll();
            }
        }

        private void OnMainTitleTap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/AboutPage.xaml", UriKind.RelativeOrAbsolute));
        }

        public void GetUserInfo()
        {
            if (Utils.Utils.GetAutoLoginSetting() && string.IsNullOrEmpty(Utils.Utils.GetUserNa()) != true && string.IsNullOrEmpty(Utils.Utils.GetUserpd()) != true)
            {
                LoginFactory _loginFactory = new LoginFactory();
                _loginFactory.HttpFailHandler += _loginFactory_GetUserInfoFailed;
                _loginFactory.HttpSucessHandler += _loginFactory_GetUserInfoSuccess;
                _loginFactory.HttpTimeOutHandler += _loginFactory_GetUserInfoTimeout;
                _loginFactory.DownLoadDatas(new UserInfo(Utils.Utils.GetUserNa(), Utils.Utils.GetUserpd(), ""));
            }
        }

        void _loginFactory_GetUserInfoTimeout(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
        }

        void _loginFactory_GetUserInfoSuccess(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
            Utils.Utils._userStateInfo = args.Result;
            Utils.Utils._isValidatedFromServer = true;
            CheckUserLog();
        }

        void _loginFactory_GetUserInfoFailed(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
        }

        private void PersonalInfo_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (Utils.Utils._isValidatedFromServer)
            {
                NavigationService.Navigate(new Uri("/PersonalInfo.xaml", UriKind.RelativeOrAbsolute));
            }
            else
            {
                if (Utils.Utils.GetAutoLoginSetting() && Utils.Utils._isLogOff == true)
                    NavigationService.Navigate(new Uri("/PersonalInfo.xaml", UriKind.RelativeOrAbsolute));
                else
                    NavigationService.Navigate(new Uri("/LoginPage.xaml", UriKind.RelativeOrAbsolute));
            }
        }

        private void MyDownload_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/DownPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void Favorite_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/FavPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void History_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/HistoryPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void ShareSetting_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(Utils.SocialUtils.SocialShareUri);
        }
        private void SystemSetting_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(new Uri("/SystemSetting.xaml", UriKind.RelativeOrAbsolute));
        }
        private void Pursuit_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            NavigationService.Navigate(Utils.PursuitUtils.PursuitPageUrl);
        }

        void xItemBorder_Tap(object sender, ChannelInfo channelInfo)
        {
            int vid = channelInfo.VID;
            string page = String.Format("/DetailPage.xaml?vid={0}&programSource=1", vid);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }
    }

    public class FavData
    {
        public string ImageUrl { get; set; }
        public string ActorText { get; set; }
        public string MovieCover { get; set; }
        public string TitleText { get; set; }
        public double Rate { get; set; }
        public int Vid { get; set; }
        public int Id { get; set; }
    }

    public enum AppBarMenuType
    {
        FIRSTPAGE,
        OTHERPAGE,
        PERSONALINFOPAGE
    }
}