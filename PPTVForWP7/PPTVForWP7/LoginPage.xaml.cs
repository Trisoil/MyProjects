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
using Microsoft.Phone.Controls;

using PPTVData.Entity;
using PPTVData.Factory;
using Microsoft.Phone.Shell;
namespace PPTVForWP7
{
    public partial class LoginPage : PhoneApplicationPage
    {
        public LoginPage()
        {
            InitializeComponent();
            Init();
        }
        LoginFactory _loginFactory;
        bool isAuto;
        bool isSave;
        string _password = "";
        bool _isPass = false;
        private void Init()
        {

            isAuto = Utils.Utils.GetAutoLoginSetting();
            isSave = Utils.Utils.GetUserInfoSaveSetting();

            if (isAuto)
            {
                xAutoLogon.IsChecked = true;
            }
            else
                xAutoLogon.IsChecked = false;
            if (isSave)
            {
                xRembPass.IsChecked = true;
                if (string.IsNullOrEmpty(Utils.Utils.GetUserNa()) == true || string.IsNullOrEmpty(Utils.Utils.GetUserpd()) == true)
                {
                    xTbUserId.Text = "";
                    xTbPassword.Password = "";
                }
                else
                {
                    xTbUserId.Text = Utils.Utils.GetUserNa();
                    xTbPassword.Password = Utils.Utils.GetUserpd();
                }
            }
            else
                xRembPass.IsChecked = false;



            ApplicationBar.Buttons.Clear();
            ApplicationBarIconButton loginButton = new ApplicationBarIconButton(new Uri("/Images/appbar.check.rest.png", UriKind.RelativeOrAbsolute));
            loginButton.Text = "提交";
            loginButton.Click += new EventHandler(loginButton_Click);
            ApplicationBar.Buttons.Add(loginButton);

            ApplicationBarIconButton registerButton = new ApplicationBarIconButton(new Uri("/Images/appbar.reg.png", UriKind.RelativeOrAbsolute));
            registerButton.Text = "注册";
            registerButton.Click += new EventHandler(registerButton_Click);
            ApplicationBar.Buttons.Add(registerButton);
            ApplicationBar.Mode = ApplicationBarMode.Default;
        }

        private void CheckTbUserId()
        {
            if (string.IsNullOrEmpty(xTbUserId.Text))
            {
                xUserNameGrid.Visibility = Visibility.Visible;
                _isPass = false;
            }
            else
            {
                xUserNameGrid.Visibility = Visibility.Collapsed;
                _isPass = true;
            }
        }

        private void CheckTbPassword()
        {
            if (string.IsNullOrEmpty(xTbPassword.Password))
            {
                xPasswordGrid.Visibility = Visibility.Visible;
                _isPass = false;
            }
            else
            {
                xPasswordGrid.Visibility = Visibility.Collapsed;
                _isPass = true;
            }
        }

        void registerButton_Click(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/RegisterPage.xaml", UriKind.RelativeOrAbsolute));
        }

        void loginButton_Click(object sender, EventArgs e)
        {
            LoginClickHandle();
        }

        private void xTbUserId_GotFocus(object sender, RoutedEventArgs e)
        {
            //xBorderUserId.Visibility = Visibility.Visible;
            //SetState(CheckVBoardShowByTextBox());
        }

        private void xTbPassword_GotFocus(object sender, RoutedEventArgs e)
        {
            //xBorderPass.Visibility = Visibility.Visible;
            //SetState(CheckVBoardShowByPasswordBox());
        }

        private void xTbUserId_LostFocus(object sender, RoutedEventArgs e)
        {
            //xBorderUserId.Visibility = Visibility.Collapsed;
            CheckTbUserId();
            //SetState(CheckVBoardShowByTextBox());
        }

        private void xTbPassword_LostFocus(object sender, RoutedEventArgs e)
        {
            //xBorderPass.Visibility = Visibility.Collapsed;
            CheckTbPassword();
            //SetState(CheckVBoardShowByPasswordBox());
        }

        private void OnxLoginButtonClick(object sender, EventArgs e)
        {
            LoginClickHandle();
        }

