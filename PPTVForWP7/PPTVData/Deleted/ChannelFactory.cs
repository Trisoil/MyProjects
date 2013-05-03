//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Windows.Threading;
//using System.Xml;
//using System.IO;
//using System.Diagnostics;
//using System.Xml.Linq;
//using PPTVData.Entity;
//using PPTVData;
//using PPTVData.Log;

//namespace PPTVData.Factory
//{
//    public class GetChannelsByPingYingSearchTimeoutEventArgs : EventArgs
//    {
//        public Exception Exception { get; set; }
//    }
//    public class ChannelFactory
//    {
//        #region 根据关键字搜索出影片事件及处理函数
//        public delegate void GetChannelsBySearchSucceedEventHandler(object sender, List<ChannelInfo> channelInfos, int channel_count, int page_count);
//        public event GetChannelsBySearchSucceedEventHandler GetChannelsBySearchSucceed;

//        public delegate void GetChannelsBySearchFailedEventHandler(object sender, Exception exc);
//        public event GetChannelsBySearchFailedEventHandler GetChannelsBySearchFailed;
//        #endregion

//        #region 根据拼音关键字搜索出影片事件及处理函数
//        public delegate void GetChannelsByPingYingSearchSucceedEventHandler(object sender, List<ChannelInfo> channelInfos, int channel_count, int page_count);
//        public event GetChannelsByPingYingSearchSucceedEventHandler GetChannelsByPingYingSearchSucceed;

//        public delegate void GetChannelsByPingYingSearchFailedEventHandler(object sender, Exception exc);
//        public event GetChannelsByPingYingSearchFailedEventHandler GetChannelsByPingYingSearchFailed;
//        #endregion

//        #region 根据List,Tag,typeid获取到影片事件及处理函数
//        public delegate void GetChannelsByListSucceedEventHandler(object sender, List<ChannelInfo> channelInfos, int channel_count, int page_count);
//        public event GetChannelsByListSucceedEventHandler GetChannelsByListSucceed;
//        public delegate void GetChannelsByListFailedEventHandler(object sender, Exception exc);
//        public event GetChannelsByListFailedEventHandler GetChannelsByListFailed;
//        #endregion

//        #region 根据typeid,数量获取热榜影片事件及处理函数
//        public delegate void GetChannelsByRankSucceedEventHandler(object sender, List<ChannelInfo> channelInfos);
//        public event GetChannelsByRankSucceedEventHandler GetChannelsByRankSucceed;
//        public delegate void GetChannelsByRankFailedEventHandler(object sender, Exception exc);
//        public event GetChannelsByRankFailedEventHandler GetChannelsByRankFailed;
//        #endregion

//        #region 根据typeid,数量获取推荐影片事件及处理函数
//        public delegate void GetChannelsByRecommendSucceedEventHandler(object sender, List<ChannelInfo> channelInfos);
//        public event GetChannelsByRecommendSucceedEventHandler GetChannelsByRecommendSucceed;
//        public delegate void GetChannelsByRecommendFailedEventHandler(object sender, Exception exc);
//        public event GetChannelsByRecommendFailedEventHandler GetChannelsByRecommendFailed;
//        #endregion

//        #region 根据首页菜单导航Mode,数量获取推荐的影片及处理函数
//        public event Action<object, List<ChannelInfo>, int> GetChannelsByRecommendNavSucceed;
//        public event Action<object, Exception> GetChannelsByRecommendNavFailed;
//        #endregion

//        #region 根据搜索获取数据 超时抛出事件
//        public delegate void GetChannelsBySearchTimeoutEventHandler(object sender);
//        public event GetChannelsBySearchTimeoutEventHandler GetChannelsBySearchTimeout;
//        #endregion

//        #region 根据拼音搜索获取数据 超时抛出事件
//        //public delegate void GetChannelsByPingYingSearchTimeoutEventHandler(object sender);
//        //public event GetChannelsByPingYingSearchTimeoutEventHandler GetChannelsByPingYingSearchTimeout;
//        public event EventHandler<GetChannelsByPingYingSearchTimeoutEventArgs> GetChannelsByPingYingSearchTimeoutEvent;
//        #endregion

