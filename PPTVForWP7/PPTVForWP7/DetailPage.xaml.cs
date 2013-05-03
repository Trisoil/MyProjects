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
using Microsoft.Phone.Shell;
using System.Xml;
using PPTVData.Factory;
using PPTVData.Entity;
using System.ComponentModel;
using System.Collections;
using System.Diagnostics;
using System.Windows.Navigation;

namespace PPTVForWP7
{
    using PPTVData.Entity.Notify;

    public partial class DetailPage : PhoneApplicationPage
    {
        private int _vID = -1;
        private int _programSource = -1;
        public int VID
        {
            get { return _vID; }
            private set { _vID = value; }
        }
        private GetDataStatusHelp _getDataStatusHelp = new GetDataStatusHelp();
        public class SingleProgram : System.ComponentModel.INotifyPropertyChanged
        {
            public SingleProgram(ChannelDetailInfo info)
            {
                ChannelDetailInfo = info;
                Title = info.Title;
                IsDown = info.IsDown;
            }
            private Visibility _isDown;
            public ChannelDetailInfo ChannelDetailInfo { get; set; }
            public string Title { get; set; }
            public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
            private void NotifyPropertyChanged(String info)
            {
                if (PropertyChanged != null)
                {
                    System.Windows.Deployment.Current.Dispatcher.BeginInvoke(PropertyChanged, new object[] { this, new System.ComponentModel.PropertyChangedEventArgs(info) });
                }
            }
            public System.Windows.Visibility IsDown
            {
                get
                {
                    return _isDown;
                }
                set
                {
                    _isDown = value;

                    NotifyPropertyChanged("IsDown");

                }
            }
        }
        DetailPageHelp _detailPageHelp = new DetailPageHelp();
        public DetailPageHelp DetailPageHelp
        {
            get { return _detailPageHelp; }
            private set
            {
                _detailPageHelp = value;
            }
        }
        ChannelDetailInfo _channelDetailInfoOfOriginal;
        private Button _downSign = null;
        private string _rid = String.Empty;
        public static bool backFromLoginPage = false;
        private ProgramInfo _piForDownload = null;
        private string _downUrl = string.Empty;
        SmallProgramListDataSource smallProgramListDataSource;//small,Source
        LongProgramListDataSource longProgramListDataSource;//long,Source

        SmallProgramListDataSource smallProgramListDataSourceForDownload;//small,Source
        LongProgramListDataSource longProgramListDataSourceForDownload;//long,Source

        ChannelDetailFactory _channelDetailFactory;
        DBFavourFactory _dbFavourFactory = new DBFavourFactory();
        private bool _isLogon = false;
        bool _isSmallItem = true;
        bool isRe = false;
        bool isReDownload = false;
        bool detailDownloaded = false;
        private Dictionary<PlayInfoFactory, ProgramInfo> _downProgramInfo = new Dictionary<PlayInfoFactory, ProgramInfo>();

        void StartDownloadData()
        {
            xLoadGrid.Visibility = Visibility.Visible;
            _channelDetailFactory = new ChannelDetailFactory();
            _channelDetailFactory.HttpFailHandler += _channelDetailFactory_GetChannelDetailByVIDFailed;
            _channelDetailFactory.HttpSucessHandler += _channelDetailFactory_GetChannelDetailByVIDSucceed;
            _channelDetailFactory.HttpTimeOutHandler += _channelDetailFactory_GetChannelDetailTimeout;
            _channelDetailFactory.DownLoadDatas(VID);
        }

        void _channelDetailFactory_GetChannelDetailTimeout(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            xLoadGrid.Visibility = Visibility.Collapsed;
            _getDataStatusHelp.Do(StartDownloadData);
        }