        private void LoginClickHandle()
        {
            bool tem = false;
            CheckTbUserId();
            tem = _isPass;
            CheckTbPassword();

            if (_isPass && tem)
            {
                LoadingWait.Visibility = Visibility.Visible;
                _loginFactory = new LoginFactory();
                _loginFactory.HttpFailHandler += _loginFactory_GetUserInfoFailed;
                _loginFactory.HttpSucessHandler += _loginFactory_GetUserInfoSuccess;
                _loginFactory.DownLoadDatas(new UserInfo(xTbUserId.Text, xTbPassword.Password, ""));
                _password = xTbPassword.Password;
            }
        }

        bool _isBack = false;

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (NavigationContext.QueryString.ContainsKey("download"))
            {  
                DetailPage.backFromLoginPage = true;
                _isBack = true;
            }
            else if (NavigationContext.QueryString.ContainsKey("pursuit"))
            {
                _isBack = true;
            }
            else if (NavigationContext.QueryString.ContainsKey("VIP"))
            {
                _isBack = true;
            }
        }

        void _loginFactory_GetUserInfoSuccess(object sender, HttpFactoryArgs< UserStateInfo> args)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            var userStateInfo = args.Result;
            if (userStateInfo.UserState == 0)
            {
                //成功
                Utils.Utils._userStateInfo = userStateInfo;
                Utils.Utils._isValidatedFromServer = true;
                if (_isBack)
                {
                    if (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                    }
                }
                else
                    NavigationService.Navigate(new Uri("/PersonalInfo.xaml?validation=true", UriKind.RelativeOrAbsolute));
                if (xRembPass.IsChecked.Value)
                {
                    Utils.Utils.SetUserNaToIOS(userStateInfo.UserName);
                    Utils.Utils.SetUserpdToIOS(_password);
                    Utils.Utils.SetUserInfoSaveSwitchToIOS(true);
                }
                else
                    Utils.Utils.SetUserInfoSaveSwitchToIOS(false);

                if (xAutoLogon.IsChecked.Value)
                    Utils.Utils.SetAutoLoginToIOS(true);
                else
                    Utils.Utils.SetAutoLoginToIOS(false);
            }
            else
            {
                //不成功
                Utils.Utils._isValidatedFromServer = false;
                Utils.Utils._userStateInfo = null;
                Utils.Utils.SetUserNaToIOS(string.Empty);
                Utils.Utils.SetUserpdToIOS(string.Empty);
                Utils.Utils.SetAutoLoginToIOS(false);
                Utils.Utils.SetUserInfoSaveSwitchToIOS(false);

                string result = "";
                switch (userStateInfo.UserState)
                {
                    case 1: result = "用户不存在"; break;
                    case 2: result = "密码错误"; break;
                    case 3: result = "该用户已锁定"; break;
                    case 4: result = "该用户已过期"; break;
                    case 5: result = "该用户已删除"; break;
                    default: result = "登录失败";
                        break;
                }
                MessageBox.Show(result);

                if (_isBack)
                {
                    if (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                    }
                }

            }
        }

        void _loginFactory_GetUserInfoFailed(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
            if (_isBack)
            {
                if (NavigationService.CanGoBack)
                {
                    NavigationService.GoBack();
                }
            }
            else
            {
                LoadingWait.Visibility = Visibility.Collapsed;
            }
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            ApplicationBar.IsVisible = true;
        }
        

        private void OnOpenRegPageClick(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/RegisterPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void xRegisterButton_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.Navigate(new Uri("/RegisterPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void xTbUserId_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                xTbPassword.Focus();
        }

        private void xTbPassword_KeyDown(object sender, KeyEventArgs e)
        {
            //if (e.Key == Key.Enter)
            //    xLoginButton.Focus();
            ApplicationBar.IsVisible = true;
        }
 
        private void xAutoLogon_Checked(object sender, RoutedEventArgs e)
        {
            if (xAutoLogon.IsChecked.Value)
                xRembPass.IsChecked = true;
        }

        private void xRembPass_Unchecked(object sender, RoutedEventArgs e)
        {
            if (!xRembPass.IsChecked.Value)
                xAutoLogon.IsChecked = false;
        }

    }
}