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
using PPTVData.Log;
using PPTVData.Entity;
namespace PPTVForWP7.Controls
{
    public partial class PlayerControlBar : UserControl
    {
        private double _thumbWidth = 35;//thumb长
        private double _totalWidth = 770;//总长

        private Point _orientalPosition;
        private double _orientalThumbPosition = 0.0;
        private bool _isDragging = false;

        Duration _naturalDuration = new Duration();//电影播放总时长
        TimeSpan _downloadPosition;
        TimeSpan _playPosition;
        double _currentPlayPosition = 0.0;
        private bool _isEnable = false;
        public bool IsEnable
        {
            get { return _isEnable; }
            set
            {
                _isEnable = value;
            }
        }

        private ChannelDetailInfo _channelDetailInfo = null;
        public ChannelDetailInfo ChannelDetailInfo
        {
            set
            {
                _channelDetailInfo = value;
                if (_channelDetailInfo == null)
                    return;
                SetProgramButtonStatus();
                xChannelTitleTextBlock.Text = _channelDetailInfo.Title;
                if (ProgramIndex != -1)
                    xProgramNameTextBlock.Text = _channelDetailInfo.ProgramInfos[ProgramIndex].Title;
                else
                    xProgramNameTextBlock.Text = "";
            }
            get { return _channelDetailInfo; }
        }
        public string localFileTitle
        {
            set
            {
                xChannelTitleTextBlock.Text = value;
                xNextButton.IsEnabled = false;
                xPreviousButton.IsEnabled = false;
                xShareButton.IsEnabled = false;
            }
        }
        private int _programIndex = -1;
        public int ProgramIndex
        {
            get { return _programIndex; }
            set
            {
                _programIndex = value;
                SetProgramButtonStatus();

                if (_channelDetailInfo == null)
                    return;
                if (ProgramIndex != -1)
                    xProgramNameTextBlock.Text = _channelDetailInfo.ProgramInfos[ProgramIndex].Title;
                else
                    xProgramNameTextBlock.Text = "";
            }
        }

        private void SetProgramButtonStatus()
        {
            if (_programIndex != -1)//有剧集
            {
                if (_programIndex > 0)
                    xPreviousButton.IsEnabled = true;
                else
                    xPreviousButton.IsEnabled = false;
                if (ChannelDetailInfo != null)
                {
                    if (_programIndex < ChannelDetailInfo.ProgramInfos.Count - 1)
                        xNextButton.IsEnabled = true;
                    else
                        xNextButton.IsEnabled = false;
                }
            }
            else
            {
                xPreviousButton.IsEnabled = xNextButton.IsEnabled = false;
            }
        }

        #region 事件

        public delegate void SeekingEventHandler(object sender);
        public event SeekingEventHandler Seeking;

        public delegate void SeekedEventHandler(object sender, double currentPosition);
        public event SeekedEventHandler Seeked;

        public delegate void VolumeSeekedEventHandler(object sender);
        public event VolumeSeekedEventHandler VolumeSeeked;

        public delegate void VolumeChangedEventHandler(object sender, double currentVaule);
        public event VolumeChangedEventHandler VolumeChanged;

        public delegate void NextProgramSelectedEventHandler(object sender);
        public event NextProgramSelectedEventHandler NextProgramSelected;

        public delegate void PreviousProgramSelectedEventHandler(object sender);
        public event PreviousProgramSelectedEventHandler PreviousProgramSelected;

        public delegate void FastSelectedEventHandler(object sender);
        public event FastSelectedEventHandler FastSelected;

        public delegate void RewindSelectedEventHandler(object sender);
        public event RewindSelectedEventHandler RewindSelected;

        public delegate void MediaElementStatusChangedEventHandler(object sender, MediaElementState mediaElementState);
        public event MediaElementStatusChangedEventHandler MediaElementStatusChanged;

        //节目分享事件
        public Action<object> ShareSocialEvent;

        #endregion

        public PlayerControlBar()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(PlayerControlBar_Loaded);
        }

