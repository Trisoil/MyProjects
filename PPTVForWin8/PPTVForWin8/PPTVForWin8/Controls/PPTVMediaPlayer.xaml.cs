using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Media.PlayTo;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Entity;
    using PPTVData.Factory.Player;

    public sealed partial class PPTVMediaPlayer : UserControl
    {
        public EventHandler<RoutedEventArgs> MediaOpened;
        public EventHandler<RoutedEventArgs> MediaEnded;
        public EventHandler<ExceptionRoutedEventArgs> MediaFailed;
        public EventHandler<RoutedEventArgs> CurrentStateChanged;
        public EventHandler<RoutedEventArgs> BufferingProgressChanged;

        private PlaySegmentManage _playManage;
        private MediaElement _mediaPlaying;
        private MediaElement _mediaDowning;
        private Duration _natural_duration = Duration.Forever;

        private double _seekPosition;

        public PPTVMediaPlayer()
        {
            this.InitializeComponent();

            _mediaPlaying = xMedia1;
            _mediaDowning = xMedia2;

            _playManage = new PlaySegmentManage();
            _playManage.Play += Play;
        }

        #region Public

        public object Source
        {
            set
            {
                var remote = value as PlayInfo;
                if (remote != null)
                {
                    _playManage.SetSource(remote);
                }
                else
                {
                    var local = value as Uri;
                    if (local != null)
                        _mediaPlaying.Source = local;
                }
            }
        }

        public void Play()
        {
            _mediaPlaying.Play();
        }

        public void Pause()
        {
            if (_mediaPlaying.CurrentState == MediaElementState.Playing
                || _mediaPlaying.CurrentState == MediaElementState.Buffering)
                _mediaPlaying.Pause();
        }

        public void Stop()
        {

        }

        public MediaElementState CurrentState
        {
            get { return _mediaPlaying.CurrentState; }
        }

        public TimeSpan Position
        {
            get { return TimeSpan.FromSeconds(_playManage.SegmentStartSeconds + _mediaPlaying.Position.TotalSeconds); }
            set
            {
                var pos = _playManage.Seek(value);
                if (pos != 0)
                    _mediaPlaying.Position = TimeSpan.FromSeconds(pos);
            }
        }

        public Duration NaturalDuration
        {
            get
            {
                if (_natural_duration == Duration.Forever)
                    return new Duration(TimeSpan.FromSeconds(_playManage.TotalSeconds));
                return _natural_duration;
            }
        }

        public double BufferingProgress
        {
            get { return _mediaPlaying.BufferingProgress; }
        }

        public double DownloadProgress
        {
            get
            {
                return (_mediaPlaying.DownloadProgress * _playManage.SegmentTotalSeconds) / _playManage.TotalSeconds;
            }
        }

        public double Volume
        {
            get { return _mediaPlaying.Volume; }
            set { _mediaPlaying.Volume = value; _mediaDowning.Volume = value; }
        }

        public PlayToSource PlayToSource
        {
            get { return _mediaPlaying.PlayToSource; }
        }

        public AudioCategory AudioCategory
        {
            set { _mediaPlaying.AudioCategory = value; }
            get { return _mediaPlaying.AudioCategory; }
        }

        #endregion

        private void Play(string playUri, double position)
        {
            if (_mediaPlaying.CurrentState == MediaElementState.Playing
                || _mediaPlaying.CurrentState == MediaElementState.Buffering)
            {
                _mediaPlaying.Pause();
                ChangePlayer();
            }
            _mediaPlaying.Source = new Uri(playUri);
            if (position > 0)
                _seekPosition = position;
        }

        private void ChangePlayer()
        {
            var tmp = _mediaPlaying;
            _mediaPlaying = _mediaDowning;
            _mediaDowning = tmp;
        }

        private void xMedia_MediaOpened(object sender, RoutedEventArgs e)
        {
            if (sender == _mediaPlaying)
            {
                if (_seekPosition > 0)
                {
                    _mediaPlaying.Position = TimeSpan.FromSeconds(_seekPosition);
                }
                else if (_playManage.CanDownload)
                {
                    _mediaDowning.Source = new Uri(_playManage.DownloadUri);
                }

                if (_playManage.Segment == 0
                    && MediaOpened != null)
                {
                    MediaOpened(sender, e);
                }
            }
            else
            {
                _mediaDowning.Pause();
            }
        }

        private void xMedia_MediaEnded(object sender, RoutedEventArgs e)
        {
            if (sender == _mediaPlaying)
            {
                ChangePlayer();

                _mediaDowning.Source = null;
                _mediaPlaying.Play();
                _mediaPlaying.Visibility = Visibility.Visible;
                _mediaDowning.Visibility = Visibility.Collapsed;
                
                _playManage.NextSegment();

                if (_playManage.CanDownload)
                {
                    _mediaDowning.Source = new Uri(_playManage.DownloadUri);
                }
                else
                {
                    if (MediaEnded != null)
                        MediaEnded(sender, e);
                }
            }
        }

        private void xMedia_MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            if (sender == _mediaPlaying)
            {
                if (MediaFailed != null)
                    MediaFailed(sender, e);
            }
        }

        private void xMedia_BufferingProgressChanged(object sender, RoutedEventArgs e)
        {
            if (sender == _mediaPlaying)
            {
                if (BufferingProgressChanged != null)
                    BufferingProgressChanged(sender, e);
            }
        }

        private void xMedia_DownloadProgressChanged(object sender, RoutedEventArgs e)
        {

        }

        private void xMedia_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            if (sender == _mediaPlaying)
            {
                if (_mediaPlaying.CurrentState == MediaElementState.Playing)
                {
                    if (_seekPosition > 0)
                        _seekPosition = 0;
                    else if (_mediaPlaying.Visibility == Visibility.Collapsed)
                    {
                        _mediaPlaying.Visibility = Visibility.Visible;
                        _mediaDowning.Visibility = Visibility.Collapsed;

                        if (_playManage.CanDownload)
                            _mediaDowning.Source = new Uri(_playManage.DownloadUri);
                    }
                }
                if (CurrentStateChanged != null)
                    CurrentStateChanged(sender, e);
            }
        }
    }
}
