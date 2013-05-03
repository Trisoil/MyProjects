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
    using WeiboSdk;
    using PPTVData.Entity;

    public class SinaSocialData : SocialBase
    {
        #region Properties

        public static string APIKey = "2374112657";

        public static string APISecret = "b3edc04c39ca2e35d98f8dd59c4ab52d";

        public static string RedirectUri = "http://www.weibo.com";

        #endregion

        static SinaSocialData()
        {
            SdkData.AppKey = SinaSocialData.APIKey;
            SdkData.AppSecret = SinaSocialData.APISecret;
            SdkData.RedirectUri = SinaSocialData.RedirectUri;
        }

        /// <summary>
        /// 新浪微博发布一条新微博
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        protected override void SocialSharePublish(string channelName, 
            string channelUrl, string message, SocialBase.SocialShareHandler callBack)
        {
            try
            {
                var social = SocialViewModel.Instance.DataInfos[SocialType.SINA];
                var netEngine = new SdkNetEngine();
                var cmdBase = new cmdUploadMessage
                {
                    status = message,
                    acessToken = social.AcessToken,
                    acessTokenSecret = social.RefreshToken
                };

                netEngine.RequestCmd(SdkRequestType.UPLOAD_MESSAGE, cmdBase, (type, response) =>
                {
                    if (response.errCode == SdkErrCode.SUCCESS)
                    {
                        callBack(true, response.content);
                    }
                    else
                    {
                        callBack(false, response.content);
                    }
                });
            }
            catch(Exception ex)
            {
                callBack(false, ex.Message);
            }
        }
    }
}
