using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Navigation;
using System.Windows.Controls.Primitives;

using Microsoft.Phone.Shell;
using Microsoft.Phone.Controls;

namespace PPTVForWP8.Pages
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;

    using PPTV.WPRT.CommonLibrary.DataModel;
    using PPTV.WPRT.CommonLibrary.ViewModel;
    using PPTV.WPRT.CommonLibrary.ViewModel.Channel;

    public partial class ChannelPage : PhoneApplicationPage
    {
        int _typeId;
        int _tagIndex = 0;
        int _currentPage = 0;
        int _maxPage = 2;
        int _requestNum = 15;

        TagFactory _tagFactory;
        ChannelListFactory _channelFactory;

        ScrollBar _gridViewScrollViewer;
        ChannelListViewModel _channelViewModel;

        public ChannelPage()
        {
            InitializeComponent();

            this.Loaded += ChannelPage_Loaded;

            _channelViewModel = new ChannelListViewModel();

            _channelFactory = new ChannelListFactory();
            _channelFactory.HttpSucessHandler += channelFactory_GetChannelsByListSucceed;
            _channelFactory.HttpFailorTimeOut = http_FailorTimeOut;

            _tagFactory = new TagFactory();
            _tagFactory.HttpSucessHandler += tagFactory_GetTagsSucceed;
            _tagFactory.HttpFailorTimeOut = http_FailorTimeOut;

            channelList.ItemsSource = _channelViewModel;
            channelTagControl.TagChanged += OnTagChanged;
            channelOrderControl.OrderChanged += OnTagChanged;
            channelSelectedControl.SetSource(channelTagControl);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                _typeId = Convert.ToInt32(NavigationContext.QueryString["id"]);
                var typeName = NavigationContext.QueryString["name"];
                channelTitleText.Text = typeName;

                if (!ChannelTypeFactory.Instance.JudgeLandscapeChannel(typeName))
                {
                    channelList.LayoutMode = LongListSelectorLayoutMode.Grid;
                    channelList.GridCellSize = new Size(154.33, 195);
                    channelList.ItemTemplate = App.Current.Resources["ChannelListTemplate"] as DataTemplate;
                }
                else
                {
                    channelList.LayoutMode = LongListSelectorLayoutMode.List;
                    channelList.ItemTemplate = App.Current.Resources["ChannelSortListTemplate"] as DataTemplate;
                }

                _tagIndex = 0;
                channelTagControl.Clear();
                channelOrderControl.Clear();
                LoadDatas(true);
            }
        }

        void ChannelPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (_gridViewScrollViewer == null)
            {
                _gridViewScrollViewer = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollBar>(channelList);
                if (_gridViewScrollViewer != null)
                    _gridViewScrollViewer.ValueChanged += _gridViewScrollViewer_ValueChanged;
            }
        }

        void OnTagChanged(int index)
        {
            var showTip = channelTagControl.catalogList.SelectedIndex != 0
                || channelTagControl.areaList.SelectedIndex != 0
                || channelTagControl.yearList.SelectedIndex != 0;
            channelTagTip.Visibility = showTip ? Visibility.Visible : Visibility.Collapsed;
            channelSelectedControl.TagSelected(index);
            LoadDatas(false);
        }

        private void LoadDatas(bool isLoadTag)
        {
            loadingTip.Visibility = Visibility.Collapsed;

            _currentPage = 0;
            _maxPage = 2;

            _channelViewModel.PerformanceClear();
            _tagFactory.Cancel();
            _channelFactory.Cancel();

            if (isLoadTag)
                _tagFactory.DownLoadDatas(_typeId, TagDimension.Catalog);
            RequestDatas();
        }

        private void tagFactory_GetTagsSucceed(object sender, HttpFactoryArgs<TagFactoryInfo> e)
        {
            if (_tagIndex < 3)
            {
                _tagIndex++;
                switch (_tagIndex)
                {
                    case 1:
                        channelTagControl.SetSources(e.Result.Tags, TagDimension.Catalog);
                        _tagFactory.DownLoadDatas(_typeId, TagDimension.Area);
                        break;
                    case 2:
                        channelTagControl.SetSources(e.Result.Tags, TagDimension.Area);
                        _tagFactory.DownLoadDatas(_typeId, TagDimension.Year);
                        break;
                    case 3:
                        channelTagControl.SetSources(e.Result.Tags, TagDimension.Year);
                        break;
                }
            }
        }

        private void RequestDatas()
        {
            if (++_currentPage <= _maxPage
                && loadingTip.Visibility == Visibility.Collapsed)
            {
                _channelFactory.DownLoadDatas(_typeId, 
                    channelTagControl.CurrentSelectedTags, _currentPage, _requestNum, channelOrderControl.Order, string.Empty, string.Empty);
                loadingTip.Visibility = Visibility.Visible;
            }
        }

        #region HTTP Handler

        private void channelFactory_GetChannelsByListSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            _maxPage = e.Result.PageCount;

            for (int i = 0; i < e.Result.Channels.Count; i++)
            {
                _channelViewModel.Add(new ChannelListItem()
                {
                    ChannelId = e.Result.Channels[i].Id,
                    ChannelFlag = e.Result.Channels[i].ChannelFlag,
                    Mark = e.Result.Channels[i].Mark,
                    ImageUri = ChannelTypeFactory.Instance.GetItemImage(channelTitleText.Text, e.Result.Channels[i]),
                    Title = e.Result.Channels[i].Title,
                    Duration = e.Result.Channels[i].Duration
                });
            }
        }

        private void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Utils.Constants.HttpFailTipText, RequestDatas);
        }

        #endregion

        private void channelList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var item = channelList.SelectedItem as ChannelListItem;
            if (item != null)
                NavigationService.Navigate(new Uri(string.Format("/Pages/DetailPage.xaml?id={0}", item.ChannelId), UriKind.Relative));
            channelList.SelectedItem = null;
        }

        private void channelList_MouseMove(object sender, MouseEventArgs e)
        {
            if (channelOrderControl.Visibility == Visibility.Visible)
                OrderCloseSB.Begin();
            if (channelTagControl.Visibility == Visibility.Visible)
                TagCloseSB.Begin();
            if (channelSelectedGrid.Visibility == Visibility.Visible)
                SelectedCloseSB.Begin();
        }

        private void _gridViewScrollViewer_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_gridViewScrollViewer != null
                && e.NewValue > e.OldValue
                && e.NewValue + 100 >= _gridViewScrollViewer.Maximum)
            {
                RequestDatas();
            }
        }

        private void channelOrder_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (channelOrderControl.Visibility == Visibility.Collapsed)
            {
                OrderShowSB.Begin();
                TagCloseSB.Begin();
                SelectedCloseSB.Begin();
            }
            else
            {
                OrderCloseSB.Begin();
            }
        }

        private void channelTag_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (channelTagControl.Visibility == Visibility.Collapsed)
            {
                TagShowSB.Begin();
                OrderCloseSB.Begin();
                SelectedCloseSB.Begin();
            }
            else
            {
                TagCloseSB.Begin();
            }
        }

        private void channelTagTip_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (channelSelectedGrid.Visibility == Visibility.Collapsed)
            {
                SelectedShowSB.Begin();
                TagCloseSB.Begin();
                OrderCloseSB.Begin();
            }
            else
            {
                SelectedCloseSB.Begin();
            }
        }

        private void ApplicationBarRefresh_Click(object sender, EventArgs e)
        {
            LoadDatas(true);
        }

    }
}