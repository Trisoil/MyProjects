using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.ViewModel.Social
{
    public abstract class SocialBase
    {
        /// <summary>
        /// 发布完成后回调委托
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public delegate void SocialShareHandler(bool isSuccess, string message);

        /// <summary>
        /// 微博分享
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        public void SocialShare(string channelName, string channelUrl, SocialShareHandler callBack)
        {
            SocialSharePublish(channelName, channelUrl, FormateMessage(channelName, channelUrl), callBack);
        }

        /// <summary>
        /// 格式化微博内容
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <returns></returns>
        private string FormateMessage(string channelName, string channelUrl)
        {
            return string.Format("我正在#PPTV WindowsPhone 客户端#观看#{0}#{1}", channelName, channelUrl);
        }

        /// <summary>
        /// 发布
        /// </summary>
        /// <param name="channelName"></param>
        /// <param name="channelUrl"></param>
        /// <param name="message"></param>
        protected abstract void SocialSharePublish(string channelName,
            string channelUrl, string message, SocialShareHandler callBack);
    }
}