//        #region 根据List获取数据 超时抛出事件
//        public delegate void GetChannelsByListTimeoutEventHandler(object sender);
//        public event GetChannelsByListTimeoutEventHandler GetChannelsByListTimeout;
//        #endregion

//        #region 根据typeid,数量获取热榜数据 超时抛出事件
//        public delegate void GetChannelsByRankTimeoutEventHandler(object sender);
//        public event GetChannelsByRankTimeoutEventHandler GetChannelsByRankTimeout;
//        #endregion

//        #region 根据typeid,数量获取推荐影片数据 超时抛出事件
//        public delegate void GetChannelsByRecommendTimeoutEventHandler(object sender);
//        public event GetChannelsByRecommendTimeoutEventHandler GetChannelsByRecommendTimeout;
//        #endregion

//        #region 根据首页菜单导航Mode,数量获取推荐的影片数据 超时抛出事件
//        public delegate void GetChannelsByRecommendNavTimeoutEventHandler(object sender);
//        public event GetChannelsByRecommendNavTimeoutEventHandler GetChannelsByRecommendNavTimeout;
//        #endregion

//        NoCacheWebClient _clientSearchAsync = null;
//        NoCacheWebClient _clientListAsync = null;
//        NoCacheWebClient _clientRankAsync = null;
//        NoCacheWebClient _clientRecommendAsync = null;
//        NoCacheWebClient m_WebClient4RecommendNav = null;
//        NoCacheWebClient _clientPingYingSearchAsync = null;

//        DispatcherTimer _timerSearchAsync = null;
//        DispatcherTimer _timerListAsync = null;
//        DispatcherTimer _timerRankAsync = null;
//        DispatcherTimer _timerRecommendAsync = null;
//        DispatcherTimer m_Timer4RecommendNav = null;
//        //DispatcherTimer _timerPingYingSearchAsync = null;

//        int _channel_count = 0;
//        int _page_count = 0;
//        //int m_Navid = 0;
//        private static int _timeout = 10;
//        private static int _contentCharNum = 40;
//        public ChannelFactory()
//        {
//            _timerSearchAsync = new DispatcherTimer();
//            _timerSearchAsync.Interval = new TimeSpan(0, 0, _timeout);
//            _timerSearchAsync.Tick += new EventHandler(_timerBySearch_Tick);

//            _timerListAsync = new DispatcherTimer();
//            _timerListAsync.Interval = new TimeSpan(0, 0, _timeout);
//            _timerListAsync.Tick += new EventHandler(_timerByList_Tick);

//            _timerRankAsync = new DispatcherTimer();
//            _timerRankAsync.Interval = new TimeSpan(0, 0, _timeout);
//            _timerRankAsync.Tick += new EventHandler(_timerByRank_Tick);

//            _timerRecommendAsync = new DispatcherTimer();
//            _timerRecommendAsync.Interval = new TimeSpan(0, 0, _timeout);
//            _timerRecommendAsync.Tick += new EventHandler(_timerByRecommend_Tick);

//            m_Timer4RecommendNav = new DispatcherTimer();
//            m_Timer4RecommendNav.Interval = new TimeSpan(0, 0, _timeout);
//            m_Timer4RecommendNav.Tick += new EventHandler(_timerRecommendNavAsync_Tick);
//        }

//        public static int Timeout
//        {
//            get { return _timeout; }
//            set
//            {
//                _timeout = value;
//            }
//        }

//        public void Cancel()
//        {
//            if (_timerSearchAsync != null)
//                _timerSearchAsync.Stop();
//            if (_timerRecommendAsync != null)
//                _timerRecommendAsync.Stop();
//            if (_timerRankAsync != null)
//                _timerRankAsync.Stop();
//            if (_timerListAsync != null)
//                _timerListAsync.Stop();
//            if (m_Timer4RecommendNav != null)
//                m_Timer4RecommendNav.Stop();
//            //if (_timerPingYingSearchAsync != null)
//            //    _timerPingYingSearchAsync.Stop();


