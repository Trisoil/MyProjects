using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Social
{
    using PPTV.WPRT.CommonLibrary.Utils;
    using PPTV.WPRT.CommonLibrary.DataModel.Social;

    public class RenrenApiClient
    {
        /// <summary>
        /// 人人新鲜事发布
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        /// <param name="callBack"></param>
        public void CallPublishFeed(string channelName,
            string channelUrl, string message, HttpAsyncHandler callBack)
        {
            var paras = new List<APIParameter>() { 
                new APIParameter("method", "feed.publishFeed"),
                new APIParameter("name", channelName),
                new APIParameter("description", message),
                new APIParameter("url", channelUrl)
            };
            CallMethod(paras, callBack);
        }

        /// <summary>
        /// 人人发布分享
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        /// <param name="callBack"></param>
        public void CallPublishShare(string channelName,
            string channelUrl, string message, HttpAsyncHandler callBack)
        {
            var paras = new List<APIParameter>() { 
                new APIParameter("method", "share.share"),
                new APIParameter("type", "10"),
                new APIParameter("comment", message),
                new APIParameter("url", channelUrl)
            };
            CallMethod(paras, callBack);
        }

        /// <summary>
        /// 获取AccessToken
        /// </summary>
        /// <param name="code"></param>
        /// <param name="callBack"></param>
        public void AccessToken(string code, HttpAsyncHandler callBack)
        {
            var paras = new List<APIParameter>() { 
                    new APIParameter("grant_type","authorization_code"),
                    new APIParameter("code",code),
                    new APIParameter("client_id",RenrenSocialFactory.APIKey),
                    new APIParameter("client_secret",RenrenSocialFactory.APISecret),
                    new APIParameter("redirect_uri",RenrenSocialFactory.RedirectUri),
                    new APIParameter("format","json")
                };
            HttpAsyncUtils.HttpPost(RenrenSocialFactory.OAuthUri,
                HttpUtils.GetQueryFromParas(paras), "application/x-www-form-urlencoded;charset=utf-8", callBack);
        }

        /// <summary>
        /// 刷新Token
        /// </summary>
        /// <param name="callBack"></param>
        public void RefreshToken(string refreshToken, HttpAsyncHandler callBack)
        {
            var paras = new List<APIParameter>(){
                new APIParameter("client_id", RenrenSocialFactory.APIKey),
                new APIParameter("client_secret", RenrenSocialFactory.APISecret),
                new APIParameter("redirect_uri", RenrenSocialFactory.RedirectUri),
                new APIParameter("grant_type", "refresh_token"),
                new APIParameter("refresh_token", refreshToken)
            };
            HttpAsyncUtils.HttpPost(RenrenSocialFactory.TokenUrl,
                HttpUtils.GetQueryFromParas(paras), "application/x-www-form-urlencoded", callBack);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="paras"></param>
        /// <param name="callBack"></param>
        public void CallMethod(List<APIParameter> paras, HttpAsyncHandler callBack)
        {
            //paras.Add(new APIParameter("api_key", RenrenSocialData.APIKey));
            //paras.Add(new APIParameter("call_id", DateTime.Now.Millisecond.ToString()));
            paras.Add(new APIParameter("v", "1.0"));
            paras.Add(new APIParameter("access_token", SocialViewModel.Instance.GetRecord(SocialType.RENREN).AcessToken));
            paras.Add(new APIParameter("format", "json"));
            var strSig = CalSig(paras);
            if (string.IsNullOrEmpty(strSig))
                return;
            paras.Add(new APIParameter("sig", strSig));
            HttpAsyncUtils.HttpPost(RenrenSocialFactory.RenrenAPIUrl,
                HttpUtils.GetQueryFromParas(paras), "application/x-www-form-urlencoded", callBack);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="methodName"></param>
        /// <param name="callBack"></param>
        public void CallMethod(string methodName, HttpAsyncHandler callBack)
        {
            if (string.IsNullOrEmpty(methodName))
                return;
            var paras = new List<APIParameter>() { 
                new APIParameter("method",methodName)
            };
            CallMethod(paras, callBack);
        }

        /// <summary>
        /// 计算签名
        /// </summary>
        /// <param name="paras">传入需要的参数</param>
        /// <returns></returns>
        public static string CalSig(List<APIParameter> paras)
        {
            paras.Sort(new ParameterComparer());
            var sbList = new StringBuilder();
            foreach (APIParameter para in paras)
            {
                sbList.AppendFormat("{0}={1}", para.Name, para.Value);
            }
            sbList.Append(RenrenSocialFactory.APISecret);
            return HttpUtils.MD5Encrpt(sbList.ToString());
        }

    }
}