        void PlayerControlBar_Loaded(object sender, RoutedEventArgs e)
        {
            //读取音量
            xVolumeSlider.Value = Utils.Utils.GetVolume();
            if (VolumeChanged != null)
                VolumeChanged(this, xVolumeSlider.Value);
            if (xVolumeSlider.Value == xVolumeSlider.Minimum)
            {
                xVolumeNormalButton.Visibility = Visibility.Collapsed;
                xVolumeMuteButton.Visibility = Visibility.Visible;
            }
            else
            {
                xVolumeNormalButton.Visibility = Visibility.Visible;
                xVolumeMuteButton.Visibility = Visibility.Collapsed;
            }
        }

        private void xThumbRectangle_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (!IsEnable)
                return;
            if (Seeking != null)
                Seeking(sender);
            xThumbRectangle.CaptureMouse();
            _orientalPosition = e.GetPosition(LayoutRoot);
            _orientalThumbPosition = e.GetPosition(xThumbRectangle).X;
            _isDragging = true;
        }

        private void xThumbRectangle_MouseMove(object sender, MouseEventArgs e)
        {
            if (!IsEnable)
                return;
            if (_isDragging)
            {
                if (!_naturalDuration.HasTimeSpan)
                    return;
                _currentPlayPosition = e.GetPosition(LayoutRoot).X - _orientalThumbPosition;
                _playPosition = TimeSpan.FromSeconds(_currentPlayPosition * _naturalDuration.TimeSpan.TotalSeconds / (_totalWidth - _thumbWidth));
                SetSliderPosition(_currentPlayPosition);
                if (_playPosition.TotalSeconds <= _naturalDuration.TimeSpan.TotalSeconds)
                    xLogTextBlock.Text = _playPosition.ToString(@"hh\:mm\:ss") + " / " + _naturalDuration.TimeSpan.ToString(@"hh\:mm\:ss");
                //LogManager.Ins.Log("_playPosition.TotalSeconds:" + _playPosition.TotalSeconds);
            }
        }

        private void xThumbRectangle_MouseLeave(object sender, MouseEventArgs e)
        {
            if (Seeked != null)
            {
                Seeked(this, _playPosition.TotalSeconds);
                //LogManager.Ins.Log("拖动已释放:" + _playPosition.TotalSeconds);
            }
            _isDragging = false;
            xThumbRectangle.ReleaseMouseCapture();
        }

        public void SetPlayerPostions(Duration naturalDuration, TimeSpan playPosition, TimeSpan downloadPosition)
        {
            _naturalDuration = naturalDuration;
            _downloadPosition = downloadPosition;
            if (!_naturalDuration.HasTimeSpan)
                return;
            _playPosition = playPosition;
            if (!_isDragging)
            {
                xLogTextBlock.Text = "" + _playPosition.ToString(@"hh\:mm\:ss") + " / " + naturalDuration.TimeSpan.ToString(@"hh\:mm\:ss");
                SetSliderPosition(_playPosition.TotalSeconds * ((_totalWidth - _thumbWidth) / naturalDuration.TimeSpan.TotalSeconds));
            }
        }

        void SetSliderPosition(double currentThumbPosition)
        {
            if (currentThumbPosition < 0)
                currentThumbPosition = 0;
            if (currentThumbPosition > xTotalRectangle.ActualWidth - _thumbWidth)
                currentThumbPosition = xTotalRectangle.ActualWidth - _thumbWidth;
            ((xThumbRectangle.RenderTransform as TransformGroup).Children[0] as TranslateTransform).X = currentThumbPosition;
            xPlayingRectangle.Width = currentThumbPosition + 10 > 0 ? currentThumbPosition + 10 : 0;
            xBufferRectangle.Width = _downloadPosition.TotalSeconds * ((_totalWidth - 3) / _naturalDuration.TimeSpan.TotalSeconds);
        }

        private void xTotalRectangle_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (!IsEnable)
                return;
            if (!_naturalDuration.HasTimeSpan)
                return;
            _currentPlayPosition = e.GetPosition(xSliderGrid).X;
            _playPosition = TimeSpan.FromSeconds(_currentPlayPosition * _naturalDuration.TimeSpan.TotalSeconds / (_totalWidth - _thumbWidth));
            SetSliderPosition(_currentPlayPosition);
            //LogManager.Ins.Log("_playPosition.TotalSeconds:" + _playPosition.TotalSeconds);
            if (Seeked != null)
                Seeked(this, _playPosition.TotalSeconds);
        }

        public void SetPlayerControlBarStuas(MediaElementState mediaElementState)
        {
            switch (mediaElementState)
            {
                case MediaElementState.Individualizing:
                case MediaElementState.AcquiringLicense:
                    break;
                case MediaElementState.Buffering:
                    IsEnable = true;
                    break;
                case MediaElementState.Closed:
                    IsEnable = false;
                    break;
                case MediaElementState.Opening:
                    IsEnable = true;
                    break;
                case MediaElementState.Paused:
                    IsEnable = true;
                    xPauseButton.Visibility = Visibility.Collapsed;
                    xPlayButton.Visibility = Visibility.Visible;
                    break;
                case MediaElementState.Playing:
                    IsEnable = true;
                    xPauseButton.Visibility = Visibility.Visible;
                    xPlayButton.Visibility = Visibility.Collapsed;
                    break;
                case MediaElementState.Stopped:
                    IsEnable = false;
                    break;
                default:
                    break;
            }

        }

        private void xPreviousButton_Tap(object sender, GestureEventArgs e)
        {
            if (PreviousProgramSelected != null)
                PreviousProgramSelected(sender);
        }

        private void xRewindButton_Tap(object sender, GestureEventArgs e)
        {
            if (RewindSelected != null)
                RewindSelected(sender);
        }

        private void xPlayButton_Tap(object sender, GestureEventArgs e)
        {
            SetPlayerControlBarStuas(MediaElementState.Playing);
            if (MediaElementStatusChanged != null)
                MediaElementStatusChanged(this, MediaElementState.Playing);
        }

        private void xPauseButton_Tap(object sender, GestureEventArgs e)
        {
            SetPlayerControlBarStuas(MediaElementState.Paused);
            if (MediaElementStatusChanged != null)
                MediaElementStatusChanged(this, MediaElementState.Paused);
        }

        private void xFastButton_Tap(object sender, GestureEventArgs e)
        {
            if (FastSelected != null)
                FastSelected(sender);
        }

        private void xNextButton_Tap(object sender, GestureEventArgs e)
        {
            if (NextProgramSelected != null)
                NextProgramSelected(sender);
        }

        private void xShareButton_Tap(object sender, GestureEventArgs e)
        {
            if (ShareSocialEvent != null)
                ShareSocialEvent(sender);
        }

        private void xVolumeSlider_ValueChanged(object sender, MouseEventArgs e)
        {
            _volumeSliding = true;
            if (VolumeChanged != null)
                VolumeChanged(this, xVolumeSlider.Value);
            if (xVolumeSlider.Value == xVolumeSlider.Minimum)
            {
                xVolumeNormalButton.Visibility = Visibility.Collapsed;
                xVolumeMuteButton.Visibility = Visibility.Visible;
            }
            else
            {
                xVolumeNormalButton.Visibility = Visibility.Visible;
                xVolumeMuteButton.Visibility = Visibility.Collapsed;
            }
        }
        bool _volumeSliding = false;
        private void xVolumeSlider_MouseLeave(object sender, MouseEventArgs e)
        {
            if (VolumeSeeked != null)
                VolumeSeeked(sender);
            _volumeSliding = false;
        }

        private void xVolumeSlider_Tap(object sender, GestureEventArgs e)
        {
            if (Seeking != null)
                Seeking(sender);
        }
        double volumeValueBeforeMute = 0.85;

        private void xVolumeMuteButton_MouseLeave(object sender, MouseEventArgs e)
        {
            xVolumeMuteButton.Visibility = Visibility.Collapsed;
            xVolumeNormalButton.Visibility = Visibility.Visible;
            xVolumeSlider.Value = volumeValueBeforeMute;
            if (VolumeChanged != null)
                VolumeChanged(this, xVolumeSlider.Value);
        }

        private void xVolumeNormalButton_MouseLeave(object sender, MouseEventArgs e)
        {
            if (_volumeSliding)
                return;
            volumeValueBeforeMute = xVolumeSlider.Value;
            xVolumeSlider.Value = 0.0;
            xVolumeMuteButton.Visibility = Visibility.Visible;
            xVolumeNormalButton.Visibility = Visibility.Collapsed;
            if (VolumeChanged != null)
                VolumeChanged(this, xVolumeSlider.Value);

        }
    }
}
