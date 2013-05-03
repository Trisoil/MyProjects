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
using System.Windows.Navigation;

namespace PPTVForWP7.Pages.Social
{
    using TencentWeiboSDK;
    using PPTVData.Entity.Social;

    public partial class TencentSocial : PhoneApplicationPage
    {
        string fromUri = string.Empty;

        public TencentSocial()
        {
            OAuthConfigruation.APP_KEY = TencentSocialData.APIKey;
            OAuthConfigruation.APP_SECRET = TencentSocialData.APISecret;
            OAuthConfigruation.IfSaveAccessToken = false;
            InitializeComponent();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            xOAuthControl.LoadCompletedAction += xAuthControl_LoadCompleted;
            var result = false;
            xOAuthControl.OAuthLogin((callback) =>
            {
                if (callback.Succeed)
                {
                    var social = SocialViewModel.Instance.DataInfos[SocialType.TENCENT];
                    social.IsBinded = true;
                    social.AcessToken = string.Format("oauth_token={0}&oauth_token_secret={1}", callback.Data.TokenKey, callback.Data.TokenSecret);
                    SocialViewModel.Instance.DataContext.SubmitChanges();
                    result = true;
                }
                Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(Utils.SocialUtils.CreateFromUri(fromUri, string.Format("callBack={0}", result)));
                });
            });
        }

        /// <summary>
        /// 授权页面加载完成
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xAuthControl_LoadCompleted(object sender, NavigationEventArgs e)
        {
            xLoadingWait.Visibility = Visibility.Collapsed;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var url = NavigationService.BackStack.FirstOrDefault();
            if (url != null)
            {
                fromUri = Utils.SocialUtils.CreateToUri(url.Source.OriginalString);
            }
        }
    }
}