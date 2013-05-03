using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Controls
{
    using PPTVData.Factory;

    public partial class LoginControl : UserControl
    {
        public LoginControl()
        {
            InitializeComponent();

            UpdateState();
        }

        public void UpdateState()
        {
            if (PersonalFactory.Instance.Logined)
            {
                personalGrid.Visibility = Visibility.Visible;
                loginGrid.Visibility = Visibility.Collapsed;
                personalGrid.DataContext = PersonalFactory.Instance.DataInfos[0].UserStateInfo;
            }
            else
            {
                loginGrid.Visibility = Visibility.Visible;
                personalGrid.Visibility = Visibility.Collapsed;
            }
        }

        public void LoginOut()
        {
            Utils.CommonUtils.ShowMessageConfirm("确定要注销当前账户吗？", LoginoutHandler);
        }

        private void LoginoutHandler()
        {
            PersonalFactory.Instance.LoginOut();
            UpdateState();
        }

        private void loginGrid_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (PersonalFactory.Instance.Logined)
                LoginOut();
            else
                Utils.CommonUtils.Navagate("/Pages/LoginPage.xaml");
        }
    }
}
