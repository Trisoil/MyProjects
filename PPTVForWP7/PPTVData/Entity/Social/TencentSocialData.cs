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

namespace PPTVData.Entity.Social
{
    using TencentWeiboSDK;
    using TencentWeiboSDK.Model;
    using TencentWeiboSDK.Services;
    using TencentWeiboSDK.Services.Util;

    public class TencentSocialData : SocialBase
    {
        #region Properties

        public static string APIKey = "801093110";

        public static string APISecret = "4b729e3d4946db304db789065fda1f10";

        #endregion

        static TencentSocialData()
        {
            OAuthConfigruation.APP_KEY = TencentSocialData.APIKey;
            OAuthConfigruation.APP_SECRET = TencentSocialData.APISecret;
            OAuthConfigruation.IfSaveAccessToken = false;
        }

        /// <summary>
        /// 发布一条新微博
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        protected override void SocialSharePublish(string channelName, 
            string channelUrl, string message, SocialBase.SocialShareHandler callBack)
        {
            var social = SocialViewModel.Instance.DataInfos[SocialType.TENCENT];
            OAuthConfigruation.AccessToken = new AccessToken(social.AcessToken);
            var share = new TService();
            var arg = new ServiceArgument()
            {
                Content = message,
                Url = channelUrl
            };
            try
            {
                share.AddVideo(arg, obj =>
                {
                    if (obj.Succeed)
                        callBack(true, string.Empty);
                    else
                        callBack(false, obj.InnerException.Message);
                });
            }
            catch (Exception ex)
            {
                callBack(false, ex.Message);
            }
        }
    }
}
