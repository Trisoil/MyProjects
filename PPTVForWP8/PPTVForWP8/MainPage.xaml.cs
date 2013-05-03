using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Threading.Tasks;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.WAY;
    using PPTVData.Factory.WAY;
    using PPTVData.Factory.Cloud;

    using PPTV.WPRT.CommonLibrary.ViewModel.Download;
    
    public partial class MainPage : PhoneApplicationPage
    {
        static WAYGetFactory _wayGetFactory;

        LoginFactory _loginFactory;

        public MainPage()
        {
            InitializeComponent();

            _wayGetFactory = new WAYGetFactory();
            _wayGetFactory.HttpSucessHandler += wayGetFactory_HttpSucess;

            _loginFactory = new LoginFactory();
            _loginFactory.HttpSucessHandler += loginFactory_HttpSurcess;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Navigate(null);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            NavigationService.RemoveBackEntry();
        }

        private async void Navigate(object parameter)
        {
            await Init();

            NavigationService.Navigate(new Uri("/Pages/FirstPage.xaml", UriKind.Relative));
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