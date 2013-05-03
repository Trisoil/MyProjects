using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Windows.Threading;
using System.Windows.Media;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData.Entity;

    using PPTV.WPRT.CommonLibrary.DataModel.Play;
    using PPTV.WPRT.CommonLibrary.ViewModel.Play;

    public partial class PlayPage : PhoneApplicationPage
    {
        IPlayFactory _playFactory;

        DispatcherTimer _timer;
        PlayMessageInfo _playMessage;

        public PlayPage()
        {
            InitializeComponent();

            _timer = new DispatcherTimer();
            _timer.Tick += _timer_Tick;
            _timer.Interval = TimeSpan.FromSeconds(1);

            mediaPlayer.MediaOpened += mediaPlayer_MediaOpened;
            mediaPlayer.MediaFailed += mediaPlayer_MediaFailed;
            mediaPlayer.MediaEnded += mediaPlayer_MediaEnded;
            mediaPlayer.CurrentStateChanged += mediaPlayer_CurrentStateChanged;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _playFactory = PlayAdapter.CreatePlayFactory(NavigationContext);
            _playFactory.PlayMessageAction += SetPlayMessage;

            Play();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {

        }

        #region MediaPlayer

        void mediaPlayer_MediaOpened(object sender, RoutedEventArgs e)
        {
            
        }

        void mediaPlayer_MediaEnded(object sender, RoutedEventArgs e)
        {
            
        }

        void mediaPlayer_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            
        }

        void mediaPlayer_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            switch (mediaPlayer.CurrentState)
            { 
                case MediaElementState.Buffering:
                    SetTipInfo("影片缓冲中");
                    break;
                case MediaElementState.Opening:
                    SetTipInfo("正在打开影片");
                    break;
                case MediaElementState.Playing:
                    _timer.Start();
                    ClearTipInfo();
                    break;
            }
        }

        #endregion

        #region PlayFactory

        void SetPlayMessage(PlayMessageInfo playMessage)
        {
            _playMessage = playMessage;

            titleText.Text = playMessage.Title;
        }

        #endregion

        void Play()
        {
            SetTipInfo("正在获取影片播放信息");
            _playFactory.Play(mediaPlayer);
        }

        void _timer_Tick(object sender, EventArgs e)
        {
            if (_playFactory.SupportSeek)
                playControl.SetPlayerPostions(mediaPlayer.NaturalDuration, mediaPlayer.Position, mediaPlayer.DownloadProgress);
        }

        void SetTipInfo(string tipInfo)
        {
            WaitText.Text = tipInfo;
            tipInfoPanel.Visibility = Visibility.Visible;
        }

        void ClearTipInfo()
        {
            tipInfoPanel.Visibility = Visibility.Collapsed;
        }

    }
}