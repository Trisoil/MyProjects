using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData;
    using PPTVData.Utils;
    using PPTVData.Entity;
    using PPTVForWin8.Pages;

    using PPTV.WinRT.CommonLibrary.Utils;

    public sealed partial class PlayerController : UserControl
    {
        static SolidColorBrush _selectedBitRateBrush = new SolidColorBrush(Utils.CommonUtils.GetColor("#FF2C99F3"));
        static SolidColorBrush _unSelectBitRateBrush = new SolidColorBrush(Colors.White);

        bool _isDraggingOfVolume = false;
        bool _isBitrateVisible = false;
        bool _isDragging = false;
        bool _isEnable = false;

        double _thumbVolumeWidth = 25;
        double _currentVolumePosition = 330;
        double _playVolumePosition = 0;
        double _orientalThumbVolumePosition = 0;
        double current = 1.0;
        double _currentPlayPosition = 0.0;
        double _thumbWidth = 47;
        double _totalWidth = 1008;//总长
        double _orientalThumbPosition = 0.0;

        Duration _naturalDuration;//电影播放总时长
        Point _orientalVolumePosition;
        Point _orientalPosition;
        TimeSpan _downloadPosition;
        TimeSpan _playPosition;
        
        #region 事件

        public delegate void BitrateChangedEventHandler(object sender, PlayFileType fileType);
        public event BitrateChangedEventHandler BitrateChanged;

        public delegate void SeekingEventHandler(object sender);
        public event SeekingEventHandler Seeking;

        public delegate void SeekedEventHandler(object sender, double currentPosition);
        public event SeekedEventHandler Seeked;

        public delegate void VolumeChangedEventHandler(object sender, double currentVaule);
        public event VolumeChangedEventHandler VolumeChanged;

        public delegate void NextProgramSelectedEventHandler(object sender);
        public event NextProgramSelectedEventHandler NextProgramSelected;

        public delegate void PreviousProgramSelectedEventHandler(object sender);
        public event PreviousProgramSelectedEventHandler PreviousProgramSelected;

        public delegate void MediaElementStatusChangedEventHandler(object sender, MediaElementState mediaElementState);
        public event MediaElementStatusChangedEventHandler MediaElementStatusChanged;

        #endregion

        public PlayerController()
        {
            this.InitializeComponent();
            Loaded += new RoutedEventHandler(PlayerControler_Loaded);
        }

        void PlayerControler_Loaded(object sender, RoutedEventArgs e)
        {
            _isEnable = true;
        }

        public PlayFileType CurrentFileType
        {
            get
            {
                return xSelectedBitrateTextBlock.Tag == null ? PlayFileType.HD : (PlayFileType)xSelectedBitrateTextBlock.Tag;
            }
        }

        public void InitBitrates(Dictionary<int, PlayInfo> infos)
        {
            if (infos == null) return;
            xBitrateCanvas.Visibility = Visibility.Visible;

            if (infos.ContainsKey((int)PlayFileType.Smooth))
            {
                xSmoothGrid.Visibility = Visibility.Visible;
                xSmoothGrid.Tag = PlayFileType.Smooth;
            }
            if (infos.ContainsKey((int)PlayFileType.HD))
            {
                xHighGrid.Visibility = Visibility.Visible;
                xHighGrid.Tag = PlayFileType.HD;
            }
            if (infos.ContainsKey((int)PlayFileType.SuperHD))
            {
                xSuperGrid.Visibility = Visibility.Visible;
                xSuperGrid.Tag = PlayFileType.SuperHD;
            }
        }

        public void InitCurrentFileType(PlayFileType fileType)
        {
            xSelectedBitrateTextBlock.Tag = fileType;
            xSelectedBitrateTextBlock.Text = fileType.CreateString();
            switch (fileType)
            {
                case PlayFileType.Smooth:
                    xSmoothTextBlock.Foreground = _selectedBitRateBrush;
                    xSuperTextBlock.Foreground = xHighTextBlock.Foreground = _unSelectBitRateBrush;
                    break;
                case PlayFileType.HD:
                    xHighTextBlock.Foreground = _selectedBitRateBrush;
                    xSuperTextBlock.Foreground = xSmoothTextBlock.Foreground = _unSelectBitRateBrush;
                    break;
                case PlayFileType.SuperHD:
                    xSuperTextBlock.Foreground = _selectedBitRateBrush;
                    xHighTextBlock.Foreground = xSmoothTextBlock.Foreground = _unSelectBitRateBrush;
                    break;
                default:
                    break;
            }
        }

        public void SetProgramButtonState(bool isPrevious, bool isNext)
        {
            xPreviousButton.IsEnabled = isPrevious;
            xNextButton.IsEnabled = isNext;
        }

        public void SetPlayerPostions(Duration naturalDuration, TimeSpan playPosition, TimeSpan downloadPosition)
        {
            _naturalDuration = naturalDuration;
            _downloadPosition = downloadPosition;
            _playPosition = playPosition;

            if (!_isDragging)
            {
                SetSliderPosition(_playPosition.TotalSeconds * ((_totalWidth - _thumbWidth) / naturalDuration.TimeSpan.TotalSeconds));
            }
        }

        public void SetPlayerControlerBarStuas(MediaElementState mediaElementState)
        {
            switch (mediaElementState)
            {
                case MediaElementState.Buffering:
                case MediaElementState.Opening:
                    _isEnable = true;
                    break;
                case MediaElementState.Closed:
                case MediaElementState.Stopped:
                    _isEnable = false;
                    break;
                case MediaElementState.Paused:
                    _isEnable = true;
                    xPauseButton.Visibility = Visibility.Collapsed;
                    xPlayButton.Visibility = Visibility.Visible;
                    break;
                case MediaElementState.Playing:
                    _isEnable = true;
                    xPauseButton.Visibility = Visibility.Visible;
                    xPlayButton.Visibility = Visibility.Collapsed;
                    break;
                default:
                    break;
            }
        }

        public void SetBitratesGridShowOrNot(bool isVis)
        {
            if (isVis && !_isBitrateVisible)
            {
                sbBitratesClose.Stop();
                sbBitratesOpen.Begin();
                _isBitrateVisible = true;
            }
            else if (!isVis && _isBitrateVisible)
            {
                sbBitratesOpen.Stop();
                sbBitratesClose.Begin();
                _isBitrateVisible = false;
            }
        }

        private void SetCurrentThumbPosition(double currentThumbPosition)
        {
            if (_naturalDuration == null) return;

            if (currentThumbPosition < 0)
                currentThumbPosition = 0;
            if (currentThumbPosition > xTotalRectangle.ActualWidth - _thumbWidth)
                currentThumbPosition = xTotalRectangle.ActualWidth - _thumbWidth;

            _currentPlayPosition = currentThumbPosition;
            _playPosition = TimeSpan.FromSeconds(_currentPlayPosition * _naturalDuration.TimeSpan.TotalSeconds / (_totalWidth - _thumbWidth));

            SetSliderPosition(_currentPlayPosition);
        }

        private void SetSliderPosition(double currentThumbPosition)
        {
            if (_naturalDuration == null) return;

            ((xThumbRectangle.RenderTransform as TransformGroup).Children[0] as TranslateTransform).X = currentThumbPosition;
            xPlayingRectangle.Width = currentThumbPosition + 10 > 0 ? currentThumbPosition + 10 : 0;
            xBufferRectangle.Width = _downloadPosition.TotalSeconds * ((_totalWidth - 3) / _naturalDuration.TimeSpan.TotalSeconds);
            xTimeTipTextBlock.Text = string.Format("{0}/{1}", _playPosition.ToString(@"hh\:mm\:ss"), _naturalDuration.TimeSpan.ToString(@"hh\:mm\:ss"));
        }

        private void xPauseButton_Click(object sender, RoutedEventArgs e)
        {
            if (MediaElementStatusChanged != null)
                MediaElementStatusChanged(sender, MediaElementState.Paused);
        }

        private void xPlayButton_Click(object sender, RoutedEventArgs e)
        {
            if (MediaElementStatusChanged != null)
                MediaElementStatusChanged(sender, MediaElementState.Playing);
        }

        private void xPreviousButton_Click(object sender, RoutedEventArgs e)
        {
            if (PreviousProgramSelected != null)
                PreviousProgramSelected(sender);
        }

        private void xNextButton_Click(object sender, RoutedEventArgs e)
        {
            if (NextProgramSelected != null)
                NextProgramSelected(sender);
        }

        private void xTotalRectangle_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (!_isEnable || _naturalDuration == null || !_naturalDuration.HasTimeSpan) return;

            var position = e.GetCurrentPoint(xSliderGrid).Position.X - _thumbWidth / 2;
            SetCurrentThumbPosition(position);
            
            if (Seeked != null) Seeked(this, _playPosition.TotalSeconds);
            _isDragging = false;
        }

        private void xThumbRectangle_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (!_isEnable) return;
            if (Seeking != null) Seeking(sender);
            xThumbRectangle.CapturePointer(e.Pointer);
            _orientalPosition = e.GetCurrentPoint(xSliderGrid).Position;
            _orientalThumbPosition = e.GetCurrentPoint(xThumbRectangle).Position.X;
            _isDragging = true;
        }

        private void xThumbRectangle_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (!_isEnable || _naturalDuration == null || !_naturalDuration.HasTimeSpan)return;
            if (_isDragging)
            {
                var positon = e.GetCurrentPoint(xSliderGrid).Position.X - _orientalThumbPosition;
                SetCurrentThumbPosition(positon);
            }
        }

        private void xThumbRectangle_PointerReleased(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (_isDragging)
            {
                if (Seeked != null) Seeked(this, _playPosition.TotalSeconds);
                _isDragging = false;
                xThumbRectangle.ReleasePointerCaptures();
            }
        }

        private void bitrateGrid_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
            SetBitratesGridShowOrNot(!_isBitrateVisible);
        }

        private void bitrateChoose_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
            SetBitratesGridShowOrNot(false);
            var fileType = (PlayFileType)(sender as FrameworkElement).Tag;

            var currentFileType = (PlayFileType)xSelectedBitrateTextBlock.Tag;
            if (currentFileType == fileType) return;

            InitCurrentFileType(fileType);

            if (BitrateChanged != null)
                BitrateChanged(sender, fileType);
        }

        private void LayoutRoot_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
        }

        #region Volume

        private void xVolumeTotalCanvas_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            _currentVolumePosition = e.GetCurrentPoint(xVolumeSliderGrid).Position.X - _thumbVolumeWidth / 2;
            SetVolumeSliderPosition();
            double value = (e.GetCurrentPoint(xVolumeSliderGrid).Position.X - _orientalThumbVolumePosition) / (350 - _thumbVolumeWidth);
            if (VolumeChanged != null)
                VolumeChanged(sender, value);
            SetVolumeState(value);
        }

        private void xThumbVolumeRectangle_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (_isDraggingOfVolume)
            {
                _currentVolumePosition = e.GetCurrentPoint(xVolumeSliderGrid).Position.X - _orientalThumbVolumePosition;
                _playVolumePosition = _currentVolumePosition / (350 - _thumbWidth);
                if (_playVolumePosition > 1)
                    _playVolumePosition = 1.0;
                if (_playVolumePosition < 0)
                    _playVolumePosition = 0.0;
                SetVolumeSliderPosition();
                if (VolumeChanged != null)
                    VolumeChanged(sender, _playVolumePosition);
                SetVolumeState(_playVolumePosition);
            }
        }

        private void xThumbVolumeRectangle_PointerPressed(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            xThumbVolumeRectangle.CapturePointer(e.Pointer);
            _orientalVolumePosition = e.GetCurrentPoint(xVolumeSliderGrid).Position;
            _orientalThumbVolumePosition = e.GetCurrentPoint(xThumbVolumeRectangle).Position.X;
            _isDraggingOfVolume = true;
        }

        private void xThumbVolumeRectangle_PointerReleased(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            e.Handled = true;
            if (_isDraggingOfVolume)
            {
                _isDraggingOfVolume = false;
                double value = (e.GetCurrentPoint(xVolumeSliderGrid).Position.X - _orientalThumbVolumePosition) / (350 - _thumbVolumeWidth);
                //if (VolumeChanged != null)
                //    VolumeChanged(sender, value);
                SetVolumeState(value);
            }
            xThumbVolumeRectangle.ReleasePointerCaptures();
        }

        void SetVolumeSliderPosition()
        {
            if (_currentVolumePosition < 0)
                _currentVolumePosition = 0;
            if (_currentVolumePosition > xTotalVolumeRectangle.ActualWidth - _thumbVolumeWidth + 5)
                _currentVolumePosition = xTotalVolumeRectangle.ActualWidth - _thumbVolumeWidth + 5;
            ((xThumbVolumeRectangle.RenderTransform as TransformGroup).Children[0] as TranslateTransform).X = _currentVolumePosition;
            xPlayingVolumeRectangle.Width = _currentVolumePosition + 10 > 0 ? _currentVolumePosition + 10 : 0;
        }
        
        public void SetVolumeValue(double value)
        {
            _currentVolumePosition = value * (350 - _thumbWidth);
            SetVolumeSliderPosition();
            SetVolumeState(value);
        }

        private void SetVolumeState(double value)
        {
            xMaxVolumeButton.Visibility = xMinVolumeButton.Visibility = xMuteVolumeButton.Visibility = xNormalVolumeButton.Visibility = Visibility.Collapsed;
            if (value > 0.66)
                xMaxVolumeButton.Visibility = Visibility.Visible;
            else if (value <= 0.66 && value > 0.33)
                xNormalVolumeButton.Visibility = Visibility.Visible;
            else if (value <= 0.33 && value > 0.005)
                xMinVolumeButton.Visibility = Visibility.Visible;
            else if (value <= 0.005)
                xMuteVolumeButton.Visibility = Visibility.Visible;

            DataStoreUtils.AddLocalSettingValue(Utils.Constants.VolumeSettingKey, value);
        }

        private void volumeMute_Click(object sender, RoutedEventArgs e)
        {
            current = _currentVolumePosition / (350 - _thumbWidth);
            if (VolumeChanged != null)
                VolumeChanged(sender, 0.0);
            SetVolumeValue(0.0);
        }

        private void PreviousVolume_Click(object sender, RoutedEventArgs e)
        {
            if (current == 0.0) current = 0.1;
            if (VolumeChanged != null)
                VolumeChanged(sender, current);
            SetVolumeValue(current);
            current = 0.0;
        }

        #endregion
    }
}
