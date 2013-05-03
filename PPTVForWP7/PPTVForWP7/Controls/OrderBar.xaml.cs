using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVForWP7.Controls
{
    public partial class OrderBar : UserControl
    {
        public OrderBar()
        {
            // 为初始化变量所必需
            InitializeComponent();
        }
        public string SelectOrder;

        public string SelectText;
        Button lastClick;
        //#FF05192B

        private void btnHot_Click(object sender, RoutedEventArgs e)
        {
            enableButtons(sender as Button, "t", "热门");
        }

        private void btnMark_Click(object sender, RoutedEventArgs e)
        {
            enableButtons(sender as Button, "g", "评分");
        }

        private void btnLastest_Click(object sender, RoutedEventArgs e)
        {
            enableButtons(sender as Button, "n", "最新");
        }

        private void enableButtons(Button btn, string order, string text)
        {
            if (lastClick != null)
            {
                lastClick.Foreground = new SolidColorBrush(Colors.White);
            }
            lastClick = btn;
            lastClick.Foreground = new SolidColorBrush(Color.FromArgb(255, 0, 153, 217));
            if (SelectOrder != order)
            {
                SelectOrder = order;
                SelectText = text;
                if (OrderChanged != null)
                {
                    OrderChanged();
                }
            }
        }

        public void ClearnSet()
        {
            SelectOrder = null;
            SelectText = null;
            lastClick.Foreground = new SolidColorBrush(Colors.White);
        }

        public Action OrderChanged;
    }
}