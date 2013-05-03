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
using System.Text.RegularExpressions;

using PPTVData.Entity;
using PPTVData.Factory;
using Microsoft.Phone.Shell;
namespace PPTVForWP7
{
    public partial class RegisterPage : PhoneApplicationPage
    {
        public RegisterPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(RegisterPage_Loaded);
        }

        void RegisterPage_Loaded(object sender, RoutedEventArgs e)
        {
            Init();
        }
        bool _isNamePass = false;
        bool _isPwd1Pass = false;
        bool _isPwd2Pass = false;
        bool _isMailPass = false;

        string _str_UserName_Null = "用户名不能为空";
        string _str_UserName_Init = "可为数字,文字,字母和下划线长度为6-16位";
        string _str_UserName_Ex20 = "用户名为6-16位";

        string _str_Password_Null = "密码不能为空";
        string _str_Password_Ex6_18 = "密码可为数字字母,且密码长度为6-18位";
        string _str_Password_DifT = "输入的两次密码不一样";

        string _str_Mail_Init = "例如:example@xxx.com";
        //string _str_Mail_Null = "邮箱不能为空";
        string _str_Mail_Dis = "填写的邮箱格式不正确";

        RegisterFactory _registerFactory = new RegisterFactory();
        #region
        private void CheckUserName()
        {
            if (xUserNameTextBox.Text == "")
            {
                xUserNameGrid.Visibility = Visibility.Visible;
                xUserNameTextBlock.Text = _str_UserName_Null;
                xUserNameTextBox.Text = _str_UserName_Init;
                _isNamePass = false;
            }
            else if (xUserNameTextBox.Text.Length > 16 || xUserNameTextBox.Text.Length < 6)
            {
                xUserNameGrid.Visibility = Visibility.Visible;
                xUserNameTextBlock.Text = _str_UserName_Ex20;
                _isNamePass = false;
            }
            else
            {
                if (!IsVail(xUserNameTextBox.Text))
                {
                    xUserNameGrid.Visibility = Visibility.Visible;
                    xUserNameTextBlock.Text = _str_UserName_Init;
                    _isNamePass = false;
                }
                else
                    _isNamePass = true;
            }
        }

        private bool IsVail(string str)
        {
            int len2 = Regex.Replace(str, @"[\u4e00-\u9fa5]", "11").Length;
            bool res = Regex.IsMatch(str, @"^[A-Za-z_\u4e00-\u9fa5][A-Za-z0-9_\u4e00-\u9fa5]{2,19}$");
            if (len2 > 5 && len2 < 21 && res == true) { return true; } else { return false; }
        }

        private void CheckPassword1Box()
        {
            if (string.IsNullOrEmpty(xPassword1Box.Password))
            {
                xPassword1Grid.Visibility = Visibility.Visible;
                xPassword1TextBlock.Text = _str_Password_Null;
                _isPwd1Pass = false;
            }
            else if (xPassword1Box.Password.Length < 6 || xPassword1Box.Password.Length > 18)
            {
                xPassword1Grid.Visibility = Visibility.Visible;
                xPassword1TextBlock.Text = _str_Password_Ex6_18;
                _isPwd1Pass = false;
            }
            else
                _isPwd1Pass = true;
        }

        private void CheckPassword2Box()
        {
            if (xPassword1Box.Password != xPassword2Box.Password)
            {
                xPassword2Grid.Visibility = Visibility.Visible;
                xPassword2TextBlock.Text = _str_Password_DifT;
                _isPwd2Pass = false;
            }
            else
                _isPwd2Pass = true;
        }

        private void CheckMail()
        {
            if (string.IsNullOrEmpty(xMailTextBox.Text) || xMailTextBox.Text == _str_Mail_Init)
            {
                xMailTextBox.Text = _str_Mail_Init;
                _isMailPass = false;
            }
            else
            {
                Regex mailRegex = new Regex(@"^\w+@\w+(\.\w+)+(\,\w+@\w+(\.\w+)+)*$");
                if (mailRegex.Match(xMailTextBox.Text).Success == false || string.IsNullOrEmpty(xMailTextBox.Text))
                {
                    xMailGrid.Visibility = Visibility.Visible;
                    xMailTextBlock.Text = _str_Mail_Dis;
                    _isMailPass = false;
                }
                else
                    _isMailPass = true;
            }
        }
        #endregion

        private void Init()
        {
            xUserNameTextBox.Text = _str_UserName_Init;
            xMailTextBox.Text = _str_Mail_Init;

            ApplicationBar.Buttons.Clear();
            ApplicationBarIconButton registerButton = new ApplicationBarIconButton(new Uri("/Images/appbar.check.rest.png", UriKind.RelativeOrAbsolute));
            registerButton.Text = "提交";
            registerButton.Click += new EventHandler(registerButton_Click);
            ApplicationBar.Buttons.Add(registerButton);
            ApplicationBar.Mode = ApplicationBarMode.Default;

            ApplicationBarIconButton loginButton = new ApplicationBarIconButton(new Uri("/Images/appbar.logon.png", UriKind.RelativeOrAbsolute));
            loginButton.Text = "登录";
            loginButton.Click += new EventHandler(loginButton_Click);
            ApplicationBar.Buttons.Add(loginButton);
        }

