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
using System.Diagnostics;
using Microsoft.Phone.Controls;

namespace PPTVForWP7.Pages.Social
{
    using WeiboSdk;
    using Newtonsoft.Json.Linq;
    using PPTVData.Entity.Social;

    public partial class SinaSocial : PhoneApplicationPage
    {
        string fromUri = string.Empty;

        public SinaSocial()
        {
            SdkData.AppKey = SinaSocialData.APIKey;
            SdkData.AppSecret = SinaSocialData.APISecret;
            SdkData.RedirectUri = SinaSocialData.RedirectUri;
            InitializeComponent();
        }

        /// <summary>
        /// 本页初始化完成
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            OAuthControl control = new OAuthControl();
            xOAuthControl.OAuthBack = oAuthControl_loginBack;
            xOAuthControl.OAuthBrowserNavigated = oauthBrowserNavigated;
            xOAuthControl.OAuthBrowserCancelled = oauthBrowserCancelled;
        }

        /// <summary>
        /// 授权页异步完成回调
        /// </summary>
        /// <param name="isSucess"></param>
        /// <param name="err"></param>
        /// <param name="responseData"></param>
        void oAuthControl_loginBack(bool isSucess, SdkAuthError err, SdkAuthRes responseData)
        {
            var result = false;
            if (isSucess)
            {
                var social = SocialViewModel.Instance.DataInfos[SocialType.SINA];
                social.IsBinded = true;
                social.AcessToken = responseData.acessToken;
                social.RefreshToken = responseData.acessTokenSecret;
                SocialViewModel.Instance.DataContext.SubmitChanges();
                result = true;
            }
            Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(Utils.SocialUtils.CreateFromUri(fromUri, string.Format("callBack={0}", result)));
            });
        }

        /// <summary>
        /// 取消授权
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void oauthBrowserCancelled(object sender, EventArgs e)
        {
            xLoadingWait.Visibility = Visibility.Collapsed;
            NavigationService.Navigate(Utils.SocialUtils.CreateFromUri(fromUri, string.Format("callBack={0}", false)));
        }

        /// <summary>
        /// 授权网页导航完成
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void oauthBrowserNavigated(object sender, EventArgs e)
        {
            xLoadingWait.Visibility = Visibility.Collapsed;
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            var url = NavigationService.BackStack.FirstOrDefault();
            if (url != null)
            {
                fromUri = Utils.SocialUtils.CreateToUri(url.Source.OriginalString);
            }
        }
    }
}