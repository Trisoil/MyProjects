using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Controls;

namespace PPTVForWin8.Utils
{
    using PPTVData.Entity;
    using PPTVForWin8.Pages;

    using PPTV.WinRT.CommonLibrary;
    using PPTV.WinRT.CommonLibrary.Factory;
    using PPTV.WinRT.CommonLibrary.DataModel;
    using PPTV.WinRT.CommonLibrary.ViewModel;

    public static class PagesUtils
    {
        /// <summary>
        /// 频道Tile导航
        /// </summary>
        /// <param name="page"></param>
        /// <param name="typeItem"></param>
        public static void ChannelTypeNavagite(Page page, ChannelTypeItem typeItem)
        {
            switch (typeItem.TypeId)
            {
                case ChannelTypeFactory.RecentTypeId:
                    page.Frame.Navigate(typeof(HistoryPage));break;
                case ChannelTypeFactory.FavoritenTypeId:
                    page.Frame.Navigate(typeof(FavoritenPage));break;
                case ChannelTypeFactory.DownloadedTypeId:
                    page.Frame.Navigate(typeof(DownloadedPage));break;
                case ChannelTypeFactory.LiveTypeId:
                    page.Frame.Navigate(typeof(LiveChannelPage));break;
                default:
                    page.Frame.Navigate(typeof(ChannelPage), typeItem);break;
            }
        }

        public static void ChangePlayState(MediaElementState mediaState)
        {
            System.Diagnostics.Debug.WriteLine(mediaState.ToString());
            var playPage = CommonUtils.GetCurrentPage() as PlayPage;
            if (playPage != null)
            {
                playPage.ChangePlayState(mediaState);
            }
        }
    }
}
