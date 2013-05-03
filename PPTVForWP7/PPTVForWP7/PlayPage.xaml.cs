using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Collections.ObjectModel;
using System.Diagnostics;
using Microsoft.Devices;
using System.Windows.Media.Imaging;
using System.IO;
using Microsoft.Phone.Controls;

namespace PPTVForWP7
{
    using PPTVData.Log;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Social;

    public partial class PlayPage : PhoneApplicationPage
    {

        private DispatcherTimer _timer = new DispatcherTimer();
        private DispatcherTimer _timerOfControlBar = new DispatcherTimer();
        private PlayInfoFactory _playInfoFactory = new PlayInfoFactory();
        private bool _isPlayDBPosition = true;
        List<int> _candidacyBitrate = null;
        PlayInfo.Item _currentPlayBitrate = null;
        private double _openedToSeekPosition = 0; // 用于码率切换后，恢复到以前的位置
        private bool _mediaIsOpened = false;
        private int _vID = -1;
        private bool _isPlaySucceeded = false;
        private int _playBufferCount = 0;
        private int _allBufferCount = 0;
        private int _dragBufferCount = 0;
        private double _dragBufferTime = 0;
        private int _dragCount = 0;
        private double _bufferTime = 0;
        private double _prepareTime = 0;
        private int _programSource = -1;
        private int _averageDownSpeed = 0;
        private double _playTime = 0;
        private bool _isDrag = false;
        private DateTime _preDragBuffer;
        private DateTime _preBuffer;
        private DateTime _programBegin;
        private DateTime _beginPlaying;
        private bool _isAutoStop = false;
        private bool _isNewBuffer = true;
        private bool _isPlayEnd = false;
        private int _connType = 0;
        private bool _isOver = false;
        private double _bufferingPosition = 0;
        private bool _playLocalFile = false;
        private string _localFileId = "";
        private string _localFilePath = "";
        private string _localFileTitle = "";

        public int VID
        {
            set
            {
                _vID = value;
                StartDownloadData();
            }
            get { return _vID; }
        }
        private PlayInfo _playInfo = new PlayInfo();
        private ChannelDetailInfo _channelDetailInfo = null;
        public ChannelDetailInfo ChannelDetailInfo
        {
            get { return _channelDetailInfo; }
            set
            {
                _channelDetailInfo = value;
                xPlayerControlBar.ChannelDetailInfo = _channelDetailInfo;
            }
        }

        private int _programIndex = -1;
        public int ProgramIndex
        {
            get { return _programIndex; }
            set
            {
                _programIndex = value;
                xPlayerControlBar.ProgramIndex = _programIndex;
            }
        }

        #region ShowBitrates Animation

        /// <summary>
        /// 分辨率选择框是否显示
        /// </summary>
        public bool IsShowBitrates
        {
            get { return (bool)GetValue(IsShowBitratesProperty); }
            set { SetValue(IsShowBitratesProperty, value); }
        }

        public static DependencyProperty IsShowBitratesProperty = DependencyProperty.Register("IsShowBitrates",
            typeof(bool), typeof(PlayPage), new PropertyMetadata(false, OnShowBitretesChannged));

        private static void OnShowBitretesChannged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var playPage = d as PlayPage;
            if (playPage != null)
            {
                var value = (bool)e.NewValue;
                if (value && playPage.xBitrates.Visibility == Visibility.Collapsed)
                {
                    playPage.xBitrates.Visibility = Visibility.Visible;
                }
                var xPoint = 63;
                var storyBoard = value == false ? Utils.Utils.XAnimation(playPage.xBitrates, 0, -xPoint, 0, 0.5) :
                    Utils.Utils.XAnimation(playPage.xBitrates, -xPoint, 0, 0, 0.5);
                storyBoard.Begin();
            }
        }

        #endregion

        private ChannelDetailFactory _channelDetailFactory = new ChannelDetailFactory();

        public PlayPage()
        {
            InitializeComponent();
            InitVariableNEventHandle();
            Loaded += new RoutedEventHandler(PlayPage_Loaded);
            Utils.Utils.NetworkInterfaceTypeChanged += new EventHandler(OnNetworkTypeChanged);
            Utils.Utils.CheckNetworkType();
        }

        private void OnNetworkTypeChanged(object sender, EventArgs e)
        {
            if (Utils.Utils.CurrentNetworkType == Microsoft.Phone.Net.NetworkInformation.NetworkInterfaceType.MobileBroadbandCdma ||
                Utils.Utils.CurrentNetworkType == Microsoft.Phone.Net.NetworkInformation.NetworkInterfaceType.MobileBroadbandGsm)
                _connType = 0;
            else
            {
                if (Utils.Utils.CurrentNetworkType == Microsoft.Phone.Net.NetworkInformation.NetworkInterfaceType.Wireless80211)
                    _connType = 1;
                else
                    _connType = 0;
            }
        }

        void PlayPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (Utils.Utils.IsDebug)
            {
                xDebugTextBlock.Visibility = Visibility.Visible;
                xDebugBox.Visibility = Visibility.Visible;
                LogManager.Ins.SetLogContainer(xDebugBox);
            }
            _programBegin = DateTime.Now;

