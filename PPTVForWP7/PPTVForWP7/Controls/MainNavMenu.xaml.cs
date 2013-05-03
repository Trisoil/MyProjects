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

using PPTVForWP7.Controls;
using PPTVData.Entity;
using PPTVData.Factory;
namespace PPTVForWP7.Controls
{
    public partial class MainNavMenu : UserControl
    {
        public delegate void NavTypeSelectedEventHandler(object sender, RecommendNav recommendNav);
        public event NavTypeSelectedEventHandler NavSelected;

        private RecommendNavFactory _recommendNavFactory;
        private List<RecommendNav> _recommendNavs = new List<RecommendNav>();
        public MainNavMenu()
        {
            InitializeComponent();
            //InitVariableNEventHandle();
            Loaded += new RoutedEventHandler(MainNavMenu_Loaded);
        }

        void MainNavMenu_Loaded(object sender, RoutedEventArgs e)
        {

        }

        public void InitVariableNEventHandle()
        {
            if (_recommendNavs.Count == 0)
            {
                _recommendNavFactory = new RecommendNavFactory();
                _recommendNavFactory.HttpFailHandler += _recommendNavFactory_GetRecommendNavFailed;
                _recommendNavFactory.HttpSucessHandler += _recommendNavFactory_GetRecommendNavSucceed;
                _recommendNavFactory.HttpTimeOutHandler += _recommendNavFactory_GetRecommendNavTimeout;
                _recommendNavFactory.DownLoadDatas();
            }
        }

        void _recommendNavFactory_GetRecommendNavTimeout(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
        }

        void _recommendNavFactory_GetRecommendNavSucceed(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
            _recommendNavs = args.Result;
            InitElement();
        }

        void _recommendNavFactory_GetRecommendNavFailed(object sender, HttpFactoryArgs<List<RecommendNav>> args)
        {
        }

        private void InitElement()
        {
            xNavMenuStackPanel.Children.Clear();
            if (_recommendNavs.Count < 1)
                return;
            for (int i = 0; i < _recommendNavs.Count; i++)
            {
                NavMenuItem navMenuItem = new NavMenuItem();
                navMenuItem.RecommendNav = _recommendNavs[i];
                navMenuItem.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                navMenuItem.NavSelected += new NavMenuItem.NavTypeSelectedEventHandler(navMenuItem_NavSelected);
                if (i != 0)
                    navMenuItem.Margin = new Thickness(16, 0, 0, 0);
                else
                    navMenuItem.ChangeBgColor(true);
                xNavMenuStackPanel.Children.Add(navMenuItem);
            }
        }

        void navMenuItem_NavSelected(object sender, RecommendNav recommendNav)
        {
            xSelectedTextBlock.Text = recommendNav.Name + " ...";
            if (NavSelected != null)
                NavSelected(sender, recommendNav);
            foreach (var item in xNavMenuStackPanel.Children)
            {
                if (item is NavMenuItem)
                    (item as NavMenuItem).ChangeBgColor(false);
            }
        }

        private void xSwitchGrid_Tap(object sender, GestureEventArgs e)
        {
            if (xNavMenuStackPanel.Visibility == Visibility.Visible)
                xNavMenuStackPanel.Visibility = Visibility.Collapsed;
            else
                xNavMenuStackPanel.Visibility = Visibility.Visible;
        }
    }
}