//            if (_clientListAsync != null)
//            {
//                _clientListAsync.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(_clientListAsync_DownloadStringCompleted);
//                _clientListAsync.CancelAsync();
//                _clientListAsync = null;
//            }
//            if (_clientRankAsync != null)
//            {
//                _clientRankAsync.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(_clientRankAsync_DownloadStringCompleted);
//                _clientRankAsync.CancelAsync();
//                _clientRankAsync = null;
//            }
//            if (_clientRecommendAsync != null)
//            {
//                _clientRecommendAsync.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(GetChannelsByRecommendDownloadCompleted);
//                _clientRecommendAsync.CancelAsync();
//                _clientRecommendAsync = null;
//            }
//            if (_clientSearchAsync != null)
//            {
//                _clientSearchAsync.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(_clientSearchAsync_DownloadStringCompleted);
//                _clientSearchAsync.CancelAsync();
//                _clientSearchAsync = null;
//            }
//            if (m_WebClient4RecommendNav != null)
//            {
//                m_WebClient4RecommendNav.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(GetChannelsByRecommendNavDownloadCompleted);
//                m_WebClient4RecommendNav.CancelAsync();
//                m_WebClient4RecommendNav = null;
//            }
//            if (_clientPingYingSearchAsync != null)
//            {
//                _clientPingYingSearchAsync.DownloadStringCompleted -= new DownloadStringCompletedEventHandler(_clientPingYingSearchAsync_DownloadStringCompleted);
//                _clientPingYingSearchAsync.CancelAsync();
//                _clientPingYingSearchAsync = null;
//            }
//        }

//        #region 获取数据公共方法

//        public void GetChannelsByPingYingSearchAsync(string key, int page_index, int page_capacity)
//        {
//            Cancel();
//            var uri = string.Format(EpgUtils.SearchChUri, page_capacity, page_index, key, _contentCharNum);
//            _clientPingYingSearchAsync = new NoCacheWebClient();
//            _clientPingYingSearchAsync.Encoding = Encoding.UTF8;
//            _clientPingYingSearchAsync.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_clientPingYingSearchAsync_DownloadStringCompleted);
//            try
//            {
//                _clientPingYingSearchAsync.DownloadStringAsync(new Uri(uri, UriKind.Absolute));
//            }
//            catch (Exception ex)
//            {
//                Debug.WriteLine("出现网络错误：{0}", ex.Message);
//                if (GetChannelsByPingYingSearchTimeoutEvent != null)
//                    GetChannelsByPingYingSearchTimeoutEvent(this, new GetChannelsByPingYingSearchTimeoutEventArgs { Exception = ex });
//            }
//            //_timerPingYingSearchAsync.Start();
//        }

//        /// <summary>
//        /// 根据搜索条件获取影片集合
//        /// </summary>
//        /// <param name="key"></param>
//        /// <param name="page_index"></param>
//        /// <param name="page_capacity"></param>
//        public void GetChannelsBySearchAsync(string key, int page_index, int page_capacity)
//        {
//            Cancel();
//            var uri = string.Format(EpgUtils.SearchSmartUri, page_capacity, page_index, key, _contentCharNum);
//            _clientSearchAsync = new NoCacheWebClient();
//            _clientSearchAsync.Encoding = Encoding.UTF8;
//            _clientSearchAsync.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_clientSearchAsync_DownloadStringCompleted);
//            _clientSearchAsync.DownloadStringAsync(new Uri(uri, UriKind.Absolute));
//            _timerSearchAsync.Start();
//        }

//        /// <summary>
//        /// 根据List,Tag,typeid获取影片集合
//        /// </summary>
//        /// <param name="type_id"></param>
//        /// <param name="tagInfos"></param>
//        /// <param name="page_index"></param>
//        /// <param name="page_capacity"></param>
//        public void GetChannelsByListAsync(int type_id, List<TagInfo> tagInfos, int page_index, int page_capacity)
//        {
//            Cancel();
//            var uri = string.Format(EpgUtils.ListUri, page_capacity, page_index, type_id, _contentCharNum);
//            if (tagInfos != null)
//            {
//                uri = uri + "&tag=" + GenerateTagString(tagInfos);
//            }
//            _clientListAsync = new NoCacheWebClient();
//            _clientListAsync.Encoding = Encoding.UTF8;
//            _clientListAsync.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_clientListAsync_DownloadStringCompleted);
//            _clientListAsync.DownloadStringAsync(new Uri(uri, UriKind.Absolute));
//            LogManager.Ins.Log("client.DownloadStringAsync " + uri);
//            _timerListAsync.Start();
//        }

