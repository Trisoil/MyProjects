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
using System.Windows.Threading;

namespace PPTVForWP7.Controls
{
    public partial class PopDialogControl : UserControl
    {
        private DispatcherTimer _timer;

        public PopDialogControl()
        {
            InitializeComponent();
            _timer = new DispatcherTimer();
        }

        /// <summary>
        /// 提示框标题
        /// </summary>
        public string Title
        {
            get { return xTitleText.Text; }
            set { xTitleText.Text = value; }
        }

        /// <summary>
        /// 提示框内容
        /// </summary>
        public string ContentText
        {
            get { return xContentText.Text; }
            set { xContentText.Text = value; }
        }

        /// <summary>
        /// 设置控件透明度
        /// </summary>
        public new double Opacity
        {
            get { return LayoutRoot.Opacity; }
            set { LayoutRoot.Opacity = value; }
        }

        /// <summary>
        /// 是否自动消失
        /// </summary>
        public bool IsAutoHidden { get; set; }

        /// <summary>
        /// 消失间隔
        /// </summary>
        public int HiddenInterval { get; set; }

        /// <summary>
        /// 是否显示
        /// </summary>
        public bool IsShowEnable
        {
            get { return (bool)GetValue(IsShowEnableProperty); }
            set
            {
                SetValue(IsShowEnableProperty, value);
                if (IsAutoHidden && value)
                {
                    AutoHidden();
                }
            }
        }

        /// <summary>
        /// 显示依赖属性
        /// </summary>
        public static DependencyProperty IsShowEnableProperty = DependencyProperty.Register("IsShowEnable",
            typeof(bool), typeof(PopDialogControl),
            new PropertyMetadata(false, OnIsShowEnabledChanged));

        /// <summary>
        /// 显示属性发生更改是回调
        /// </summary>
        /// <param name="d"></param>
        /// <param name="e"></param>
        private static void OnIsShowEnabledChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var dialog = d as PopDialogControl;
            if (dialog != null)
            {
                var value = (bool)e.NewValue;
                if (value &&
                    dialog.Visibility == Visibility.Collapsed)
                {
                    dialog.Visibility = Visibility.Visible;
                }
                var ypoint = 120;
                var storyBoard = value == true ? Utils.Utils.YAnimation(dialog, -ypoint, 0, 0, 0.5) :
                    Utils.Utils.YAnimation(dialog, 0, -ypoint, 0, 0.5);
                storyBoard.Begin();
            }
        }

        /// <summary>
        /// 自动隐藏
        /// </summary>
        private void AutoHidden()
        {
            _timer.Interval = new TimeSpan(0, 0, HiddenInterval);
            _timer.Tick += (o, e) =>
            {
                try
                {
                    IsShowEnable = false;
                    _timer.Stop();
                }
                catch
                {
                    Visibility = Visibility.Collapsed;
                }
            };
            _timer.Start();
        }
    }
}