        void _channelDetailFactory_GetChannelDetailByVIDSucceed(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            detailDownloaded = true;
            _getDataStatusHelp.Clear();
            var channelDetailInfo = args.Result;
            xLoadGrid.Visibility = Visibility.Collapsed;

            if (DetailPageHelp.ChannelDetailInfoForPrograms.VID == -1)
            {
                ApplicationBar.Buttons.Add(Utils.CommonUtils.CreateFavAppBar(VID, _dbFavourFactory, OnAddToFavClick));
                if (channelDetailInfo.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.电视剧 ||
                    channelDetailInfo.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.动漫 ||
                    channelDetailInfo.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.综艺)
                {
                    ApplicationBar.Buttons.Add(Utils.PursuitUtils.CreatePursuitAppBar(VID, OnPursuit_Click));
                }
            }

            DetailPageHelp.ChannelDetailInfoForPrograms = channelDetailInfo;
            foreach (ProgramInfo info in DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos)
            {
                info.IsDown = DownPage.DownFactory.IsExistVid(DetailPageHelp.ChannelDetailInfoForPrograms.VID.ToString() + "|" + info.Index.ToString() + "|" + info.ChannelVID.ToString()) ? Visibility.Visible : Visibility.Collapsed;
            }
            channelDetailInfo.IsDown = DownPage.DownFactory.IsExistVid(DetailPageHelp.ChannelDetailInfoForPrograms.VID.ToString() + "||") ? Visibility.Visible : Visibility.Collapsed;

            _channelDetailInfoOfOriginal = DetailPageHelp.ChannelDetailInfoForPrograms.Clone();
            DetailPageHelp.ChannelDetailInfoForDownload = DetailPageHelp.ChannelDetailInfoForPrograms.Clone();

            xChannelTitleTextBlock.Text = channelDetailInfo.Title;
            TitleTicker();
            xActTextBlock.Text = channelDetailInfo.ActTags;
            xCatalogTextBlock.Text = channelDetailInfo.CatalogTags;
            xYearTextBlock.Text = channelDetailInfo.YearTags;
            xChannelImage.Source = Utils.Utils.GetImage(channelDetailInfo.ImgUrl);
            xMarkTextBlock.Text = channelDetailInfo.Mark.ToString("0.0");
            var duration = TimeSpan.FromMinutes(channelDetailInfo.Duration);
            xDurationTextBlock.Text = duration.Hours == 0 ?
                string.Format("{0}分钟", duration.Minutes) :
                string.Format("{0}小时{1}分钟", duration.Hours, duration.Minutes);

            InitChannelProgramItem();
            InitDownloadProgramItem();
        }

        private void TitleTicker()
        {
            TranslateTransform translateTransform = (xChannelTitleTextBlock.RenderTransform as TransformGroup).Children[0] as TranslateTransform;
            double offset = 268 - xChannelTitleTextBlock.ActualWidth;
            if (offset <= 0)
                Utils.Utils.GenerateStoryboardForTicker(xChannelTitleTextBlock, offset, TimeSpan.FromMilliseconds(1500)).Begin();
        }

        private void StartDownloadProgram()
        {
            if (!detailDownloaded)
                return;
            if (_downSign != null)
            {
                _downSign.Foreground = App.Current.Resources["Orange"] as SolidColorBrush; ;
                if (_piForDownload != null)
                    _piForDownload.IsDown = Visibility.Visible;
                Utils.CustomAnimation.DownloadAnimation(xDownAnima);

                if (DownPage.AddTask(DetailPageHelp.ChannelDetailInfoForPrograms, _piForDownload) == false)
                    return;

            }
            PlayInfoFactory playFac = new PlayInfoFactory();
            if (_piForDownload != null)
            {
                playFac.HttpSucessHandler += playFac_GetPlayInfoSucceed;
                playFac.HttpFailHandler += playFac_GetPlayInfoFailed;
                playFac.HttpTimeOutHandler += playFac_GetPlayInfoTimeout;
                _downProgramInfo.Add(playFac, _piForDownload);
                playFac.DownLoadDatas(_piForDownload.ChannelVID);
            }
            else
            {
                playFac.HttpSucessHandler += playFac_GetPlayInfoSucceed;
                playFac.HttpFailHandler += playFac_GetPlayInfoFailed;
                playFac.HttpTimeOutHandler += playFac_GetPlayInfoTimeout;
                _downProgramInfo.Add(playFac, _piForDownload);
                playFac.DownLoadDatas(DetailPageHelp.ChannelDetailInfoForPrograms.VID);
            }
        }

        private void playFac_GetPlayInfoTimeout(object sender, HttpFactoryArgs<PlayInfo> args)
        {
        }
        private void playFac_GetPlayInfoFailed(object sender, HttpFactoryArgs<PlayInfo> args)
        {
        }

