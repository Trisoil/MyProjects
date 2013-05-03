using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Windows.UI;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Media.Imaging;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVForWin8.Pages;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary.Utils;

    public sealed partial class SlidesShowControl : UserControl
    {
        CoverNavFactory _coverNavFactory;
        CoverListFactory _coverChannelFactory;

        int _currentSelectedIndex = 0;
        double _slideOriginalOffset;
        double _slideCurrentOffset;
        double _imageWidth = 768.0;
        double _imageHeight = 380.0;
        double[] _originalOffsets;
        double[] _currentOffsets;

        DispatcherTimer _timer;
        List<CoverInfo> _covers;
        Action _httpFailorTimeOut;

        public SlidesShowControl()
        {
            this.InitializeComponent();

            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromMilliseconds(6000);
            _timer.Tick += _timer_Tick;

            _coverNavFactory = new CoverNavFactory();
            _coverNavFactory.HttpSucessHandler += coverNav_Successed;
        }

        public void Init(Action httpFailorTimeOut)
        {
            _httpFailorTimeOut = httpFailorTimeOut;
            if (_httpFailorTimeOut != null)
                _coverNavFactory.HttpFailorTimeOut = _httpFailorTimeOut;
        }

        public void RequestDatas()
        {
            _coverNavFactory.DownLoadDatas();
        }

        public void StartSlidesShow()
        {
            var eleCount = slidesShowGrid.Children.Count;
            if (eleCount > 0
                && eleCount == _covers.Count)
                _timer.Start();
        }

        public void EndSlidesShow()
        {
            _timer.Stop();
        }

        public void Cancel()
        {
            _coverNavFactory.Cancel();
        }

        #region HttpHandler

        void coverNav_Successed(object sender, HttpFactoryArgs<List<RecommandNav>> args)
        {
            _coverChannelFactory = new CoverListFactory();
            _coverChannelFactory.HttpSucessHandler += coverFactory_GetCoverInfosSuccess;
            if (_httpFailorTimeOut != null)
                _coverChannelFactory.HttpFailorTimeOut = _httpFailorTimeOut;

            if (args.Result.Count > 0)
            {
                var random = new Random();
                var index = random.Next(0, args.Result.Count - 1);
                _coverChannelFactory.DownLoadDatas(args.Result[index].Navid);
            }
        }

        void coverFactory_GetCoverInfosSuccess(object sender, HttpFactoryArgs<List<CoverInfo>> args)
        {
            var random = new Random();
            if (args.Result.Count > 0)
            {
                for (var i = 0; i < args.Result.Count && i < 4; i++)
                    TileUtils.CreateLocalNotifications(args.Result[i]);

                _covers = args.Result;
                Calculate(_covers.Count);
                InitElements();
            }
        }

        #endregion

        void InitElements()
        {
            slidesShowGrid.Children.Clear();

            for (var i = 0; i < _covers.Count; i++)
            {
                var grid = new Grid();

                var transformGroup = new TransformGroup();
                transformGroup.Children.Add(new TranslateTransform() { X = _currentOffsets[i] });
                transformGroup.Children.Add(new ScaleTransform());
                transformGroup.Children.Add(new SkewTransform());
                transformGroup.Children.Add(new RotateTransform());
                grid.RenderTransform = transformGroup;

                grid.Tag = i;
                grid.Width = _imageWidth;
                grid.Height = _imageHeight;
                grid.Background = new SolidColorBrush(Colors.Transparent);
                grid.HorizontalAlignment = HorizontalAlignment.Left;
                grid.VerticalAlignment = VerticalAlignment.Top;
                grid.Tapped += grid_Tapped;

                var image = new Image();
                image.HorizontalAlignment = HorizontalAlignment.Left;
                image.Height = _imageHeight;
                image.VerticalAlignment = VerticalAlignment.Top;
                image.Width = _imageWidth;
                image.Source = new BitmapImage(new Uri(_covers[i].ImageUri));
                image.Stretch = Stretch.Fill;
                grid.Children.Add(image);

                slidesShowGrid.Children.Add(grid);
            }
            channelTitleBlock.Text = _covers[0].Title;
            _currentSelectedIndex = 0;
            _timer.Start();
        }

        void grid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            if (_covers != null && _covers.Count > _currentSelectedIndex)
                CommonUtils.DACNavagate(CommonUtils.GetCurrentFrame(), 
                    DACPageType.COVER, typeof(DetailPage), _covers[_currentSelectedIndex].Vid);
        }

        void Calculate(int count)
        {
            slideGrid.Width = _imageWidth / (double)count;
            var transformGroup = slideGrid.RenderTransform as TransformGroup;
            var translateTransform = transformGroup.Children[0] as TranslateTransform;
            translateTransform.X = 0;

            _slideOriginalOffset = 0;
            _slideCurrentOffset = 0;

            _originalOffsets = new double[count];
            _currentOffsets = new double[count];

            for (int i = 0; i < _currentOffsets.Length; i++)
            {
                _currentOffsets[i] = _originalOffsets[i] = i * _imageWidth;
            }
        }

        void _timer_Tick(object sender, object e)
        {
            rightMove();
        }

        void rightMove()
        {
            if (_currentOffsets == null) return;
            _currentSelectedIndex++;
            var count = _currentOffsets.Length;
            if (_currentSelectedIndex >= count)
            {
                _currentSelectedIndex = count - 1;
                for (int i = 0; i < count; i++)
                {
                    _currentOffsets[i] = _currentOffsets[i] + ((count - 1) * _imageWidth);
                    _currentSelectedIndex = 0;
                }
                _slideCurrentOffset = 0;
            }
            else
            {
                for (int i = 0; i < _currentOffsets.Length; i++)
                {
                    _currentOffsets[i] = _currentOffsets[i] - _imageWidth;
                }
                _slideCurrentOffset = _slideOriginalOffset + slideGrid.Width;
            }
            BeginMove();
        }

        private void leftButton_Click(object sender, RoutedEventArgs e)
        {
            if (_currentOffsets == null) return;

            _timer.Stop();
            _currentSelectedIndex--;
            var count = _currentOffsets.Length;
            if (_currentSelectedIndex < 0)
            {
                _currentSelectedIndex = 0;
                for (int i = 0; i < count; i++)
                {
                    _currentOffsets[i] = _currentOffsets[i] - ((count - 1) * _imageWidth);
                }
                _currentSelectedIndex = count - 1;
                _slideCurrentOffset = _imageWidth - slideGrid.Width;
            }
            else
            {
                for (int i = 0; i < count; i++)
                {
                    _currentOffsets[i] = _currentOffsets[i] + _imageWidth;
                }
                _slideCurrentOffset = _slideOriginalOffset - slideGrid.Width;
            }

            BeginMove();
            _timer.Start();
        }

        private void rightButton_Click(object sender, RoutedEventArgs e)
        {
            _timer.Stop();
            rightMove();
            _timer.Start();
        }

        private void BeginMove()
        {
            foreach (var item in slidesShowGrid.Children)
            {
                Grid image = item as Grid;
                int index = (int)image.Tag;
                Moving(image, _originalOffsets[index], _currentOffsets[index], false);
            }
            Moving(slideGrid, _slideOriginalOffset, _slideCurrentOffset, false);
            _slideOriginalOffset = _slideCurrentOffset;
            channelTitleBlock.Text = _covers[_currentSelectedIndex].Title;
        }

        private void Moving(Grid image, double originalOffset, double currentOffset, bool hasOriginalOffset = true)
        {
            CommonUtils.GenerateStoryboardByTwoHPoint(image, originalOffset, currentOffset, TimeSpan.FromMilliseconds(1000), hasOriginalOffset).Begin();
        }
    }
}