        void loginButton_Click(object sender, EventArgs e)
        {
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

        void registerButton_Click(object sender, EventArgs e)
        {
            Sumbit();
        }

        private void xUserNameTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            CheckUserName();
            if (string.IsNullOrEmpty(xUserNameTextBox.Text))
                xUserNameTextBox.Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(0xFF, 0x89, 0x89, 0x89));
        }

        private void xUserNameTextBox_GotFocus(object sender, RoutedEventArgs e)
        {
            (sender as TextBox).Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(255, 255, 255, 255));
            xUserNameGrid.Visibility = Visibility.Collapsed;
            if (xUserNameTextBox.Text == _str_UserName_Init)
                xUserNameTextBox.Text = "";
        }

        private void xPassword1Box_GotFocus(object sender, RoutedEventArgs e)
        {
            (sender as PasswordBox).Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(255, 255, 255, 255));
            xPassword1Grid.Visibility = Visibility.Collapsed;
        }

        private void xPassword1Box_LostFocus(object sender, RoutedEventArgs e)
        {
            CheckPassword1Box();
            if (string.IsNullOrEmpty(xPassword1TextBlock.Text))
                xPassword1TextBlock.Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(0xFF, 0x89, 0x89, 0x89));
        }

        private void xPassword2Box_GotFocus(object sender, RoutedEventArgs e)
        {
            //xMailTextBoxBak.Focus();


            xPassword2Grid.Visibility = Visibility.Collapsed;
            (sender as PasswordBox).Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(255, 255, 255, 255));
        }

        protected override Size ArrangeOverride(Size bound)
        {
            return base.ArrangeOverride(bound);
        }

        private void xPassword2Box_LostFocus(object sender, RoutedEventArgs e)
        {
            CheckPassword2Box();
            if (string.IsNullOrEmpty(xPassword2TextBlock.Text))
                xPassword2TextBlock.Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(0xFF, 0x89, 0x89, 0x89));
        }

        private void xMailTextBox_GotFocus(object sender, RoutedEventArgs e)
        {
            (sender as TextBox).Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(255, 255, 255, 255));
            xMailGrid.Visibility = Visibility.Collapsed;
            if (xMailTextBox.Text == _str_Mail_Init)
                xMailTextBox.Text = "";
        }

        private void xMailTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            CheckMail();
            if (string.IsNullOrEmpty(xPassword2TextBlock.Text))
                xPassword2TextBlock.Foreground = new System.Windows.Media.SolidColorBrush(Color.FromArgb(0xFF, 0x89, 0x89, 0x89));
        }

        private void Sumbit()
        {
            CheckUserName();
            CheckPassword2Box();
            CheckMail();
            if (_isNamePass && _isPwd1Pass && _isPwd2Pass && _isMailPass)
            {
                //开始注册
                RegisterEventHandle();
                _registerFactory.PostRegisterInfoAsync(new RegisterInfo(xUserNameTextBox.Text, xPassword2Box.Password, xMailTextBox.Text));
                LoadingWait.Visibility = Visibility.Visible;
            }
            else
            {

            }
        }

        private void xRegisterButton_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            Sumbit();
        }

        void RegisterEventHandle()
        {
            _registerFactory = new RegisterFactory();
            _registerFactory.PostRegisterInfoFailed += new RegisterFactory.PostRegisterInfoFailedEventHandler(_registerFactory_PostRegisterInfoFailed);
            _registerFactory.PostRegisterInfoSuccess += new RegisterFactory.PostRegisterInfoSuccessEventHandler(_registerFactory_PostRegisterInfoSuccess);
            _registerFactory.PostRegisterInfoTimeout += new RegisterFactory.PostRegisterInfoTimeoutEventHandler(_registerFactory_PostRegisterInfoTimeout);
        }

        void _registerFactory_PostRegisterInfoTimeout(object sender)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            MessageBox.Show("网络超时");
        }

        void _registerFactory_PostRegisterInfoSuccess(object sender, string resultState, string resultContent)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            MessageBox.Show(resultContent);
        }

        void _registerFactory_PostRegisterInfoFailed(object sender, Exception exc)
        {
            LoadingWait.Visibility = Visibility.Collapsed;
            MessageBox.Show("注册失败");
        }

        private void xUserNameTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                xPassword1Box.Focus();
        }

        private void xPassword1Box_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                xPassword2Box.Focus();
        }

        private void xPassword2Box_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                xMailTextBox.Focus();
        }

        private void xMailTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                Sumbit();
        }
    }
}