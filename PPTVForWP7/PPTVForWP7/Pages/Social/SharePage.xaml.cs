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

    public partial class SharePage : PhoneApplicationPage
    {
        public SharePage()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (e.NavigationMode != System.Windows.Navigation.NavigationMode.Back)
            {
                Utils.SocialUtils.ClearBackStack(this, string.Empty);
                var response = Utils.SocialUtils.CreateBindResponse(this);
                if (!string.IsNullOrEmpty(response))
                {
                    xPopDialog.ContentText = response;
                    xPopDialog.IsShowEnable = true;
                }
            }
            if (SocialViewModel.Instance.DataInfos[SocialType.SINA].IsBinded)
            {
                xSinaSocial.IsBinded = true;
            }
            if (SocialViewModel.Instance.DataInfos[SocialType.TENCENT].IsBinded)
            {
                xTencentSocial.IsBinded = true;
            }
            if (SocialViewModel.Instance.DataInfos[SocialType.RENREN].IsBinded)
            {
                xRenrenSocial.IsBinded = true;
            }
        }

        /// <summary>
        /// 绑定事件
        /// </summary>
        private void xSocial_BindSocialEvent(object sender, RoutedEventArgs e)
        {
            var shareItem = sender as ShareSettingItem;
            if (shareItem != null)
            {
                Utils.SocialUtils.NavigateToSocial(shareItem.SocialType, this);
            }
        }

        /// <summary>
        /// 取消绑定事件
        /// </summary>
        private void xSocial_UnBindSocialEvent(object sender, RoutedEventArgs e)
        {
            var shareItem = sender as ShareSettingItem;
            if (shareItem != null &&
                MessageBox.Show("确定取消绑定吗?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                SocialViewModel.Instance.DataInfos[shareItem.SocialType].IsBinded = false;
                SocialViewModel.Instance.DataContext.SubmitChanges();
                switch (shareItem.SocialType)
                {
                    case SocialType.SINA:
                        xSinaSocial.IsBinded = false;
                        break;
                    case SocialType.TENCENT:
                        xTencentSocial.IsBinded = false;
                        break;
                    case SocialType.RENREN:
                        xRenrenSocial.IsBinded = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}