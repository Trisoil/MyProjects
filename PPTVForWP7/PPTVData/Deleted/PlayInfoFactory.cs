//using System;
//using System.Windows;
//using System.Windows.Controls;
//using System.Windows.Documents;
//using System.Windows.Ink;
//using System.Windows.Input;
//using System.Windows.Media;
//using System.Windows.Media.Animation;
//using System.Windows.Shapes;
//using PPTVData.Entity;
//using System.Windows.Threading;
//using System.Xml;
//using System.IO;
//using System.Collections.Generic;
//using System.Diagnostics;
//using PPTVData;

//namespace PPTVData.Factory
//{
//    public class PlayInfoFactory
//    {
//        #region 获取播放信息事件及处理函数
//        public event Action<object, PlayInfo> GetPlayInfoSucceed;
//        public event Action<object, Exception> GetPlayInfoFailed;
//        #endregion

//        #region 获取播放 超时抛出事件
//        public delegate void GetPlayInfoTimeoutEventHandler(object sender);
//        public event GetPlayInfoTimeoutEventHandler GetPlayInfoTimeout;
//        #endregion

//        #region 获取jump信息事件及处理函数
//        public event Action<object, PlayInfo.DTInfo> GetDTInfoSucceed;
//        public event Action<object, Exception> GetDTInfoFailed;
//        #endregion

//        #region 获取jump超时抛出事件
//        public delegate void GetDTInfoTimeoutEventHandler(object sender);
//        public event GetDTInfoTimeoutEventHandler GetDTInfoTimeout;
//        #endregion

//        DispatcherTimer _timer = null;
//        NoCacheWebClient _client = null;
//        private int _timeout = 20;
//        private bool _getDTInfo = false;
//        public PlayInfoFactory()
//        {
//            _timer = new DispatcherTimer();
//            _timer.Tick += new EventHandler(timer_Tick);
//            _timer.Interval = new TimeSpan(0, 0, _timeout);
//        }

//        void timer_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetPlayInfoTimeout != null)
//            {
//                GetPlayInfoTimeout(this);
//            }
//        }

//        public void GetPlayInfoByIdAsync(int id)
//        {
//            Cancel();
//            _getDTInfo = false;
//            string uri = String.Format("http://play.api.pptv.com/boxplay.api?id={0}&auth={1}&platform={2}", id, EPGSetting.Auth, EPGSetting.PlatformName);
//            _client = new NoCacheWebClient();
//            _client.DownloadStringCompleted += new DownloadStringCompletedEventHandler(PlayInfoDownloadCompleted);
//            _client.DownloadStringAsync(new Uri(uri, UriKind.RelativeOrAbsolute));
//            _timer.Start();
//        }
//        public void GetPlayInfoByIdAndFtAsync(int id, int ft)
//        {
//            Cancel();
//            _getDTInfo = false;
//            string uri = String.Format("http://play.api.pptv.com/boxplay.api?id={0}&auth={1}&platform={2}&ft={3}", id, EPGSetting.Auth, EPGSetting.PlatformName, ft);
//            _client = new NoCacheWebClient();
//            _client.DownloadStringCompleted += new DownloadStringCompletedEventHandler(PlayInfoDownloadCompleted);
//            _client.DownloadStringAsync(new Uri(uri, UriKind.RelativeOrAbsolute));
//            _timer.Start();
//        }
//        /// <summary>
//        /// 异步获取DTInfo
//        /// </summary>
//        /// <param name="rid">资源ID</param>
//        public void GetDTInfoByRid(string rid)
//        {
//            Cancel();
//            _getDTInfo = true;
//            string url = "http://wp7.jump.synacast.com/" + rid + "dt?type=mwp7";
//            _client = new NoCacheWebClient();
//            _client.DownloadStringCompleted += new DownloadStringCompletedEventHandler(DTInfoDownloadCompleted);
//            _client.DownloadStringAsync(new Uri(url, UriKind.RelativeOrAbsolute));
//            _timer.Start();
//        }

