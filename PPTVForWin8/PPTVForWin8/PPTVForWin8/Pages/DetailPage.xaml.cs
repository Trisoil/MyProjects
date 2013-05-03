using System;
using System.Linq;
using System.Threading.Tasks;
using System.Collections.Generic;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media.Animation;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.StartScreen;
using Windows.UI.ViewManagement;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVData.Entity.Cloud;
    using PPTVData.Factory.Cloud;
    using PPTVForWin8.Common;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.DataModel.Play;
    using PPTV.WinRT.CommonLibrary.DataModel.Download;
    using PPTV.WinRT.CommonLibrary.ViewModel.Download;

    public sealed partial class DetailPage : LayoutAwarePage
    {
        ChannelDetailFactory _channelDetailFactory;

        int _catchVid = 0;

        ChannelDetailInfo _dataSource;
        List<ProgramInfo> _downloadList;

        public DetailPage()
        {
            this.InitializeComponent();
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            _channelDetailFactory = new ChannelDetailFactory();
            _channelDetailFactory.HttpSucessHandler += channelDetailFactory_Successed;
            _channelDetailFactory.HttpFailorTimeOut = channelDetailFactory_HttpFailorTimeOut;

            DownloadViewModel.Instance.DownloadingCountChannge += AdapteDownloadState;

            _downloadList = new List<ProgramInfo>();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                _catchVid = int.Parse(e.Parameter.ToString());
                LoadDatas(_catchVid);
                favolitenInit(_catchVid);
            }
            else
            {
                ShareUtils.ShareInfo = new PlayShareInfo(_dataSource.Id, _dataSource.Title, _dataSource.ImageUri);
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ShareUtils.ShareInfo = null;

            foreach (var downInfo in _downloadList)
                downInfo.IsSelected = false;

            _channelDetailFactory.Cancel();
            _downloadList.Clear();
            BottomAppBar.IsOpen = false;
            downloadBar.Visibility = Visibility.Collapsed;
            (channelListView.RenderTransform as Windows.UI.Xaml.Media.CompositeTransform).TranslateX = 0;
        }

        #region HttpHandler

        private void channelDetailFactory_Successed(object sender, HttpFactoryArgs<ChannelDetailInfo> e)
        {
            _dataSource = e.Result;

            if (_dataSource.ProgramInfos == null)
            {
                TileUtils.CreateToastNotifications("该节目信息不存在!");
                GoBack(this, new RoutedEventArgs());
                return;
            }
            if (!_dataSource.IsNumber && _dataSource.ProgramInfos.Count == 1)
                _dataSource.ProgramInfos[0].Title = string.Format("{0} 全一集", _dataSource.ProgramInfos[0].Title);
            imgOrder.Visibility = e.Result.ProgramInfos.Count == 1 ? Visibility.Collapsed : Visibility.Visible;
            DataContext = _dataSource;
            channelListView.ItemsSource = _dataSource.ProgramInfos;
            ShareUtils.ShareInfo = new PlayShareInfo (_dataSource.Id, _dataSource.Title, _dataSource.ImageUri);
            CloseWaitToolTips();
        }

        private void channelDetailFactory_HttpFailorTimeOut()
        {
            CloseWaitToolTips();
            Utils.CommonUtils.ShowMessageConfirm(Constants.DetailFail, Refresh_Command, "刷新");
        }

        private void Refresh_Command()
        {
            btnRefresh_Click(this, new RoutedEventArgs());
            BottomAppBar.IsOpen = false;
        }

        #endregion

        private void StartToPlay(PlayInfoHelp playInfo)
        {
            this.Frame.Navigate(typeof(PlayPage), playInfo);
        }

        private void listView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var value = e.ClickedItem as ProgramInfo;
            if (value != null)
            {
                PlayInfoHelp obj;
                if (ChannelUtils.JudgeSingle(_dataSource))
                {
                    obj = new PlayInfoHelp(_dataSource, -1);
                }
                else
                {
                    var index = ChannelUtils.CreateProgramIndex(value.Index, _dataSource);
                    obj = new PlayInfoHelp(_dataSource, index);
                }
                StartToPlay(obj);
            }
        }

        private void playButton_Click(object sender, RoutedEventArgs e)
        {
            if (_dataSource != null)
            {
                PlayInfoHelp obj;
                if (ChannelUtils.JudgeSingle(_dataSource))
                    obj = new PlayInfoHelp(_dataSource, -1);
                else
                    obj = new PlayInfoHelp(_dataSource, 0);
                StartToPlay(obj);
            }
        }

        private void imgOrder_Tapped(object sender, TappedRoutedEventArgs e)
        {
            double value = imgProj.RotationX == 0 ? -180 : 0;
            AnlgeAnimation(value);
            _dataSource.ProgramInfos.Reverse();

            channelListView.ItemsSource = null;
            downloadListView.ItemsSource = null;
            channelListView.ItemsSource = _dataSource.ProgramInfos;
            downloadListView.ItemsSource = _dataSource.ProgramInfos;
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            LoadDatas(_catchVid);
            BottomAppBar.IsOpen = false;
        }

        private void LoadDatas(int vid)
        {
            CloseWaitToolTips();
            _downloadList.Clear();

            BottomAppBar.IsOpen = false;
            downloadBar.Visibility = Visibility.Collapsed;

            btnPlayList.IsChecked = true;
            btnDownload.IsChecked = false;
            channelListView.Visibility = Visibility.Visible;
            downloadListView.Visibility = Visibility.Collapsed;

            _dataSource = null;
            DataContext = null;
            channelListView.ItemsSource = null;
            downloadListView.ItemsSource = null;

            if (loadingTip.Visibility == Visibility.Collapsed)
            {
                OpenWaitToolTips();
                _channelDetailFactory.DownLoadDatas(vid);
            }
        }

        private void CloseWaitToolTips()
        {
            titleTextBlock.Visibility = Visibility.Visible;
            gridContentt.Visibility = Visibility.Visible;
            loadingTip.Visibility = Visibility.Collapsed;
        }

        private void OpenWaitToolTips()
        {
            titleTextBlock.Visibility = Visibility.Collapsed;
            gridContentt.Visibility = Visibility.Collapsed;
            loadingTip.Visibility = Visibility.Visible;
        }

        private void AnlgeAnimation(double angle)
        {
            Storyboard sb = new Storyboard();
            DoubleAnimation dbl = new DoubleAnimation(); ;
            dbl.To = angle;
            dbl.Duration = new Duration(TimeSpan.FromMilliseconds(400));
            Storyboard.SetTarget(dbl, imgProj);
            Storyboard.SetTargetProperty(dbl, "(PlaneProjection.RotationX)");
            sb.Children.Add(dbl);
            sb.Begin();
        }

        private async void favolitenInit(int channelId)
        {
            var result = await LocalFavoritesFactory.Instance.GetRecord(channelId) == null ? false : true;
            if (result)
                favourite.Style = App.Current.Resources["HasFavoriteAppButton"] as Style;
            else
                favourite.Style = App.Current.Resources["FavoriteAppButton"] as Style;
        }

        private void favourite_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (_dataSource != null && favourite.Style == App.Current.Resources["FavoriteAppButton"] as Style)
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
                    favourite.Style = App.Current.Resources["HasFavoriteAppButton"] as Style;
                    TileUtils.CreateToastNotifications("收藏成功!");
                    BottomAppBar.IsOpen = false;
                }
            }
            catch
            {
                TileUtils.CreateToastNotifications("收藏失败!");
            }
        }

        private async void PineToStart_Click(object sender, RoutedEventArgs e)
        {
            await TileUtils.Pin_Start(_dataSource, false, Utils.Constants.LogoUri);
        }

        private async void toggleButton_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as ToggleButton;
            if (button != null)
            {
                var cmdParas = button.CommandParameter.ToString();
                if (cmdParas == "playlist")
                {
                    btnPlayList.IsChecked = true;
                    btnDownload.IsChecked = false;
                    channelListView.Visibility = Visibility.Visible;
                    downloadListView.Visibility = Visibility.Collapsed;

                    downloadListShowSB.Stop();
                    channelListShowSB.Begin();
                    (downloadListView.RenderTransform as Windows.UI.Xaml.Media.CompositeTransform).TranslateX = 36;
                }
                else
                {
                    btnDownload.IsChecked = true;
                    btnPlayList.IsChecked = false;
                    downloadListView.Visibility = Visibility.Visible;
                    channelListView.Visibility = Visibility.Collapsed;

                    channelListShowSB.Stop();
                    downloadListShowSB.Begin();
                    (channelListView.RenderTransform as Windows.UI.Xaml.Media.CompositeTransform).TranslateX = 36;

                    if (downloadListView.ItemsSource == null)
                    {
                        downloadListView.ItemsSource = _dataSource.ProgramInfos;
                        await UpdateDownloadState();
                    }
                }
            }
        }

        private void downloadBar_Click(object sender, RoutedEventArgs e)
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
                        TileUtils.CreateToastNotifications("已加入下载，请到个人中心查看");
                    _downloadList.Clear();
                    downloadBar.Visibility = Visibility.Collapsed;
                }
                else
                {
                    TileUtils.CreateToastNotifications("请先至个人中心登录");
                }
                bottomBar.IsOpen = false;
            }
            catch
            {
                TileUtils.CreateToastNotifications("加入下载失败");
            }
        }

        private void downloadListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ProgramInfo;
            if (dataItem != null && !dataItem.IsDownloading)
            {
                dataItem.IsSelected = !dataItem.IsSelected;
                if (dataItem.IsSelected)
                {
                    BottomAppBar.IsSticky = true;
                    BottomAppBar.IsOpen = true;
                    _downloadList.Add(dataItem);
                }
                else
                {
                    _downloadList.Remove(dataItem);
                }
            }

            Visibility downVisibility = Visibility.Visible;
            if (_downloadList.Count <= 0)
            {
                BottomAppBar.IsSticky = false;
                BottomAppBar.IsOpen = false;
                downVisibility = Visibility.Collapsed;
            }
            downloadBar.Visibility = downVisibility;
        }

        private async void AdapteDownloadState()
        {
            await UpdateDownloadState();
        }

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
