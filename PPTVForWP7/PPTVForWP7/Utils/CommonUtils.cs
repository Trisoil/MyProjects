using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Linq;
using System.Collections.Generic;
using System.Windows.Media.Imaging;

using Microsoft.Phone.Shell;

namespace PPTVForWP7.Utils
{
    using PPTVData.Factory;

    public static class CommonUtils
    {
        #region Properties

        /// <summary>
        /// 等待提示语
        /// </summary>
        public static string LoadingTips = "请稍候";

        /// <summary>
        /// Tile 图片URI
        /// </summary>
        public static Uri TileImageUri = new Uri("/Images/TILE_LOGO.png", UriKind.Relative);

        /// <summary>
        /// Tile 背面图片URI
        /// </summary>
        public static Uri BackTileImageUri = new Uri("/Images/BACK_TILE_LOGO.png", UriKind.Relative);

        /// <summary>
        /// 获取数据超时
        /// </summary>
        public static string HttpTimeOutTips = "服务超时，请检查网络后按刷新按钮";

        /// <summary>
        /// 使用3G网络提示
        /// </summary>
        public static string Use3GNetTips = "检测到您当前正在使用3G网络，运营商会收取流量费用，建议切换到WiFi环境下使用，是否进入个人中心观看本地视频?";

        /// <summary>
        /// 收藏图片地址
        /// </summary>
        public static Uri FavAddImageUri = new Uri("/Images/appbar.favs.addto.rest.png", UriKind.Relative);

        /// <summary>
        /// 已收藏图片地址
        /// </summary>
        public static Uri FavedImageUri = new Uri("/Images/appbar.favs.rest.png", UriKind.Relative);

        /// <summary>
        /// 刷新图片地址
        /// </summary>
        public static Uri RefreshImageUri = new Uri("/Images/appbar.refresh.rest.png",UriKind.Relative);

        public static BitmapImage LeftImage = new BitmapImage(new Uri("/PPTVForWP7;component/Images/left.png",UriKind.Relative));

        public static BitmapImage RightImage = new BitmapImage(new Uri("/PPTVForWP7;component/Images/right.png", UriKind.Relative));

        #endregion

        #region Methods

        public static void HttpTimeOutWarn()
        {
            MessageBox.Show(HttpTimeOutTips);
        }

        /// <summary>
        /// 更新桌面磁贴
        /// </summary>
        /// <param name="page"></param>
        public static void UpdateTile()
        {
            var shell = ShellTile.ActiveTiles.FirstOrDefault();
            if (shell != null)
            {
                try
                {
                    var data = new StandardTileData()
                    {
                        Title = "PPTV网络电视",
                        BackgroundImage = TileImageUri,
                        Count = 0,
                        BackTitle = "PPTV",
                        BackContent = "每个人的网络电视",
                        BackBackgroundImage = BackTileImageUri
                    };
                    shell.Update(data);
                }
                catch { }
            }
        }

        public static ApplicationBarIconButton CreateFavAppBar(int channelId, DBFavourFactory factory, EventHandler handler, bool isPursuit = false)
        {
            var bar = new ApplicationBarIconButton();
            bar.Click += handler;
            if (isPursuit || factory.GetRowByVID(channelId) != null)
            {
                bar.Text = "已收藏";
                bar.IsEnabled = false;
                bar.IconUri = FavedImageUri;
            }
            else
            {
                bar.Text = "收藏";
                bar.IconUri = FavAddImageUri;
            }
            return bar;
        }

        public static ApplicationBarIconButton CreateRefreshAppBar(EventHandler hander)
        {
            var bar = new ApplicationBarIconButton();
            bar.Text = "刷新";
            bar.IconUri = RefreshImageUri;
            bar.Click += hander;
            return bar;
        }

        #endregion
    }
}
