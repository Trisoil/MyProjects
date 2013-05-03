using System;
using System.Collections.Generic;
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTVForWin8.Pages;
    using PPTV.WinRT.CommonLibrary.Utils;

    public sealed partial class UserLogin : UserControl
    {
        LoginFactory _loginFactory;
        Popup _popUp;

        public UserLogin()
        {
            this.InitializeComponent();
            this.Loaded += UserLogin_Loaded;

            _popUp = new Popup();
            _popUp.IsLightDismissEnabled = false;
            _popUp.Opened += _popUp_Opened;

            closeSB.Completed += closeSB_Completed;

            _loginFactory = new LoginFactory();
            _loginFactory.HttpSucessHandler += LoginFactory_Sucess;
            _loginFactory.HttpFailHandler += LoginFactory_Fail;
            _loginFactory.HttpTimeOutHandler += LoginFactory_TimeOut;
            Init();
        }

        void closeSB_Completed(object sender, object e)
        {
            _popUp.IsOpen = false;
        }

        public Popup Popup { get { return _popUp; } }

        public EventHandler<RoutedEventArgs> LoginSucessHandler;

        private void Init()
        {
            if (PersonalFactory.Instance.HaveLoginInfo)
            {
                var userInfo = PersonalFactory.Instance.DataInfos[0];
                TxtUserName.Text = userInfo.UserStateInfo.UserName;
                TxtPassWord.Password = PPTVData.Utils.DataCommonUtils.Decode(userInfo.UserPassWord, Utils.Constants.PasswordKey);
                ToggleUserInfo.IsOn = true;
                ToggleAutoLogin.IsOn = userInfo.IsAutoLogin;
            }
        }

        void _popUp_Opened(object sender, object e)
        {
            VisualStateManager.GoToState(this, "Open", true);
        }

        void UserLogin_Loaded(object sender, RoutedEventArgs e)
        {
            TxtUserName.Focus(FocusState.Programmatic);
        }

        private void LoginFactory_Sucess(object sender, HttpFactoryArgs<UserStateInfo> e)
        {
            progressRing.Visibility = Visibility.Collapsed;
            var userInfo = e.Result;
            if (userInfo.UserState == 0)
            {
                var personalInfo = new PersonalInfo();
                personalInfo.IsSaveInfo = ToggleUserInfo.IsOn;
                personalInfo.IsAutoLogin = ToggleAutoLogin.IsOn;
                personalInfo.UserPassWord = PPTVData.Utils.DataCommonUtils.Encode(TxtPassWord.Password, Utils.Constants.PasswordKey);
                personalInfo.UserStateInfo = e.Result;
                PersonalFactory.Instance.InsertRecord(personalInfo);
                Popup.IsOpen = false;
                if (LoginSucessHandler != null)
                    LoginSucessHandler(this, new RoutedEventArgs());
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
                TileUtils.CreateToastNotifications(response);
            }
        }

        private void LoginFactory_Fail(object sender, HttpFactoryArgs<UserStateInfo> e)
        {
            progressRing.Visibility = Visibility.Collapsed;
            TileUtils.CreateToastNotifications(Utils.Constants.HttpFailTipText);
        }

        private void LoginFactory_TimeOut(object sender, HttpFactoryArgs<UserStateInfo> e)
        {
            progressRing.Visibility = Visibility.Collapsed;
            TileUtils.CreateToastNotifications(Utils.Constants.HttpTimeOutTipText);
        }

        /// <summary>
        /// 登录
        /// </summary>
        private void BtnLogin_Click(object sender, RoutedEventArgs e)
        {
            Login();
        }

        /// <summary>
        /// 密码输入框回车键
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TxtPassWork_KeyUp(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key == Windows.System.VirtualKey.Enter)
            {
                Login();
            }
        }

        private void Login()
        {
            var userName = TxtUserName.Text.Trim();
            var pwd = TxtPassWord.Password.Trim();
            if (!string.IsNullOrEmpty(userName)
                && !string.IsNullOrEmpty(pwd))
            {
                progressRing.Visibility = Visibility.Visible;
                _loginFactory.DownLoadDatas(userName, pwd);
            }
            else
            {
                TileUtils.CreateToastNotifications("用户名或密码为空");
            }
        }

        /// <summary>
        /// 取消
        /// </summary>
        private void BtnCancel_Click(object sender, RoutedEventArgs e)
        {
            closeSB.Begin();
        }

        /// <summary>
        /// 注册
        /// </summary>
        private async void BtnRegister_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(Utils.Constants.HttpRegisterUri);
        }

        private void rootGrid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            cancelButton.Focus(Windows.UI.Xaml.FocusState.Programmatic);
        }
    }
}
