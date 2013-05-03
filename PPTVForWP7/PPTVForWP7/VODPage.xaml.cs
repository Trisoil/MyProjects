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
using Microsoft.Phone.Controls;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Windows.Threading;
using System.Diagnostics;
using Microsoft.Phone.Info;
using System.Collections;

using PPTVForWP7.Controls;
using PPTVData.Log;
using PPTVData.Factory;
using PPTVData.Entity;
using PPTVForWP7.Utils;

namespace PPTVForWP7
{
    public partial class VODPage : PhoneApplicationPage
    {
        int _channel_count = 0;//总电影个数
        int _page_count = 0;//总页数
        private GetDataStatusHelp _getDataStatusHelp = new GetDataStatusHelp();
        public string CurrentOrder { get; set; }
        private int _channelItemCount = 60;
        private DispatcherTimer _timerOfDebug = new DispatcherTimer();
        private VodChannelFactory _channelFactory = new VodChannelFactory();
        ObservableCollection<ChannelInfo> _channelInfos = new ObservableCollection<ChannelInfo>();//下载所有的影片
        public ObservableCollection<ChannelInfo> ChannelInfos
        {
            get { return _channelInfos; }
            set
            {
                _channelInfos = value;
            }
        }
        ObservableCollection<ObservableCollection<ChannelInfo>> _allOneRow3ChannelInfos = new ObservableCollection<ObservableCollection<ChannelInfo>>();//处理过后的数据
        public ObservableCollection<ObservableCollection<ChannelInfo>> AllOneRow3ChannelInfos
        {
            get { return _allOneRow3ChannelInfos; }
            set
            {
                _allOneRow3ChannelInfos = value;
            }
        }
        List<TagInfo> _currentSelectedTagInfos = new List<TagInfo> { new TagInfo(1, "catalog", "全部", -1), new TagInfo(1, "year", "全部", -1), new TagInfo(1, "area", "全部", -1) };//当前选中的Tag
        private bool _isLoadingData = false;
        private bool _isLongItem = false;
        private int _typeID = -1;
        private int _programSource = -1;
        public int TypeID
        {
            get { return _typeID; }
            set
            {
                _typeID = value;
                xTagsBar.TypeID = value;
                BeginGetChannles(true);
            }
        }

        private int _pageIndex = 1;

        public VODPage()
        {
            DataContext = this;
            InitializeComponent();
            InitVariableNEventHandle();
            Loaded += new RoutedEventHandler(VODPage_Loaded);
        }

        void VODPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (Utils.Utils.IsDebug)
            {
                xDebugTextBlock.Visibility = Visibility.Visible;
                xLogTextBlock.Visibility = Visibility.Visible;
                _timerOfDebug.Start();
            }
            else
            {
                xDebugTextBlock.Visibility = Visibility.Collapsed;
                xLogTextBlock.Visibility = Visibility.Collapsed;
            }
            LogManager.Ins.Log("VODPage_Loaded,设置xTagsBar.CurrentSelectedTagInfos");
        }

