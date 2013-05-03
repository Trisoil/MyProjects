using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Social
{
    using TencentWeiboSDK;
    using TencentWeiboSDK.Model;
    using TencentWeiboSDK.Services;
    using TencentWeiboSDK.Services.Util;

    using PPTV.WPRT.CommonLibrary.DataModel.Social;

    public class TencentSocialFactory : SocialBase
    {
        #region Properties

        public static string APIKey = "801093110";

        public static string APISecret = "4b729e3d4946db304db789065fda1f10";

        #endregion

        static TencentSocialFactory()
        {
            OAuthConfigruation.APP_KEY = TencentSocialFactory.APIKey;
            OAuthConfigruation.APP_SECRET = TencentSocialFactory.APISecret;
            OAuthConfigruation.IfSaveAccessToken = false;
        }

        protected override void SocialSharePublish(string channelName, string channelUrl, string message, SocialBase.SocialShareHandler callBack)
        {
            var social = SocialViewModel.Instance.GetRecord(SocialType.TENCENT);
            OAuthConfigruation.AccessToken = new AccessToken(social.AcessToken);
            var share = new TService();
            var arg = new ServiceArgument()
            {
                Content = message
                //Url = channelUrl
            };
            try
            {
                share.Add(arg, obj =>
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