        void playFac_GetPlayInfoSucceed(object sender, HttpFactoryArgs<PlayInfo> args)
        {
            ProgramInfo curProgramInfo = _downProgramInfo[sender as PlayInfoFactory];
            PlayInfo.Item item = args.Result.GetItemByFileType(5);
            if (item == null)
            {

                // _downSign.Visibility = Visibility.Collapsed;
                _downSign.Foreground = new SolidColorBrush(Colors.White);
                if (_piForDownload != null)
                    _piForDownload.IsDown = Visibility.Collapsed;
                string id = string.Empty;

                if (curProgramInfo == null)
                    id = DetailPageHelp.ChannelDetailInfoForPrograms.VID.ToString() + "||";
                else
                {
                    id = DetailPageHelp.ChannelDetailInfoForPrograms.VID + "|" + curProgramInfo.Index + "|" + curProgramInfo.ChannelVID;
                }

                foreach (DownPage.DownInfo info in DownPage.MyDownloading)
                {
                    if (info.VID == id)
                    {
                        DownPage.MyDownloading.Remove(info);
                        break;
                    }
                }
                DownPage.DownFactory.DeleteRecord(id);
                MessageBox.Show("无法下载该文件", "提示", MessageBoxButton.OK);
                return;
            }
            _rid = item.Rid;

            if (item.DtInfo != null)
            {
                _downUrl = item.url;
                if (curProgramInfo == null)
                    DownPage.UpdateTask(DetailPageHelp.ChannelDetailInfoForPrograms.VID.ToString() + "||", _downUrl);
                else
                {
                    DownPage.UpdateTask(DetailPageHelp.ChannelDetailInfoForPrograms.VID + "|" + curProgramInfo.Index + "|" + curProgramInfo.ChannelVID, _downUrl);
                }
            }
            else
            {
                string jump_url = "http://wp7.jump.synacast.com/" + item.Rid + "dt?type=mwp7";
                PPTVData.HttpWebClient jumpWebClient = new PPTVData.HttpWebClient();
                jumpWebClient.DownloadStringCompleted += new PPTVData.DownloadStringCompletedEventHandler(JumpWebClientDownloadStringCompleted);
                jumpWebClient.DownloadStringAsync(jump_url);
            }
        }