        void BeginGetChannles(bool isReStart)
        {
            if (isReStart)//重新加载
            {
                xTypeTitleTextBlock.Text = Enum.GetName(typeof(ChannelTypes), TypeID);
                AllOneRow3ChannelInfos.Clear();
                ChannelInfos.Clear();
                if (TypeID == (int)ChannelTypes.VIP尊享)
                {
                    xTypeTitleTextBlock.Foreground = App.Current.Resources["Orange"] as SolidColorBrush;
                }
                if (TypeID == 5 || TypeID == 6 || TypeID == 7)
                {
                    //	体育 新闻 游戏
                    xChannelsListBox.ItemTemplate = Resources["LongItemTemplate"] as DataTemplate;
                    xChannelsListBox.ItemsSource = ChannelInfos;
                    _isLongItem = true;
                }
                else
                {
                    xChannelsListBox.ItemTemplate = Resources["SmallItemTemplate"] as DataTemplate;
                    xChannelsListBox.ItemsSource = AllOneRow3ChannelInfos;

                }
                _pageIndex = 1;
            }
            else//翻页
            {
                _pageIndex++;
            }

            int leftCount = _channel_count - (_isLongItem ? ChannelInfos.Count : 3 * AllOneRow3ChannelInfos.Count);
            if (leftCount > 0 || isReStart)
            {
                xLoadChannelsGrid.Visibility = Visibility.Visible;
                xLoadingTextBlock.Text = (isReStart ? CommonUtils.LoadingTips : "剩余" + leftCount + "部片子");
                _isLoadingData = true;
                xLogTextBlock.Text = "加载新的数据 _pageIndex:" + _pageIndex + " 电影个数:" + (_isLongItem == true ? ChannelInfos.Count.ToString() : AllOneRow3ChannelInfos.Count.ToString());
                _channelFactory.DownLoadDatas(TypeID, _currentSelectedTagInfos, _pageIndex, _channelItemCount, CurrentOrder);
            }
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (!(e.NavigationMode == System.Windows.Navigation.NavigationMode.Back))
            {
                LogManager.Ins.Log("OnNavigatedTo");
                Reset();
                TypeID = Convert.ToInt32(NavigationContext.QueryString["typeid"]);
                _programSource = Convert.ToInt32(NavigationContext.QueryString["programSource"]);
            }
        }

        private void Reset()
        {
            _currentSelectedTagInfos = new List<TagInfo> { new TagInfo(1, "catalog", "全部", -1), new TagInfo(1, "year", "全部", -1), new TagInfo(1, "area", "全部", -1) };
            xAreaButton.Visibility = xYearButton.Visibility = xCatalogButton.Visibility = Visibility.Collapsed;
        }

        protected override void OnBackKeyPress(CancelEventArgs e)
        {
            if (gridtools.Visibility == System.Windows.Visibility.Visible)
            {
                SetTagsBarStatus(false);
                SetOrderBarStatus(false);
                e.Cancel = true;
                return;
            }
            xLoadChannelsGrid.Margin = new Thickness(0, 0, 0, 30);
            xLoadChannelsGrid.Visibility = Visibility.Visible;
            xLoadingTextBlock.Text = CommonUtils.LoadingTips;
            AllOneRow3ChannelInfos.Clear();
            ChannelInfos.Clear();
            base.OnBackKeyPress(e);
        }

        private void InitVariableNEventHandle()
        {
            _timerOfDebug.Interval = TimeSpan.FromSeconds(1);
            _timerOfDebug.Tick += new EventHandler(_timerOfDebug_Tick);
            xTagsBar.TagChanged += new Controls.TagsBar.TagSelectEventHandler(xTagsBar_TagChanged);
            orderBar.OrderChanged += BeginLoad;
            _channelFactory.HttpFailHandler += _channelFactory_GetChannelsByListFailed;
            _channelFactory.HttpSucessHandler += _channelFactory_GetChannelsByListSucceed;
            _channelFactory.HttpTimeOutHandler += _channelFactory_GetChannelsByListTimeout;
            _getDataStatusHelp.DataDownloadFailure += new GetDataStatusHelp.DownloadFailEventHandler(_getDataStatusHelp_DataDownloadFailure);
        }

        void _getDataStatusHelp_DataDownloadFailure(object sender)
        {
            MessageBox.Show(Utils.Utils.NetExceptInfo);
        }

        void _timerOfDebug_Tick(object sender, EventArgs e)
        {
            xDebugTextBlock.Text = "总内存:" + (long)DeviceExtendedProperties.GetValue("DeviceTotalMemory") / 1024 / 1024 + "当前使用:" + (long)DeviceExtendedProperties.GetValue("ApplicationCurrentMemoryUsage") / 1024 / 1024;
        }

