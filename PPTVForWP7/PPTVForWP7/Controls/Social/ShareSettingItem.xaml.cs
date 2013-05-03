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

namespace PPTVForWP7.Controls
{
    using PPTVData.Entity.Social;

    public partial class ShareSettingItem : UserControl
    {
        public ShareSettingItem()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 社交网站图片
        /// </summary>
        public ImageSource ShareImage
        {
            get
            {
                return xShareImage.Source;
            }
            set
            {
                xShareImage.Source = value;
            }
        }

        /// <summary>
        /// 社交网站名称
        /// </summary>
        public string ShareName
        {
            get
            {
                return xShareName.Text;
            }
            set
            {
                xShareName.Text = value;
            }
        }

        /// <summary>
        /// 是否绑定
        /// </summary>
        public bool IsBinded
        {
            get
            {
                return xShareBindTxt.Content.ToString() == "绑定" ? false : true;
            }
            set
            {
                if (value)
                {
                    xShareBindTxt.Background = new SolidColorBrush(Color.FromArgb(0xFF, 0xFF, 0x84, 00));
                    xShareBindTxt.Content = "取消绑定";
                }
                else
                {
                    xShareBindTxt.Background = new SolidColorBrush(Color.FromArgb(0xFF, 0x00, 0xAE, 0xFF));
                    xShareBindTxt.Content = "绑定";
                }
            }
        }

        /// <summary>
        /// 社交网站类型
        /// </summary>
        public SocialType SocialType { get; set; }

        /// <summary>
        /// 绑定委托
        /// </summary>
        public delegate void BindSocialDelgate(object sender, RoutedEventArgs e);

        /// <summary>
        /// 绑定社交网站触发事件
        /// </summary>
        public event BindSocialDelgate BindSocialEvent;

        /// <summary>
        /// 取消绑定社交网站触发事件
        /// </summary>
        public event BindSocialDelgate UnBindSocialEvent;

        /// <summary>
        /// 绑定Tap事件
        /// </summary>
        protected void Social_Click(object sender, RoutedEventArgs e)
        {
            if (!IsBinded)
            {
                if (BindSocialEvent != null)
                {
                    BindSocialEvent(this, e);
                }
                
            }
            else
            {
                if (UnBindSocialEvent != null)
                {
                    UnBindSocialEvent(this, e);
                }
            }
        }
    }
}
