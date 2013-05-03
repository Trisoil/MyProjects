using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Collections.ObjectModel;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Basic Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234237

namespace PPTVForWin8.Pages
{
    using PPTVData;
    using PPTVData.Entity;
    using PPTVData.Factory;
    using PPTVForWin8.Utils;

    using PPTV.WinRT.CommonLibrary;
    using PPTV.WinRT.CommonLibrary.Utils;
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    /// <summary>
    /// A basic page that provides characteristics common to most applications.
    /// </summary>
    public sealed partial class ChannelPage : PPTVForWin8.Common.LayoutAwarePage
    {
        ChannelListFactory _channelFactory;
        TagFactory _tagFactory;

        ChannelTypeItem _typeItem;
        ChannelListViewModel _channelViewModel;
        ScrollViewer _gridViewScrollViewer;
        ScrollViewer _snapGridViewScrollViewer;

        int _groupNmu = 9;
        int _requestNum = 45;
        int _tagIndex = 0;
        int _currentPage = 0;
        int _maxPage = 2;

        public ChannelPage()
        {
            this.InitializeComponent();

            this.Loaded += load_Handler;
            snapChannelListView.LayoutUpdated += load_Handler;
            this.NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Enabled;

            _channelViewModel = new ChannelListViewModel();
            _channelViewModel.Groups = new ObservableCollection<ChannelListGroup>();
            this.DefaultViewModel["Items"] = _channelViewModel.Groups;

            _channelFactory = new ChannelListFactory();
            _channelFactory.HttpSucessHandler += channelFactory_GetChannelsByListSucceed;
            _channelFactory.HttpFailorTimeOut = http_FailorTimeOut;

            _tagFactory = new TagFactory();
            _tagFactory.HttpSucessHandler += tagFactory_GetTagsSucceed;
            _tagFactory.HttpFailorTimeOut = http_FailorTimeOut;
            tagBar.TagChanged += OnTagChanged;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.NavigationMode != NavigationMode.Back)
            {
                _tagIndex = 0;
                tagBar.Clear();
                _typeItem = (ChannelTypeItem)e.Parameter;

                vodTitle.Text = _typeItem.TypeName;
                var itemWidth = ChannelTypeFactory.Instance.GetItemWidth(_typeItem.TypeName);
                var itemHeight = ChannelTypeFactory.Instance.GetItemHeight(_typeItem.TypeName);
                _groupNmu = ScreenUtils.CreateColNum(itemHeight) * 3;
                _requestNum = ScreenUtils.CreateGroupRequestNum(itemWidth, itemHeight, 3);
                
                LoadDatas(true);
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            _channelFactory.Cancel();
            _tagFactory.Cancel();
            if (e.NavigationMode == NavigationMode.Back)
                _channelViewModel.Groups.PerformanceClear();
        }

        private void load_Handler(object sender, object e)
        {
            if (_gridViewScrollViewer == null)
            {
                _gridViewScrollViewer = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollViewer>(channelGridView);
                if (_gridViewScrollViewer != null)
                    _gridViewScrollViewer.ViewChanged += _gridViewScrollViewer_ViewChanged;
            }
            if (_snapGridViewScrollViewer == null)
            {
                _snapGridViewScrollViewer = PPTVData.Utils.DataCommonUtils.FindChildOfType<ScrollViewer>(snapChannelListView);
                if (_snapGridViewScrollViewer != null)
                    _snapGridViewScrollViewer.ViewChanged += snapGridViewScrollViewer_ViewChanged;
            }
        }

        private void RequestDatas()
        {
            if (++_currentPage <= _maxPage
                && loadingTip.Visibility == Visibility.Collapsed)
            {
                _channelFactory.DownLoadDatas(_typeItem.TypeId, tagBar.currentSelectedTagInfos, _currentPage, _requestNum, tagBar.Order, tagBar.VT, tagBar.FT);
                loadingTip.Visibility = Visibility.Visible;
            }
        }

        private void OnTagChanged()
        {
            LoadDatas(false);
        }

