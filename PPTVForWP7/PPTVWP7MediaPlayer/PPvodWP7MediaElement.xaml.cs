using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Xml;
using System.IO;
using System.Globalization;
using PPTVData;
using PPTVData.Entity;
using System.IO.IsolatedStorage;
namespace PPTVWP7MediaPlayer
{
    public partial class PPvodWP7MediaElement : UserControl
    {
        //private PlayLinkInfo _playlinkinfo;
        private PlayInfo.Item _playInfo = new PlayInfo.Item();
        private HttpWebClient _jump_webclient = new HttpWebClient();
        private string _media_server_host = "";
        private DateTime _server_base_time;
        private DateTime _local_base_time;
        private string _playDecodeUrl = "";//解码后的URL
        private DispatcherTimer _timer = new DispatcherTimer();
        double _total_play_positon = 0.0;
        double _total_download_positon = 0.0;
        #region 事件

        /// <summary>
        /// 缓冲的百分比发生变化 事件
        /// </summary>
        public event BufferingProgressChangedHandler BufferingProgressChanged = null;
        public delegate void BufferingProgressChangedHandler(object sender, PPLiveRoutedEventArgs e);

        /// <summary>
        /// 播放器当前状态发生变化事件
        /// </summary>
        public event CurrentStateChangedHandler CurrentStateChanged = null;
        public delegate void CurrentStateChangedHandler(object sender, PPLiveRoutedEventArgs e);

        /// <summary>
        /// 视频播放结束事件
        /// </summary>
        public event MediaEndedHandler MediaEnded = null;
        public delegate void MediaEndedHandler(object sender, PPLiveRoutedEventArgs e);

        /// <summary>
        /// 视频打开成功事件
        /// </summary>
        public event MediaOpenedHandler MediaOpened = null;
        public delegate void MediaOpenedHandler(object sender, PPLiveRoutedEventArgs e);

        /// <summary>
        /// 视频打开播放失败事件
        /// </summary>
        public event MediaFailedHandler MediaFailed = null;
        public delegate void MediaFailedHandler(object sender, PPLiveExceptionRoutedEventArgs e);

        /// <summary>
        /// PPLive 播放器核心组件播放日志输出事件
        /// </summary>
        public event OutputLogHandler OutputLog = null;
        public delegate void OutputLogHandler(object sender, LogEventArgs e);

        #endregion

        public PPvodWP7MediaElement()
        {
            InitializeComponent();
            
            xMediaCurrentStatusTextBlockA.Text = "视频状态：" + xMedia.CurrentState.ToString();
            _timer.Interval = new TimeSpan(0, 0, 1);
            _timer.Tick += new EventHandler(_timer_Tick);
            _timer.Start();
        }

        #region 对外接口
        /// <summary>
        /// 播放
        /// </summary>
        public void Play()
        {
            xMedia.Play();
        }

        /// <summary>
        /// 暂停
        /// </summary>
        public void Pause()
        {
            if (xMedia.CurrentState == MediaElementState.Playing || xMedia.CurrentState == MediaElementState.Buffering)
                xMedia.Pause();
        }

        /// <summary>
        /// 停止
        /// </summary>
        public void Stop()
        {
            Reset();
        }

        /// <summary>
        /// 获得 或者 设置 当前播放的位置，设置当前播放的位置可以理解为拖动
        /// </summary>
        public TimeSpan Position
        {
            get
            {
                // 当前播放的总的时间点
                _total_play_positon = xMedia.Position.TotalSeconds;
                return TimeSpan.FromSeconds(_total_play_positon);
            }
            set
            {
                xMedia.Position = value;
            }
        }

        public bool CanSeek
        {
            get { return xMedia.CanSeek; }
        }

        /// <summary>
        /// 获得 或者 设置 当前下载的位置
        /// </summary>
        public TimeSpan DownloadPosition
        {
            get
            {
                // 当前下载到的总的时间点    
                if (xMedia.NaturalDuration.HasTimeSpan)
                    _total_download_positon = xMedia.NaturalDuration.TimeSpan.TotalSeconds * xMedia.DownloadProgress;
                else
                    _total_download_positon = 0;
                return TimeSpan.FromSeconds(_total_download_positon);
            }
        }

        /// <summary>
        /// 获得 视频的总时长
        /// </summary>
        public Duration NaturalDuration
        {
            get
            {
                return xMedia.NaturalDuration;
            }
        }

        /// <summary>
        /// 获得 视频的当前状态
        /// </summary>
        public MediaElementState CurrentState
        {
            get { return xMedia.CurrentState; }
        }

        #region MyRegion
        //不用
        /// <summary>
        /// 获得 或者 设置 播放器的播放连接串，这个连接串是个加密的字符串
        /// </summary>
        //public string Source
        //{
        //    get
        //    {
        //        if (_playlinkinfo == null) return "";
        //        return _playlinkinfo.PlayLink;
        //    }
        //    set
        //    {
        //        Reset();
        //        _playlinkinfo = new PlayLinkInfo(value);
        //    }
        //} 
        #endregion