//        /// <summary>
//        /// 根据typeid获取热榜影片集合
//        /// </summary>
//        /// <param name="type_id"></param>
//        /// <param name="count"></param>
//        public void GetChannelsByRankAsync(int type_id, int count)
//        {
//            var uri = string.Format(EpgUtils.ListOrderUri, count, '1', type_id, _contentCharNum, 't');
//            Cancel();
//            _clientRankAsync = new NoCacheWebClient();
//            _clientRankAsync.Encoding = Encoding.UTF8;
//            _clientRankAsync.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_clientRankAsync_DownloadStringCompleted);
//            _clientRankAsync.DownloadStringAsync(new Uri(uri, UriKind.Absolute));
//            LogManager.Ins.Log("client.DownloadStringAsync " + uri);
//            _timerRankAsync.Start();
//        }

//        /// <summary>
//        /// 根据数量,typeid获取推荐电影集合
//        /// </summary>
//        /// <param name="count"></param>
//        public void GetChannelsByRecommendAsync(int type_id, int count)
//        {
//            var uri = string.Format(EpgUtils.ListUri, count, '1', type_id, _contentCharNum);
//            Cancel();
//            _clientRecommendAsync = new NoCacheWebClient();
//            _clientRecommendAsync.Encoding = Encoding.UTF8;
//            _clientRecommendAsync.DownloadStringCompleted += new DownloadStringCompletedEventHandler(GetChannelsByRecommendDownloadCompleted);
//            _clientRecommendAsync.DownloadStringAsync(new Uri(uri, UriKind.Absolute));
//            _timerRecommendAsync.Start();
//        }

//        /// <summary>
//        /// 根据Mode,navid,数量获取首页推荐电影集合
//        /// </summary>
//        /// <param name="mode"></param>
//        /// <param name="navid"></param>
//        /// <param name="count"></param>
//        public void GetChannelsByRecommendNavAsync(int mode, int navid, int count)
//        {
//            var uri = string.Format(EpgUtils.RecommandListUri, count, '1', navid, mode, _contentCharNum);
//            Cancel();
//            m_WebClient4RecommendNav = new NoCacheWebClient();
//            m_WebClient4RecommendNav.Encoding = Encoding.UTF8;
//            m_WebClient4RecommendNav.DownloadStringCompleted += new DownloadStringCompletedEventHandler(GetChannelsByRecommendNavDownloadCompleted);
//            //m_Navid = navid;
//            m_WebClient4RecommendNav.DownloadStringAsync(new Uri(uri, UriKind.Absolute), navid);
//            m_Timer4RecommendNav.Start();
//        }
//        #endregion

//        #region 超时 抛出的事件
//        void _timerBySearch_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetChannelsBySearchTimeout != null)
//                GetChannelsBySearchTimeout(this);
//        }
//        void _timerByList_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetChannelsByListTimeout != null)
//                GetChannelsByListTimeout(this);
//        }
//        void _timerByRank_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetChannelsByRankTimeout != null)
//                GetChannelsByRankTimeout(this);
//        }
//        void _timerByRecommend_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetChannelsByRecommendTimeout != null)
//                GetChannelsByRecommendTimeout(this);
//        }


//        void _timerRecommendNavAsync_Tick(object sender, EventArgs e)
//        {
//            Cancel();
//            if (GetChannelsByRecommendNavTimeout != null)
//                GetChannelsByRecommendNavTimeout(this);
//        }

//        //void _timerPingYingSearchAsync_Tick(object sender, EventArgs e)
//        //{
//        //    Cancel();
//        //    if (GetChannelsByPingYingSearchTimeout != null)
//        //        GetChannelsByPingYingSearchTimeout(this);
//        //}
//        #endregion

//        string GenerateTagString(List<TagInfo> tagInfos)
//        {
//            if (tagInfos == null)
//                return "||";
//            string catalogTag = "";
//            string areaTag = "";
//            string yearTag = "";

//            if (tagInfos[0] == null)
//                catalogTag = "";
//            else if (tagInfos[0].Dimension == "catalog" && tagInfos[0].TagName != "全部")
//                catalogTag = tagInfos[0].TagName;

