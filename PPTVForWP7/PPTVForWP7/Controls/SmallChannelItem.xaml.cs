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
using System.Collections.ObjectModel;

namespace PPTVForWP7.Controls
{
    public partial class SmallChannelItem : UserControl
    {
        public delegate void TapItem(object sender, ChannelInfo channelInfo);
        public event TapItem ItemTap;
        public static DependencyProperty ChannelInfosProperty =
            DependencyProperty.Register(
               "ChannelInfos", typeof(ObservableCollection<ChannelInfo>), typeof(SmallChannelItem),
                new PropertyMetadata(new ObservableCollection<ChannelInfo>(), new PropertyChangedCallback(OnChannelInfosChanged)));

        private static void OnChannelInfosChanged(object obj, DependencyPropertyChangedEventArgs args)
        {
            (obj as SmallChannelItem).InitElement();
        }

        public ObservableCollection<ChannelInfo> ChannelInfos
        {
            get { return (ObservableCollection<ChannelInfo>)GetValue(ChannelInfosProperty); }
            set
            {
                SetValue(ChannelInfosProperty, value);
            }
        }

        public SmallChannelItem()
        {
            InitializeComponent();
        }

        private void InitElement()
        {
            if (ChannelInfos.Count >= 1)
            {
                Utils.Utils.GetImage(xChannelImage1,ChannelInfos[0].ImgUrl);
                xTitleTextBlock1.Text = StrHandler(ChannelInfos[0].Title);
                xItemBorder1.Visibility = Visibility.Visible;
                xItemBorder1.Tag = ChannelInfos[0];
            }
            if (ChannelInfos.Count >= 2)
            {
                Utils.Utils.GetImage(xChannelImage2, ChannelInfos[1].ImgUrl);
                //Delay.LowProfileImageLoader.SetUriSource(xChannelImage2, new Uri(ChannelInfos[1].ImgUrl == "" ? "http://none" : ChannelInfos[1].ImgUrl));
                xTitleTextBlock2.Text = StrHandler(ChannelInfos[1].Title);
                xItemBorder2.Visibility = Visibility.Visible;
                xItemBorder2.Tag = ChannelInfos[1];
            }
            if (ChannelInfos.Count >= 3)
            {
                Utils.Utils.GetImage(xChannelImage3, ChannelInfos[2].ImgUrl);
                //Delay.LowProfileImageLoader.SetUriSource(xChannelImage3, new Uri(ChannelInfos[2].ImgUrl == "" ? "http://none" : ChannelInfos[2].ImgUrl));
                xTitleTextBlock3.Text = StrHandler(ChannelInfos[2].Title);
                xItemBorder3.Visibility = Visibility.Visible;
                xItemBorder3.Tag = ChannelInfos[2];
            }
        }

        private string StrHandler(string value)
        {
            string str = (String)value;
            if (str == null)
                return "";
            if (str.Length <= 5)
                return value;
            else
                return str.Substring(0, 5) + "...";
        }

        private void xItemBorder_Tap(object sender, GestureEventArgs e)
        {
            //Border border = sender as Border;
            //ChannelInfo channelInfo = border.Tag as ChannelInfo;
            //if (ItemTap != null)
            //    ItemTap(sender, channelInfo.VID);
        }

        private void Grid_Tap(object sender, GestureEventArgs e)
        {
            Grid grid = sender as Grid;
            Border border = grid.Children[0] as Border;
            ChannelInfo channelInfo = border.Tag as ChannelInfo;
            if (ItemTap != null)
                ItemTap(sender, channelInfo);
        }

    }
}
