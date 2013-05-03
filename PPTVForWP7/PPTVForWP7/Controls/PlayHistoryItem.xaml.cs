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
    public partial class PlayHistoryItem : UserControl
    {
        public PlayHistoryItem()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 频道ID
        /// </summary>
        public int Vid
        {
            get { return (int)this.GetValue(VidProperty); }
            set { this.SetValue(VidProperty, value); }
        }

        /// <summary>
        /// 频道名称
        /// </summary>
        public string HistoryTitle
        {
            get { return (string)this.GetValue(HistoryTitleProperty); }
            set { this.SetValue(HistoryTitleProperty, value); }
        }

        /// <summary>
        /// 分集索引
        /// </summary>
        public int ProgramIndex
        {
            get { return (int)this.GetValue(ProgramIndexProperty); }
            set { this.SetValue(ProgramIndexProperty, value); }
        }

        /// <summary>
        /// 分集的名称
        /// </summary>
        public string ProgrameTitle
        {
            get { return (string)GetValue(ProgrameTitleProperty); }
            set { SetValue(ProgrameTitleProperty, value); }
        }

        /// <summary>
        /// 是否观看结束
        /// </summary>
        public bool IsOver
        {
            get { return (bool)GetValue(IsOverProperty); }
            set { SetValue(IsOverProperty, value); }
        }

        /// <summary>
        /// 频道观看时刻
        /// </summary>
        public double Position
        {
            get { return (double)GetValue(PositionProperty); }
            set { SetValue(PositionProperty, value); }
        }

        public static readonly DependencyProperty VidProperty = DependencyProperty.Register("Vid",
            typeof(int), typeof(PlayHistoryItem), new PropertyMetadata(-1, (o, e) =>
            {
                var item = o as PlayHistoryItem;
                if (item != null)
                {
                    item.Vid = (int)e.NewValue;
                }
            }));

        public static readonly DependencyProperty ProgramIndexProperty = DependencyProperty.Register("ProgramIndex",
            typeof(int), typeof(PlayHistoryItem), new PropertyMetadata(-1, (o, e) =>
            {
                var item = o as PlayHistoryItem;
                if (item != null)
                {
                    item.ProgramIndex = (int)e.NewValue;
                }
            }));

        public static readonly DependencyProperty ProgrameTitleProperty = DependencyProperty.Register("ProgrameTitle",
           typeof(string), typeof(PlayHistoryItem), new PropertyMetadata(string.Empty, (o, e) =>
           {
               var item = o as PlayHistoryItem;
               if (item != null)
               {
                   item.ProgrameTitle = (string)e.NewValue;
               }
           }));

        public static readonly DependencyProperty HistoryTitleProperty = DependencyProperty.Register("HistoryTitle",
            typeof(string), typeof(PlayHistoryItem), new PropertyMetadata(string.Empty, (o, e) =>
            {
                var item = o as PlayHistoryItem;
                if (item != null)
                {
                    var newValue = (string)e.NewValue;
                    var title = newValue.Length <= 7 ?
                    newValue : string.Format("{0}...", newValue.Substring(0, 7));
                    item.Title.Text = title;
                }
            }));

        public static readonly DependencyProperty IsOverProperty = DependencyProperty.Register("IsOver",
            typeof(bool), typeof(PlayHistoryItem), new PropertyMetadata(false, (o, e) =>
            {
                var item = o as PlayHistoryItem;
                if (item != null)
                {
                    item.IsOver = (bool)e.NewValue;
                }
            }));

        public static readonly DependencyProperty PositionProperty = DependencyProperty.Register("Position",
            typeof(double), typeof(PlayHistoryItem), new PropertyMetadata(-1.0, (o, e) =>
            {
                var item = o as PlayHistoryItem;
                if (item != null)
                {
                    var newValue = (double)e.NewValue;
                    var subTitle = !string.IsNullOrEmpty(item.ProgrameTitle) ? item.ProgrameTitle :
                        (item.ProgramIndex == -1 ? string.Empty : (item.ProgramIndex + 1).ToString());
                    var result = string.Empty;
                    int programeTitle = 0;
                    if (int.TryParse(subTitle, out programeTitle))
                        subTitle = string.Format("第{0}集", subTitle);
                    if (item.IsOver)
                        result = string.Format("{0}{1}", subTitle, "已观看完毕");
                    else
                        result = string.Format("观看到{0}第{1}", subTitle,
                            (programeTitle = (int)(newValue / 60)) > 0 ?
                            string.Format("{0}分钟", programeTitle) : string.Format("{0}秒钟", newValue.ToString(".")));
                    item.Progress.Text = result;
                }
            }));


        public event RoutedEventHandler PlayClick;
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (PlayClick != null)
            {
                PlayClick(this, e);
            }
        }
        public event RoutedEventHandler PlayHistoryDeleteClick;
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (PlayHistoryDeleteClick != null)
            {
                PlayHistoryDeleteClick(this, e);
            }
        }
    }
}
