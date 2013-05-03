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
using PPTVData.Factory;
using PPTVData.Entity;

namespace PPTVForWP7
{
    public partial class PersonalInfo : PhoneApplicationPage
    {
        public PersonalInfo()
        {
            InitializeComponent();
        }
        private bool _validation = false;
        LoginFactory _loginFactory;

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (!(e.NavigationMode == System.Windows.Navigation.NavigationMode.Back))
            {
                try
                {
                    _validation = Convert.ToBoolean(NavigationContext.QueryString["validation"]);
                }
                catch (KeyNotFoundException)
                {
                    _validation = false;
                }
                if (_validation)
                {
                    InitElement();
                    NavigationService.RemoveBackEntry();
                }
                else
                {
                    //从其它页面过来,没有验证
                    if (Utils.Utils._isValidatedFromServer)
                    {
                        InitElement();
                    }
                    else if (!string.IsNullOrEmpty(Utils.Utils.GetUserNa())
                        && !string.IsNullOrEmpty(Utils.Utils.GetUserpd()))
                    {
                        _loginFactory = new LoginFactory();
                        _loginFactory.HttpFailHandler += _loginFactory_GetUserInfoFailed;
                        _loginFactory.HttpSucessHandler += _loginFactory_GetUserInfoSuccess;
                        _loginFactory.HttpTimeOutHandler += _loginFactory_GetUserInfoTimeout;
                        _loginFactory.DownLoadDatas(new UserInfo(Utils.Utils.GetUserNa(), Utils.Utils.GetUserpd(), ""));
                    }
                }
            }

        }

        private void LoginFail()
        {
            MessageBox.Show("网络异常,登录不成功");
        }

        void _loginFactory_GetUserInfoTimeout(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
            
        }

        void _loginFactory_GetUserInfoSuccess(object sender, HttpFactoryArgs<UserStateInfo> args)
        {
            var userStateInfo = args.Result;
            if (userStateInfo.UserState == 0)
            {
                Utils.Utils._isValidatedFromServer = true;
                Utils.Utils._userStateInfo = userStateInfo;
                InitElement();
            }
            else
            {
                //验证不正确
                Utils.Utils.SetAutoLoginToIOS(false);
                if (NavigationService.CanGoBack)
                    NavigationService.GoBack();
            }
        }

        void _loginFactory_GetUserInfoFailed(object sender, HttpFactoryArgs<UserStateInfo> args)
        {

        }

        private void InitElement()
        {
            if (Utils.Utils._userStateInfo != null)
            {
                xPersonalHeadPic.Source = Utils.Utils.GetImage(Utils.Utils._userStateInfo.FaceUrl);
                xID.Text = Utils.Utils._userStateInfo.UserName;
                switch (Utils.Utils._userStateInfo.VIP)
                {
                    case 0: xStatus.Text = "会员状态:普通会员"; xVipLogo.Visibility = Visibility.Collapsed; break;
                    case 1: xStatus.Text = "会员状态:VIP会员"; xVipLogo.Visibility = Visibility.Visible; break;
                    default:
                        break;
                }
                xLevel.Text = "等级:" + Utils.Utils._userStateInfo.Degree;
                xExp.Text = "经验:" + Utils.Utils._userStateInfo.Expence;
            }

        }

        private void ApplicationBarIconButton_Click(object sender, EventArgs e)
        {
            Utils.Utils.LogOff();
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

    }
}