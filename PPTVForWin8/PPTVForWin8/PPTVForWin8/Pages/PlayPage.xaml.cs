using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

using Windows.System;
using Windows.System.Display;
using Windows.System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Media.PlayTo;

namespace PPTVForWin8.Pages
{
    using PPTVData.Entity;

    using PPTVForWin8.Utils;
    using PPTV.WinRT.CommonLibrary.DataModel.Play;
    using PPTV.WinRT.CommonLibrary.Factory.Play;
    using PPTV.WinRT.CommonLibrary.Utils;

    public sealed partial class PlayPage : PPTVForWin8.Common.LayoutAwarePage
    {
        private DispatcherTimer _timer;
        private DispatcherTimer _timerOfControler;
        private ThreadPoolTimer _timerOfDAC;

        private DisplayRequest _displayRequest;
        private PlayToManager _playToManager;
        private CoreDispatcher _dispatcher;
        private IPlayFactory _playFactory;
        private PlayMessageInfo _playMessage;
        private static CoreCursor _coreCursor;

        bool _isBuffering;
        bool _isDragBuffering;
        bool _isBufferingOneTime;
        bool _ispreing;
        double _bufferingOneTime;

        public static DACPageType DACPageType;

        public PlayPage()
        {
            this.InitializeComponent();

            _dispatcher = Window.Current.CoreWindow.Dispatcher;
            _playToManager = PlayToManager.GetForCurrentView();
            _displayRequest = new DisplayRequest();
            _coreCursor = Window.Current.CoreWindow.PointerCursor;
            _timerOfDAC = ThreadPoolTimer.CreatePeriodicTimer(_timerOfDAC_Tick, TimeSpan.FromSeconds(1));

            _timer = new DispatcherTimer();
            _timer.Tick += _timer_Tick;
            _timer.Interval = TimeSpan.FromSeconds(1);

            _timerOfControler = new DispatcherTimer();
            _timerOfControler.Tick += _timerOfControler_Tick;
            _timerOfControler.Interval = TimeSpan.FromSeconds(3);

            xMediaPlayer.BufferingProgressChanged += xMediaPlayer_BufferingProgressChanged;
            xMediaPlayer.CurrentStateChanged += xMediaPlayer_CurrentStateChanged;
            xMediaPlayer.MediaEnded += xMediaPlayer_MediaEnded;
            xMediaPlayer.MediaFailed += xMediaPlayer_MediaFailed;
            xMediaPlayer.MediaOpened += xMediaPlayer_MediaOpened;

            xPlayerControler.Seeking += xPlayerControler_Seeking;
            xPlayerControler.Seeked += xPlayerControler_Seeked;
            xPlayerControler.VolumeChanged += xPlayerControler_VolumeChanged;
            xPlayerControler.MediaElementStatusChanged += xPlayerControler_MediaElementStatusChanged;
            xPlayerControler.NextProgramSelected += xPlayerControler_NextProgramSelected;
            xPlayerControler.PreviousProgramSelected += xPlayerControler_PreviousProgramSelected;
            xPlayerControler.BitrateChanged += xPlayerControler_BitrateChanged;

            var volume = DataStoreUtils.GetLocalSettingValue(Utils.Constants.VolumeSettingKey);
            if (volume != null)
            {
                var value = (double)volume;
                xMediaPlayer.Volume = value;
                xPlayerControler.SetVolumeValue(value);
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Window.Current.CoreWindow.PointerMoved += CoreWindow_PointerMoved;
            rootGrid.PointerPressed += CoreWindows_PointerPressed;

            _playToManager.SourceRequested += playToManager_SourceRequested;
            _playToManager.SourceSelected += playToManager_SourceSelected;

            _playFactory = PlayAdapter.CreatePlayFactory(e.Parameter);
            _playFactory.PlayMessageAction += SetPlayMessage;
            _playFactory.PlayErrorAction += SetPlayError;
            _playFactory.HttpFailOrTimeoutAction += HttpFailOrTimeout;
            _playFactory.DacPlayInfo.programSource = (int)DACPageType;

            Play();
            DisplayActive();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.CoreWindow.PointerMoved -= CoreWindow_PointerMoved;
            rootGrid.PointerPressed -= CoreWindows_PointerPressed;

            _playToManager.SourceRequested -= playToManager_SourceRequested;
            _playToManager.SourceSelected -= playToManager_SourceSelected;

            _timerOfDAC.Cancel();
            _timer.Stop();
            _timerOfControler.Stop();

            ShowMouse();
            DisplayRelease();
            ShareUtils.ShareInfo = null;
        }

        protected override void GoBack(object sender, RoutedEventArgs e)
        {
            _playFactory.End();
            base.GoBack(sender, e);
        }

        private void Play()
        {
            SetTipInfo("正在获取影片播放信息");
            _playFactory.Play(xMediaPlayer);
        }

        public void ChangePlayState(MediaElementState mediaElementState)
        {
            switch (mediaElementState)
            {
                case MediaElementState.Paused:
                    xMediaPlayer.Pause();
                    if (xMediaPlayer.CurrentState == MediaElementState.Playing
                        || xMediaPlayer.CurrentState == MediaElementState.Buffering)
                        xPlayerControler.SetPlayerControlerBarStuas(MediaElementState.Paused);
                    break;
                case MediaElementState.Playing:
                    xMediaPlayer.Play();
                    xPlayerControler.SetPlayerControlerBarStuas(MediaElementState.Playing);
                    _timerOfControler.Start();
                    break;
                case MediaElementState.Stopped:
                    xMediaPlayer.Stop();
                    break;
                default:
                    break;
            }
        }

        #region PlayTo

        void playToManager_SourceRequested(PlayToManager sender, PlayToSourceRequestedEventArgs args)
        {
            var deferral = args.SourceRequest.GetDeferral();
            var handler = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                args.SourceRequest.SetSource(xMediaPlayer.PlayToSource);
                deferral.Complete();
            });
        }

