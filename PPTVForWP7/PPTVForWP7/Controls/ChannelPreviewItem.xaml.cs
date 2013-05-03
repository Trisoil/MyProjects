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
using System.Windows.Navigation;

using PPTVData.Entity;
namespace PPTVForWP7.Controls
{

    public partial class ChannelPreviewItem : UserControl
    {
        public ChannelPreviewItem()
        {
            InitializeComponent();
        }

        public delegate void ItemTapHandle(object sender, ChannelInfo channelInfo);
        public event ItemTapHandle ItemTap;

        public int ChannelId
        {
            get { return (int)GetValue(ChannelIdProperty); }
            set { SetValue(ChannelIdProperty, value); }
        }

        public string ImgUrl
        {
            get { return (string)GetValue(ImageUrlProperty); }
            set { SetValue(ImageUrlProperty, value); }
        }

        public static DependencyProperty ChannelIdProperty = DependencyProperty.Register("ChannelId",
        typeof(int), typeof(ChannelPreviewItem), new PropertyMetadata(-1, (o, e) =>
        {
            var item = o as ChannelPreviewItem;
            if (item != null)
            {
                item.ChannelId = (int)e.NewValue;
            }
        }));

        public static DependencyProperty ImageUrlProperty = DependencyProperty.Register("ImgUrl",
        typeof(string), typeof(ChannelPreviewItem), new PropertyMetadata(string.Empty, (o, e) =>
        {
            var item = o as ChannelPreviewItem;
            if (item != null)
            {
                Utils.Utils.GetImage(item.xChannelImage, (string)e.NewValue);
            }
        }));

        private void xItemBorder_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            if (ItemTap != null)
                ItemTap(this, DataContext as ChannelInfo);
        }
    }
}
