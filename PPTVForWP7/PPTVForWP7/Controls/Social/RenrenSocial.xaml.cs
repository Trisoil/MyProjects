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

namespace PPTVForWP7.Controls
{
    using PPTVData;
    using PPTVData.Entity.Social;
    using PPTVData.Entity.Social.Renren;

    public class RenrenEventArgs : EventArgs
    {
        public RenrenEventArgs()
        {
        }

        public RenrenEventArgs(string response)
        {
            Response = response;
        }

        public string Response { get; private set; }

    }

    public partial class RenrenSocial : UserControl
    {
        public delegate void RenrenHttpAsyncHandler(object sender, RenrenEventArgs e);
        public event RenrenHttpAsyncHandler AcessTokenCompletedEvent;
        public event RenrenHttpAsyncHandler OAuthDeniedEvent;
        public event RenrenHttpAsyncHandler LoadPageCompleted;

        public RenrenSocial()
        {
            InitializeComponent();
            string uri = string.Format("{0}client_id={1}&response_type=code&scope={2}&redirect_uri={3}&display=touch",
                RenrenSocialData.LoginAuth, RenrenSocialData.APIKey, RenrenSocialData.Scope, RenrenSocialData.RedirectUri);
            Dispatcher.BeginInvoke(() =>
            {
                xWebBrowser.Navigate(new Uri(uri));
            });
        }

        /// <summary>
        /// 页面正在导航，用于获取页面跳转间的参数决定下一步动作
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xWebBrowser_Navigating(object sender, Microsoft.Phone.Controls.NavigatingEventArgs e)
        {
            var code = HttpUtils.GetQuerystring(e.Uri, "code");
            if (!string.IsNullOrEmpty(code))
            {
                var renren = new RenrenApiClient();
                renren.AccessToken(code, responseData => {
                    if (AcessTokenCompletedEvent != null)
                        AcessTokenCompletedEvent(this, new RenrenEventArgs(responseData));
                });
            }
            var error = HttpUtils.GetQuerystring(e.Uri, "error");
            if (error == "login_denied")
            {
                if (OAuthDeniedEvent != null)
                {
                    OAuthDeniedEvent(this, new RenrenEventArgs ());
                }
            }
        }

        /// <summary>
        /// 页面导航完成，触发完成时间
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xWebBrowser_LoadCompleted(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            if (LoadPageCompleted != null)
                LoadPageCompleted(this, new RenrenEventArgs());
        }
    }
}