        private void JumpWebClientDownloadStringCompleted(object sender, PPTVData.DownloadStringCompletedEventArgs e)
        {
            string mediaServerHost = String.Empty;
            string serverTime = String.Empty;
            DateTime serverBaseTime;
            DateTime localBaseTime;
            if (e.Error == null)
            {
                try
                {
                    using (XmlReader reader = XmlReader.Create(new System.IO.StringReader(e.Result)))
                    {
                        if (true == reader.ReadToFollowing("server_host"))
                        {
                            mediaServerHost = reader.ReadElementContentAsString("server_host", "");
                        }
                        reader.ReadToFollowing("server_time");
                        serverTime = reader.ReadElementContentAsString("server_time", "");

                        System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo("en-US");
                        serverBaseTime = DateTime.ParseExact(serverTime, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                        localBaseTime = DateTime.Now;

                        _downUrl = String.Format("http://{0}/{1}?type=wp7&w=1&key={2}", mediaServerHost, _rid, PPTVData.KeyGenerator.GetKey(serverBaseTime));

                        if (DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos == null || DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count() <= 1)
                            DownPage.UpdateTask(DetailPageHelp.ChannelDetailInfoForPrograms.VID.ToString() + "||", _downUrl);
                        else
                        {
                            DownPage.UpdateTask(DetailPageHelp.ChannelDetailInfoForPrograms.VID + "|" + _piForDownload.Index + "|" + _piForDownload.ChannelVID, _downUrl);
                        }
                        if (_downSign != null)
                        {
                            _downSign.Foreground = App.Current.Resources["Orange"] as SolidColorBrush; ;
                            //_downSign.Visibility = Visibility.Visible;
                            if (_piForDownload != null)
                                _piForDownload.IsDown = Visibility.Visible;
                            Utils.CustomAnimation.DownloadAnimation(xDownAnima);
                        }
                    }
                }
                catch (Exception) { }
            }
        }

        private void InitDownloadProgramItem()
        {
            foreach (ProgramInfo info in DetailPageHelp.ChannelDetailInfoForDownload.ProgramInfos)
            {
                info.IsDown = DownPage.DownFactory.IsExistVid(DetailPageHelp.ChannelDetailInfoForDownload.VID.ToString() + "|" + info.Index.ToString() + "|" + info.ChannelVID.ToString()) ? Visibility.Visible : Visibility.Collapsed;
            }
            DetailPageHelp.ChannelDetailInfoForDownload.IsDown = DownPage.DownFactory.IsExistVid(DetailPageHelp.ChannelDetailInfoForDownload.VID.ToString() + "||") ? Visibility.Visible : Visibility.Collapsed;
            //if (DetailPageHelp.ChannelDetailInfoForDownload.ProgramInfos.Count <= 1)
            //{
            //    SingleProgram single = new SingleProgram(DetailPageHelp.ChannelDetailInfoForDownload);
            //    List<SingleProgram> singList = new List<SingleProgram>();

            //    singList.Add(single);
            //    xProgramsListDownload.ItemsSource = singList;
            //    xProgramsListDownload.ItemTemplate = Resources["SingleProgramItemDataTemplate"] as DataTemplate;
            //}
        }
        void InitChannelProgramItem()
        {
            if (DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count <= 1)
            {
                xInfoTextBlock.Visibility = Visibility.Visible;
                SingleProgram single = new SingleProgram(DetailPageHelp.ChannelDetailInfoForPrograms);
                List<SingleProgram> singList = new List<SingleProgram>();
                singList.Add(single);
                xProgramsListDownload.ItemsSource = singList;
                xProgramsListDownload.ItemTemplate = Resources["SingleProgramItemDataTemplate"] as DataTemplate;

                xDownloadTagBlock.Margin = new Thickness(0, 0, 0, 0);
                xProgramsListDownload.Margin = new Thickness(0, 22, 0, 0);
            }
            else
            {
                InitElement();
            }
            Utils.PursuitUtils.JudgePursuitUpdate(_vID, DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count);
        }

        void InitElement()
        {
            xUpDownStackPanel.Visibility = Visibility.Visible;
            xUpDownDownload.Visibility = Visibility.Visible;
            if (DetailPageHelp.ChannelDetailInfoForPrograms.IsNumber)
            {
                _isSmallItem = true;
                smallProgramListDataSource = new SmallProgramListDataSource(DetailPageHelp.ChannelDetailInfoForPrograms);
                smallProgramListDataSourceForDownload = new SmallProgramListDataSource(DetailPageHelp.ChannelDetailInfoForDownload);
                xProgramsListBox.ItemsSource = smallProgramListDataSource;
                xProgramsListBox.ItemTemplate = Resources["SmallProgramItemDataTemplate"] as DataTemplate;

                xProgramsListDownload.ItemsSource = smallProgramListDataSourceForDownload;
                xProgramsListDownload.ItemTemplate = Resources["MultiProgramDataTemplate"] as DataTemplate;
            }
            else
            {
                _isSmallItem = false;
                longProgramListDataSource = new LongProgramListDataSource(DetailPageHelp.ChannelDetailInfoForPrograms);
                longProgramListDataSourceForDownload = new LongProgramListDataSource(DetailPageHelp.ChannelDetailInfoForDownload);
                xProgramsListBox.ItemsSource = longProgramListDataSource;
                xProgramsListBox.ItemTemplate = Resources["LongProgramItemDataTemplate"] as DataTemplate;

                xProgramsListDownload.ItemsSource = longProgramListDataSourceForDownload;
                xProgramsListDownload.ItemTemplate = Resources["MultiLongProgramItemDataTemplate"] as DataTemplate;
            }
        }

        void _channelDetailFactory_GetChannelDetailByVIDFailed(object sender, HttpFactoryArgs<ChannelDetailInfo> args)
        {
            xLoadGrid.Visibility = Visibility.Collapsed;
            ShowFavAddDialog(true, "点击刷新重新获取影片信息");
        }

        public DetailPage()
        {
            DataContext = this;
            _getDataStatusHelp.DataDownloadFailure += new GetDataStatusHelp.DownloadFailEventHandler(_getDataStatusHelp_DataDownloadFailure);
            InitializeComponent();
        }

        void _getDataStatusHelp_DataDownloadFailure(object sender)
        {
            MessageBox.Show(Utils.Utils.NetExceptInfo);
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (Utils.Utils._userStateInfo == null)
                _isLogon = false;
            else
                _isLogon = true;
            xLoadGrid.Visibility = Visibility.Collapsed;
            if (e.NavigationMode != System.Windows.Navigation.NavigationMode.Back)
            {
                VID = Convert.ToInt32(NavigationContext.QueryString["vid"]);
                _programSource = Convert.ToInt32(NavigationContext.QueryString["programSource"]);
                StartDownloadData();
            }
            else
            {
                if (backFromLoginPage && _isLogon)
                {
                    StartDownloadProgram();
                }
                backFromLoginPage = false;
                InitDownloadProgramItem();
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            _getDataStatusHelp.Clear();
            PlayPage playPage = e.Content as PlayPage;
            if (playPage != null)
                playPage.ChannelDetailInfo = _channelDetailInfoOfOriginal;
            base.OnNavigatedFrom(e);
            xLoadGrid.Visibility = Visibility.Visible;
        }

        private void xDetailPivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            switch (xDetailPivot.SelectedIndex)
            {
                case 0:
                case 1:
                    {
                        ApplicationBar.Buttons.Clear();

                        ApplicationBar.Buttons.Add(Utils.CommonUtils.CreateRefreshAppBar(OnRefreshPage));
                        if (DetailPageHelp.ChannelDetailInfoForPrograms.TypeID != 0)
                            ApplicationBar.Buttons.Add(Utils.CommonUtils.CreateFavAppBar(VID, _dbFavourFactory, OnAddToFavClick));
                        if (DetailPageHelp.ChannelDetailInfoForPrograms.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.电视剧 ||
                          DetailPageHelp.ChannelDetailInfoForPrograms.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.动漫 ||
                          DetailPageHelp.ChannelDetailInfoForPrograms.TypeID == (int)PPTVForWP7.Utils.ChannelTypes.综艺)
                            ApplicationBar.Buttons.Add(Utils.PursuitUtils.CreatePursuitAppBar(VID, OnPursuit_Click));
                    }
                    break;
                case 2:
                    {
                        ApplicationBar.Buttons.Clear();
                        ApplicationBarIconButton button = new ApplicationBarIconButton();
                        button.Click += new EventHandler(OnNavigateToMyDownClick);
                        button.IconUri = new Uri("/Images/appbar.down.png", UriKind.RelativeOrAbsolute);
                        button.Text = "我的下载";
                        ApplicationBar.Buttons.Add(button);
                    }
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// 下载分集排序
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xDownButtonDownload_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_isSmallItem)
            {
                smallProgramListDataSourceForDownload = new SmallProgramListDataSource(DetailPageHelp.ChannelDetailInfoForDownload, !isReDownload);
                xProgramsListDownload.ItemsSource = smallProgramListDataSourceForDownload;
            }
            else
            {
                longProgramListDataSource = new LongProgramListDataSource(DetailPageHelp.ChannelDetailInfoForDownload, !isReDownload);
                xProgramsListDownload.ItemsSource = longProgramListDataSource;
            }
            (xDownButtonDownload.RenderTransform as CompositeTransform).Rotation = (xDownButtonDownload.RenderTransform as CompositeTransform).Rotation == 0 ? 180 : 0;
            //if ((xDownButtonDownload.RenderTransform as CompositeTransform).Rotation != 90)
            //{
            //    (xDownButtonDownload.RenderTransform as CompositeTransform).Rotation = 90;
            //}
            //else
            //{
            //    (xDownButtonDownload.RenderTransform as CompositeTransform).Rotation = 270;
            //}
            isReDownload = !isReDownload;
        }

        /// <summary>
        /// 剧集分集排序
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xDownButton_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_isSmallItem)
            {
                smallProgramListDataSource = new SmallProgramListDataSource(DetailPageHelp.ChannelDetailInfoForPrograms, !isRe);
                xProgramsListBox.ItemsSource = smallProgramListDataSource;
            }
            else
            {
                longProgramListDataSource = new LongProgramListDataSource(DetailPageHelp.ChannelDetailInfoForPrograms, !isRe);
                xProgramsListBox.ItemsSource = longProgramListDataSource;
            }
            (xDownButton.RenderTransform as CompositeTransform).Rotation = (xDownButton.RenderTransform as CompositeTransform).Rotation == 0 ? 180 : 0;
            //if ((xDownButton.RenderTransform as CompositeTransform).Rotation != 90)
            //{
            //    (xDownButton.RenderTransform as CompositeTransform).Rotation = 90;
            //}
            //else
            //{
            //    (xDownButton.RenderTransform as CompositeTransform).Rotation = 270;
            //}
            isRe = !isRe;
        }

