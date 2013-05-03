using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace PPTVForWin8.Controls
{
    using PPTVData;
    using PPTVData.Factory;
    using PPTVData.Entity.Live;
    using PPTVData.Factory.Live;

    using PPTVForWin8.Utils;
    using PPTVForWin8.Pages;
    using PPTV.WinRT.CommonLibrary.DataModel;

    public class LiveGroupComboBox : ComboBox 
    {
        static int _selectIndex;

        //LiveCMSListFactory _cmsFactory;
        LiveCenterSportsFactory _sportsFactory;
        LiveCenterGameFacotry _gamesFactory;

        public LiveGroupComboBox()
        {
            var source = new List<TextBlock>(5);
            source.Add(CommonUtils.CreateComoboxItem("(今天)", 0));
            source.Add(CommonUtils.CreateComoboxItem(string.Format("({0})", DateTime.Now.AddDays(1).ToString("MM月dd日")), 1));
            source.Add(CommonUtils.CreateComoboxItem(string.Format("({0})", DateTime.Now.AddDays(2).ToString("MM月dd日")), 2));
            source.Add(CommonUtils.CreateComoboxItem(string.Format("({0})", DateTime.Now.AddDays(3).ToString("MM月dd日")), 3));
            source.Add(CommonUtils.CreateComoboxItem(string.Format("({0})", DateTime.Now.AddDays(4).ToString("MM月dd日")), 4));

            _selectIndex = 0;
            this.ItemsSource = source;
            this.SelectedIndex = _selectIndex;

            this.SelectionChanged += LiveGroupComboBox_SelectionChanged;

            //_cmsFactory = new LiveCMSListFactory();
            //_cmsFactory.HttpSucessHandler += listFactory_HttpSucessed;
            //_cmsFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _sportsFactory = new LiveCenterSportsFactory();
            _sportsFactory.HttpSucessHandler += listFactory_HttpSucessed;
            _sportsFactory.HttpFailorTimeOut += http_FailorTimeOut;

            _gamesFactory = new LiveCenterGameFacotry();
            _gamesFactory.HttpSucessHandler += listFactory_HttpSucessed;
            _gamesFactory.HttpFailorTimeOut += http_FailorTimeOut;
        }

        void LiveGroupComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var page = CommonUtils.GetCurrentPage() as LiveChannelPage;
            if (page == null) return;
            page.LoadingTipVisibility = Visibility.Visible;

            _selectIndex = this.SelectedIndex;

            var text = this.SelectedItem as TextBlock;
            var addDay = Convert.ToInt32(text.Tag);
            var paras = DateTime.Now.AddDays(addDay).ToString("yyyy-MM-dd");

            //if (Tag.ToString() == LiveType.Recommend.CreateString())
            //    _cmsFactory.DownLoadDatas(paras);
            if (Tag.ToString() == LiveType.Sports.CreateString())
                _sportsFactory.DownLoadDatas(paras);
            else if (Tag.ToString() == LiveType.Games.CreateString())
                _gamesFactory.DownLoadDatas(paras);
        }

        #region HttpHandler

        void listFactory_HttpSucessed(object sender, HttpFactoryArgs<LiveArgs> e)
        {
            var page = CommonUtils.GetCurrentPage() as LiveChannelPage;
            if (page == null) return;
            page.LoadingTipVisibility = Visibility.Collapsed;

            LiveListGroup group = null;
            //if (Tag.ToString() == LiveType.Recommend.CreateString())
            //    group = page.ViewModel.Groups[0];
            if (Tag.ToString() == LiveType.Sports.CreateString())
                group = page.ViewModel.Groups[2];
            else
                group = page.ViewModel.Groups[3];

            var newCount = e.Result.Channels.Count;
            group.ItemsGroup.PerformanceClear();

            for (var i = 0; i < newCount; i++)
            {
                group.ItemsGroup.Add(new LiveListItem() { ChannelInfo = e.Result.Channels[i] });
            }

            if (newCount <= 0)
            {
                group.ItemsGroup.Add(new LiveListItem() { ChannelInfo = new LiveChannelInfo(LiveType.Sports) { Id = 0, Title = string.Format("暂无{0}节目", group.GroupName), StartTime = DateTime.Now.AddDays(1) } });
            }
        }

        void http_FailorTimeOut()
        {
            var page = CommonUtils.GetCurrentPage() as LiveChannelPage;
            if (page == null) return;
            page.http_FailorTimeOut();
        }

        #endregion
    }
}
