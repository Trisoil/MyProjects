using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.Utils;

    public partial class LoginPage : PhoneApplicationPage
    {
        LoginFactory _loginFactory;

        public static Action LoginSucessHandler;

        public LoginPage()
        {
            InitializeComponent();

            _loginFactory = new LoginFactory();
            _loginFactory.HttpSucessHandler += LoginFactory_Sucess;
            _loginFactory.HttpFailorTimeOut += LoginFactory_TimeoutOrFail;

            Init();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            NavigationService.RemoveBackEntry();
        }

        private void Init()
        {
            if (PersonalFactory.Instance.HaveLoginInfo)
            {
                var userInfo = PersonalFactory.Instance.DataInfos[0];
                txtUserName.Text = userInfo.UserStateInfo.UserName;
                txtPassWord.Password = PPTVData.Utils.DataCommonUtils.Decode(userInfo.UserPassWord, Utils.Constants.PasswordKey);
                toggleUserInfo.IsChecked = true;
                toggleAutoLogin.IsChecked = userInfo.IsAutoLogin;
            }
        }

        #region HttpHandler

        private void LoginFactory_Sucess(object sender, HttpFactoryArgs<UserStateInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            var userInfo = e.Result;
            if (userInfo.UserState == 0)
            {
                var personalInfo = new PersonalInfo();
                personalInfo.IsSaveInfo = toggleUserInfo.IsChecked.Value;
                personalInfo.IsAutoLogin = toggleAutoLogin.IsChecked.Value;
                personalInfo.UserPassWord = PPTVData.Utils.DataCommonUtils.Encode(txtPassWord.Password, Utils.Constants.PasswordKey);
                personalInfo.UserStateInfo = e.Result;
                personalInfo.IsCurrentLogin = true;
                PersonalFactory.Instance.InsertRecord(personalInfo);
                if (NavigationService.CanGoBack)
                    NavigationService.GoBack();
                if (LoginSucessHandler != null)
                    LoginSucessHandler();
            }
            else
            {
                var response = string.Empty;
                switch (userInfo.UserState)
                {
                    case 1:
                        response = "该用户名不存在"; break;
                    case 2:
                        response = "密码错误"; break;
                    case 3:
                        response = "该用户已锁定"; break;
                    case 4:
                        response = "该用户已过期"; break;
                    case 5:
                        response = "该用户已删除"; break;
                    default:
                        response = "登录失败"; break;
                }
                TileUtils.CreateBasicToast(response);
            }
        }

        private void LoginFactory_TimeoutOrFail()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            TileUtils.CreateBasicToast(Utils.Constants.HttpFailTipText);
        }

        #endregion

        private void Login()
        {
            var userName = txtUserName.Text.Trim();
            var pwd = txtPassWord.Password.Trim();
            if (!string.IsNullOrEmpty(userName)
                && !string.IsNullOrEmpty(pwd))
            {
                loadingTip.Visibility = Visibility.Visible;
                _loginFactory.DownLoadDatas(userName, pwd);
            }
            else
            {
                TileUtils.CreateBasicToast("用户名或密码为空");
            }
        }

        private void ApplicationBarLogin_Click(object sender, EventArgs e)
        {
            Login();
        }
    }
}