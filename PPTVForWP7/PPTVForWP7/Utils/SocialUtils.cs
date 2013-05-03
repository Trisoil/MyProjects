using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using System.Linq;

namespace PPTVForWP7.Utils
{
    using PPTVData.Entity.Social;

    public static class SocialUtils
    {
        #region Properties

        /// <summary>
        /// 分享设置页面
        /// </summary>
        public static readonly Uri SocialShareUri = new Uri("/Pages/Social/SharePage.xaml", UriKind.Relative);

        /// <summary>
        /// 新浪微博授权页面
        /// </summary>
        public static readonly Uri SinaSocialUri = new Uri("/Pages/Social/SinaSocial.xaml", UriKind.Relative);

        /// <summary>
        /// 腾讯微博授权页面
        /// </summary>
        public static readonly Uri TencentSocialUri = new Uri("/Pages/Social/TencentSocial.xaml", UriKind.Relative);

        /// <summary>
        /// 人人网授权页面
        /// </summary>
        public static readonly Uri RenrenSocialUri = new Uri("/Pages/Social/RenRenSocial.xaml", UriKind.Relative);

        #endregion

        /// <summary>
        /// 导航到具体社交网站授权页
        /// </summary>
        /// <param name="socialType"></param>
        /// <param name="page"></param>
        public static void NavigateToSocial(SocialType socialType, PhoneApplicationPage page)
        {
            switch (socialType)
            {
                case SocialType.SINA:
                    page.NavigationService.Navigate(SinaSocialUri); break;
                case SocialType.TENCENT:
                    page.NavigationService.Navigate(TencentSocialUri); break;
                case SocialType.RENREN:
                    page.NavigationService.Navigate(RenrenSocialUri); break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 生成授权完成返回页面URI
        /// </summary>
        /// <param name="fromUri"></param>
        /// <param name="parameters"></param>
        /// <returns></returns>
        public static Uri CreateFromUri(string fromUri, string parameters)
        {
            if (fromUri.Contains('?'))
                fromUri += ('&' + parameters);
            else
                fromUri += ('?' + parameters);
            return new Uri(fromUri, UriKind.Relative);
        }

        /// <summary>
        /// 生成从何面导航到授权页的
        /// </summary>
        /// <param name="toUri"></param>
        /// <returns></returns>
        public static string CreateToUri(string toUri)
        {
            return toUri.Contains("callBack") ? toUri.Substring(0, toUri.IndexOf("callBack") - 1) : toUri;
        }

        /// <summary>
        /// 清除授权页在页面后退堆栈中的记录
        /// </summary>
        /// <param name="page"></param>
        public static void ClearBackStack(PhoneApplicationPage page, string clearUri)
        {
            var url = page.NavigationService.BackStack.FirstOrDefault();
            while (url != null && (url.Source.OriginalString.Contains("Social") ||
                (!string.IsNullOrEmpty(clearUri) && url.Source.OriginalString.Contains(clearUri))))
            {
                page.NavigationService.RemoveBackEntry();
                url = page.NavigationService.BackStack.FirstOrDefault();
            }
        }

        /// <summary>
        /// 生成绑定回调提示信息
        /// </summary>
        /// <param name="page"></param>
        /// <returns></returns>
        public static string CreateBindResponse(PhoneApplicationPage page)
        {
            if (page.NavigationContext.QueryString.ContainsKey("callBack"))
            {
                var result = false;
                if (bool.TryParse(page.NavigationContext.QueryString["callBack"], out result))
                {
                    return result ? "绑定成功" : "绑定失败";
                }
            }
            return string.Empty;
        }
    }
}
