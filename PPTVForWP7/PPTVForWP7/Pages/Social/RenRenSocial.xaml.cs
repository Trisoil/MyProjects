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

namespace PPTVForWP7.Pages.Social
{
    using PPTVData.Entity.Social;
    using PPTVForWP7.Controls;
    using Newtonsoft.Json.Linq;

    public partial class RenRenSocial : PhoneApplicationPage
    {
        string fromUri = string.Empty;

        public RenRenSocial()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 获取AcessToken完成后回调
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xOAuthControl_AcessTokenCompletedEvent(object sender, RenrenEventArgs e)
        {
            var result = false;
            try
            {
                var jsonObj = JObject.Parse(e.Response);
                var social = SocialViewModel.Instance.DataInfos[SocialType.RENREN];
                social.AcessToken = jsonObj["access_token"].ToString();
                social.RefreshToken = jsonObj["refresh_token"].ToString();
                social.Expire = DateTime.Now.AddSeconds(Convert.ToInt32(jsonObj["expires_in"].ToString())).Ticks;
                social.IsBinded = true;
                SocialViewModel.Instance.DataContext.SubmitChanges();
                result = true;
            }
            catch { }
            Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(Utils.SocialUtils.CreateFromUri(fromUri, string.Format("callBack={0}", result)));
            });
        }

        /// <summary>
        /// 授权网页导航完成
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xOAuthControl_LoadPageCompleted(object sender, RenrenEventArgs e)
        {
            xLoadingWait.Visibility = Visibility.Collapsed;
        }

        /// <summary>
        /// 用户拒绝授权
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xOAuthControl_OAuthDeniedEvent(object sender, RenrenEventArgs e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(Utils.SocialUtils.CreateFromUri(fromUri, string.Format("callBack={0}", false)));
            });
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