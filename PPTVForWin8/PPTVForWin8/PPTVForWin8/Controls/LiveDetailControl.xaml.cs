using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Factory;
    using PPTVData.Entity.Live;
    using PPTVData.Factory.Live;

    using PPTV.WinRT.CommonLibrary.DataModel;

    public sealed partial class LiveDetailControl : UserControl
    {
        string _paradeTime;

        ToggleButton _lastSelectBtn;
        LiveListItem _liveInfo;
        LiveParadeFactory _paradeFactory;

        public Action CloseAction;
        public Action HttpTimeOutOrFail
        {
            get {
                if (_paradeFactory != null)
                    return _paradeFactory.HttpFailorTimeOut;
                return null;
            }
            set {
                if (_paradeFactory != null)
                    _paradeFactory.HttpFailorTimeOut += value;
            }
        }

        public LiveDetailControl()
        {
            this.InitializeComponent();

            btntoday.Content = DateTime.Now.ToString("yyyy-MM-dd");
            btntomorrow.Content = DateTime.Now.AddDays(1).ToString("yyyy-MM-dd");
            btntomorrow1.Content = DateTime.Now.AddDays(2).ToString("yyyy-MM-dd");
            btntomorrow2.Content = DateTime.Now.AddDays(3).ToString("yyyy-MM-dd");
            btntomorrow3.Content = DateTime.Now.AddDays(4).ToString("yyyy-MM-dd");

            _paradeFactory = new LiveParadeFactory();
            _paradeFactory.HttpSucessHandler += paradeFactory_HttpSucessed;
        }

        public void SetSource(LiveListItem liveItem)
        {
            this.DataContext = liveItem;

            if (_lastSelectBtn != null)
                _lastSelectBtn.IsChecked = false;
            _lastSelectBtn = btntoday;
            _lastSelectBtn.IsChecked = true;
            _liveInfo = liveItem;
            _paradeTime = DateTime.Now.ToString("yyyy-MM-dd");

            RequestDatas();
        }

        public void RequestDatas()
        {
            loadingTip.Visibility = Visibility.Visible;
            _paradeFactory.DownLoadDatas(_liveInfo.ChannelInfo.Id, _paradeTime);
        }

        private void paradeFactory_HttpSucessed(object sender, HttpFactoryArgs<List<LiveParadeInfo>> e)
        {
            loadingTip.Visibility = Visibility.Collapsed;

            paradeListView.ItemsSource = e.Result;
        }

        private void closedGrid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            if (CloseAction != null)
                CloseAction();
        }

        private void toggleButton_Click(object sender, RoutedEventArgs e)
        {
             var button = sender as ToggleButton;
             if (button != null)
             {
                 _lastSelectBtn.IsChecked = false;
                 _lastSelectBtn = button;
                 _lastSelectBtn.IsChecked = true;
                 _paradeTime = button.Content.ToString();

                 RequestDatas();
             }
        }

        private void playBtn_Click(object sender, RoutedEventArgs e)
        {
            Play();
        }

        private void paradeListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as LiveParadeInfo;
            if (item != null 
                && item.Index == 0)
            {
                Play();
            }
        }

        private void Play()
        {
            Utils.CommonUtils.GetCurrentFrame().Navigate(typeof(Pages.PlayPage), _liveInfo);
        }
    }
}
