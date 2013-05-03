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
