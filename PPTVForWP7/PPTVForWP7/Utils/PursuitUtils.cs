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
using Microsoft.Phone.Notification;
using Microsoft.Phone.Shell;
using Microsoft.Phone.Info;
using System.Xml;
using System.IO;
using Microsoft.Phone.Controls;
using System.Linq;

namespace PPTVForWP7.Utils
{
    using PPTVData.Entity.Notify;

    public class PursuitUtils
    {
        static PursuitUtils()
        {
            InitHttpNotify();
        }

        #region Properties

        /// <summary>
        /// HttpNotify推送通道名称,也用于本地键值对存储URI的键值
        /// </summary>
        public static readonly string HttpNotifyChannelName = "PPTVFORWP7CHANNEL";

        /// <summary>
        /// 是否追剧独立存储键值
        /// </summary>
        public static readonly string ISHttpNotifyName = "HTTPNOTIFY_ISBIND";

        /// <summary>
        /// PursuitPage.xaml路径
        /// </summary>
        public static readonly Uri PursuitPageUrl = new Uri("/Pages/Notify/PursuitPage.xaml", UriKind.RelativeOrAbsolute);

        /// <summary>
        /// 清空全部提示
        /// </summary>
        public static readonly string RemovePursuitAlert = "清空全部正在追的内容？";

        /// <summary>
        /// 追剧工具栏图标
        /// </summary>
        public static readonly Uri PursuitImageUri = new Uri("/Images/appbar.pursuit.png", UriKind.Relative);

        /// <summary>
        /// 已追剧工具栏图标
        /// </summary>
        public static readonly Uri PursuitedImageUri = new Uri("/Images/appbar.pursuited.png", UriKind.Relative);

        /// <summary>
        /// 追剧定位到详细页，programSource=7用于DAC统计，说明节目是从追剧进入的。
        /// </summary>
        public static readonly string PursuitToDetailUri = "/DetailPage.xaml?vid={0}&programSource=7";

        /// <summary>
        /// 用户提交设备信息uri
        /// </summary>
        public static readonly string RegisterDeviceUri = "http://push.pptv.com/register?dt={0}&platform=4&uri={1}";

        /// <summary>
        /// 获取频道详细信息EPG
        /// </summary>
        public static readonly string DetailEpgUri = "http://epg.api.pptv.com/detailonlyset.api?auth={0}&vid={1}&c=1&s=1&platform={2}";

        /// <summary>
        /// 提交订阅频道EPG
        /// </summary>
        public static readonly string PursuitEpgUri = "http://subscription.api.pptv.com/sub?cid={0}&uid={1}&platform=wp7&type={2}&time={3}&key={4}";
            //"http://192.168.31.4/sub/sub?cid={0}&uid={1}&platform=wp7&type={2}&time={3}&key={4}";

        /// <summary>
        /// 设备唯一ID
        /// </summary>
        public static readonly string DeviceId = CreateDeviceId();

        /// <summary>
        /// 最新NotifyUri
        /// </summary>
        private static string _httpNotifyUri = string.Empty;

        #endregion

        #region HttpNotify

        /// <summary>
        /// 初始化HttpNotify,创建绑定通道
        /// </summary>
        public static void InitHttpNotify()
        {
            try
            {
                var _pushChannel = HttpNotificationChannel.Find(PursuitUtils.HttpNotifyChannelName);
                if (_pushChannel == null)
                {
                    _pushChannel = new HttpNotificationChannel(PursuitUtils.HttpNotifyChannelName);
                    _pushChannel.ChannelUriUpdated += new EventHandler<NotificationChannelUriEventArgs>(_pushChannel_ChannelUriUpdated);
                    _pushChannel.ErrorOccurred += new EventHandler<NotificationChannelErrorEventArgs>(_pushChannel_ErrorOccurred);
                    _pushChannel.Open();
                    _pushChannel.BindToShellTile();
                    _pushChannel.BindToShellToast();
                }
                else
                {
                    _pushChannel.ChannelUriUpdated += new EventHandler<NotificationChannelUriEventArgs>(_pushChannel_ChannelUriUpdated);
                    _pushChannel.ErrorOccurred += new EventHandler<NotificationChannelErrorEventArgs>(_pushChannel_ErrorOccurred);
                }
                if (_pushChannel.ChannelUri != null)
                    JudgeUri(_pushChannel.ChannelUri.AbsoluteUri);
            }
            catch (InvalidOperationException)
            {
            }
        }

        /// <summary>
        /// 关闭HttpNotify通道
        /// </summary>
        public static void CloseHttpNotify()
        {
            var _pushChannel = HttpNotificationChannel.Find(PursuitUtils.HttpNotifyChannelName);
            if (_pushChannel != null)
            {
                _pushChannel.UnbindToShellTile();
                _pushChannel.UnbindToShellToast();
                _pushChannel.Close();
            }
            Utils.SetIOSData(HttpNotifyChannelName, string.Empty);
        }

        /// <summary>
        /// 推送通知发生错误时触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void _pushChannel_ErrorOccurred(object sender, NotificationChannelErrorEventArgs e)
        {
        }

        /// <summary>
        /// 推送通知URL更新时触发
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void _pushChannel_ChannelUriUpdated(object sender, NotificationChannelUriEventArgs e)
        {
            JudgeUri(e.ChannelUri.AbsoluteUri);
        }

