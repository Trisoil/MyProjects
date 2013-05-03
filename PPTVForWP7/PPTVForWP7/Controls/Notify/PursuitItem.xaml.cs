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
using System.ComponentModel;
using System.Xml;

namespace PPTVForWP7.Controls
{
    using PPTVData;

    public partial class PursuitItem : UserControl
    {
        public PursuitItem()
        {
            InitializeComponent();
        }

        #region Properties

        /// <summary>
        /// 频道ID
        /// </summary>
        public int ChannelId
        {
            get { return (int)GetValue(ChannelIdProperty); }
            set { SetValue(ChannelIdProperty, value); }
        }

        /// <summary>
        /// 频道缩略图
        /// </summary>
        public string ImageUrl
        {
            get { return (string)GetValue(ImageUrlProperty); }
            set { SetValue(ImageUrlProperty, value); }
        }

        /// <summary>
        /// 频道名称
        /// </summary>
        public string ChannelName
        {
            get { return (string)GetValue(ChannelNameProperty); }
            set { SetValue(ChannelNameProperty, value); }
        }

        /// <summary>
        /// 频道评分
        /// </summary>
        public double Mark
        {
            get { return (double)GetValue(MarkProperty); }
            set { SetValue(MarkProperty, value); }
        }

        /// <summary>
        /// 是否显示删除按钮
        /// </summary>
        public bool IsShowDelte
        {
            get { return (bool)GetValue(IsShowDelteProperty); }
            set { SetValue(IsShowDelteProperty, value); }
        }

        /// <summary>
        /// 已更新了多少集
        /// </summary>
        public int ChannelCount
        {
            get { return (int)GetValue(CountProperty); }
            set { SetValue(CountProperty, value); LoadRefresh(value); }
        }

        /// <summary>
        /// 频道最新更新到多少集
        /// </summary>
        public int NewCount { get; set; }

        public static DependencyProperty ChannelIdProperty = DependencyProperty.Register("ChannelId",
            typeof(int), typeof(PursuitItem), new PropertyMetadata(-1, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    pursuit.ChannelId = (int)e.NewValue;
                    
                }
            }));

        public static DependencyProperty ImageUrlProperty = DependencyProperty.Register("ImageUrl",
            typeof(string), typeof(PursuitItem), new PropertyMetadata(string.Empty, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    PPTVForWP7.Utils.Utils.GetImage(pursuit.xPursuitImage, (string)e.NewValue);
                }
            }));

        public static DependencyProperty ChannelNameProperty = DependencyProperty.Register("ChannelName",
            typeof(string), typeof(PursuitItem), new PropertyMetadata(string.Empty, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    pursuit.xPursuitName.Text = (string)e.NewValue;
                }
            }));

        public static DependencyProperty MarkProperty = DependencyProperty.Register("Mark",
            typeof(double), typeof(PursuitItem), new PropertyMetadata(0.0, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    pursuit.xMarkImageItem.Mark = (double)e.NewValue;
                }
            }));

        public static DependencyProperty IsShowDelteProperty = DependencyProperty.Register("IsShowDelte",
            typeof(bool), typeof(PursuitItem), new PropertyMetadata(false, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    pursuit.xDeleteBtn.Visibility = (bool)e.NewValue ? Visibility.Visible : Visibility.Collapsed; 
                }
            }));

        public static DependencyProperty CountProperty = DependencyProperty.Register("ChannelCount",
            typeof(int), typeof(PursuitItem), new PropertyMetadata(-1, (o, e) =>
            {
                var pursuit = o as PursuitItem;
                if (pursuit != null)
                {
                    pursuit.ChannelCount = (int)e.NewValue;
                }
            }));

        #endregion

        public event RoutedEventHandler PursuitDeleteEvent;

        /// <summary>
        /// 删除事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xDeleteBtn_Click(object sender, RoutedEventArgs e)
        {
            if (PursuitDeleteEvent != null)
                PursuitDeleteEvent(this, e);
        }

        public event EventHandler<GestureEventArgs> PursuitTapEvent;

        /// <summary>
        /// 追剧页点击
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xPursuit_Tap(object sender, GestureEventArgs e)
        {
            xRefreshText.Text = string.Empty;
            if (PursuitTapEvent != null)
                PursuitTapEvent(this, e);
        }

        /// <summary>
        /// 加载频道是否有更新
        /// </summary>
        /// <param name="count"></param>
        private void LoadRefresh(int count)
        {
            var channelId = ChannelId;
            if (channelId > 0)
            {
                xRefreshWait.Visibility = Visibility.Visible;
                var uri = string.Format(PPTVForWP7.Utils.PursuitUtils.DetailEpgUri, EPGSetting.Auth, channelId, EPGSetting.PlatformName);
                HttpAsyncUtils.HttpGet(uri, null, null, response =>
                {
                    try
                    {
                        using (var reader = XmlReader.Create(new System.IO.StringReader(response)))
                        {
                            if (reader.ReadToFollowing("video_list_count"))
                            {
                                NewCount = reader.ReadElementContentAsInt();
                            }
                        }
                    }
                    catch { }
                    Dispatcher.BeginInvoke(() =>
                    {
                        xRefreshWait.Visibility = Visibility.Collapsed;
                        if (NewCount > count)
                        {
                            xRefreshText.Text = string.Format("已更新到{0}集", NewCount);
                        }
                    });
                });
            }
        }
    }
}
