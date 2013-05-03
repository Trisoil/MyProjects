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

    public partial class ShareItem : UserControl
    {
        public ShareItem()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 社交网站名称
        /// </summary>
        public string Text
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
        /// 社交网站图片
        /// </summary>
        public ImageSource Image
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
        /// 是否绑定
        /// </summary>
        public bool IsBinded
        {
            get
            {
                return xBinded.Text == "已绑定" ? true : false;
            }
            set
            {
                if (value)
                {
                    LayoutRoot.Background = new SolidColorBrush(Color.FromArgb(255, 240, 131, 7));
                    xBinded.Text = "已绑定";
                }
                else
                {
                    LayoutRoot.Background = new SolidColorBrush(Color.FromArgb(255, 0, 140, 214));
                    xBinded.Text = "未绑定";
                }
            }
        }

        /// <summary>
        /// 社交网站类型
        /// </summary>
        public SocialType SocialType { get; set; }

        /// <summary>
        /// 社交网站处理类型
        /// </summary>
        public SocialBase SocialData { get; set; }

        /// <summary>
        /// 分享单击事件
        /// </summary>
        public event EventHandler<SocialEventArgs> SocialTapEvent;

        /// <summary>
        /// 未绑定单击事件
        /// </summary>
        public event EventHandler<SocialEventArgs> UnBindTapEvent;

        private void LayoutRoot_Tap(object sender, GestureEventArgs e)
        {
            if (IsBinded)
            {
                if (SocialTapEvent != null)
                    SocialTapEvent(this, new SocialEventArgs(SocialType, SocialData));
            }
            else
            {
                if (UnBindTapEvent != null)
                    UnBindTapEvent(this, new SocialEventArgs(SocialType, SocialData));
            }
        }
    }

    /// <summary>
    /// 分享单击事件传递参数
    /// </summary>
    public class SocialEventArgs : EventArgs
    {
        public SocialEventArgs(SocialType socialType, SocialBase socialData)
        {
            SocialType = socialType;
            SocialData = socialData;
        }

        public SocialBase SocialData { get; set; }

        public SocialType SocialType { get; set; }
    }
}