        #endregion

        /// <summary>
        /// 判断用户是否开启追剧
        /// </summary>
        /// <returns></returns>
        public static bool JudgePursuit()
        {
            return Utils.GetIOSData(PursuitUtils.ISHttpNotifyName, "1") == "1" ? true : false;
        }

        /// <summary>
        /// 判断是否需要重新提交或存储设备URI
        /// </summary>
        /// <param name="newUri"></param>
        public static void JudgeUri(string newUri)
        {
            System.Diagnostics.Debug.WriteLine(newUri);
            var oldUri = Utils.GetIOSData(HttpNotifyChannelName, string.Empty);
            if (oldUri != newUri)
            {
                if (JudgePursuit())
                {
                    _httpNotifyUri = newUri;
                    var uri = string.Format(RegisterDeviceUri, DeviceId, HttpUtility.UrlEncode(newUri));
                    PPTVData.HttpAsyncUtils.HttpGet(uri, null, null, response =>
                    {
                        try
                        {
                            using (var reader = XmlReader.Create(new StringReader(response)))
                            {
                                if (reader.ReadToFollowing("code"))
                                {
                                    if (reader.ReadElementContentAsInt() == 0 ? true : false)
                                    {
                                        _httpNotifyUri = string.Empty;
                                        Utils.SetIOSData(HttpNotifyChannelName, newUri);
                                    }
                                }
                            }
                        }
                        catch { }
                    });
                }
            }
        }

        /// <summary>
        /// 订阅或取消订阅该频道
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="isSub"></param>
        public static void PursuitChannel(int channelId, bool isSub, EventHandler<PursuitEventargs> callBack)
        {
            if (JudgePursuit())
            {
                if (!string.IsNullOrEmpty(_httpNotifyUri)) JudgeUri(_httpNotifyUri);
                var time = DateTime.Now.Subtract(new DateTime(1969, 12, 31, 23, 59, 59)).TotalSeconds.ToString("F0");
                var key = string.Format("{0}{1}{2}{3}", channelId, DeviceId, time, "2s56a6a4w382s1a56q12s16335");
                var md5 = PPTVData.HttpUtils.MD5Encrpt(key);
                var uri = string.Format(PursuitEpgUri, channelId, DeviceId, isSub ? "sub" : "unsub", time, md5);
                PPTVData.HttpAsyncUtils.HttpGet(uri, null, null, response =>
                {
                    try
                    {
                        using (var reader = XmlReader.Create(new StringReader(response)))
                        {
                            if (reader.ReadToFollowing("status"))
                            {
                                var sucess = reader.ReadElementContentAsInt() == 1 ? true : false;
                                if (sucess)
                                {
                                    callBack(null, new PursuitEventargs(true, string.Empty));
                                }
                                else
                                {
                                    var message = string.Empty;
                                    if (reader.ReadToFollowing("err"))
                                    {
                                        message = reader.ReadElementContentAsString();
                                    }
                                    callBack(null, new PursuitEventargs(false, message));
                                }
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        callBack(null, new PursuitEventargs(false, ex.Message));
                    }
                });
            }
            else
            {
                callBack(null, new PursuitEventargs(true, string.Empty));
            }
        }

        /// <summary>
        /// 判断该频道是否有更新
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="count"></param>
        public static void JudgePursuitUpdate(int channelId, int count)
        {
            System.Threading.ThreadPool.QueueUserWorkItem(o =>
            {
                var pursuit = PursuitViewModel.Instance.GetByChannelId(channelId);
                if (pursuit != null && count > pursuit.ChannelCount)
                {
                    PursuitViewModel.Instance.UpdateCountByChannelId(channelId, count);
                }
            });
        }

        /// <summary>
        /// 生成追剧工具栏图标
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="handler"></param>
        /// <param name="isPursuit"></param>
        /// <returns></returns>
        public static ApplicationBarIconButton CreatePursuitAppBar(int channelId, EventHandler handler, bool isPursuit = false)
        {
            var bar = new ApplicationBarIconButton();
            bar.Click += handler;
            if (isPursuit || PursuitViewModel.Instance.HasByChannelId(channelId))
            {
                bar.Text = "已追剧";
                bar.IsEnabled = false;
                bar.IconUri = PursuitedImageUri;
            }
            else
            {
                bar.Text = "追剧";
                bar.IconUri = PursuitImageUri;
            }
            return bar;
        }

        /// <summary>
        /// 生成设备唯一ID
        /// </summary>
        /// <returns></returns>
        public static string CreateDeviceId()
        {
            object bytes = null;
            if (DeviceExtendedProperties.TryGetValue("DeviceUniqueId", out bytes))
            {
                var buffer = (byte[])bytes;
                var build = new System.Text.StringBuilder(buffer.Length * 2);
                foreach(var b in buffer)
                {
                    build.Append(b.ToString("X"));
                }
                return build.ToString();
            }
            return string.Empty;
        }

        /// <summary>
        /// 发送追剧请求事件参数
        /// </summary>
        public class PursuitEventargs : EventArgs
        {
            public PursuitEventargs(bool isSucess, string message)
            {
                IsSucess = isSucess;
                Message = message;
            }

            public bool IsSucess { get; set; }

            public string Message { get; set; }
        }
    }
}
