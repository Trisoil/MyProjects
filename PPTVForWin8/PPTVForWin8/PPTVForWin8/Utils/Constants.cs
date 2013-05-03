using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Media.Imaging;

namespace PPTVForWin8.Utils
{
    public static class Constants
    {
        public static readonly string NotNetworkTip = "当前检测不到网络，去看已下载的内容？";

        public static readonly string HttpTimeOutTipText = "服务超时，请检测网络后刷新重试";

        public static readonly string HttpFailTipText = "服务获取失败，请检测网络后刷新重试";

        public static readonly string CoverFail = "封面信息获取失败，请检查网络后刷新重试";

        public static readonly string VodPageFail = "节目列表信息获取失败，请检查网络后刷新重试";

        public static readonly string DetailFail = "节目详情获取失败，请检查网络后刷新重试";

        public static readonly string ClearConfirm = "确定清空所有的记录吗？";

        public static readonly string LogginOut = "确定要注销账户{0}吗？";

        public static readonly string NotVip = "您还不是VIP会员,请先到PPTV官网充值成为VIP会员";

        public static readonly string NeedVip = "观看此节目需要先登录并成为VIP会员才能观看，如果你不是VIP会员，请去PPTV官网购买";

        public static readonly string ShareTips = "我正在#PPTV Win8客户端#观看#{0}#{1}";

        public static readonly string VolumeSettingKey = "VolumeSettingKey";

        public static readonly string TeachSettingKey = "TeachSetttingKey";

        public static readonly string PasswordKey = "70706C6976656F6B";

        /// <summary>
        /// 网页注册Uri
        /// </summary>
        public static readonly Uri HttpRegisterUri = new Uri("http://passport.pptv.com/registerandlogin.do?regfromurl=http%3A%2F%2Fwww.pptv.com%2F");

        /// <summary>
        /// 隐私策略Uri
        /// </summary>
        public static readonly Uri HttpPolicesUri = new Uri("http://www.pptv.com/aboutus/certificate/privacy/");

        /// <summary>
        /// 未登录图标
        /// </summary>
        public static BitmapImage LoginImage = new BitmapImage(new Uri("ms-appx:///Images/login.png"));

        /// <summary>
        /// 已登录图标
        /// </summary>
        public static BitmapImage LoginedImage = new BitmapImage(new Uri("ms-appx:///Images/logined.png"));

        /// <summary>
        /// Logo图标uri
        /// </summary>
        public static Uri LogoUri = new Uri("ms-appx:///Assets/Logo.png");

    }
}