        /// <summary>
        /// 不走Jump
        /// </summary>
        public PlayInfo.Item DirectSource
        {
            set
            {
                _playInfo = value;
                CultureInfo ci = new CultureInfo("en-US");
                _server_base_time = DateTime.ParseExact(_playInfo.DtInfo.St, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                _media_server_host = _playInfo.DtInfo.Sh;
                _playDecodeUrl = "http://" + _media_server_host + ":80/" + _playInfo.Rid + "?type=wp7&w=1&key=" + GetServerRelativeTimeString();

                
                xMedia.Source = new Uri(_playDecodeUrl, UriKind.RelativeOrAbsolute);
              //  xMedia.Source = new Uri("http://180.153.106.139/%5Bmobile%5D%B4%F3%C4%A7%CA%F5%CA%A6%28%B8%DF%C7%E5%29.mp4?type=android&w=1&key=de00bd5ba491b1570000000000000000", UriKind.RelativeOrAbsolute);
                Output("播放地址:" + _playDecodeUrl);
            }
        }
                /// <summary>
        /// 直接播放器URL
        /// </summary>
        /// 
        public string Source
        {
            set
            {
                string mediaSource = value.Replace("\\", "/");                
                System.IO.IsolatedStorage.IsolatedStorageFileStream stream = System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().OpenFile(mediaSource, FileMode.Open);
                xMedia.SetSource(stream);                
            }
            private get
            {
                return xMedia.Source.AbsoluteUri;
            }
        }
        /// <summary>
        /// 走Jump
        /// </summary>
        public PlayInfo.Item OriSource
        {
            set
            {
                _playInfo = value;
                _playDecodeUrl = _playInfo.Rid;
                if (string.IsNullOrEmpty(_playDecodeUrl))
                {
                    xMedia.Source = null;
                    return;
                }
                string jump_url = "http://wp7.jump.synacast.com/" + _playInfo.Rid + "dt?type=mwp7";
                Output("获取jump文件 " + jump_url);
                _jump_webclient.DownloadStringCompleted += new DownloadStringCompletedEventHandler(_jump_webclient_DownloadStringCompleted);
                _jump_webclient.DownloadStringAsync(jump_url);
                xMediaStatusTextBlockA.Text = "";
            }
            get { return _playInfo; }
        }

        /// <summary>
        /// 获得 或者 设置 是否显示播放器内部状态信息
        /// </summary>
        public bool IsShowStatus
        {
            get
            {
                return xPlayInfoPanel.Visibility == Visibility.Visible;
            }
            set
            {
                if (value == true) xPlayInfoPanel.Visibility = Visibility.Visible;
                else xPlayInfoPanel.Visibility = Visibility.Collapsed;
            }
        }

        /// <summary>
        /// 获得 或者 设置 播放器的音量
        /// </summary>
        public double Volume
        {
            get { return xMedia.Volume; }
            set
            {
                xMedia.Volume = value;

            }
        }

        /// <summary>
        /// 获得 或者 设置 播放器的亮度
        /// </summary>
        public double Brightness
        {
            get { return 1 - xBrightnessRect.Opacity; }
            set
            {
                double brightness = value;
                if (brightness < 0) brightness = 0;
                if (brightness > 1) brightness = 1;
                xBrightnessRect.Opacity = 1 - brightness;
            }
        }

        /// <summary>
        /// 获得缓冲百分比
        /// </summary>
        public double BufferingProgress
        {
            get { return xMedia.BufferingProgress; }
        }
        #endregion

        void _timer_Tick(object sender, EventArgs e)
        {
            if (xMedia.NaturalDuration.HasTimeSpan)
            xMediaProgressTextBlockA.Text = "当前时间:" + xMedia.Position.ToString("g") + " / " + xMedia.NaturalDuration.TimeSpan.ToString("g");
            else
            {
                xMediaProgressTextBlockA.Text = "当前时间:" + xMedia.Position.ToString("g") + " / " + TimeSpan.FromSeconds(0).ToString("g");
            }
             
        }

        void Reset()
        {
            //StopMedia();
            _jump_webclient.CancelAsync();
            _jump_webclient = new HttpWebClient();
        }

        void StopMedia()
        {
            xMedia.Stop();
            xMediaBufferTextBlockA.Text = "暂无";
            xMediaDownloadTextBlockA.Text = "暂无";
            xMediaCurrentStatusTextBlockA.Text = "视频状态：" + xMedia.CurrentState;
            if (CurrentStateChanged != null)
            {
                PPLiveRoutedEventArgs args = new PPLiveRoutedEventArgs(this);
                CurrentStateChanged(this, args);
            }
        }

        void Output(string text)
        {
            if (OutputLog != null)
            {
                LogEventArgs args = new LogEventArgs(text);
                OutputLog(this, args);
            }
        }

        private string GetServerRelativeTimeString()
        {
            try
            {
                //DateTime server_relative_time = DateTime.Now.Add(_server_base_time - _local_base_time);
                return PPTVData.KeyGenerator.GetKey(_server_base_time);
            }
            catch (Exception exp)
            {
                Output(exp.ToString());
                return "";
            }
        }

        void _jump_webclient_DownloadStringCompleted(object sender, DownloadStringCompletedEventArgs e)
        {
            if (e.Error == null)
            {
                Output("成功获得jump文件 " + e.Result);
                Output("解析xml");
                try
                {
                    using (XmlReader reader = XmlReader.Create(new StringReader(e.Result)))
                    {
                        if (true == reader.ReadToFollowing("server_host"))
                        {
                            string server_host = reader.ReadElementContentAsString("server_host", "");
                            _media_server_host = server_host;
                            Output("server_host=" + server_host);
                        }
                        else
                        {
                            //
                        }

                        reader.ReadToFollowing("server_time");
                        string server_time = reader.ReadElementContentAsString("server_time", "");
                        Output("server_time=" + server_time);

                        CultureInfo ci = new CultureInfo("en-US");
                        _server_base_time = DateTime.ParseExact(server_time, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                        _local_base_time = DateTime.Now;
                        Output("_server_base_time=" + _server_base_time);

                        _playDecodeUrl = "http://" + _media_server_host + ":80/" + _playInfo.Rid + "?type=wp7&w=1&key=" + GetServerRelativeTimeString();
                        Dispatcher.BeginInvoke(() =>
                        {
                            xMedia.Source = new Uri(_playDecodeUrl, UriKind.Absolute);
                        });

                        Output("播放地址:" + _playDecodeUrl);
                    }
                }
                catch (Exception exp)
                {
                    Output("jump_webclient_DownloadStringCompleted " + exp.ToString());
                    if (MediaFailed != null)
                    {
                        PPLiveExceptionRoutedEventArgs args = new PPLiveExceptionRoutedEventArgs(this, new PPLiveOpenException("获得jump文件失败 " + exp.Message));
                        MediaFailed(this, args);
                    }
                }
            }
            else
            {
                Output("获得jump文件失败 " + e.Error.ToString());
                if (MediaFailed != null)
                {
                    PPLiveExceptionRoutedEventArgs args = new PPLiveExceptionRoutedEventArgs(this, new PPLiveOpenException("获得jump文件失败 " + e.Error.Message));
                    MediaFailed(this, args);
                }
            }
        }

        private void xMedia_BufferingProgressChanged(object sender, RoutedEventArgs e)
        {
            string info = "缓冲进度:" + xMedia.BufferingProgress + " 缓冲时间:" + xMedia.BufferingTime;
            xMediaBufferTextBlockA.Text = info;
            if (BufferingProgressChanged != null)
            {
                PPLiveRoutedEventArgs args = new PPLiveRoutedEventArgs(this);
                BufferingProgressChanged(this, args);
            }
        }

        private void xMedia_DownloadProgressChanged(object sender, RoutedEventArgs e)
        {
            string info = "下载进度:" + xMedia.DownloadProgress + " 下载进度偏移:" + xMedia.DownloadProgressOffset;
            xMediaDownloadTextBlockA.Text = info;
        }

        private void xMedia_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            string info = "视频状态:" + xMedia.CurrentState;
            Output(xMedia.Name + ": xMedia_CurrentStateChanged " + info);
            xMediaCurrentStatusTextBlockA.Text = info;
            if (CurrentStateChanged != null)
            {
                PPLiveRoutedEventArgs args = new PPLiveRoutedEventArgs(this);
                CurrentStateChanged(this, args);
            }
        }

        private void xMedia_MediaEnded(object sender, RoutedEventArgs e)
        {
            xMediaStatusTextBlockA.Text = "视频文件播放结束";
            if (MediaEnded != null)
            {
                PPLiveRoutedEventArgs args = new PPLiveRoutedEventArgs(this);
                MediaEnded(this, args);
            }
        }

        private void xMedia_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            try
            {
                xMedia.Stop();                
                if (MediaFailed != null)
                {
                    PPLiveExceptionRoutedEventArgs args = new PPLiveExceptionRoutedEventArgs(this, e.ErrorException);
                    MediaFailed(this, args);
                }

                if (sender == null)
                {
                    Output("xMedia_MediaFailed sender==null");
                    return;
                }
                if (e == null)
                {
                    Output("xMedia_MediaFailed e==null");
                    return;
                }
                xMediaStatusTextBlockA.Text = "视频文件播放失败";
            }
            catch (Exception exp)
            {
                Output(exp.ToString());
            }
        }

        private void xMedia_MediaOpened(object sender, RoutedEventArgs e)
        {
            try
            {
               
                Output(xMedia.Name + ": xMedia_MediaOpened ");
                xMediaStatusTextBlockA.Text = "视频文件播放打开";
                if (MediaOpened != null)
                {
                    PPLiveRoutedEventArgs args = new PPLiveRoutedEventArgs(this);
                    MediaOpened(this, args);
                }
            }
            catch (Exception exp)
            {
                Output(exp.ToString());
            }
        }
    }
}
