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
using PPTVData.Entity;

namespace PPTVForWP7.Controls
{
    public partial class LongChannelItem : UserControl
    {
        public LongChannelItem()
        {
            InitializeComponent();
        }

        public delegate void TapItem(object sender, ChannelInfo channelInfo);
        public event TapItem ItemTap;

        public static DependencyProperty ChannelInfoProperty =
            DependencyProperty.Register(
               "ChannelInfo", typeof(ChannelInfo), typeof(LongChannelItem),
                new PropertyMetadata(new ChannelInfo(), new PropertyChangedCallback(OnChannelInfoChanged)));

        private static void OnChannelInfoChanged(object obj, DependencyPropertyChangedEventArgs args)
        {
            (obj as LongChannelItem).InitElement();
        }

        private void InitElement()
        {
            Utils.Utils.GetImage(xChannelImage, ChannelInfo.ImgUrl);
            xTitleTextBlock.Text = StrHandler(ChannelInfo.Title);
            xDurationTextBlock.Text = ChannelInfo.Duration + "分钟";
            xMarkImageItem.Mark = ChannelInfo.Mark;
        }

        private string StrHandler(string value)
        {
            string str = (String)value;
            if (str == null)
                return "";
            if (str.Length <= 12)
                return value;
            else
                return str.Substring(0, 12) + "...";
        }

        public ChannelInfo ChannelInfo
        {
            get { return (ChannelInfo)GetValue(ChannelInfoProperty); }
            set
            {
                SetValue(ChannelInfoProperty, value);
            }
        }

        private void xItemGrid_Tap(object sender, GestureEventArgs e)
        {
            if (ItemTap != null)
                ItemTap(sender, ChannelInfo);
        }
    }
}
