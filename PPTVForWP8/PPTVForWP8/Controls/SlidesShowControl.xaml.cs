using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Windows.Threading;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Controls
{
    using PPTVData.Entity;
    using PPTVData.Factory;

    public partial class SlidesShowControl : UserControl
    {
        CoverNavFactory _coverNavFactory;
        CoverListFactory _coverChannelFactory;

        int _preSelectedIndex = 0;
        int _currentSelectedIndex = 0;
        double _imageWidth = 450.0;
        double _imageHeight = 202.5;
        double[] _originalOffsets;
        double[] _currentOffsets;

        DispatcherTimer _timer;
        List<CoverInfo> _covers;
        Action _httpFailorTimeOut;

        public SlidesShowControl()
        {
            InitializeComponent();

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
                _covers = args.Result;
                Calculate(_covers.Count);
                InitElements();
            }
        }

        #endregion

        void InitElements()
        {
            slidesShowGrid.Children.Clear();
            channelPreviewStack.Children.Clear();

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
                grid.Tap += grid_Tap;

                var image = new Image();
                image.HorizontalAlignment = HorizontalAlignment.Left;
                image.Height = _imageHeight;
                image.VerticalAlignment = VerticalAlignment.Top;
                image.Width = _imageWidth;
                image.Source = new BitmapImage(new Uri(_covers[i].ImageUri));
                image.Stretch = Stretch.Fill;
                grid.Children.Add(image);

                slidesShowGrid.Children.Add(grid);

                var rectangle = new Rectangle();
                rectangle.Margin = new Thickness(5, 0, 0, 0);
                if (i == 0)
                {
                    rectangle.Width = 7.5;
                    rectangle.Height = 7.5;
                    rectangle.Fill = new SolidColorBrush(Utils.CommonUtils.GetColor("#FF00A8FF"));
                }
                else
                {
                    rectangle.Width = 6.25;
                    rectangle.Height = 6.25;
                    rectangle.Fill = new SolidColorBrush(Utils.CommonUtils.GetColor("#4CFFFFFF"));
                }
                channelPreviewStack.Children.Add(rectangle);
            }
            channelTitleBlock.Text = _covers[0].Title;
            _preSelectedIndex = 0;
            _currentSelectedIndex = 0;
            _timer.Start();
        }

        void grid_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_covers != null && _covers.Count > _currentSelectedIndex)
            {
                var vid = _covers[_currentSelectedIndex].Vid;
                Utils.CommonUtils.DACNavagate(DACPageType.COVER, string.Format("/Pages/DetailPage.xaml?id={0}", vid));
            }
        }

        void Calculate(int count)
        {
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
            _preSelectedIndex = _currentSelectedIndex;
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
            }
            else
            {
                for (int i = 0; i < _currentOffsets.Length; i++)
                {
                    _currentOffsets[i] = _currentOffsets[i] - _imageWidth;
                }
            }
            BeginMove();
        }

        private void BeginMove()
        {
            foreach (var item in slidesShowGrid.Children)
            {
                Grid image = item as Grid;
                int index = (int)image.Tag;
                Moving(image, _originalOffsets[index], _currentOffsets[index], false);
            }
            channelTitleBlock.Text = _covers[_currentSelectedIndex].Title;

            var preRectangle = channelPreviewStack.Children[_preSelectedIndex] as Rectangle;
            if (preRectangle != null)
            {
                preRectangle.Width = 6.25;
                preRectangle.Height = 6.25;
                preRectangle.Fill = new SolidColorBrush(Utils.CommonUtils.GetColor("#4CFFFFFF"));
            }

            var curRectangle = channelPreviewStack.Children[_currentSelectedIndex] as Rectangle;
            if (curRectangle != null)
            {
                curRectangle.Width = 7.5;
                curRectangle.Height = 7.5;
                curRectangle.Fill = new SolidColorBrush(Utils.CommonUtils.GetColor("#FF00A8FF"));
            }
        }

        private void Moving(Grid image, double originalOffset, double currentOffset, bool hasOriginalOffset = true)
        {
            Utils.CommonUtils.GenerateStoryboardByTwoHPoint(image, originalOffset, currentOffset, TimeSpan.FromMilliseconds(1000), hasOriginalOffset).Begin();
        }
    }
}