        void playToManager_SourceSelected(PlayToManager sender, PlayToSourceSelectedEventArgs args)
        {
            var handler = _dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                SetControlerState(false, true);
            });
        }

        #endregion

        #region TouchEvents

        void CoreWindows_PointerPressed(object sender, PointerRoutedEventArgs args)
        {
            if (xPlayerControlGrid.Visibility == Visibility.Collapsed)
            {
                SetControlerState(true, true);
            }
            else if (xMediaPlayer.CurrentState == MediaElementState.Playing)
            {
                SetControlerState(false, false);
                HideMouse();
            }
        }

        void CoreWindow_PointerMoved(CoreWindow sender, PointerEventArgs args)
        {
            DelayHideControl();
        }

        protected override void CoreDispatcher_AcceleratorKeyActivated(CoreDispatcher sender, AcceleratorKeyEventArgs args)
        {
            if (args.EventType != CoreAcceleratorKeyEventType.KeyDown
                && args.EventType != CoreAcceleratorKeyEventType.SystemKeyDown) return;
            switch (args.VirtualKey)
            {
                case VirtualKey.Space:
                    if (xMediaPlayer.CurrentState == MediaElementState.Playing)
                        xPlayerControler_MediaElementStatusChanged(this, MediaElementState.Paused);
                    else if (xMediaPlayer.CurrentState == MediaElementState.Paused)
                        xPlayerControler_MediaElementStatusChanged(this, MediaElementState.Playing);
                    break;
                case VirtualKey.Left:
                    if (!_playFactory.SupportSeek) return;
                    var leftposition = xMediaPlayer.Position.Add(TimeSpan.FromSeconds(-5));
                    if (leftposition.TotalSeconds > 0)
                    {
                        _timer.Stop();
                        xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, leftposition, GetPlayDownloadPosition());
                        xMediaPlayer.Position = leftposition;
                        _timer.Start();
                    }
                    break;
                case VirtualKey.Right:
                    if (!_playFactory.SupportSeek) return;
                    var rigthposition = xMediaPlayer.Position.Add(TimeSpan.FromSeconds(5));
                    if (rigthposition.TotalSeconds > 0)
                    {
                        _timer.Stop();
                        xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, rigthposition, GetPlayDownloadPosition());
                        xMediaPlayer.Position = rigthposition;
                        _timer.Start();
                    }
                    break;
                case VirtualKey.Up:
                    if (xMediaPlayer.IsMuted)
                        xMediaPlayer.IsMuted = false;
                    if (xMediaPlayer.Volume <= 1)
                        xPlayerControler.SetVolumeValue(xMediaPlayer.Volume += .1);
                    break;
                case VirtualKey.Down:
                    if (xMediaPlayer.Volume > 0)
                        xPlayerControler.SetVolumeValue(xMediaPlayer.Volume -= .1);
                    else
                        xMediaPlayer.IsMuted = true;
                    break;
            }
            base.CoreDispatcher_AcceleratorKeyActivated(sender, args);
        }

        #endregion

        #region PlayControler

        void xPlayerControler_Seeking(object sender)
        {
            _timer.Stop();
        }

        void xPlayerControler_Seeked(object sender, double currentPosition)
        {
            if (_playFactory.SupportSeek)
            {
                _isDragBuffering = true;
                _playFactory.DacPlayInfo.dragCount++;
                _timer.Stop();
                xMediaPlayer.Position = TimeSpan.FromSeconds(currentPosition);
                _timer.Start();
            }
            DelayHideControl();
        }

        void xPlayerControler_VolumeChanged(object sender, double currentVaule)
        {
            xMediaPlayer.Volume = currentVaule;
            DelayHideControl();
        }

        void xPlayerControler_MediaElementStatusChanged(object sender, MediaElementState mediaElementState)
        {
            ChangePlayState(mediaElementState);
        }

        void xPlayerControler_PreviousProgramSelected(object sender)
        {
            xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, TimeSpan.FromSeconds(0), TimeSpan.FromSeconds(0));
            _playFactory.PreProgram();
        }

        void xPlayerControler_NextProgramSelected(object sender)
        {
            xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, TimeSpan.FromSeconds(0), TimeSpan.FromSeconds(0));
            _playFactory.NextProgram();
        }

        void xPlayerControler_BitrateChanged(object sender, PlayFileType fileType)
        {
            SetControlerState(false, true);
            _playFactory.BtirateSelect(fileType);
        }

        #endregion

        #region MediaElement

        TimeSpan GetPlayDownloadPosition()
        {
            if (!double.IsNaN(xMediaPlayer.DownloadProgress))
            {
                var position = xMediaPlayer.NaturalDuration.TimeSpan.TotalSeconds * xMediaPlayer.DownloadProgress;
                return TimeSpan.FromSeconds(position);
            }
            return xMediaPlayer.Position;
        }

        void xMediaPlayer_MediaOpened(object sender, RoutedEventArgs e)
        {
            SetControlerState(true, true);

            if (_playMessage.StartPosition > 0
                && _playMessage.StartPosition < xMediaPlayer.NaturalDuration.TimeSpan.TotalSeconds - 3)
            {
                xMediaPlayer.Position = TimeSpan.FromSeconds(_playMessage.StartPosition);
                xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, xMediaPlayer.Position, GetPlayDownloadPosition());
            }
        }

        void xMediaPlayer_MediaEnded(object sender, RoutedEventArgs e)
        {
            _playFactory.DacPlayInfo.stopReason = 0;
            if (_playMessage.HasNextProgram)
            {
                xPlayerControler_NextProgramSelected(sender);
            }
            else
            {
                TileUtils.CreateToastNotifications("播放已完成");
                GoBack(sender, new RoutedEventArgs());
            }
        }

        void xMediaPlayer_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            SetPlayError(e.ErrorMessage);
        }

        void xMediaPlayer_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            switch (xMediaPlayer.CurrentState)
            {
                case Windows.UI.Xaml.Media.MediaElementState.Buffering:
                    if (!_isBufferingOneTime)
                        _playFactory.DacPlayInfo.allBufferCount++;
                    _isBufferingOneTime = true;
                    _isBuffering = true;
                    SetTipInfo("影片缓冲中");
                    break;
                case Windows.UI.Xaml.Media.MediaElementState.Opening:
                    _ispreing = true;
                    _isBufferingOneTime = false;
                    SetTipInfo("正在打开影片");
                    break;
                case Windows.UI.Xaml.Media.MediaElementState.Playing:
                    _playFactory.DacPlayInfo.isPlaySucceeded = 1;
                    _ispreing = false;
                    _isBufferingOneTime = false;
                    _isDragBuffering = false;
                    _isBuffering = false;
                    _timer.Start();
                    ClearTipInfo();
                    break;
                case Windows.UI.Xaml.Media.MediaElementState.Paused:
                    _isBufferingOneTime = false;
                    SetControlerState(false, true);
                    break;
                case Windows.UI.Xaml.Media.MediaElementState.Stopped:
                    _playFactory.DacPlayInfo.stopReason = 1;
                    _isBufferingOneTime = false;
                    _isBuffering = false;
                    SetControlerState(false, true);
                    break;
                case Windows.UI.Xaml.Media.MediaElementState.Closed:
                    _isBufferingOneTime = false;
                    _isBuffering = false;
                    break;
                default:
                    break;
            }
        }

        void xMediaPlayer_BufferingProgressChanged(object sender, RoutedEventArgs e)
        {
            int offset = Convert.ToInt32(xMediaPlayer.BufferingProgress * 100);
            if (offset == 100
                || xMediaPlayer.CurrentState != MediaElementState.Buffering)
                ClearTipInfo();
            else
                SetTipInfo(string.Format("影片缓冲中 {0}%", offset));
        }

        #endregion

        private void DisplayActive()
        {
            try
            {
                _displayRequest.RequestActive();
            }
            catch { }
        }

        private void DisplayRelease()
        {
            try
            {
                _displayRequest.RequestRelease();
            }
            catch { }
        }

        private void SetPlayMessage(PlayMessageInfo playMessage)
        {
            _playMessage = playMessage;
            SetControlerState(false, true);
            ShareUtils.ShareInfo = _playMessage.ShareInfo;

            xChannelTitleTextBlock.Text = playMessage.Title;
            xPlayerControler.SetProgramButtonState(playMessage.HasPreProgram, playMessage.HasNextProgram);
            xPlayerControler.InitBitrates(playMessage.PlayInfos);
            if (playMessage.CurrentPlayInfo != null)
                xPlayerControler.InitCurrentFileType((PlayFileType)playMessage.CurrentPlayInfo.ft);
        }

        private void SetPlayError(string errorMessage)
        {
            Debug.WriteLine(errorMessage);
            _playFactory.DacPlayInfo.isPlaySucceeded = 0;
            _playFactory.DacPlayInfo.stopReason = 2;
            ClearTipInfo();
            TileUtils.CreateToastNotificationsText02("播放失败", errorMessage);
            GoBack(this, new RoutedEventArgs());
        }

        private void HttpFailOrTimeout()
        {
            _playFactory.DacPlayInfo.isPlaySucceeded = 0;
            ClearTipInfo();
            Utils.CommonUtils.ShowMessageConfirm(Constants.HttpTimeOutTipText, Play, "刷新");
        }

        private void SetControlerState(bool isDelayHidden, bool isVisible)
        {
            if (isVisible)
            {
                ShowMouse();
                xPlayerControlGrid.Visibility = Visibility.Visible;
            }
            else
            {
                xPlayerControlGrid.Visibility = Visibility.Collapsed;
            }

            if (isDelayHidden)
                _timerOfControler.Start();
            else
                _timerOfControler.Stop();
        }

        private void DelayHideControl()
        {
            ShowMouse();
            if (xMediaPlayer.CurrentState == MediaElementState.Playing)
                _timerOfControler.Start();
        }

        private void ShowMouse()
        {
            if (Window.Current.CoreWindow.PointerCursor == null)
                Window.Current.CoreWindow.PointerCursor = _coreCursor;
        }

        private void HideMouse()
        {
            Window.Current.CoreWindow.PointerCursor = null;
        }

        private void SetTipInfo(string tipInfo)
        {
            xInfoTextBlock.Text = tipInfo;
            xInfoGrid.Visibility = Visibility.Visible;
        }

        private void ClearTipInfo()
        {
            xInfoGrid.Visibility = Visibility.Collapsed;
        }

        private void LayoutAwarePage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (e.NewSize.Width == 320)
            {
                backButton.Margin = new Thickness(20, 0, 0, 0);
                xChannelTitleTextBlock.Margin = new Thickness(60, 0, 0, 0);
                xChannelTitleTextBlock.FontSize = 23;
                titleView.Width = 1366;
                titleView.Height = e.NewSize.Height;
            }
            else
            {
                backButton.Margin = new Thickness(50, 0, 0, 0);
                xChannelTitleTextBlock.Margin = new Thickness(100, 0, 0, 0);
                xChannelTitleTextBlock.FontSize = 28;
                titleView.Width = e.NewSize.Width;
                titleView.Height = e.NewSize.Height;
            }

            rootView.Height = (e.NewSize.Width * 900) / 1600;
            controlView.Width = e.NewSize.Width;
            controlView.Height = e.NewSize.Height;
        }

        private void titleView_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
        }

        private void _timer_Tick(object sender, object e)
        {
            if (_playFactory.SupportSeek)
                xPlayerControler.SetPlayerPostions(xMediaPlayer.NaturalDuration, xMediaPlayer.Position, GetPlayDownloadPosition());
        }

        private void _timerOfControler_Tick(object sender, object e)
        {
            xPlayerControler.SetBitratesGridShowOrNot(false);
            _timerOfControler.Stop();
            SetControlerState(false, false);
            HideMouse();
        }

        private void _timerOfDAC_Tick(ThreadPoolTimer timer)
        {
            if (_ispreing)
                _playFactory.DacPlayInfo.prepareTime++;

            if (_isBuffering)
                _playFactory.DacPlayInfo.bufferTime++;

            if (_isDragBuffering)
                _playFactory.DacPlayInfo.dragBufferTime++;

            if (_isBufferingOneTime)
            {
                _bufferingOneTime++;
                if (_bufferingOneTime >= 3)
                {
                    _playFactory.DacPlayInfo.playBufferCount++;
                }
            }
            else
            {
                _bufferingOneTime = 0;
            }
        }
    }
}