        void xTagsBar_TagChanged(object sender, List<TagInfo> tagInfos)
        {
            xLogTextBlock.Text = "改变了 Tag信息";
            _currentSelectedTagInfos = tagInfos;
            for (int i = 0; i < tagInfos.Count; i++)
            {
                if (i == 0)
                {
                    if (tagInfos[i].TagName == "全部" || string.IsNullOrEmpty(tagInfos[i].TagName))
                        xCatalogButton.Visibility = Visibility.Collapsed;
                    else
                    {
                        xCatalogButton.Visibility = Visibility.Visible;
                        xCatalogButton.Content = tagInfos[i].TagName + "    ";
                    }
                }
                else if (i == 1)
                {
                    if (tagInfos[i].TagName == "全部" || string.IsNullOrEmpty(tagInfos[i].TagName))
                        xAreaButton.Visibility = Visibility.Collapsed;
                    else
                    {
                        xAreaButton.Visibility = Visibility.Visible;
                        xAreaButton.Content = tagInfos[i].TagName + "    ";
                    }
                }
                else if (i == 2)
                {
                    if (tagInfos[i].TagName == "全部" || string.IsNullOrEmpty(tagInfos[i].TagName))
                        xYearButton.Visibility = Visibility.Collapsed;
                    else
                    {
                        xYearButton.Visibility = Visibility.Visible;
                        xYearButton.Content = tagInfos[i].TagName + "    ";
                    }
                }
            }
            xTagsBar.CurrentSelectedTagInfos = _currentSelectedTagInfos;
            BeginGetChannles(true);
        }

        #region 获取数据处理函数
        void _channelFactory_GetChannelsByListTimeout(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            xLoadChannelsGrid.Visibility = Visibility.Collapsed;
            if (_pageIndex == 1)
            {
                _getDataStatusHelp.Do(BeginGetChannles);
            }
            _pageIndex--;
            if (_pageIndex < 1)
                _pageIndex = 1;
            xLogTextBlock.Text = "加载新的数据失败 _pageIndex:" + _pageIndex + " 电影个数:" + (_isLongItem ? ChannelInfos.Count : AllOneRow3ChannelInfos.Count);

            xChannelsListBox.Visibility = Visibility.Collapsed;
            xNullTextBlock.Text = "加载数据失败";
            xNullTextBlock.Visibility = Visibility.Visible;
            _isLoadingData = false;
        }

        void _channelFactory_GetChannelsByListSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            _getDataStatusHelp.Clear();
            var channelInfos = args.Result.Channels;
            var channel_count = args.Result.ChannelCount;
            var page_count = args.Result.PageCount;
            _isLoadingData = false;
            _channel_count = channel_count;
            _page_count = page_count;
            if (_isLongItem)
            {
                foreach (ChannelInfo item in channelInfos)
                {
                    item.SlotUrl = item.SlotUrl.Substring(0, item.SlotUrl.IndexOf(".jpg")) + "_165.jpg";
                    ChannelInfos.Add(item);
                }
            }
            else//small
            {
                foreach (ChannelInfo item in channelInfos)
                {
                    ChannelInfos.Add(item);
                }

                int currentIndex = 0;
                for (int i = 0; i < ChannelInfos.Count; i++)
                {
                    if (currentIndex < i || i == 0)
                    {
                        ObservableCollection<ChannelInfo> tempChannelInfos = new ObservableCollection<ChannelInfo>();
                        for (int j = 0; j < 3; j++)
                        {
                            currentIndex = i + j;
                            LogManager.Ins.DebugLog(currentIndex + "");
                            if (currentIndex < channelInfos.Count)
                            {
                                tempChannelInfos.Add(channelInfos[currentIndex]);
                                Debug.WriteLine("添加了" + currentIndex);
                            }
                        }
                        AllOneRow3ChannelInfos.Add(tempChannelInfos);
                    }
                }
                Debug.WriteLine("总个数:" + (_isLongItem ? ChannelInfos.Count : AllOneRow3ChannelInfos.Count));
            }

