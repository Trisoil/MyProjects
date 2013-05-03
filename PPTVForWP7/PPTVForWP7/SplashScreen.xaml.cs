using System;
using Microsoft.Phone.Controls;
using System.Windows.Threading;
using System.Windows;
using System.Windows.Navigation;

namespace PPTVForWP7
{
    public partial class SplashScreen : PhoneApplicationPage
    {
        const string KEYSTRING = "HubTag";
        PPTVNavigateMode _navigateMode;

        public SplashScreen()
        {
            Utils.Utils.NetworkInterfaceTypeChanged += new EventHandler(OnNetworkTypeChanged);
            Utils.Utils.CheckNetworkType();
            InitializeComponent();
            _navigateMode = PPTVNavigateMode.MainPage;
        }

        private void OnNetworkTypeChanged(object sender, EventArgs e)
        {
            if (Utils.Utils.CurrentNetworkType == Microsoft.Phone.Net.NetworkInformation.NetworkInterfaceType.MobileBroadbandCdma
                || Utils.Utils.CurrentNetworkType == Microsoft.Phone.Net.NetworkInformation.NetworkInterfaceType.MobileBroadbandGsm)
            {
                if (System.Windows.MessageBoxResult.OK ==
                    MessageBox.Show(Utils.CommonUtils.Use3GNetTips, "友情提示", MessageBoxButton.OKCancel))
                {
                    NavigationService.Navigate(new Uri("/DownPage.xaml?source=splash", UriKind.RelativeOrAbsolute));
                }
            }
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            if (e.NavigationMode != System.Windows.Navigation.NavigationMode.Back)
            {
                Utils.CommonUtils.UpdateTile();
                _navigateMode = 
                    NavigationContext.QueryString.ContainsKey(KEYSTRING) ?
                    PPTVNavigateMode.HubVideo : _navigateMode;
                _navigateMode =
                    NavigationContext.QueryString.ContainsKey("pursuittoast") ?
                    PPTVNavigateMode.ToastNotify : _navigateMode;
            }
            else
            {
                _navigateMode = PPTVNavigateMode.MainPage;
            }
            Navigate();
        }

        private void Navigate()
        {
            if (!Microsoft.Phone.Net.NetworkInformation.DeviceNetworkInformation.IsNetworkAvailable)
            {
                if (System.Windows.MessageBox.Show("没有检测到网络,是否进入个人中心观看本地视频?", "提示", System.Windows.MessageBoxButton.OKCancel)
                    == System.Windows.MessageBoxResult.OK)
                {
                    NavigationService.Navigate(new Uri("/DownPage.xaml?source=splash", UriKind.Relative));
                }
            }
            NetWorkNavigate();
        }

        private void NetWorkNavigate()
        {
            var uri = "/MainPage.xaml";
            try
            {
                switch (_navigateMode)
                {
                    case PPTVNavigateMode.ToastNotify:
                        var toastvid = NavigationContext.QueryString["vid"];
                        var programSource = NavigationContext.QueryString["programSource"];
                        uri = String.Format("/DetailPage.xaml?vid={0}&programSource={1}", toastvid, programSource);
                        break;
                    case PPTVNavigateMode.HubVideo:
                        var hubTag = NavigationContext.QueryString[KEYSTRING].Split(new string[] { "|||" }, StringSplitOptions.RemoveEmptyEntries);
                        var hubvid = hubTag[0];
                        var programIndex = hubTag[1];
                        uri = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource=5", hubvid, programIndex);
                        break;
                }
            }
            catch { }
            NavigationService.Navigate(new Uri(uri, UriKind.RelativeOrAbsolute));
        }
    }

    enum PPTVNavigateMode
    {
        MainPage,
        ToastNotify,
        HubVideo
    }
}