            //LogManager.Ins.SetLogContainer(xDebugBox);
        }

        /// <summary>
        /// 开始下载电影信息
        /// </summary>
        private void StartDownloadData()
        {
            BeginGetInfoNPlay();
        }

        private void InitVariableNEventHandle()
        {
            _isPlaySucceeded = false;
            _playBufferCount = 0;
            _allBufferCount = 0;
            _dragBufferCount = 0;
            _dragBufferTime = 0;
            _dragCount = 0;
            _bufferTime = 0;
            _prepareTime = 0;
            _programSource = -1;
            _averageDownSpeed = 0;
            _playTime = 0;
            _isDrag = false;

            DateTime _preDragBuffer = new DateTime(0);
            DateTime _preBuffer = new DateTime(0);
            DateTime _programBegin = new DateTime(0);
            DateTime _beginPlaying = new DateTime(0);

            _timerOfControlBar.Interval = TimeSpan.FromSeconds(5);
            _timerOfControlBar.Tick += new EventHandler(_timerOfControlBar_Tick);
            _timer.Interval = TimeSpan.FromSeconds(1);
            _timer.Tick += new EventHandler(_timer_Tick);
            _channelDetailFactory.HttpFailHandler += _channelDetailFactory_GetChannelDetailByVIDFailed;
            _channelDetailFactory.HttpSucessHandler += _channelDetailFactory_GetChannelDetailByVIDSucceed;
            _channelDetailFactory.HttpTimeOutHandler += _channelDetailFactory_GetChannelDetailTimeout;

            xPlayerControlBar.Seeking += new Controls.PlayerControlBar.SeekingEventHandler(xPlayerControlBar_Seeking);
            xPlayerControlBar.Seeked += new Controls.PlayerControlBar.SeekedEventHandler(xPlayerControlBar_Seeked);
            xPlayerControlBar.VolumeChanged += new Controls.PlayerControlBar.VolumeChangedEventHandler(xPlayerControlBar_VolumeChanged);
            xPlayerControlBar.MediaElementStatusChanged += new Controls.PlayerControlBar.MediaElementStatusChangedEventHandler(xPlayerControlBar_MediaElementStatusChanged);
            xPlayerControlBar.NextProgramSelected += new Controls.PlayerControlBar.NextProgramSelectedEventHandler(xPlayerControlBar_NextProgramSelected);
            xPlayerControlBar.PreviousProgramSelected += new Controls.PlayerControlBar.PreviousProgramSelectedEventHandler(xPlayerControlBar_PreviousProgramSelected);
            xPlayerControlBar.FastSelected += new Controls.PlayerControlBar.FastSelectedEventHandler(xPlayerControlBar_FastSelected);
            xPlayerControlBar.RewindSelected += new Controls.PlayerControlBar.RewindSelectedEventHandler(xPlayerControlBar_RewindSelected);
            xPlayerControlBar.VolumeSeeked += new Controls.PlayerControlBar.VolumeSeekedEventHandler(xPlayerControlBar_VolumeSeeked);
            xPlayerControlBar.ShareSocialEvent += xPlayerControlBar_ShareSocial;
        }

        void _playInfoFactory_GetPlayInfoTimeout(object sender, HttpFactoryArgs<PlayInfo> args)
        {
            MessageBox.Show("获取播放信息超时", "提示", MessageBoxButton.OK);
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

        void _playInfoFactory_GetPlayInfoSucceed(object sender, HttpFactoryArgs<PlayInfo> args)
        {
            _playInfo = args.Result;
            SetVarDefault();

            BuildBitrates();
            if (BeginPlay(-1, false))
            {
                xPlayerControlBar.ChannelDetailInfo = ChannelDetailInfo;
            }
            else
            {
                SetInfo(false, "", false);
                MessageBox.Show("不能播放该影片");
            }
        }

        void xPlayerControlBar_VolumeSeeked(object sender)
        {
            SetControlBarStatus(true, true);
        }

        void xPlayerControlBar_Seeking(object sender)
        {
            SetControlBarStatus(false, true);
        }

        void xPlayerControlBar_RewindSelected(object sender)
        {
            double position = xPPvodWP7MediaElement.Position.TotalSeconds - 30;

            TimeSpan positionTimeSpan = TimeSpan.FromSeconds(position < 0 ? 0 : position);

            xPPvodWP7MediaElement.Position = positionTimeSpan;
        }

        void xPlayerControlBar_FastSelected(object sender)
        {
            double position = xPPvodWP7MediaElement.Position.TotalSeconds + 30;
            TimeSpan positionTimeSpan = TimeSpan.FromSeconds(position >= xPPvodWP7MediaElement.NaturalDuration.TimeSpan.TotalSeconds ? xPPvodWP7MediaElement.NaturalDuration.TimeSpan.TotalSeconds : position);
            xPPvodWP7MediaElement.Position = positionTimeSpan;
        }

        void xPlayerControlBar_PreviousProgramSelected(object sender)
        {
            xPlayerControlBar.SetPlayerPostions(new Duration(TimeSpan.FromSeconds(0)), TimeSpan.FromSeconds(0), TimeSpan.FromSeconds(0));
            PlayPreviousProgram();
        }

        void xPlayerControlBar_NextProgramSelected(object sender)
        {
            _timer.Stop();
            xPlayerControlBar.SetPlayerPostions(xPPvodWP7MediaElement.NaturalDuration, TimeSpan.FromSeconds(0), TimeSpan.FromSeconds(0));
            PlayNextProgram();
        }

        /// <summary>
        /// 节目分享按钮事件
        /// </summary>
        /// <param name="sender"></param>
        void xPlayerControlBar_ShareSocial(object sender)
        {
            if (xPPvodWP7MediaElement.CurrentState == MediaElementState.Opening
                || xPPvodWP7MediaElement.CurrentState == MediaElementState.Playing
                || xPPvodWP7MediaElement.CurrentState == MediaElementState.Buffering)
            {
                SetControlBarStatus(false, false);
            }
            xShareItemGroup.IsShowEnabled = xShareItemGroup.IsShowEnabled ? false : true;
        }

        /// <summary>
        /// 节目分享单击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void xShareItemGroup_SocialTapAction(object sender, Controls.SocialEventArgs e)
        {
            var channelTitle = _programIndex == -1 ? _channelDetailInfo.Title
                : string.Format("{0} {1}", _channelDetailInfo.Title, _channelDetailInfo.ProgramInfos[_programIndex].Title);
            var channelId = _programIndex == -1 ? _channelDetailInfo.VID
                : _channelDetailInfo.ProgramInfos[_programIndex].ChannelVID;
            e.SocialData.SocialShare(channelTitle,
                PPTVData.Utils.ConvertToWebSite(channelId), xShareItemGroup_ShareCompleted);
            xShareItemGroup.IsShowEnabled = false;
        }

        /// <summary>
        /// 绑定单击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void xShareItemGroup_UnBindTapEvent(object sender, Controls.SocialEventArgs e)
        {
            Utils.SocialUtils.NavigateToSocial(e.SocialType, this);
        }

        /// <summary>
        /// 分享完成后回调
        /// </summary>
        /// <param name="isSuccess"></param>
        /// <param name="message"></param>
        void xShareItemGroup_ShareCompleted(bool isSuccess, string message)
        {
            Dispatcher.BeginInvoke(() =>
            {
                xPopDialog.ContentText = isSuccess ? "分享成功" : "未分享成功";
                xPopDialog.IsShowEnable = true;
            });
        }

        /// <summary>
        /// 开始播放某个码流率的片子
        /// </summary>
        /// <param name="bitrate">要播放的码流率</param>
        /// <param name="keepCurrentPosition">是否保持当期的播放位置</param>
        /// <returns></returns>
        bool BeginPlay(int bitrate, bool keepCurrentPosition)
        {
            if (_currentPlayBitrate != null && _currentPlayBitrate.FileType == bitrate)
            {
                return true;
            }
            if (_playInfo == null)
            {
                return false;
            }

            _candidacyBitrate = new List<int>();
            if (bitrate == -1)
            {
                if (Utils.Utils.GetEnableDefinition())
                {
                    if (_playInfo.GetItemByFileType(0) != null)
                    {
                        _candidacyBitrate.Add(0);
                    }
                }
            }
            else
            {
                _candidacyBitrate.Add(bitrate);
            }
            if (bitrate != 5 && _playInfo.GetItemByFileType(5) != null)
            {
                _candidacyBitrate.Add(5);
            }
            else if (bitrate != 6 && _playInfo.GetItemByFileType(6) != null)
            {
                _candidacyBitrate.Add(6);
            }
            return PlayFirstCondidacyBitrate(keepCurrentPosition);
        }

        /// <summary>
        /// 播放候选码流率的第一个
        /// </summary>
        /// <param name="keepCurrentPosition">是否保存当前的播放位置</param>
        /// <returns>false 播放失败，已经没有码流率可供选择了</returns>
        bool PlayFirstCondidacyBitrate(bool keepCurrentPosition)
        {
            if (_candidacyBitrate.Count <= 0)
            {
                return false;
            }
            while (_candidacyBitrate.Count > 0)
            {
                int bitrate = _candidacyBitrate.First();
                _candidacyBitrate.RemoveAt(0);
                PlayInfo.Item item = _playInfo.GetItemByFileType(bitrate);
                if (item != null)
                {
                    if (keepCurrentPosition)
                    {
                        if (_mediaIsOpened)
                        {
                            _openedToSeekPosition = xPPvodWP7MediaElement.Position.TotalSeconds;
                        }
                    }
                    else
                    {
                        _openedToSeekPosition = 0;
                    }
                    if (item.DtInfo != null)
                    {
                        xPPvodWP7MediaElement.DirectSource = item;
                    }
                    else if (!string.IsNullOrEmpty(item.Rid) && item.Rid != "null")
                    {
                        xPPvodWP7MediaElement.OriSource = item;
                    }
                    _mediaIsOpened = false;
                    _currentPlayBitrate = item;
                    HighlightCurentBitrate();
                    return true;
                }
            }
            return false;
        }
        void PlayPreviousProgram()
        {
            if (ProgramIndex == -1)
                return;
            else
                BeginGetInfoNPlay(true, false);//开始获取上一集信息
        }

        void BeginGetInfoNPlay(bool isPlayOtherProgram = false, bool isPlayNextProgram = true)
        {
            if (isPlayOtherProgram)
            {
                if (ChannelDetailInfo != null)
                {
                    if (isPlayNextProgram)
                    {
                        //下一集
                        int temp = ProgramIndex + 1;
                        if (temp < ChannelDetailInfo.ProgramInfos.Count)
                        {
                            //有下一集时, 1.保存当集 2.播放下一集
                            _currentPlayBitrate = null;
                            SetInfo(true, "正在加载第" + ChannelDetailInfo.ProgramInfos[temp].Title + "集");

                            SetControlBarStatus(false, true);
                            ProgramIndex = temp;
                            SaveToDB(temp - 1);
                        }
                        else
                        {
                            if (NavigationService.CanGoBack)
                                NavigationService.GoBack();
                        }
                    }
                    else
                    {
                        //上一集
                        int temp = ProgramIndex - 1;
                        if (temp >= 0)
                        {
                            //有上一集时, 1.保存当集 2.播放上一集
                            _currentPlayBitrate = null;
                            SetInfo(true, "正在加载第" + ChannelDetailInfo.ProgramInfos[temp].Title + "集");
                            SetControlBarStatus(false, true);
                            ProgramIndex = temp;
                            SaveToDB(temp + 1);
                        }
                    }
                }
                else
                {
                    //_isPlayProgram = true;
                    //_isPlayNextProgram = true;
                }
            }
            if (ChannelDetailInfo == null)
            {
                //重新读取影片信息
                SetControlBarStatus(false, true);
                SetInfo(true, "正在读取影片信息");
                _channelDetailFactory.DownLoadDatas(VID);
            }
            else if (_currentPlayBitrate == null || string.IsNullOrEmpty(_currentPlayBitrate.Rid)
                || _currentPlayBitrate.Rid == "null")
            {
                //只读Play接口信息
                StartGetPlayAPI();
            }
        }

        void PlayNextProgram()
        {
            if (ProgramIndex == -1)
            {
                if (NavigationService.CanGoBack)
                    NavigationService.GoBack();
                return;
            }
            else
                BeginGetInfoNPlay(true, true);//开始获取下一集信息
        }

        void _timerOfControlBar_Tick(object sender, EventArgs e)
        {
            if (_mediaIsOpened)
            {
                _timerOfControlBar.Stop();
                SetControlBarStatus(false, false);
            }
        }

        void xPlayerControlBar_MediaElementStatusChanged(object sender, MediaElementState mediaElementState)
        {
            switch (mediaElementState)
            {
                case MediaElementState.Paused:
                    xPPvodWP7MediaElement.Pause();
                    break;
                case MediaElementState.Playing:
                    {
                        if (_playLocalFile)
                        {
                            xPPvodWP7MediaElement.Play();
                            break;
                        }
                        if (ChannelDetailInfo != null
                            && (_currentPlayBitrate != null && string.IsNullOrEmpty(_currentPlayBitrate.Rid) == false
                              && _currentPlayBitrate.Rid != "null")
                            && string.IsNullOrEmpty(xPPvodWP7MediaElement.OriSource.Rid) == false)
                            xPPvodWP7MediaElement.Play();
                        else
                            BeginGetInfoNPlay();

                        Utils.PlayInfo playInfo = new Utils.PlayInfo();
                        playInfo.stopReason = 1;
                    }
                    break;
                case MediaElementState.Stopped:
                    xPPvodWP7MediaElement.Stop();
                    break;
                case MediaElementState.Buffering:

                    break;
                default:
                    break;
            }
        }

        void xPlayerControlBar_VolumeChanged(object sender, double currentValue)
        {
            Utils.Utils.SaveVolumeToIOS(currentValue);
            xPPvodWP7MediaElement.Volume = (currentValue == 0 ? currentValue : (currentValue / 2 + 0.50));


            SetControlBarStatus(false, true);
        }

        void xPlayerControlBar_Seeked(object sender, double currentPosition)
        {

            _bufferingPosition = currentPosition;
            _isPlayDBPosition = false;
            _timer.Stop();
            _isDrag = true;
            if (currentPosition < 0)
                currentPosition = 0;
            xPPvodWP7MediaElement.Position = TimeSpan.FromSeconds(currentPosition);
            _timer.Start();
            _dragCount++;
        }

        void _timer_Tick(object sender, EventArgs e)
        {
            xPlayerControlBar.SetPlayerPostions(xPPvodWP7MediaElement.NaturalDuration, xPPvodWP7MediaElement.Position, xPPvodWP7MediaElement.DownloadPosition);
            xDebugTextBlock.Text = "MediaElement状态是:" + xPPvodWP7MediaElement.CurrentState.ToString();
        }

        void _channelDetailFactory_GetChannelDetailTimeout(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            SetInfo(false, string.Empty, false);
            MessageBox.Show("连接超时，获取影片信息失败!");
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

        void _channelDetailFactory_GetChannelDetailByVIDFailed(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            SetInfo(false, "", false);
            MessageBox.Show("获取影片信息失败!");
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

        void _channelDetailFactory_GetChannelDetailByVIDSucceed(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            ChannelDetailInfo = args.Result;

            LogManager.Ins.Log("ChannelDetailInfo信息下载成功");
            BeginGetInfoNPlay();
            //_isPlayProgram = false;
        }

        void StartGetPlayAPI()
        {
            xPPvodWP7MediaElement.Pause();
            _playInfoFactory = new PlayInfoFactory();
            _playInfoFactory.HttpSucessHandler += _playInfoFactory_GetPlayInfoSucceed;
            _playInfoFactory.HttpTimeOutHandler += _playInfoFactory_GetPlayInfoTimeout;
            if (ProgramIndex != -1)//多集
                _playInfoFactory.DownLoadDatas(ChannelDetailInfo.ProgramInfos[ProgramIndex].ChannelVID);
            else//单集
            {
                if (ChannelDetailInfo.ProgramInfos.Count > 0)
                    _playInfoFactory.DownLoadDatas(ChannelDetailInfo.ProgramInfos[0].ChannelVID);
                else
                    _playInfoFactory.DownLoadDatas(VID);
            }
            SetVarDefault();
        }


        private void xPPvodWP7MediaElement_MediaEnded(object sender, PPTVWP7MediaPlayer.PPLiveRoutedEventArgs e)
        {
            _isPlayEnd = true;
            _isOver = true;
            if (_playLocalFile)
            {
                //
                LocalPlayFactory localFactory = new LocalPlayFactory();
                localFactory.UpdateRecord(_localFileId, xPPvodWP7MediaElement.Position.Seconds);
                if (NavigationService.CanGoBack)
                    NavigationService.GoBack();
                return;
            }
            SaveToDB(ProgramIndex);
            xPPvodWP7MediaElement.Stop();
            _playBufferCount = _allBufferCount - _dragBufferCount;
            PPTVData.Factory.DACPlayInfo playInfo = new PPTVData.Factory.DACPlayInfo();
            playInfo.castType = 3;
            playInfo.stopReason = 0;
            playInfo.isPlaySucceeded = _isPlaySucceeded ? 1 : 0;
            playInfo.connType = _connType;
            playInfo.dragCount = _dragCount;
            playInfo.playBufferCount = _playBufferCount;
            playInfo.dragBufferTime = _dragBufferTime;
            playInfo.allBufferCount = _allBufferCount;
            playInfo.bufferTime = _bufferTime;
            playInfo.prepareTime = _prepareTime;
            playInfo.programSource = _programSource;
            playInfo.averageDownSpeed = _averageDownSpeed;
            playInfo.playTime = _playTime;

            playInfo.mp4Name = Utils.Utils.UrlDecode(_currentPlayBitrate != null ? _currentPlayBitrate.Rid : "");
            PPTVData.Factory.DACFactory.DACPlaying(playInfo, ChannelDetailInfo, Utils.Utils.GetClientVersion());

            _isAutoStop = true;
            PlayNextProgram();
        }

        private void xPPvodWP7MediaElement_MediaOpened(object sender, PPTVWP7MediaPlayer.PPLiveRoutedEventArgs e)
        {
            if (_playLocalFile)
            {
                // 接着从上次停止的地方接着观看

                LocalPlayFactory localFactory = new LocalPlayFactory();
                xPPvodWP7MediaElement.Position = TimeSpan.FromSeconds(localFactory.GetPosition(_localFileId));
                return;
            }
            _isPlaySucceeded = true;
            _prepareTime = (DateTime.Now - _programBegin).TotalSeconds;
            _beginPlaying = DateTime.Now;
            if (!_mediaIsOpened)
            {
                _mediaIsOpened = true;
                // seek 到以前的位置
                if (_openedToSeekPosition > 0)
                {
                    if (xPPvodWP7MediaElement.CanSeek)
                    {
                        xPPvodWP7MediaElement.Position = TimeSpan.FromSeconds(_openedToSeekPosition);
                        _openedToSeekPosition = 0;
                    }
                }
            }
        }

        private void xPPvodWP7MediaElement_MediaFailed(object sender, PPTVWP7MediaPlayer.PPLiveExceptionRoutedEventArgs e)
        {
            if (_playLocalFile)
            {
                MessageBox.Show("无法播放该影片");
                if (NavigationService.CanGoBack)
                    NavigationService.GoBack();
                return;
            }
            int oldBitrate = _currentPlayBitrate != null ? _currentPlayBitrate.FileType : -1;
            if (PlayFirstCondidacyBitrate(true))
            {
                if (oldBitrate == 0 || oldBitrate == 1)
                {
                    SetInfo(true, "当前高清版本无法播放，正在请求其他版本！");
                }
                return;
            }

            Debug.WriteLine("错误异常:" + e.ErrorException.Message);
            SetInfo(false);
            _playBufferCount = _allBufferCount - _dragBufferCount;
            PPTVData.Factory.DACPlayInfo playInfo = new PPTVData.Factory.DACPlayInfo();
            playInfo.castType = 3;
            playInfo.stopReason = 2;
            playInfo.isPlaySucceeded = _isPlaySucceeded ? 1 : 0;
            playInfo.connType = _connType;
            playInfo.dragCount = _dragCount;
            playInfo.playBufferCount = _playBufferCount;
            playInfo.dragBufferTime = _dragBufferTime;
            playInfo.allBufferCount = _allBufferCount;
            playInfo.bufferTime = _bufferTime;
            playInfo.prepareTime = _prepareTime;
            playInfo.programSource = _programSource;
            playInfo.averageDownSpeed = _averageDownSpeed;
            playInfo.playTime = _playTime;
            if (_currentPlayBitrate != null)
                playInfo.mp4Name = Utils.Utils.UrlDecode(_currentPlayBitrate.Rid);
            if (ChannelDetailInfo != null)
                PPTVData.Factory.DACFactory.DACPlaying(playInfo, ChannelDetailInfo, Utils.Utils.GetClientVersion());
            _isPlaySucceeded = false;
            ChannelDetailInfo = null;
            MessageBox.Show(e.ErrorException.Message);
            if (NavigationService.CanGoBack)
                NavigationService.GoBack();
        }

        private void SetControlBarStatus(bool isTimer = true, bool isVisible = true)
        {
            if (isVisible)
            {
                xPlayerControlBar.SetPlayerPostions(xPPvodWP7MediaElement.NaturalDuration, xPPvodWP7MediaElement.Position, xPPvodWP7MediaElement.DownloadPosition);
                xPlayerControlBar.Visibility = Visibility.Visible;
                xShareItemGroup.IsShowEnabled = false;
                if (!_playLocalFile)
                {
                    xShowBitrates.Visibility = Visibility.Visible;
                    IsShowBitrates = false;
                }
                _timer.Start();
            }
            else
            {
                xPlayerControlBar.Visibility = Visibility.Collapsed;
                xShareItemGroup.IsShowEnabled = false;
                xShowBitrates.Visibility = Visibility.Collapsed;
                IsShowBitrates = false;
                _timer.Stop();
            }

            if (isTimer && xPPvodWP7MediaElement.CurrentState != MediaElementState.Paused && xPPvodWP7MediaElement.CurrentState != MediaElementState.Stopped && xPPvodWP7MediaElement.CurrentState != MediaElementState.Buffering && xPPvodWP7MediaElement.CurrentState != MediaElementState.Opening && xPPvodWP7MediaElement.CurrentState != MediaElementState.Closed)
                _timerOfControlBar.Start();
            else
                _timerOfControlBar.Stop();
        }

        private void xPPvodWP7MediaElement_OutputLog(object sender, PPTVWP7MediaPlayer.LogEventArgs e)
        {
            LogManager.Ins.Log(e.Text);
        }

        private void SetInfo(bool isVisible, string message = "", bool isShowProgressBar = true)
        {
            if (isVisible)
            {
                xLoadInfoGrid.Visibility = Visibility.Visible;
            }
            else
            {
                xLoadInfoGrid.Visibility = Visibility.Collapsed;
            }
            if (isShowProgressBar)
            {
                xProgressBar.Visibility = Visibility.Visible;
            }
            else
                xProgressBar.Visibility = Visibility.Collapsed;
            xInfoTextBlock.Text = message;
        }

        private void xPPvodWP7MediaElement_CurrentStateChanged(object sender, PPTVWP7MediaPlayer.PPLiveRoutedEventArgs e)
        {
            switch (xPPvodWP7MediaElement.CurrentState)
            {
                case MediaElementState.AcquiringLicense:
                    break;
                case MediaElementState.Buffering:
                    _bufferingPosition = xPPvodWP7MediaElement.Position.TotalSeconds;
                    SetInfo(true, "正在缓冲影片");
                    SetControlBarStatus(false, true);
                    break;
                case MediaElementState.Closed:
                    _timer.Stop();
                    break;
                case MediaElementState.Individualizing:
                    break;
                case MediaElementState.Opening:
                    SetInfo(true, "正在打开影片");
                    BeginSetChannelToVideoHub(ChannelDetailInfo);
                    break;
                case MediaElementState.Paused:
                    SetControlBarStatus(false, true);
                    break;
                case MediaElementState.Playing:
                    //判断是否有播放过,如果有播放过 则从用户播放过的位置开始播放
                    _isDrag = false;
                    if (_isPlayDBPosition)
                    {
                        if (_playLocalFile)
                        {
                            // 接着从上次停止的地方接着观看
                            LocalPlayFactory localFactory = new LocalPlayFactory();
                            xPPvodWP7MediaElement.Position = TimeSpan.FromSeconds(localFactory.GetPosition(_localFileId));
                        }
                        else
                        {
                            var playHistroy = PlayHistoryFactory.Instance.GetEntity(VID);
                            if (playHistroy != null)
                            {
                                _timer.Stop();
                                if (playHistroy.ProgramIndex == ProgramIndex)
                                    xPPvodWP7MediaElement.Position = TimeSpan.FromSeconds(playHistroy.Position);
                            }
                        }

                    }

                    SetControlBarStatus();
                    SetInfo(false);
                    break;
                case MediaElementState.Stopped:
                    {
                        _playTime = (DateTime.Now - _beginPlaying).TotalSeconds;
                        _timer.Stop();
                        SetInfo(false);
                        SetControlBarStatus(false, true);
                        if (!_isAutoStop)
                        {
                            _isAutoStop = false;
                            _playBufferCount = _allBufferCount - _dragBufferCount;
                            PPTVData.Factory.DACPlayInfo playInfo = new PPTVData.Factory.DACPlayInfo();
                            playInfo.castType = 3;
                            playInfo.stopReason = 0;
                            playInfo.isPlaySucceeded = _isPlaySucceeded ? 1 : 0;
                            playInfo.connType = _connType;
                            playInfo.dragCount = _dragCount;
                            playInfo.playBufferCount = _playBufferCount;
                            playInfo.dragBufferTime = _dragBufferTime;
                            playInfo.allBufferCount = _allBufferCount;
                            playInfo.bufferTime = _bufferTime;
                            playInfo.prepareTime = _prepareTime;
                            playInfo.programSource = _programSource;
                            playInfo.averageDownSpeed = _averageDownSpeed;
                            playInfo.playTime = _playTime;

                            playInfo.mp4Name = Utils.Utils.UrlDecode(_currentPlayBitrate != null ? _currentPlayBitrate.Rid : "");

                            PPTVData.Factory.DACFactory.DACPlaying(playInfo, ChannelDetailInfo, Utils.Utils.GetClientVersion());

                        }
                    }
                    break;
                default:
                    break;
            }
            //xDebugTextBlock.Text = xPPvodWP7MediaElement.CurrentState.ToString();
            xPlayerControlBar.SetPlayerControlBarStuas(xPPvodWP7MediaElement.CurrentState);
            if (xPPvodWP7MediaElement.CurrentState != MediaElementState.Buffering)
                _isDrag = false;
        }

        private void BeginSetChannelToVideoHub(ChannelDetailInfo channelDetailInfo)
        {
            if (channelDetailInfo != null)
            {
                if (!string.IsNullOrEmpty(channelDetailInfo.ImgUrl))
                {
                    //WebClient clientByImage = new WebClient();
                    //clientByImage.OpenReadCompleted += new OpenReadCompletedEventHandler(clientByImage_OpenReadCompleted);
                    //clientByImage.OpenReadAsync(new Uri(channelDetailInfo.ImgUrl, UriKind.RelativeOrAbsolute));
                    PPTVData.HttpWebClient webClient = new PPTVData.HttpWebClient();
                    webClient.OpenReadCompleted += new PPTVData.OpenReadCompletedEventHandler(webClient_OpenReadCompleted);
                    webClient.OpenReadAsync(channelDetailInfo.ImgUrl);
                }
            }
        }

        void webClient_OpenReadCompleted(object sender, PPTVData.OpenReadCompletedEventArgs e)
        {
            try
            {
                MediaHistoryItem mediaHistoryItem1 = new MediaHistoryItem();
                mediaHistoryItem1.ImageStream = e.Result;
                mediaHistoryItem1.Title = _channelDetailInfo.Title;
                mediaHistoryItem1.PlayerContext.Add("HubTag", _channelDetailInfo.VID + "|||" + ProgramIndex);
                MediaHistory.Instance.WriteAcquiredItem(mediaHistoryItem1);
                e.Result.Position = 0;
                MediaHistory.Instance.WriteRecentPlay(mediaHistoryItem1);
            }
            catch { }
        }

        //void clientByImage_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        //{
        //    try
        //    {
        //        MediaHistoryItem mediaHistoryItem1 = new MediaHistoryItem();
        //        mediaHistoryItem1.ImageStream = e.Result;
        //        mediaHistoryItem1.Title = _channelDetailInfo.Title;
        //        mediaHistoryItem1.PlayerContext.Add("HubTag", _channelDetailInfo.VID + "|||" + ProgramIndex);
        //        MediaHistory.Instance.WriteAcquiredItem(mediaHistoryItem1);
        //        e.Result.Position = 0;
        //        MediaHistory.Instance.WriteRecentPlay(mediaHistoryItem1);
        //    }
        //    catch { }
        //}

        private void xPPvodWP7MediaElement_BufferingProgressChanged(object sender, PPTVWP7MediaPlayer.PPLiveRoutedEventArgs e)
        {
            if (_isDrag)
            {
                if (_isNewBuffer)
                {
                    _preDragBuffer = DateTime.Now;

                    _dragBufferCount++;
                }
                else
                {
                    _dragBufferTime += (DateTime.Now - _preDragBuffer).TotalSeconds;
                    _preDragBuffer = DateTime.Now;
                }
            }

            if (_isNewBuffer)
            {
                _preBuffer = DateTime.Now;
                _isNewBuffer = false;
                _allBufferCount++;
            }
            else
            {
                _bufferTime += (DateTime.Now - _preBuffer).TotalSeconds;
                _preBuffer = DateTime.Now;
            }


            int progress = Convert.ToInt32(xPPvodWP7MediaElement.BufferingProgress * 100);
            if (progress >= 100)
            {
                _isNewBuffer = true;
                _isDrag = false;
            }
            if (progress == 100 || xPPvodWP7MediaElement.CurrentState != MediaElementState.Buffering || (xPPvodWP7MediaElement.Position.TotalSeconds > _bufferingPosition))
                SetInfo(false);
            else
                SetInfo(true, "正在缓冲" + (progress + "%" == "0%" ? "" : progress + "%"));
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (e.NavigationMode != System.Windows.Navigation.NavigationMode.Back)
            {
                Utils.SocialUtils.ClearBackStack(this, "PlayPage.xaml");
                var response = Utils.SocialUtils.CreateBindResponse(this);
                if (!string.IsNullOrEmpty(response))
                {
                    xPopDialog.ContentText = response;
                    xPopDialog.IsShowEnable = true;
                }
            }
            if (e.NavigationMode == System.Windows.Navigation.NavigationMode.New)
            {
                if (NavigationContext.QueryString.ContainsKey("programIndex"))
                {
                    if (NavigationContext.QueryString["programIndex"] != null)
                    {
                        ProgramIndex = Convert.ToInt32(NavigationContext.QueryString["programIndex"]);
                    }
                }
            }
            _playLocalFile = false;
            if (NavigationContext.QueryString.ContainsKey("localplay"))
            {
                _playLocalFile = true;
            }

            // _playLocalFile = true;
            if (!_playLocalFile)
            {
                VID = Convert.ToInt32(NavigationContext.QueryString["vid"]);
                _programSource = Convert.ToInt32(NavigationContext.QueryString["programSource"]);
                SetVarDefault();
            }
            else
            {
                SetVarDefault();
                _localFileId = NavigationContext.QueryString.ContainsKey("id") ?
                    NavigationContext.QueryString["id"] : "";
                LocalPlayFactory localFactory = new LocalPlayFactory();
                if (localFactory.GetPosition(_localFileId) == 0)
                {
                    localFactory.InsertRecord(_localFileId, 0);
                }
                _localFilePath = NavigationContext.QueryString.ContainsKey("path") ?
                    NavigationContext.QueryString["path"] : "";
                _localFileTitle = NavigationContext.QueryString.ContainsKey("title") ?
                    NavigationContext.QueryString["title"] : "";
                if (!System.IO.IsolatedStorage.IsolatedStorageFile.GetUserStoreForApplication().FileExists(_localFilePath))
                {
                    MessageBox.Show("无法找到文件", "提示", MessageBoxButton.OK);
                    DownPage.DownFactory.DeleteRecord(_localFileId);
                    if (NavigationService.CanGoBack)
                        NavigationService.GoBack();
                    return;

                }
                xPPvodWP7MediaElement.Source = _localFilePath; // path;                
                xPlayerControlBar.localFileTitle = _localFileTitle;// title;
            }


        }

        private void SetVarDefault()
        {
            _candidacyBitrate = null;
            _currentPlayBitrate = null;
            _isPlayDBPosition = true;
            _isPlayEnd = false;
        }
        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            base.OnBackKeyPress(e);
        }

        protected override void OnNavigatingFrom(System.Windows.Navigation.NavigatingCancelEventArgs e)
        {
            _timer.Stop();
            _timerOfControlBar.Stop();
            if (_playLocalFile)
            {
                xPPvodWP7MediaElement.Pause();
                if (xPPvodWP7MediaElement.CurrentState == MediaElementState.Paused || xPPvodWP7MediaElement.CurrentState == MediaElementState.Playing)
                {// 更新当前的播放位置                  

                    LocalPlayFactory localFacotry = new LocalPlayFactory();
                    localFacotry.UpdateRecord(_localFileId, (long)xPPvodWP7MediaElement.Position.TotalSeconds);
                }
                return;
            }
            LogManager.Ins.Log("开始退出执行DAC");
            if (xPPvodWP7MediaElement.CurrentState != MediaElementState.Stopped)
            {
                _playTime = (DateTime.Now - _beginPlaying).TotalSeconds;

                _playBufferCount = _allBufferCount - _dragBufferCount;
                PPTVData.Factory.DACPlayInfo playInfo = new PPTVData.Factory.DACPlayInfo();
                playInfo.castType = 3;
                playInfo.stopReason = 1;
                playInfo.isPlaySucceeded = _isPlaySucceeded ? 1 : 0;
                LogManager.Ins.Log("查Wifi开始");
                playInfo.connType = _connType;
                LogManager.Ins.Log("查Wifi结束");
                playInfo.dragCount = _dragCount;
                playInfo.playBufferCount = _playBufferCount;
                playInfo.dragBufferTime = _dragBufferTime;
                playInfo.allBufferCount = _allBufferCount;
                playInfo.bufferTime = _bufferTime;
                playInfo.prepareTime = _prepareTime;
                playInfo.programSource = _programSource;
                playInfo.averageDownSpeed = _averageDownSpeed;
                playInfo.playTime = _playTime;
                
                playInfo.mp4Name = Utils.Utils.UrlDecode(_currentPlayBitrate != null ?
                    _currentPlayBitrate.Rid : "");


                if (ChannelDetailInfo != null && !_isAutoStop)
                    PPTVData.Factory.DACFactory.DACPlaying(playInfo, ChannelDetailInfo, Utils.Utils.GetClientVersion());
                _isAutoStop = false;
            }
            LogManager.Ins.Log("DAC完成");
            SetInfo(true);
            LogManager.Ins.Log("开始保存到数据库");
            if (xPPvodWP7MediaElement.CurrentState == MediaElementState.Paused || xPPvodWP7MediaElement.CurrentState == MediaElementState.Playing)
                SaveToDB(ProgramIndex);
            LogManager.Ins.Log("数据库保存完成");
            //if (_isBackPress)
            //    xPPvodWP7MediaElement.OriSource = "";

            ChannelDetailInfo = null;
            _currentPlayBitrate = null;
            LogManager.Ins.Log("清空Source");
        }

        private void SaveToDB(int programIndex)
        {
            if (_channelDetailInfo == null)
            {
                _isOver = false;
                return;
            }
            var playHistory = new DBWatchHistoryChanneInfo()
                {
                    VID = VID,
                    Title = _channelDetailInfo.Title,
                    Position = xPPvodWP7MediaElement.Position.TotalSeconds,
                    IsOver = _isOver,
                    ProgramIndex = programIndex,
                    Time = DateTime.Now
                };
            playHistory.ProgrameTitle =
                programIndex == -1 ? string.Empty : _channelDetailInfo.ProgramInfos[programIndex].Title;
            if (_isPlayEnd)
                playHistory.Position = xPPvodWP7MediaElement.NaturalDuration.TimeSpan.TotalSeconds;
            PlayHistoryFactory.Instance.AddEntity(playHistory);
            _isOver = false;
        }

        private void xPPvodWP7MediaElement_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (xPPvodWP7MediaElement.CurrentState == MediaElementState.Opening || xPPvodWP7MediaElement.CurrentState == MediaElementState.Playing || xPPvodWP7MediaElement.CurrentState == MediaElementState.Buffering)
            {
                if (xPlayerControlBar.Visibility == Visibility.Collapsed)
                    SetControlBarStatus();
                else
                    SetControlBarStatus(false, false);
            }
        }

        private void BuildBitrates()
        {
            if (_playInfo == null)
            {
                return;
            }
            if (_playInfo.GetItemByFileType(1) != null)
            {
                xBitrateBluray.Visibility = Visibility.Visible;
            }
            else
            {
                xBitrateBluray.Visibility = Visibility.Collapsed;
            }
            if (_playInfo.GetItemByFileType(0) != null)
            {
                xBitrateHD.Visibility = Visibility.Visible;
            }
            else
            {
                xBitrateHD.Visibility = Visibility.Collapsed;
            }
            if (_playInfo.GetItemByFileType(5) != null || _playInfo.GetItemByFileType(6) != null)
            {
                xBitrateFluid.Visibility = Visibility.Visible;
            }
            else
            {
                xBitrateFluid.Visibility = Visibility.Collapsed;
            }
        }
        private void HighlightCurentBitrate()
        {
            foreach (Grid b in new Grid[] { xBitrateBluray, xBitrateHD, xBitrateFluid })
            {
                b.Children[0].Visibility = Visibility.Collapsed;
                b.Children[1].Visibility = Visibility.Collapsed;
            }
            if (_currentPlayBitrate == null)
            {
                return;
            }
            int bitrate = _currentPlayBitrate.FileType;
            Grid h = bitrate == 1 ? xBitrateBluray : (bitrate == 0 ? xBitrateHD :
                (bitrate == 5 || bitrate == 6 ? xBitrateFluid : null));
            if (h != null)
            {
                h.Children[0].Visibility = Visibility.Visible;
                h.Children[1].Visibility = Visibility.Visible;
            }
        }
        private void xShowBitrates_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            xShowBitrates.Visibility = Visibility.Collapsed;
            IsShowBitrates = true;
        }

        private void xHideBitrates_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            xShowBitrates.Visibility = Visibility.Visible;
            IsShowBitrates = false;
        }

        private void xBitrateBluray_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_currentPlayBitrate != null && _currentPlayBitrate.FileType == 1)
            {
                return;
            }

            SetInfo(true, "正在帮您切换到蓝光版本，请稍候...");
            BeginPlay(1, true);
        }

        private void xBitrateHD_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_currentPlayBitrate != null && _currentPlayBitrate.FileType == 0)
            {
                return;
            }

            SetInfo(true, "正在帮您切换到高清版本，请稍候...");
            BeginPlay(0, true);
        }

        private void xBitrateFluid_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_currentPlayBitrate != null && _currentPlayBitrate.FileType == 5)
            {
                return;
            }

            SetInfo(true, "正在帮您切换到流畅版本，请稍候...");
            BeginPlay(5, true);
        }
    }
}