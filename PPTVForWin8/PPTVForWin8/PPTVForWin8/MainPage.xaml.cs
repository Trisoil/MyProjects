using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Popups;
using Windows.Networking.Connectivity;
using Windows.ApplicationModel.Activation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Factory.Cloud;
    using PPTVData.Entity.WAY;
    using PPTVData.Factory.WAY;

    using PPTVForWin8.Pages;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.ViewModel.Download;

    public sealed partial class MainPage : Page
    {
        static WAYGetFactory _wayGetFactory;

        LoginFactory _loginFactory;
        object _navigatePara;

        public MainPage()
        {
            this.InitializeComponent();
            Window.Current.SizeChanged += Current_SizeChanged;

            _wayGetFactory = new WAYGetFactory();
            _wayGetFactory.HttpSucessHandler += wayGetFactory_HttpSucess;

            _loginFactory = new LoginFactory();
            _loginFactory.HttpSucessHandler += loginFactory_HttpSurcess;
        }

        public Frame MainPageFrame
        {
            get { return MainFrame; }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _navigatePara = e.Parameter;
            Navigate(_navigatePara);
        }

        private void Current_SizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            if (teachGrid.Visibility == Visibility.Visible)
                TeachNavigate();

            var viewState = ApplicationView.Value.ToString();
            VisualStateManager.GoToState(this, viewState, false);
        }

        private void teachGrid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            TeachNavigate();
        }

        private void TeachNavigate()
        {
            teachGrid.Visibility = Visibility.Collapsed;
            Navigate(_navigatePara);
        }

        private async void Navigate(object parameter)
        {
            await Init();
            TileUtils.CreateLogoNotifications();
            loadingGrid.Visibility = Visibility.Collapsed;

            var profile = NetworkInformation.GetInternetConnectionProfile();
            if (profile != null)
            {
                var netWork = profile.GetNetworkConnectivityLevel();
                if (netWork == NetworkConnectivityLevel.InternetAccess)
                {
                    this.MainFrame.Navigate(typeof(FirstPage)); 

                    if (parameter is LaunchActivatedEventArgs)
                    {
                        var paras = (parameter as LaunchActivatedEventArgs).Arguments;
                        ((App.Current) as App).SecondaryTileNavgate(paras);
                    }
                    else if (parameter is string)
                    {
                        this.MainFrame.Navigate(typeof(Pages.SearchPage), parameter);
                    }
                    else if (parameter is Windows.Storage.StorageFile)
                    {
                        this.MainFrame.Navigate(typeof(Pages.PlayPage), parameter);
                    }
                    return;
                }
            }
            CommonUtils.ShowMessageConfirm(Constants.NotNetworkTip, navigate_Determimine, navigate_Refresh, "确定", "刷新");
        }

        private async Task Init()
        {
            //这里的先后顺序有依赖的，不要随便调整
            await PersonalFactory.Instance.Init();
            await DeviceInfoFactory.Instance.Init();
            await LocalRecentFactory.Instance.Init();
            await LocalFavoritesFactory.Instance.Init();
            await DownloadViewModel.Instance.Init();
            await WAYLocalFactory.Instance.Init();

            InitWAY();
            InitLogin();
            DACFactory.Instance.StartUp();
        }

        private void navigate_Determimine()
        {
            this.MainFrame.Navigate(typeof(FirstPage));
            this.MainFrame.Navigate(typeof(Pages.DownloadedPage));
        }

        private void navigate_Refresh()
        {
            this.MainFrame.Navigate(typeof(FirstPage));
        }

        #region Personal Information

        public static void InitWAY()
        {
            var userName = string.Empty;
            if (PersonalFactory.Instance.Logined)
            {
                userName = PersonalFactory.Instance.DataInfos[0].UserStateInfo.UserName;
                WAYLocalFactory.Instance.SetUserName(userName);
            }
            _wayGetFactory.DownLoadDatas(userName, DeviceInfoFactory.Instance.DataInfos[0].DeviceId);
        }

        public void InitLogin()
        {
            if (PersonalFactory.Instance.Logined)
            {
                var personInfo = PersonalFactory.Instance.DataInfos[0];
                var pwd = PPTVData.Utils.DataCommonUtils.Decode(personInfo.UserPassWord, Utils.Constants.PasswordKey);
                _loginFactory.DownLoadDatas(personInfo.UserStateInfo.UserName, pwd);
            }
        }

        private void wayGetFactory_HttpSucess(object sender, HttpFactoryArgs<WAYGetInfo> e)
        {
            WAYGetFactory.WayGetInfo = e.Result;
        }

        private void loginFactory_HttpSurcess(object sender, HttpFactoryArgs<UserStateInfo> e)
        {
            if (e.Result.UserState == 0)
            {
                var personInfo = PersonalFactory.Instance.DataInfos[0];
                personInfo.UserStateInfo = e.Result;
                PersonalFactory.Instance.SubmitChange();
            }
        }

        #endregion
    }
}