        private void ShowFavAddDialog(bool isShow = true, string txtInfo = "已加入收藏")
        {
            xAddFavInfoTextBlock.Text = txtInfo;
            if (isShow)
            {
                sbFavShow.Begin();
            }
            else
                xAddFavGrid.Visibility = Visibility.Collapsed;
        }

        private void ProgramItem_ItemTap(object sender, ProgramInfo programInfo)
        {
            xLoadGrid.Visibility = Visibility.Visible;
            string page = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource={2}", VID, programInfo.Index, _programSource);
            NavigationService.Navigate(new Uri(page, UriKind.Relative));
        }

        void button_Click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            ProgramInfo programInfo = button.Tag as ProgramInfo;
            xLoadGrid.Visibility = Visibility.Visible;
            string page = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource={2}", VID, programInfo.Index, _programSource);
            NavigationService.Navigate(new Uri(page, UriKind.Relative));
        }

        private void OnLongStartDownload(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;
            //Grid grid = button.Parent as Grid;
            //_downSign = grid.Children[1] as Rectangle;
            _downSign = button;
            ProgramInfo programInfo = button.Tag as ProgramInfo;
            _piForDownload = programInfo;

            if (!_isLogon)
            {
                MessageBox.Show("请先登录", "提示", MessageBoxButton.OK);
                NavigationService.Navigate(new Uri("/LoginPage.xaml?download=1", UriKind.RelativeOrAbsolute));
            }
            else
            {
                StartDownloadProgram();
            }
        }

        private void OnStartDownloadTap(object sender, ProgramInfo programInfo)
        {
            _piForDownload = programInfo;
            //Button button = sender as Button;
            //Grid grid = button.Parent as Grid;
            //_downSign = grid.Children[1] as Rectangle;
            _downSign = sender as Button;
            if (!_isLogon)
            {
                MessageBox.Show("请先登录", "提示", MessageBoxButton.OK);
                NavigationService.Navigate(new Uri("/LoginPage.xaml?download=1", UriKind.RelativeOrAbsolute));
            }
            else
            {
                StartDownloadProgram();
            }
        }

        private void OnStartDownloadSingleClick(object sender, RoutedEventArgs e)
        {
            //Button button = sender as Button;
            //Grid grid = button.Parent as Grid;
            //_downSign = grid.Children[1] as Rectangle;
            _downSign = sender as Button;
            _piForDownload = null;
            if (!_isLogon)
            {
                MessageBox.Show("请先登录", "提示", MessageBoxButton.OK);
                NavigationService.Navigate(new Uri("/LoginPage.xaml?download=1", UriKind.RelativeOrAbsolute));
            }
            else
            {
                StartDownloadProgram();
            }
        }

        private void OnAddToFavClick(object sender, EventArgs e)
        {
            try
            {
                var bar = sender as ApplicationBarIconButton;
                if (bar != null)
                {
                    if (DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count == 0)
                        return;
                    bool insert = _dbFavourFactory.InsertRecord(DetailPageHelp.ChannelDetailInfoForPrograms);
                    if (insert)
                    {
                        ShowFavAddDialog();
                        bar.IsEnabled = false;
                        bar.Text = "已收藏";
                        bar.IconUri = Utils.CommonUtils.FavedImageUri;
                    }
                    else
                    {
                        ShowFavAddDialog(true, "请勿重复添加收藏");
                    }
                }
            }
            catch (Exception) { }
        }

        private void OnNavigateToMyDownClick(object sender, EventArgs e)
        {
            NavigationService.Navigate(new Uri("/DownPage.xaml", UriKind.RelativeOrAbsolute));
        }

        private void OnRefreshPage(object sender, EventArgs e)
        {
            xLoadGrid.Visibility = Visibility.Visible;
            _channelDetailFactory.DownLoadDatas(VID);
        }

        static object _pursuitObj = new object();

        /// <summary>
        /// 追剧事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnPursuit_Click(object sender, EventArgs e)
        {
            if (!_isLogon)
            {
                try
                {
                    if (MessageBox.Show("请先登录", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
                        NavigationService.Navigate(new Uri("/LoginPage.xaml?pursuit=1", UriKind.RelativeOrAbsolute));
                }
                catch { }
                return;
            }
            var bar = sender as ApplicationBarIconButton;
            if (bar != null)
            {
                try
                {
                    Utils.PursuitUtils.PursuitChannel(_vID, true, (o, ee) =>
                    {
                        Dispatcher.BeginInvoke(() =>
                        {
                            var response = ee.IsSucess ? "追剧成功" : "追剧未成功";
                            if (ee.IsSucess)
                            {
                                bar.IsEnabled = false;
                                bar.IconUri = Utils.PursuitUtils.PursuitedImageUri;
                                bar.Text = "已追剧";
                                if (!PursuitViewModel.Instance.HasByChannelId(_vID))
                                {
                                    var pursuit = new PursuitInfo()
                                    {
                                        ChannelId = _vID,
                                        ChannelName = DetailPageHelp.ChannelDetailInfoForPrograms.Title,
                                        ImageUrl = DetailPageHelp.ChannelDetailInfoForPrograms.ImgUrl,
                                        Mark = DetailPageHelp.ChannelDetailInfoForPrograms.Mark,
                                        IsShowDelte = false,
                                        ChannelCount = DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count
                                    };
                                    PursuitViewModel.Instance.AddEntity(pursuit);
                                }
                            }
                            else
                            {
                                bar.IsEnabled = true;
                            }
                            ShowFavAddDialog(true, response);
                        });
                    });
                    bar.IsEnabled = false;
                }
                catch
                {
                    ShowFavAddDialog(true, "追剧未成功");
                }
            }
        }

        private void xChannelImage_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            string page = string.Empty;
            try
            {
                if (DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count == 1 || DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count == 0)//单集
                {
                    page = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource={2}", VID, -1, _programSource);
                }
                else if (DetailPageHelp.ChannelDetailInfoForPrograms.ProgramInfos.Count > 1)
                {
                    page = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource={2}", VID, 0, _programSource);//播放第一集
                }
                else if (VID != -1)//没有获取到数据
                {
                    OnRefreshPage(null, null);
                    return;
                }
                NavigationService.Navigate(new Uri(page, UriKind.Relative));
            }
            catch (Exception)
            {
                if (VID != -1)
                    OnRefreshPage(null, null);
            }
        }
    }

    public class DetailPageHelp : INotifyPropertyChanged
    {
        ChannelDetailInfo _channelDetailInfoForPrograms = new ChannelDetailInfo();
        public ChannelDetailInfo ChannelDetailInfoForPrograms
        {
            get { return _channelDetailInfoForPrograms; }
            set
            {
                _channelDetailInfoForPrograms = value;
                OnPropertyChanged("ChannelDetailInfoForPrograms");
            }
        }

        ChannelDetailInfo _channelDetailInfoForDownload = new ChannelDetailInfo();
        public ChannelDetailInfo ChannelDetailInfoForDownload
        {
            get { return _channelDetailInfoForDownload; }
            set
            {
                _channelDetailInfoForDownload = value;
                OnPropertyChanged("ChannelDetailInfoForDownload");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class DetailInfoHelp
    {
        public DetailInfoHelp(ProgramInfo programInfo, int programIndex)
        {
            ProgramInfo = programInfo;
            ProgramIndex = programIndex;
        }
        public ProgramInfo ProgramInfo { get; set; }
        public int ProgramIndex { get; set; }
    }

    /// <summary>
    /// Programs数据源
    /// </summary>
    public class SmallProgramListDataSource : IList
    {
        ChannelDetailInfo _channelDetailInfo = null;
        private List<List<ProgramInfo>> _programInfos = new List<List<ProgramInfo>>();
        bool _is_reservered = false;

        public SmallProgramListDataSource(ChannelDetailInfo channel_detail_info, bool isReservered = false)
        {
            IdReservered = isReservered;
            _channelDetailInfo = channel_detail_info;
            Controls.SmallProgramItem._count = _channelDetailInfo.ProgramInfos.Count;
            if (isReservered == false)
            {
                int currentIndex = 0;
                for (int i = 0; i < _channelDetailInfo.ProgramInfos.Count; i++)
                {
                    if (currentIndex < i || i == 0)
                    {
                        List<ProgramInfo> _programInfoBy5InOne = new List<ProgramInfo>();
                        for (int j = 0; j < 5; j++)
                        {
                            currentIndex = i + j;
                            if (currentIndex < _channelDetailInfo.ProgramInfos.Count)
                            {
                                _programInfoBy5InOne.Add(_channelDetailInfo.ProgramInfos[currentIndex]);
                            }
                        }
                        _programInfos.Add(_programInfoBy5InOne);
                    }
                }
            }
            else
            {
                int currentIndex = _channelDetailInfo.ProgramInfos.Count - 1;
                for (int i = _channelDetailInfo.ProgramInfos.Count - 1; i > 0; i--)
                {
                    if (currentIndex > i || i == _channelDetailInfo.ProgramInfos.Count - 1)
                    {
                        List<ProgramInfo> _programInfoBy5InOne = new List<ProgramInfo>();
                        for (int j = 0; j < 5; j++)
                        {
                            currentIndex = i - j;
                            if (currentIndex >= 0)
                            {
                                _programInfoBy5InOne.Add(_channelDetailInfo.ProgramInfos[currentIndex]);
                            }
                        }
                        _programInfos.Add(_programInfoBy5InOne);
                    }
                }
                if (currentIndex != 0)
                {
                    for (int i = currentIndex - 1; i >= 0; i--)
                    {
                        List<ProgramInfo> _programInfoBy5InOne = new List<ProgramInfo>();
                        if (i >= 0)
                        {
                            _programInfoBy5InOne.Add(_channelDetailInfo.ProgramInfos[i]);
                        }
                        _programInfos.Add(_programInfoBy5InOne);
                    }

                }
            }
        }

        public bool IdReservered
        {
            get { return _is_reservered; }
            set
            {
                _is_reservered = value;
            }
        }

        public int Count
        {
            get { return _programInfos.Count; }
        }

        public int IndexOf(object value)
        {
            return -1;
        }

        public object this[int index]
        {
            get
            {
                if (!(0 <= index && index < _programInfos.Count))
                {
                    return null;
                }
                string text = " ";
                foreach (ProgramInfo pi in _programInfos[index])
                {
                    text += pi.Title + " ";
                }
                return _programInfos[index];
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        #region Unimplemented stuff

        public int Add(object value)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(object value)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        public bool IsFixedSize
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsReadOnly
        {
            get { throw new NotImplementedException(); }
        }

        public void Remove(object value)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public bool IsSynchronized
        {
            get { throw new NotImplementedException(); }
        }

        public object SyncRoot
        {
            get { throw new NotImplementedException(); }
        }

        public IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
    public class ProgramInfoContainer
    {
        public ProgramInfoContainer(List<ProgramInfo> programInfos)
        {
            ProgramInfos = programInfos;
        }
        public List<ProgramInfo> ProgramInfos
        {
            get;
            set;
        }
    }
    public class DownSource : IList
    {
        List<ProgramInfo> _programInfos = null;
        ProgramInfoContainer _container;
        bool _is_reservered = false;

        public DownSource(ProgramInfoContainer container, bool isReservered = false)
        {
            IdReservered = isReservered;
            _container = container;
            if (isReservered)
            {
                _programInfos.Reverse();
            }
        }

        public bool IdReservered
        {
            get { return _is_reservered; }
            set { _is_reservered = value; }
        }

        public int Count
        {
            get { return _container.ProgramInfos.Count; }
        }

        public int IndexOf(object value)
        {
            return -1;
        }

        public object this[int index]
        {
            get
            {
                if (!(0 <= index && index < _container.ProgramInfos.Count))
                {
                    return null;
                }
                return _container.ProgramInfos[index];
            }
            set
            {
                throw new NotImplementedException();
            }
        }
        #region Unimplemented stuff

        public int Add(object value)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(object value)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        public bool IsFixedSize
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsReadOnly
        {
            get { throw new NotImplementedException(); }
        }

        public void Remove(object value)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public bool IsSynchronized
        {
            get { throw new NotImplementedException(); }
        }

        public object SyncRoot
        {
            get { throw new NotImplementedException(); }
        }

        public IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
    /// <summary>
    /// Programs数据源
    /// </summary>
    public class LongProgramListDataSource : IList
    {
        ChannelDetailInfo _channelDetailInfo = null;
        bool _is_reservered = false;

        public LongProgramListDataSource(ChannelDetailInfo channel_detail_info, bool isReservered = false)
        {
            IdReservered = isReservered;
            _channelDetailInfo = channel_detail_info;
            _channelDetailInfo.ProgramInfos.Reverse();
        }

        public bool IdReservered
        {
            get { return _is_reservered; }
            set { _is_reservered = value; }
        }

        public int Count
        {
            get { return _channelDetailInfo.ProgramInfos.Count; }
        }

        public int IndexOf(object value)
        {
            return -1;
        }

        public object this[int index]
        {
            get
            {
                if (!(0 <= index && index < _channelDetailInfo.ProgramInfos.Count))
                {
                    return null;
                }
                return _channelDetailInfo.ProgramInfos[index];
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        #region Unimplemented stuff

        public int Add(object value)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(object value)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, object value)
        {
            throw new NotImplementedException();
        }

        public bool IsFixedSize
        {
            get { throw new NotImplementedException(); }
        }

        public bool IsReadOnly
        {
            get { throw new NotImplementedException(); }
        }

        public void Remove(object value)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public bool IsSynchronized
        {
            get { throw new NotImplementedException(); }
        }

        public object SyncRoot
        {
            get { throw new NotImplementedException(); }
        }

        public IEnumerator GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}