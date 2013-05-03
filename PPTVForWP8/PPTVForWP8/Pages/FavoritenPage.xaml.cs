using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Pages
{
    using PPTVData.Factory.Cloud;

    public partial class FavoritenPage : PhoneApplicationPage
    {
        ApplicationBarIconButton _selectBar;
        ApplicationBarIconButton _deletedBar;

        public FavoritenPage()
        {
            InitializeComponent();

            if (LocalFavoritesFactory.Instance.DataInfos.Count >= 4)
                favoritenList.Width = 485;
            else
                favoritenList.Width = 480;

            favoritenList.ItemsSource = LocalFavoritesFactory.Instance.DataInfos;

            _selectBar = new ApplicationBarIconButton();
            _selectBar.IconUri = new Uri("/Images/AppBar/select.png", UriKind.Relative);
            _selectBar.Text = "选择";
            _selectBar.Click += selectBar_Click;

            _deletedBar = new ApplicationBarIconButton();
            _deletedBar.IconUri = new Uri("/Images/AppBar/delete.png", UriKind.Relative);
            _deletedBar.Text = "删除";
            _deletedBar.Click += deletedBar_Click;

            if (favoritenList.IsSelectionEnabled)
            {
                favoritenList.IsSelectionEnabled = true;
            }
            else
            {
                SetUpFavoritenAppbar();
            }
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            if (favoritenList.IsSelectionEnabled)
            {
                favoritenList.IsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        void SetUpFavoritenAppbar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (favoritenList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_deletedBar);
                UpdateFavoritenAppbar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateFavoritenAppbar()
        {
            _deletedBar.IsEnabled = favoritenList.SelectedItems != null && favoritenList.SelectedItems.Count > 0;
        }

        private void selectBar_Click(object sender, EventArgs e)
        {
            favoritenList.IsSelectionEnabled = true;
        }

        private void deletedBar_Click(object sender, EventArgs e)
        {
            LocalFavoritesFactory.Instance.DeleteRecord(favoritenList.SelectedItems);
        }

        private void favoritenList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateFavoritenAppbar();
        }

        private void favoritenList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpFavoritenAppbar();
        }
    }
}