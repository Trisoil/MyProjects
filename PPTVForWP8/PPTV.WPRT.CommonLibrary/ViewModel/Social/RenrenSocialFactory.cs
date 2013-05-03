using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Social
{
    using Newtonsoft.Json.Linq;

    using PPTV.WPRT.CommonLibrary.DataModel.Social;

    public class RenrenSocialFactory : SocialBase
    {
        #region Properties

        public static string APIId = "178032";

        public static string APIKey = "dcae4bccfd1f4c8ba1ac7e92421ec928";

        public static string APISecret = "6e7c80e70b41491582a17a269eafaac4";

        public static string RenrenAPIUrl = "http://api.renren.com/restserver.do";

        public static string TokenUrl = "https://graph.renren.com/oauth/token";

        public static string RedirectUri = "http://graph.renren.com/oauth/login_success.html";

        public static string LoginAuth = "https://graph.renren.com/oauth/authorize?";

        public static string OAuthUri = "https://graph.renren.com/oauth/token";

        public static string Scope = "publish_feed publish_share";

        #endregion

        /// <summary>
        /// 判断Token是否需要刷新
        /// </summary>
        public static void JudgeToken(SocialDataInfo social)
        {
            if (string.IsNullOrEmpty(social.RefreshToken))
            {
                social.IsBinded = false;
                return;
            }
            if (DateTime.Now.Ticks >= social.Expire)
            {
                var renren = new RenrenApiClient();
                renren.RefreshToken(social.RefreshToken, responseData =>
                {
                    try
                    {
                        var jsonObj = JObject.Parse(responseData);
                        var new_token = jsonObj["access_token"].ToString();
                        if (social.AcessToken == new_token)
                            return;
                        social.AcessToken = new_token;
                        social.Expire = DateTime.Now.AddSeconds(Convert.ToInt32(jsonObj["expires_in"].ToString())).Ticks;
                        social.RefreshToken = jsonObj["refresh_token"].ToString();
                    }
                    catch
                    {
                        social.IsBinded = false;
                    }
                    SocialViewModel.Instance.SubmitChange();
                });
            }
        }

        /// <summary>
        /// 人人网发布
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        protected override void SocialSharePublish(string channelName,
            string channelUrl, string message, SocialBase.SocialShareHandler callBack)
        {
            JudgeToken(SocialViewModel.Instance.GetRecord(SocialType.RENREN));
            var renren = new RenrenApiClient();
            try
            {
                renren.CallPublishShare(channelName, channelUrl, message, responseData =>
                {
                    var responseMsg = string.Empty;
                    try
                    {
                        JObject.Parse(responseData);
                        callBack(true, responseData);
                    }
                    catch
                    {
                        callBack(false, responseData);
                    }
                });
            }
            catch (Exception ex)
            {
                callBack(false, ex.Message);
            }
        }
    }
}