//        public void Cancel()
//        {
//            if (_timer != null)
//                _timer.Stop();
//            if (_client != null)
//            {
//                if (_getDTInfo)
//                {
//                    _client.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(DTInfoDownloadCompleted);
//                }
//                else
//                {
//                    _client.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(PlayInfoDownloadCompleted);
//                }
//                _client.CancelAsync();
//                _client = null;
//            }
//        }

//        void PlayInfoDownloadCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                PlayInfo playInfo = AnalysisXML(e.Result);
//                if (GetPlayInfoSucceed != null)
//                    GetPlayInfoSucceed(this, playInfo);
//            }
//            catch (Exception exp)
//            {
//                if (GetPlayInfoFailed != null)
//                    GetPlayInfoFailed(this, exp);
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void DTInfoDownloadCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                PlayInfo.DTInfo dtInfo = AnalysisXMLDTInfo(e.Result);
//                if (GetDTInfoSucceed != null)
//                {
//                    GetDTInfoSucceed(this, dtInfo);
//                }
//            }
//            catch (System.Exception ex)
//            {
//                if (GetDTInfoFailed != null)
//                {
//                    GetDTInfoFailed(this, ex);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }
//        protected PlayInfo AnalysisXML(string result)
//        {
//            XmlReader reader = XmlReader.Create(new System.IO.StringReader(result));
//            reader.ReadToFollowing("channel");

//            Dictionary<int, PlayInfo.Item> playInfoItems = new Dictionary<int, PlayInfo.Item>();
//            while (reader.Read())
//            {
//                if (reader.Name == "item")
//                {
//                    if (reader.GetAttribute("bitrate") == null || reader.GetAttribute("ft") == null)
//                        continue;
//                    int bitrate = Convert.ToInt32(reader.GetAttribute("bitrate"));
//                    int ft = Convert.ToInt32(reader.GetAttribute("ft"));
//                    string rid = reader.GetAttribute("rid");
//                    PlayInfo.Item playInfo = new PlayInfo.Item(bitrate, ft, rid, null);
//                    playInfoItems.Add(ft, playInfo);
//                }

//                if (reader.Name == "dt" && reader.NodeType != XmlNodeType.EndElement)
//                {
//                    int ft = Convert.ToInt32(reader.GetAttribute("ft"));
//                    PlayInfo.DTInfo dt = new PlayInfo.DTInfo();
//                    reader.ReadToFollowing("sh");
//                    reader.Read();
//                    dt.Sh = reader.Value;
//                    reader.ReadToFollowing("st");
//                    reader.Read();
//                    dt.St = reader.Value;
//                    reader.ReadToFollowing("bwt");
//                    reader.Read();
//                    dt.Bwt = Convert.ToInt32(reader.Value);
//                    if (playInfoItems.ContainsKey(ft))
//                        playInfoItems[ft].DtInfo = dt;
//                }
//            }

//            PlayInfo retPlayInfo = new PlayInfo();
//            foreach (PlayInfo.Item item in playInfoItems.Values)
//            {
//                retPlayInfo.Add(item);
//            }
//            Debug.WriteLine(result);
//            return retPlayInfo;
//        }
//        protected PlayInfo.DTInfo AnalysisXMLDTInfo(string result)
//        {
//            string server_host = null;
//            string server_time = null;
//            using (XmlReader reader = XmlReader.Create(new StringReader(result)))
//            {
//                if (true == reader.ReadToFollowing("server_host"))
//                {
//                    server_host = reader.ReadElementContentAsString("server_host", "");
//                }

//                reader.ReadToFollowing("server_time");
//                server_time = reader.ReadElementContentAsString("server_time", "");

//                if (!string.IsNullOrEmpty(server_host) && !string.IsNullOrEmpty(server_time))
//                {
//                    return new PlayInfo.DTInfo(server_host, server_time, 0);
//                }
//            }

//            throw new Exception("Invalid File Format");
//        }
//    }
//}