//            if (tagInfos[1] == null)
//                areaTag = "";
//            else if (tagInfos[1].Dimension == "area" && tagInfos[1].TagName != "全部")
//                areaTag = tagInfos[1].TagName;

//            if (tagInfos[2] == null)
//                yearTag = "";
//            else if (tagInfos[2].Dimension == "year" && tagInfos[2].TagName != "全部")
//                yearTag = tagInfos[2].TagName;
//            return catalogTag + "|" + areaTag + "|" + yearTag;
//        }


//        void _clientPingYingSearchAsync_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {

//                List<ChannelInfo> channelInfos = AnalysisXML(e.Result);
//                if (GetChannelsByPingYingSearchSucceed != null)
//                {
//                    GetChannelsByPingYingSearchSucceed(this, channelInfos, _channel_count, _page_count);
//                }
//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsByPingYingSearchFailed != null)
//                {
//                    GetChannelsByPingYingSearchFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void GetChannelsByRecommendNavDownloadCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                List<ChannelInfo> infos = AnalysisXML(e.Result);
//                if (GetChannelsByRecommendNavSucceed != null)
//                {
//                    GetChannelsByRecommendNavSucceed(this, infos, (int)e.UserState);
//                }
//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsByRecommendNavFailed != null)
//                {
//                    GetChannelsByRecommendNavFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void GetChannelsByRecommendDownloadCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                List<ChannelInfo> channelInfos = AnalysisXML(e.Result);
//                if (GetChannelsByRecommendSucceed != null)
//                {
//                    GetChannelsByRecommendSucceed(this, channelInfos);
//                }
//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsByRecommendFailed != null)
//                {
//                    GetChannelsByRecommendFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void _clientRankAsync_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                List<ChannelInfo> channelInfos = AnalysisXML(e.Result);
//                if (GetChannelsByRankSucceed != null)
//                {
//                    GetChannelsByRankSucceed(this, channelInfos);
//                }
//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsByRankFailed != null)
//                {
//                    GetChannelsByRankFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void _clientListAsync_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {
//                LogManager.Ins.Log("client1_DownloadStringCompleted " + e.Result.Length);
//                List<ChannelInfo> channelInfos = AnalysisXML(e.Result);
//                if (GetChannelsByListSucceed != null)
//                {
//                    LogManager.Ins.Log("throw GetChannelsByListSucceed event");
//                    GetChannelsByListSucceed(this, channelInfos, _channel_count, _page_count);
//                }
//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsByListFailed != null)
//                {
//                    GetChannelsByListFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        void _clientSearchAsync_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
//        {
//            try
//            {

//                List<ChannelInfo> channelInfos = AnalysisXML(e.Result);
//                if (GetChannelsBySearchSucceed != null)
//                {
//                    GetChannelsBySearchSucceed(this, channelInfos, _channel_count, _page_count);
//                }

//            }
//            catch (Exception exp)
//            {
//                if (GetChannelsBySearchFailed != null)
//                {
//                    GetChannelsBySearchFailed(this, exp);
//                }
//            }
//            finally
//            {
//                Cancel();
//            }
//        }

//        List<ChannelInfo> AnalysisXML(string result)
//        {

//            _channel_count = 0;
//            _page_count = 0;
//            XmlReader reader = XmlReader.Create(new StringReader(result));
//            List<ChannelInfo> channelList = new List<ChannelInfo>();
//            while (reader.Read())
//            {
//                if (reader.IsStartElement("count"))
//                {
//                    reader.Read();
//                    _channel_count = int.Parse(reader.Value);
//                }
//                if (reader.IsStartElement("page_count"))
//                {
//                    reader.Read();
//                    _page_count = int.Parse(reader.Value);
//                }

