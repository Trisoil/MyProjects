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
    public partial class NavMenuItem : UserControl
    {

        public delegate void NavTypeSelectedEventHandler(object sender, RecommendNav recommendNav);
        public event NavTypeSelectedEventHandler NavSelected;

        public NavMenuItem()
        {
            InitializeComponent();
        }
        
        private RecommendNav _recommendNav = new RecommendNav();
        public RecommendNav RecommendNav
        {
            get { return _recommendNav; }
            set
            {
                _recommendNav = value;
                Init();
            }
        }

        private void Init()
        {
            if (RecommendNav != null)
            {
                if (!string.IsNullOrEmpty(RecommendNav.Name))
                {
                    this.Visibility = Visibility.Visible;
                    xTextBlock1.Text = RecommendNav.Name.Substring(0, 2);
                    xTextBlock2.Text = RecommendNav.Name.Substring(2, 2);
                }
            }
        }

        public void ChangeBgColor(bool isSelected)
        {
            if (isSelected)
                xBackgroundRectangle.Fill = new SolidColorBrush(Utils.Utils.GetColor("FFFF8400"));
            else
                xBackgroundRectangle.Fill = new SolidColorBrush(Utils.Utils.GetColor("FF00AEFF"));
        }

        private void UserControl_Tap(object sender, GestureEventArgs e)
        {
            if (NavSelected != null)
                NavSelected(sender, _recommendNav);
            ChangeBgColor(true);
        }
    }
}
