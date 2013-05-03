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

using PPTVData.Entity;
using PPTVData.Factory;
using PPTVData;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
namespace PPTVForWP7.Controls
{
    public partial class CoversBar : UserControl
    {
        public delegate void DownloadedEventHandler(object sender);
        public event DownloadedEventHandler CoverImagesDownloadCompleted;

        public delegate void ImageTapEventHandler(object sender,CoverInfo coverInfo);
        public event ImageTapEventHandler ItemTap;

        CoverFactory _coverFactory;
        List<BitmapImage> _bitmapImages;
        List<CoverInfo> _coverInfos;
        DispatcherTimer _timer;
        int _count;
        int _markCount;
        int _currentSelectedIndex = 0;

        double[] originalOffsets;
        double[] currentOffsets;
        double[] markPosition = new double[2];//标志位
        int _startIndex = 0;
        double _imageWidth = 405.0;
        double _imageHeight = 182.0;
        double _bottomBlockHeight = 32.0;
        Dictionary<HttpWebClient, int> _webClients;

        public CoversBar()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            _coverFactory = new CoverFactory();
            _coverFactory.HttpFailHandler += _coverFactory_GetCoverInfosFailed;
            _coverFactory.HttpSucessHandler += _coverFactory_GetCoverInfosSuccess;
            _coverFactory.HttpTimeOutHandler += _coverFactory_GetCoverInfosTimeout;
            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromMilliseconds(5000);
            _timer.Tick += new EventHandler(_timer_Tick);
            _bitmapImages = new List<BitmapImage>();
            _coverInfos = new List<CoverInfo>();
        }

        private void _timer_Tick(object sender, EventArgs e)
        {
            Rigth_Move();  
        }

        private void LeftImage_Tap(object sender, GestureEventArgs e)
        {
            _currentSelectedIndex--;
            if (_currentSelectedIndex < 0)
            {
                _currentSelectedIndex = 0;
                for (int i = 0; i < currentOffsets.Length; i++)
                {
                    currentOffsets[i] = currentOffsets[i] - ((_bitmapImages.Count - 1) * _imageWidth);
                }
                _currentSelectedIndex = _bitmapImages.Count - 1;
            }
            else
            {
                for (int i = 0; i < currentOffsets.Length; i++)
                {
                    currentOffsets[i] = currentOffsets[i] + _imageWidth;
                }
            }

            BeginMove();
        }

        private void RightImage_Tap(object sender, GestureEventArgs e)
        {
            Rigth_Move();
        }

        private void Rigth_Move()
        {
            _currentSelectedIndex++;
            if (_currentSelectedIndex >= _bitmapImages.Count)
            {
                _currentSelectedIndex = _bitmapImages.Count - 1;
                for (int i = 0; i < currentOffsets.Length; i++)
                {
                    currentOffsets[i] = currentOffsets[i] + ((_bitmapImages.Count - 1) * _imageWidth);
                }
                _currentSelectedIndex = 0;
            }
            else
            {
                for (int i = 0; i < currentOffsets.Length; i++)
                {
                    currentOffsets[i] = currentOffsets[i] - _imageWidth;
                }
            }
            BeginMove();
        }

        private void DownloadImage(List<CoverInfo> coverInfos)
        {
            _bitmapImages.Clear();
            _webClients = new Dictionary<HttpWebClient, int>();
            for (int i = 0; i < coverInfos.Count; i++)
            {
                _bitmapImages.Add(new BitmapImage());
            }
            _count = coverInfos.Count;
            _markCount = 0;
            _coverInfos = coverInfos;
            for (int m = 0; m < coverInfos.Count; m++)
            {
                HttpWebClient _clientByImage = new HttpWebClient();//图片异步下载
                _clientByImage.OpenReadCompleted += new OpenReadCompletedEventHandler(_clientByImage_OpenReadCompleted);
                _clientByImage.OpenReadAsync(coverInfos[m].CoverImgUrl);
                _webClients.Add(_clientByImage, m);
            }
        }