//                if (reader.NodeType == XmlNodeType.Element && reader.Name == "v")
//                {
//                    //LogManager.Ins.Log("v begin");
//                    int vid = 0;
//                    string title = "";
//                    string playLink = "";
//                    int typeID = 0;
//                    string imgUrl = "";
//                    string catalogTags = "";
//                    string directorTags = "";
//                    string actTags = "";
//                    string yearTags = "";
//                    string areaTags = "";
//                    int state = 0;
//                    string note = "";
//                    double mark = 0;
//                    int bitrate = 0;
//                    int resolutionWidth = 0;
//                    int resolutionHeight = 0;
//                    bool isHD = false;
//                    bool isRecommend = false;
//                    bool isNew = false;
//                    bool is3D = false;
//                    double duration = 0;
//                    string resolution = "";
//                    string content = "";
//                    string slotUrl = "";
//                    while (reader.Read())
//                    {
//                        if (reader.NodeType == XmlNodeType.Element)
//                        {
//                            string node_name = reader.Name;
//                            reader.Read();
//                            switch (node_name)
//                            {
//                                case "vid": vid = int.Parse(reader.Value); break;
//                                case "type": typeID = int.Parse(reader.Value); break;
//                                case "playlink": playLink = reader.Value; break;
//                                case "title": title = reader.Value; break;
//                                case "catalog": catalogTags = reader.Value; break;
//                                case "director": directorTags = reader.Value; break;
//                                case "act": actTags = reader.Value; break;
//                                case "year": yearTags = reader.Value; break;
//                                case "area": areaTags = reader.Value; break;
//                                case "imgurl": imgUrl = reader.Value; break;
//                                case "state":
//                                    try
//                                    {
//                                        state = int.Parse(reader.Value); 
//                                    }
//                                    catch (Exception)
//                                    {
//                                        state = 1;
//                                    }
                                    
//                                    break;
//                                case "note": note = reader.Value; break;
//                                case "mark": mark = double.Parse(reader.Value); break;
//                                case "bitrate": bitrate = int.Parse(reader.Value); break;
//                                case "resolution": resolution = reader.Value;
//                                    string[] widthHeight = resolution.Split('|');
//                                    if (widthHeight.Length > 0)
//                                    {
//                                        resolutionWidth = int.Parse(widthHeight[0]);
//                                        resolutionHeight = int.Parse(widthHeight[1]);
//                                    }
//                                    break;
//                                case "flag":
//                                    string[] flags = reader.Value.Split('|');
//                                    foreach (string flag in flags)
//                                    {
//                                        //h表示高清，r表示推荐，n表示最新更新，b表示蓝光, d表示3d
//                                        switch (flag)
//                                        {
//                                            case "h": isHD = true; break;
//                                            case "r": isRecommend = true; break;
//                                            case "n": isNew = true; break;
//                                            case "d": is3D = true; break;
//                                            default: break;
//                                        }
//                                    }
//                                    break;
//                                case "duration": duration = double.Parse(reader.Value); break;
//                                case "content": content = reader.Value; break;
//                                case "sloturl": slotUrl = reader.Value; break;
//                                default: break;
//                            }
//                        }   // end if
//                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "v")
//                        {
//                            ChannelInfo channelInfo = new ChannelInfo(vid, title, playLink, typeID, imgUrl, catalogTags, directorTags, actTags, yearTags, areaTags, state, note, mark, bitrate, resolutionWidth, resolutionHeight, isHD, isRecommend, isNew, is3D, duration, content,slotUrl);
//                            channelList.Add(channelInfo);
//                            break;
//                        }   // end if
//                    }   //end while
//                }   // end if
//            }   // end while
//            return channelList;
//        }

//        List<ChannelInfo> AnalysisXMLByLinq(string result)
//        {
//            XElement xml = XElement.Parse(result);

//            var query = from x in xml.Descendants("v")
//                        select new ChannelInfo(1, x.Element("title").Value, x.Element("playlink").Value, "", 1, true, true, true, true, 1, 1, 1,x.Element("sloturl").Value);
//            //{
//            //    vid = x.Element("vid").Value,
//            //    playlink = x.Element("playlink").Value,
//            //    title = x.Element("title").Value,
//            //    type = x.Element("type").Value,
//            //    catalog = x.Element("catalog").Value,
//            //    imageurl = x.Element("imageurl").Value,
//            //    sloturl = x.Element("sloturl").Value,
//            //    content = x.Element("content").Value,
//            //};
//            foreach (var a in query)
//            {
//                Debug.WriteLine(a.Title + " url:" + a.PlayLink);
//            }
//            return null;
//        }
//    }
//}
