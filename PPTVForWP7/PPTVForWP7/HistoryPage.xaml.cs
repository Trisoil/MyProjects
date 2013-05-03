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
using Microsoft.Phone.Controls;
using PPTVForWP7.Controls;
using PPTVData.Factory;
using System.Collections.ObjectModel;
using PPTVData.Entity;
using Microsoft.Phone.Shell;

namespace PPTVForWP7
{
    public partial class HistoryPage : PhoneApplicationPage
    {
        static HistoryPage()
        {
            TiltEffect.TiltableItems.Add(typeof(PlayHistoryItem));
        }

        public HistoryPage()
        {
            InitializeComponent();
            xMyPlayHistory.ItemsSource = PlayHistoryFactory.Instance.DataInfos;
        }

        private void OnPlayHistoryPlay(object sender, RoutedEventArgs args)
        {
            int vid = (sender as PlayHistoryItem).Vid;
            int programIndex = (sender as PlayHistoryItem).ProgramIndex;
            string page = String.Format("/PlayPage.xaml?vid={0}&programIndex={1}&programSource=5", vid, programIndex);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }

        private void OnPlayHistoryDelete(object sender, RoutedEventArgs args)
        {
            int vid = (sender as PlayHistoryItem).Vid;
            if (vid != -1)
            {
                PlayHistoryFactory.Instance.RemoveEntity(vid);
            }
        }

        private void ApplicationBarIconButton_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("删除全部历史观看记录?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                PlayHistoryFactory.Instance.RemoveAll();
            }
        }
    }
}