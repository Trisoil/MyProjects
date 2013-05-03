using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Windows.Media;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Controls
{
    public partial class PlayerController : UserControl
    {
        bool _isDragging;
        double _totalWidth;
        double _thumbWidth;
        double _downloadProgress;

        Duration _totalDuration;
        TimeSpan _playPosition;

        public PlayerController()
        {
            InitializeComponent();

            this.Loaded += PlayerController_Loaded;
        }

        void PlayerController_Loaded(object sender, RoutedEventArgs e)
        {
            _totalWidth = totalGrid.ActualWidth;
            _thumbWidth = thumbRectangle.ActualWidth;
        }

        public void SetPlayerPostions(Duration naturalDuration, TimeSpan playPosition, double downloadProgress)
        {
            if (_totalDuration == null) return;

            _playPosition = playPosition;
            _totalDuration = naturalDuration;
            _downloadProgress = downloadProgress;

            if (!_isDragging)
            {
                SetSliderPosition(_playPosition.TotalSeconds * ((_totalWidth - _thumbWidth) / naturalDuration.TimeSpan.TotalSeconds));
            }
        }

        private void SetSliderPosition(double currentThumbPosition)
        {
            ((thumbRectangle.RenderTransform as TransformGroup).Children[0] as TranslateTransform).X = currentThumbPosition;
            playingGrid.Width = currentThumbPosition + 10 > 0 ? currentThumbPosition + 10 : 0;
            bufferGrid.Width = _downloadProgress * _totalWidth;
            timeTipText.Text = string.Format("{0}/{1}", _playPosition.ToString(@"hh\:mm\:ss"), _totalDuration.TimeSpan.ToString(@"hh\:mm\:ss"));
        }
    }
}