        private void LoadDatas(bool isLoadTag)
        {
            loadingTip.Visibility = Visibility.Collapsed;

            if (_gridViewScrollViewer != null)
                _gridViewScrollViewer.ScrollToHorizontalOffset(0);
            if (_snapGridViewScrollViewer != null)
                _snapGridViewScrollViewer.ScrollToVerticalOffset(0);

            _currentPage = 0;
            _maxPage = 2;

            _channelViewModel.Groups.PerformanceClear();
            _tagFactory.Cancel();
            _channelFactory.Cancel();

            if (isLoadTag)
                _tagFactory.DownLoadDatas(_typeItem.TypeId, TagDimension.Catalog);
            RequestDatas();
        }

        #region HTTP Handler

        private void channelFactory_GetChannelsByListSucceed(object sender, HttpFactoryArgs<VodChannelListInfo> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;
            _maxPage = e.Result.PageCount;
            ChannelListGroup group = null;
            for (int i = 0; i < e.Result.Channels.Count; i++)
            {
                if (i % _groupNmu == 0)
                {
                    group = new ChannelListGroup();
                    group.ItemsGroup = new ObservableCollection<ChannelListItem>();
                    _channelViewModel.Groups.Add(group);
                }
                group.ItemsGroup.Add(new ChannelListItem()
                {
                    ChannelId = e.Result.Channels[i].Id,
                    ChannelFlag = e.Result.Channels[i].ChannelFlag,
                    TypeInfo = _typeItem,
                    Mark = e.Result.Channels[i].Mark,
                    ImageUri = ChannelTypeFactory.Instance.GetItemImage(_typeItem.TypeName, e.Result.Channels[i]),
                    Title = e.Result.Channels[i].Title
                });
            }
        }

        private void tagFactory_GetTagsSucceed(object sender, HttpFactoryArgs<TagFactoryInfo> e)
        {
            if (_tagIndex < 3)
            {
                _tagIndex++;
                switch (_tagIndex)
                {
                    case 1:
                        tagBar.SetSources(e.Result.Tags, TagDimension.Catalog);
                        _tagFactory.DownLoadDatas(_typeItem.TypeId, TagDimension.Area);
                        break;
                    case 2:
                        tagBar.SetSources(e.Result.Tags, TagDimension.Area);
                        _tagFactory.DownLoadDatas(_typeItem.TypeId, TagDimension.Year);
                        break;
                    case 3:
                        tagBar.SetSources(e.Result.Tags, TagDimension.Year);
                        break;
                }
            }
        }

        private void http_FailorTimeOut()
        {
            loadingTip.Visibility = Visibility.Collapsed;
            Utils.CommonUtils.ShowMessageConfirm(Constants.VodPageFail,  RequestDatas, "刷新");
        }

        #endregion

        private void _gridViewScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_gridViewScrollViewer != null
                && channelGridView.Items.Count > 0
                && _gridViewScrollViewer.HorizontalOffset >= _gridViewScrollViewer.ScrollableWidth)
            {
                RequestDatas();
            }
        }

        private void snapGridViewScrollViewer_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            if (_snapGridViewScrollViewer != null
                && snapChannelListView.Items.Count > 0
                && _snapGridViewScrollViewer.VerticalOffset >= _snapGridViewScrollViewer.ScrollableHeight)
            {
                RequestDatas();
            }
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            LoadDatas(true);
        }

        private void channelGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var dataItem = e.ClickedItem as ChannelListItem;
            if (dataItem != null)
            {
                if (ChannelTypeFactory.Instance.JudgeVipChannel(_typeItem.TypeName))
                {
                    if (PersonalFactory.Instance.Logined)
                    {
                        if (PersonalFactory.Instance.DataInfos[0].UserStateInfo.VIP != 1)
                        {
                            CommonUtils.ShowMessage(Utils.Constants.NotVip);
                            return;
                        }
                    }
                    else
                    {
                        CommonUtils.ShowMessageConfirm(Utils.Constants.NeedVip, () =>
                        {
                            Utils.CommonUtils.ShowUserLogin((ss, ee) =>
                            {
                                FirstPage.LoginSucess_Handler();
                                channelGridView_ItemClick(sender, e);
                            });
                        }, "登录");
                        return;
                    }
                }
                CommonUtils.DACNavagate(this.Frame, DACPageType.VOD, typeof(DetailPage), dataItem.ChannelId);
            }
        }
    }
}
