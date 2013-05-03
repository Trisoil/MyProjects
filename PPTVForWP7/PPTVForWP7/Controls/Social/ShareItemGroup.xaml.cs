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

    public partial class ShareItemGroup : UserControl
    {
        public ShareItemGroup()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 是否显示
        /// </summary>
        public bool IsShowEnabled
        {
            get { return (bool)GetValue(IsShowEnabledProperty); }
            set { SetValue(IsShowEnabledProperty, value); }
        }

        /// <summary>
        /// 显示动画绑定对象
        /// </summary>
        public static readonly DependencyProperty IsShowEnabledProperty =
            DependencyProperty.Register("IsShowEnabled", typeof(bool), typeof(ShareItemGroup),
            new PropertyMetadata(false, OnIsShowEnabledChanged));

        /// <summary>
        /// 对象值发生改变触发事件
        /// </summary>
        /// <param name="d"></param>
        /// <param name="e"></param>
        private static void OnIsShowEnabledChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var shareControl = d as ShareItemGroup;
            if (shareControl != null)
            {
                var value = (bool)e.NewValue;
                if (value &&
                    shareControl.Visibility == Visibility.Collapsed)
                {
                    shareControl.Visibility = Visibility.Visible;
                }
                var xPoint = System.Windows.Application.Current.Host.Content.ActualHeight ;
                var storyBoard = value == false ? Utils.Utils.XAnimation(shareControl.LayoutRoot, 0, xPoint, 0, 0.5) :
                    Utils.Utils.XAnimation(shareControl.LayoutRoot, -xPoint, 0, 0, 0.5);
                storyBoard.Begin();
            }
        }

        /// <summary>
        /// 取消按钮事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xBtnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.IsShowEnabled = false;
        }

        /// <summary>
        /// 用户控件已加载
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (SocialViewModel.Instance.DataInfos[SocialType.SINA].IsBinded)
            {
                xSinaSocial.IsBinded = true;
            }
            if (SocialViewModel.Instance.DataInfos[SocialType.TENCENT].IsBinded)
            {
                xTencentSocial.IsBinded = true;
            }
            if (SocialViewModel.Instance.DataInfos[SocialType.RENREN].IsBinded)
            {
                xRenrenSocial.IsBinded = true;
            }
        }

        /// <summary>
        /// 分享事件
        /// </summary>
        public event EventHandler<SocialEventArgs> SocialTapEvent;

        /// <summary>
        /// 绑定事件
        /// </summary>
        public event EventHandler<SocialEventArgs> UnBindTapEvent;

        /// <summary>
        /// 分享单击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xSocial_SocialTapEvent(object sender, SocialEventArgs e)
        {
            if (SocialTapEvent != null)
                SocialTapEvent(sender, e);
        }

        /// <summary>
        /// 绑定单击事件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void xSocial_UnBindTapEvent(object sender, SocialEventArgs e)
        {
            if (UnBindTapEvent != null)
                UnBindTapEvent(sender, e);
        }
    }
}