        public void InitElement()
        {
            Calculate();

            var left = LayoutRoot.Children[0];
            var right = LayoutRoot.Children[1];
            LayoutRoot.Children.Clear();
            LayoutRoot.Children.Add(left);
            LayoutRoot.Children.Add(right);

            Grid xCoversGrid = new Grid();
            xCoversGrid.Name = "xCoversGrid";
            xCoversGrid.Background = new SolidColorBrush(Colors.Gray);
            xCoversGrid.Width = _imageWidth;
            xCoversGrid.Height = _imageHeight;
            LayoutRoot.Children.Add(xCoversGrid);

            Grid xBlocksGrid = new Grid();
            xBlocksGrid.Height = _bottomBlockHeight;
            xBlocksGrid.VerticalAlignment = VerticalAlignment.Bottom;
            xBlocksGrid.Name = "xBlocksGrid";
            LayoutRoot.Children.Add(xBlocksGrid);

            for (int i = 0; i < _bitmapImages.Count; i++)
            {

                Image image = new Image();
                image.Source = _bitmapImages[i];
                image.Stretch = Stretch.Fill;
                image.Name = "xImage" + i;
                image.Tag = i;
                TransformGroup transformGroup = new TransformGroup();
                transformGroup.Children.Add(new TranslateTransform() { X = currentOffsets[i + _startIndex] });
                transformGroup.Children.Add(new ScaleTransform());
                transformGroup.Children.Add(new SkewTransform());
                transformGroup.Children.Add(new RotateTransform());
                image.RenderTransform = transformGroup;
                //image.ManipulationStarted += new EventHandler<ManipulationStartedEventArgs>(image_ManipulationStarted);
                //image.ManipulationDelta += new EventHandler<ManipulationDeltaEventArgs>(image_ManipulationDelta);
                //image.ManipulationCompleted += new EventHandler<ManipulationCompletedEventArgs>(image_ManipulationCompleted);
                image.Tap += new EventHandler<GestureEventArgs>(image_Tap);
                xCoversGrid.Children.Add(image);


                Grid grid = new Grid();

                Rectangle rectangle = new Rectangle();
                rectangle.Width = _imageWidth;
                rectangle.Fill = new SolidColorBrush(Utils.Utils.GetColor("#B2000000"));
                grid.Children.Add(rectangle);

                StackPanel stackPanel = new StackPanel();
                stackPanel.VerticalAlignment = VerticalAlignment.Bottom;
                stackPanel.HorizontalAlignment = HorizontalAlignment.Center;
                stackPanel.Orientation = Orientation.Horizontal;
                for (int j = 0; j < _bitmapImages.Count; j++)
                {
                    Ellipse ellipse = new Ellipse();
                    ellipse.Width = 8.0;
                    ellipse.Height = 8.0;
                    ellipse.Margin = new Thickness(5, 0, 0, 0);
                    if (i == j)
                    {
                        ellipse.Fill = new SolidColorBrush(Colors.White);
                    }
                    else
                    {
                        ellipse.Fill = new SolidColorBrush(Colors.Gray);
                    }
                    ellipse.Name = "xEllipse" + i + "_" + j;

                    stackPanel.Children.Add(ellipse);
                }
                if (i != 0)
                    grid.Visibility = Visibility.Collapsed;
                TextBlock textBlock = new TextBlock();
                textBlock.Text = _coverInfos[i].Title;
                textBlock.Foreground = new SolidColorBrush(Colors.White);
                textBlock.HorizontalAlignment = HorizontalAlignment.Center;
                textBlock.VerticalAlignment = VerticalAlignment.Top;
                textBlock.Margin = new Thickness(0, -3, 0, 0);
                grid.Children.Add(stackPanel);
                grid.Children.Add(textBlock);
                xBlocksGrid.Children.Add(grid);
            }


            TextBlock xLogTextBlock = new TextBlock();
            xLogTextBlock.Visibility = Visibility.Collapsed;//Debug
            xLogTextBlock.Height = 30.0;
            xLogTextBlock.HorizontalAlignment = HorizontalAlignment.Left;
            xLogTextBlock.VerticalAlignment = VerticalAlignment.Top;
            xLogTextBlock.Name = "xLogTextBlock";
            xLogTextBlock.Foreground = new SolidColorBrush(Colors.Black);
            LayoutRoot.Children.Add(xLogTextBlock);
            _currentSelectedIndex = 0;
            _timer.Start();
        }

        void image_Tap(object sender, GestureEventArgs e)
        {
            //
            if (ItemTap != null)
                ItemTap(sender, _coverInfos[Convert.ToInt32((sender as Image).Tag)]);
        }

        private void BeginMove()
        {
            foreach (var item in GetGridByName("xCoversGrid").Children)
            {
                Image image = item as Image;
                int index = (int)image.Tag;
                Moving(image, originalOffsets[index], currentOffsets[index], false);
            }

            Grid grid = GetGridByName("xBlocksGrid");
            for (int i = 0; i < grid.Children.Count; i++)
            {
                if (i == _currentSelectedIndex)
                    grid.Children[i].Visibility = Visibility.Visible;
                else
                    grid.Children[i].Visibility = Visibility.Collapsed;
            }
            _timer.Start();
        }

        private void Moving(Image image, double originalOffset, double currentOffset, bool hasOriginalOffset = true)
        {
            Utils.Utils.GenerateStoryboardByTwoHPoint(image, originalOffset, currentOffset, TimeSpan.FromMilliseconds(300), hasOriginalOffset).Begin();
        }

        #region 触摸手势

        //void image_ManipulationStarted(object sender, ManipulationStartedEventArgs e)
        //{
        //    _timer.Stop();
        //    Image image = sender as Image;
        //    int index = Convert.ToInt32(image.Tag);
        //    Grid xCoversGrid = GetGridByName("xCoversGrid");
        //    _currentSelectedImage = xCoversGrid.Children[index] as Image;
        //    e.Handled = true;
        //}