            if (_isLongItem ? ChannelInfos.Count == 0 : AllOneRow3ChannelInfos.Count == 0)
            {
                xChannelsListBox.Visibility = Visibility.Collapsed;
                xNullTextBlock.Text = "暂无节目";
                xNullTextBlock.Visibility = Visibility.Visible;
            }
            else
            {
                xChannelsListBox.Visibility = Visibility.Visible;
                xNullTextBlock.Visibility = Visibility.Collapsed;
            }
            xLoadChannelsGrid.Visibility = Visibility.Collapsed;
        }

        void SmallChannelItem_ItemTap(object sender, ChannelInfo channelInfo)
        {
            if (ValidateFilms(channelInfo))
            {
                string page = String.Format("/DetailPage.xaml?vid={0}&programSource={1}", channelInfo.VID, _programSource);
                NavigationService.Navigate(new Uri(page, UriKind.Relative));
            }
        }

        void _channelFactory_GetChannelsByListFailed(object sender, HttpFactoryArgs<VodChannelListInfo> args)
        {
            _pageIndex--;
            if (_pageIndex < 1)
                _pageIndex = 1;
            xLogTextBlock.Text = "加载新的数据失败 _pageIndex:" + _pageIndex + " 电影个数:" + (_isLongItem ? ChannelInfos.Count : AllOneRow3ChannelInfos.Count);
            xLoadChannelsGrid.Visibility = Visibility.Collapsed;
            xChannelsListBox.Visibility = Visibility.Collapsed;
            xNullTextBlock.Text = "加载数据失败";
            xNullTextBlock.Visibility = Visibility.Visible;
            _isLoadingData = false;
        }
        #endregion

        private void xChannelsListBox_MouseMove(object sender, MouseEventArgs e)
        {
            ScrollViewer scrollViewer = Utils.Utils.FindChildOfType<ScrollViewer>(sender as ListBox);
            if (scrollViewer == null)
            {
                xLogTextBlock.Text = "scrollViewer == null";
                return;
            }
            if (scrollViewer.VerticalOffset + 2 >= scrollViewer.ScrollableHeight && _isLoadingData == false && xChannelsListBox.Items.Count != 0)
            {
                xLogTextBlock.Text = "移动完成,VerticalOffset:" + scrollViewer.VerticalOffset + "ScrollableHeight:" + scrollViewer.ScrollableHeight;
                BeginGetChannles(false);
            }
            xLogTextBlock.Text = "移动中,VerticalOffset:" + scrollViewer.VerticalOffset;
        }

        private void xChannelsListBox_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            xLogTextBlock.Text = "按下ListBox";
            //if (xTagsBar.Visibility == Visibility.Visible)
            //    SetTagsBarStatus(false);
            if (gridtools.Visibility == Visibility.Visible)
            {
                SetTagsBarStatus(false);
                SetOrderBarStatus(false);
            }
        }

        private void xChannelsListBox_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            xLogTextBlock.Text = "ManipulationCompleted";
            xLogTextBlock.Text = "MouseLeave";
            ScrollViewer scrollViewer = Utils.Utils.FindChildOfType<ScrollViewer>(sender as ListBox);

        }

        private void xChannelsListBox_MouseLeave(object sender, MouseEventArgs e)
        {

        }

        private void xHideTagsButton_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            SetTagsBarStatus(false);
        }

        private void xShowTagsButton_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            SetTagsBarStatus(true);
        }

        private void SetOrderBarStatus(bool IsShowOrder)
        {
            if (IsShowOrder)
            {
                gridtools.Visibility = System.Windows.Visibility.Visible;
                xTagsBar.Visibility = System.Windows.Visibility.Collapsed;
                orderBar.Visibility = System.Windows.Visibility.Visible;
                xChannlesNSelectedTagsStackPanel.Margin = new Thickness(0, xTitleGrid.ActualHeight + 65 + 10, 0, 0);
            }
            else
            {
                showOrder = false;
                gridtools.Visibility = System.Windows.Visibility.Collapsed;
                orderBar.Visibility = System.Windows.Visibility.Collapsed;
                xChannlesNSelectedTagsStackPanel.Margin = new Thickness(0, xTitleGrid.ActualHeight, 0, 0);
            }
        }

        private void SetTagsBarStatus(bool isShowTag)
        {
            if (isShowTag)
            {
                //显示面板
                //Canvas.SetZIndex(xTagsBar, 5);
                gridtools.Visibility = System.Windows.Visibility.Visible;
                xShowTagsButton.Visibility = Visibility.Collapsed;
                orderBar.Visibility = System.Windows.Visibility.Collapsed;
                xHideTagsButton.Visibility = Visibility.Visible;
                xTagsBar.Visibility = Visibility.Visible;
                xChannlesNSelectedTagsStackPanel.Margin = new Thickness(0, xTitleGrid.ActualHeight + 220 + 10, 0, 0);
            }
            else
            {
                gridtools.Visibility = System.Windows.Visibility.Collapsed;
                xShowTagsButton.Visibility = Visibility.Visible;
                xHideTagsButton.Visibility = Visibility.Collapsed;
                xTagsBar.Visibility = Visibility.Collapsed;
                xChannlesNSelectedTagsStackPanel.Margin = new Thickness(0, xTitleGrid.ActualHeight, 0, 0);
            }
        }

        private void xTagButton_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            Button button = (sender as Button);
            button.Visibility = Visibility.Collapsed;
            if (button.Equals(xCatalogButton))
            {
                _currentSelectedTagInfos[0] = new TagInfo(1, "catalog", "全部", -1);
            }
            else if (button.Equals(xYearButton))
            {
                _currentSelectedTagInfos[2] = new TagInfo(1, "year", "全部", -1);
            }
            else if (button.Equals(xAreaButton))
            {
                _currentSelectedTagInfos[1] = new TagInfo(1, "area", "全部", -1);
            }
            else if (button.Equals(xorderButton))
            {
                CurrentOrder = null;
                orderBar.ClearnSet();
            }
            xTagsBar.CurrentSelectedTagInfos = _currentSelectedTagInfos;
            BeginGetChannles(true);
        }

        private void LongChannelItem_ItemTap(object sender, ChannelInfo channelInfo)
        {
            if (ValidateFilms(channelInfo))
            {
                string page = String.Format("/DetailPage.xaml?vid={0}&programSource={1}", channelInfo.VID, _programSource);
                NavigationService.Navigate(new Uri(page, UriKind.Relative));
            }
        }

        bool showOrder = false;
        void btnOrder_Click(object sender, RoutedEventArgs e)
        {
            showOrder = !showOrder;
            SetTagsBarStatus(false);
            SetOrderBarStatus(showOrder);
        }
        /// <summary>
        /// VIP验证
        /// </summary>
        /// <param name="channelInfo"></param>
        /// <returns></returns>
        bool ValidateFilms(ChannelInfo channelInfo)
        {
            if (TypeID == (int)ChannelTypes.VIP尊享)
            {
                if (!PPTVForWP7.Utils.Utils._isValidatedFromServer)
                {
                    if (MessageBox.Show("请先登录", "", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                    {
                        NavigationService.Navigate(new Uri("/LoginPage.xaml?download=1", UriKind.RelativeOrAbsolute));
                    }
                    return false;
                }
                else
                {
                    if (Utils.Utils._userStateInfo.VIP != 1)
                    {
                        MessageBox.Show("您还不是VIP会员,请先到PPTV官网充值成为VIP会员");
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                }
            }
            return true;
        }

        void BeginLoad()
        {
            CurrentOrder = orderBar.SelectOrder;
            xorderButton.Visibility = System.Windows.Visibility.Visible;
            xorderButton.Content = orderBar.SelectText;
            _pageIndex = 1;
            AllOneRow3ChannelInfos.Clear();
            ChannelInfos.Clear();
            xLoadChannelsGrid.Visibility = Visibility.Visible;
            _isLoadingData = true;
            xLogTextBlock.Text = "加载新的数据 _pageIndex:" + _pageIndex + " 电影个数:" + (_isLongItem == true ? ChannelInfos.Count.ToString() : AllOneRow3ChannelInfos.Count.ToString());
            _channelFactory.DownLoadDatas(TypeID, _currentSelectedTagInfos, _pageIndex, _channelItemCount, CurrentOrder);
        }
    }
}