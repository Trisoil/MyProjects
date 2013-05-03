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
using PPTVData.Factory;
using System.Collections.ObjectModel;
using PPTVData.Entity;
using PPTVForWP7.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP7
{
    public partial class FavPage : PhoneApplicationPage
    {
        static FavPage()
        {
            TiltEffect.TiltableItems.Add(typeof(FavoriteItem));
        }

        public FavPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(FavPage_Loaded);
        }
        ChannelInfo _currentSelectedChannelInfo = new ChannelInfo();
        private void Init()
        {

        }

        private void LoadFavDB()
        {
            ApplicationBar.Buttons.Clear();

            ApplicationBarIconButton button = new ApplicationBarIconButton(new Uri("/Images/appbar.delete.rest.png", UriKind.RelativeOrAbsolute));
            button.Text = "清除";
            button.Click += new EventHandler(ApplicationBarIconButton_Click);
            ApplicationBar.Buttons.Add(button);
            DBFavourFactory dbFav = new DBFavourFactory();
            var allRows = dbFav.GetAllRows();
            ObservableCollection<FavData> allDatas = new ObservableCollection<FavData>();
            foreach (DBFavourInfo info in allRows)
            {
                FavData data = new FavData();
                data.Id = info.ID;
                data.Vid = info.VID;
                data.ActorText = info.ActorTag;
                data.TitleText = info.Title;
                data.MovieCover = info.ImageUrl;
                data.Rate = info.Mark;
                allDatas.Add(data);
            }
            xMyFav.ItemsSource = allDatas;
        }

        void FavPage_Loaded(object sender, RoutedEventArgs e)
        {
            LoadFavDB();
        }

        private void OnFavoriteDelete(object sender, RoutedEventArgs e)
        {
            DBFavourFactory dbFav = new DBFavourFactory();
            int id = (sender as FavoriteItem).Id;
            if (id != -1)
            {
                dbFav.DeleteRecord(id);
            }
            LoadFavDB();
        }

        private void OnFavoriteDetail(object sender, RoutedEventArgs e)
        {
            int vid = (sender as FavoriteItem).Vid;
            string page = String.Format("/DetailPage.xaml?vid={0}&programSource=6", vid);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }

        private void OnClearFavorite()
        {
            if (MessageBox.Show("删除全部收藏记录?", "提示", MessageBoxButton.OKCancel) == MessageBoxResult.OK)
            {
                DBFavourFactory dbFav = new DBFavourFactory();
                var allRows = dbFav.GetAllRows();
                foreach (DBFavourInfo info in allRows)
                {
                    dbFav.DeleteRecord(info.ID);
                }
            }
            LoadFavDB();
        }
        private void OnFavoriteTap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            FavoriteItem favoriteItem = sender as FavoriteItem;
            _currentSelectedChannelInfo = new ChannelInfo(favoriteItem.Vid, favoriteItem.TitleText, "", favoriteItem.ImageUrl, 0, false, false, false, false, 0, 0, 1, "");
            int vid = favoriteItem.Vid;
            string page = String.Format("/DetailPage.xaml?vid={0}&programSource=6", vid);
            NavigationService.Navigate(new Uri(page, UriKind.RelativeOrAbsolute));
        }

        private void ApplicationBarIconButton_Click(object sender, EventArgs e)
        {
            OnClearFavorite();
        }
    }
}