        //void image_ManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        //{
        //    Image senderImage = sender as Image;
        //    TranslateTransform translateTransform1 = GetTranslateTransform(senderImage);
        //    (LayoutRoot.FindName("xLogTextBlock") as TextBlock).Text = senderImage.Name + ":" + translateTransform1.X;
        //    foreach (var item in (LayoutRoot.FindName("xCoversGrid") as Grid).Children)
        //    {
        //        if (item is Image)
        //        {
        //            Image image = item as Image;
        //            TranslateTransform translateTransform = GetTranslateTransform(image);
        //            if ((_currentSelectedIndex == _bitmapImages.Count - 1 && e.DeltaManipulation.Translation.X < 0) || (_currentSelectedIndex == 0 && e.DeltaManipulation.Translation.X > 0))
        //            {

        //            }
        //            else
        //                translateTransform.X += e.DeltaManipulation.Translation.X;
        //        }
        //    }
        //    e.Handled = true;
        //}

        //void image_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        //{
        //    Image senderImage = sender as Image;
        //    TranslateTransform translateTransform1 = GetTranslateTransform(senderImage);
        //    double offset = translateTransform1.X + _imageWidth;
        //    if (translateTransform1.X <= markPosition[0] || e.FinalVelocities.LinearVelocity.X < -200)
        //    {
        //        //左
        //        _currentSelectedIndex++;
        //        if (_currentSelectedIndex >= _bitmapImages.Count)
        //        {
        //            _currentSelectedIndex = _bitmapImages.Count - 1;
        //            e.Handled = true;
        //            _timer.Start();
        //            return;
        //        }

        //        for (int i = 0; i < currentOffsets.Length; i++)
        //        {
        //            currentOffsets[i] = currentOffsets[i] - _imageWidth;
        //        }

        //        //currentOffsets
        //    }
        //    else if (translateTransform1.X > markPosition[1] || e.FinalVelocities.LinearVelocity.X > 200)
        //    {
        //        //右
        //        _currentSelectedIndex--;
        //        if (_currentSelectedIndex < 0)
        //        {
        //            _currentSelectedIndex = 0;
        //            e.Handled = true;
        //            _timer.Start();
        //            return;
        //        }
        //        for (int i = 0; i < currentOffsets.Length; i++)
        //        {
        //            currentOffsets[i] = currentOffsets[i] + _imageWidth;
        //        }
        //    }
        //    else
        //    {

        //    }
        //    BeginMove();
        //    e.Handled = true;
        //}

        #endregion

        private TranslateTransform GetTranslateTransform(Image image)
        {
            TransformGroup transformGroup = (image.RenderTransform as TransformGroup);
            return transformGroup.Children[0] as TranslateTransform;
        }

        private Grid GetGridByName(string name)
        {
            return (LayoutRoot.FindName(name) as Grid);
        }

        private void Calculate()
        {
            int count = _bitmapImages.Count;// * 2 - 1;
            //offsets = new double[count];
            originalOffsets = new double[count];
            currentOffsets = new double[count];
            //_startIndex = (count - 1) / 2;
            for (int i = 0; i < currentOffsets.Length; i++)
            {
                if (i < _startIndex)
                {
                    currentOffsets[i] = originalOffsets[i] = (i - _startIndex) * _imageWidth;
                }
                else
                {
                    currentOffsets[i] = originalOffsets[i] = (i - _startIndex) * _imageWidth;
                }
            }

            markPosition[0] = -(_imageWidth / 2.0);
            markPosition[1] = _imageWidth / 2.0;
        }

        #region 数据处理函数
        void _clientByImage_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        {
            
            if (e.Error == null && !e.Cancelled)
            {
                try
                {
                    BitmapImage image = new BitmapImage();
                    image.SetSource(e.Result);
                    _bitmapImages[_webClients[sender as HttpWebClient]] = image;
                    _markCount++;
                    if (_markCount == _count)
                    {
                        //下载完成
                        if (CoverImagesDownloadCompleted != null)
                            CoverImagesDownloadCompleted(sender);
                        InitElement();
                    }
                }
                catch { }
            }
        }

        void _coverFactory_GetCoverInfosTimeout(object sender, HttpFactoryArgs<List<CoverInfo>> args)
        {
        }

        void _coverFactory_GetCoverInfosSuccess(object sender, HttpFactoryArgs<List<CoverInfo>> args)
        {
            DownloadImage(args.Result);
        }

        void _coverFactory_GetCoverInfosFailed(object sender, HttpFactoryArgs<List<CoverInfo>> args)
        {
        }

        #endregion

        public void BeginDownload(int navid)
        {
            //开始下载图片
            _coverFactory.DownLoadDatas(navid);
        }
    }
}
