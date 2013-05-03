using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Threading.Tasks;

using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;

    using PPTVForWP8.Utils;
    using PPTVForWP8.Common;

    using PPTV.WPRT.CommonLibrary.Utils;
    using PPTV.WPRT.CommonLibrary.DataModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Play;

    public partial class DetailPage : PhoneApplicationPage
    {
        ChannelDetailFactory _channelDetailFactory;

        int _id;
        ApplicationBarIconButton _favBar;
        ApplicationBarIconButton _downloadBar;
        ChannelDetailInfo _dataSource;
        List<ProgramInfo> _downloadList;

        public DetailPage()
        {
            InitializeComponent();

            _channelDetailFactory = new ChannelDetailFactory();
            _channelDetailFactory.HttpSucessHandler += channelDetailFactory_Successed;
            _channelDetailFactory.HttpFailorTimeOut = channelDetailFactory_HttpFailorTimeOut;

            _downloadList = new List<ProgramInfo>();

            _favBar = new ApplicationBarIconButton();
            _favBar.IconUri = new Uri("/Images/AppBar/like.png", UriKind.Relative);
            _favBar.Click += ApplicationBarFav_Click;

            _downloadBar = new ApplicationBarIconButton();
            _downloadBar.Text = "下载";
            _downloadBar.IconUri = new Uri("/Images/AppBar/download.png", UriKind.Relative);
            _downloadBar.Click += ApplicationBarDownload_Click;
            _downloadBar.IsEnabled = false;

            //DownloadViewModel.Instance.DownloadingCountChannge += AdapteDownloadState;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                _id = Convert.ToInt32(NavigationContext.QueryString["id"]);

                RequestDatas();
                favolitenInit(_id);
            }
        }

        #region HttpHandler

        private async void channelDetailFactory_Successed(object sender, HttpFactoryArgs<ChannelDetailInfo> e)
        {
            _dataSource = e.Result;

            if (_dataSource.IsNumber)
                channelList.GridCellSize = new Size(154, 147);
            else
                channelList.GridCellSize = new Size(154, 195);

            if (_dataSource.ProgramInfos == null)
            {
                TileUtils.CreateBasicToast("该节目信息不存在!");
                NavigationService.GoBack();
                return;
            }

            if (!_dataSource.IsNumber && _dataSource.ProgramInfos.Count == 1)
                _dataSource.ProgramInfos[0].Title = string.Format("{0} 全一集", _dataSource.ProgramInfos[0].Title);
            DataContext = _dataSource;

            channelList.ItemsSource = _dataSource.ProgramInfos;
            downloadList.ItemsSource = _dataSource.ProgramInfos;
            
            await UpdateDownloadState();
            loadingTip.Visibility = Visibility.Collapsed;
            //ShareUtils.ShareInfo = new PlayShareInfo(_dataSource.Id, _dataSource.Title, _dataSource.ImageUri);
        }

        private void channelDetailFactory_HttpFailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Constants.HttpFailTipText, RequestDatas);
        }

        #endregion

        private void RequestDatas()
        {
            _dataSource = null;
            DataContext = null;

            channelList.ItemsSource = null;
            downloadList.ItemsSource = null;
            loadingTip.Visibility = Visibility.Visible;

            _downloadList.Clear();
            _channelDetailFactory.DownLoadDatas(_id);
        }

        private async void favolitenInit(int channelId)
        {
            var result = await LocalFavoritesFactory.Instance.GetRecord(channelId) == null ? false : true;
            if (result)
            {
                _favBar.IsEnabled = false;
                _favBar.Text = "已收藏";
            }
            else
            {
                _favBar.IsEnabled = true;
                _favBar.Text = "收藏";
            }
            ApplicationBar.Buttons.Add(_favBar);
        }

        private void Play_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (_dataSource != null)
            {
                int id = _dataSource.Id;
                int index = 0;
                if (ChannelUtils.JudgeSingle(_dataSource))
                    index = -1;
                StartToPlay(id, index);
            }
        }

        private void channelList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var value = channelList.SelectedItem as ProgramInfo;
            if (value != null)
            {
                int id = value.ChannelId;
                int index = -1;
                if (!ChannelUtils.JudgeSingle(_dataSource))
                {
                    index = ChannelUtils.CreateProgramIndex(value.Index, _dataSource);
                }
                StartToPlay(id, index);
            }
            channelList.SelectedItem = null;
        }

        private void StartToPlay(int id, int index)
        {
            NavigationService.Navigate(new Uri(string.Format("/Pages/PlayPage.xaml?id={0}&index={1}&playtype={2}", id, index, (int)PlayType.Vod), UriKind.Relative));
        }

        private void detailPivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (detailPivot.SelectedIndex == 0)
            {
                orderBorder.Visibility = Visibility.Collapsed;
                ApplicationBar.Buttons.Remove(_downloadBar);
            }
            else if (detailPivot.SelectedIndex == 1)
            {
                orderBorder.Visibility = _dataSource == null || _dataSource.ProgramInfos.Count == 1 ? 
                    Visibility.Collapsed : Visibility.Visible;
                ApplicationBar.Buttons.Remove(_downloadBar);
            }
            else
            {
                orderBorder.Visibility = _dataSource == null || _dataSource.ProgramInfos.Count == 1 ? 
                    Visibility.Collapsed : Visibility.Visible;
                ApplicationBar.Buttons.Add(_downloadBar);
            }
        }

        private void downloadList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var dataItem = downloadList.SelectedItem as ProgramInfo;
            if (dataItem != null && !dataItem.IsDownloading)
            {
                dataItem.IsSelected = !dataItem.IsSelected;
                if (dataItem.IsSelected)
                {
                    _downloadList.Add(dataItem);
                }
                else
                {
                    _downloadList.Remove(dataItem);
                }
            }
            _downloadBar.IsEnabled = _downloadList.Count <= 0 ? false : true;
            downloadList.SelectedItem = null;
        }

        private void ApplicationBarRefresh_Click(object sender, EventArgs e)
        {
            RequestDatas();
        }

        private void ApplicationBarFav_Click(object sender, EventArgs e)
        {
            try
            {
                if (_dataSource != null && _favBar.Text == "收藏")
                {
                    var v = new CloudDataInfo()
                    {
                        Id = _dataSource.Id,
                        Name = _dataSource.Title,
                        SubId = _dataSource.Id,
                        SubName = _dataSource.Title,
                        Mark = _dataSource.Mark,
                        Duration = (int)(_dataSource.Duration * 60),
                        ImageUri = _dataSource.ImageUri
                    };
                    LocalFavoritesFactory.Instance.InsertLocalRecord(v);
                    _favBar.IsEnabled = false;
                    _favBar.Text = "已收藏";
                    TileUtils.CreateBasicToast("收藏成功!");
                }
            }
            catch
            {
                TileUtils.CreateBasicToast("收藏失败!");
            }
        }

        private void ApplicationBarDownload_Click(object sender, EventArgs e)
        {
            try
            {
                if (PersonalFactory.Instance.Logined)
                {
                    var downinfos = new List<DownloadInfo>(_downloadList.Count);
                    foreach (var downInfo in _downloadList)
                    {
                        var index = ChannelUtils.CreateProgramIndex(downInfo.Index, _dataSource);
                        downInfo.IsSelected = false;
                        downInfo.IsDownloading = true;
                        var down = new DownloadInfo()
                        {
                            TypeId = _dataSource.Type,
                            ParentId = _dataSource.Id,
                            ParentName = _dataSource.Title,
                            ProgramIndex = downInfo.Index,
                            ChannelId = downInfo.ChannelId,
                            ImageUri = _dataSource.ImageUri,
                            DownloadState = DownloadState.Await,
                            Title = ChannelUtils.CreateChannelTitle(index, _dataSource, false)
                        };
                        down.LocalFileName = string.Format("{0}.mp4", down.Title);
                        downinfos.Add(down);
                    }
                    DownloadViewModel.Instance.AddDownloads(downinfos);
                    if (_downloadList.Count > 0)
                        TileUtils.CreateBasicToast("已加入下载，请到个人中心查看");
                    _downloadList.Clear();
                    _downloadBar.IsEnabled = false;
                }
                else
                {
                    TileUtils.CreateBasicToast("请先至个人中心登录");
                }
            }
            catch
            {
                TileUtils.CreateBasicToast("加入下载失败");
            }
        }

        private void orderBorder_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (loadingTip.Visibility == Visibility.Visible) return;

            if (orderText.Text == "正序")
            {
                orderText.Text = "倒序";
                orderImage.Source = CommonUtils.GetImage("/Images/desc.png");
            }
            else
            {
                orderText.Text = "正序";
                orderImage.Source = CommonUtils.GetImage("/Images/asc.png");
            }
            _dataSource.ProgramInfos.Reverse();

            channelList.ItemsSource = null;
            downloadList.ItemsSource = null;
            channelList.ItemsSource = _dataSource.ProgramInfos;
            downloadList.ItemsSource = _dataSource.ProgramInfos;
        }

        //private async void AdapteDownloadState()
        //{
        //    await UpdateDownloadState();
        //}

        private async Task UpdateDownloadState()
        {
            foreach (var program in _dataSource.ProgramInfos)
            {
                var downInfo = await Task.Run(() => { return DownloadViewModel.Instance.DownloadingItems.FirstOrDefault(v => v.ChannelId == program.ChannelId); });
                if (downInfo != null)
                    program.IsDownloading = true;
                else
                    program.IsDownloading = false;
            }
        }
    }
}