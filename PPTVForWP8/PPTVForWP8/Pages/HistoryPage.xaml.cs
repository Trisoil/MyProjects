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

    public partial class HistoryPage : PhoneApplicationPage
    {
        ApplicationBarIconButton _selectBar;
        ApplicationBarIconButton _deletedBar;

        public HistoryPage()
        {
            InitializeComponent();

            if (LocalRecentFactory.Instance.DataInfos.Count >= 4)
                historyList.Width = 485;
            else
                historyList.Width = 480;

            historyList.ItemsSource = LocalRecentFactory.Instance.DataInfos;

            _selectBar = new ApplicationBarIconButton();
            _selectBar.IconUri = new Uri("/Images/AppBar/select.png", UriKind.Relative);
            _selectBar.Text = "选择";
            _selectBar.Click += selectBar_Click;

            _deletedBar = new ApplicationBarIconButton();
            _deletedBar.IconUri = new Uri("/Images/AppBar/delete.png", UriKind.Relative);
            _deletedBar.Text = "删除";
            _deletedBar.Click += deletedBar_Click;

            if (historyList.IsSelectionEnabled)
            {
                historyList.IsSelectionEnabled = true;
            }
            else
            {
                SetUpHistoryAppbar();
            }
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            if (historyList.IsSelectionEnabled)
            {
                historyList.IsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        void SetUpHistoryAppbar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (historyList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_deletedBar);
                UpdateHistoryAppbar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateHistoryAppbar()
        {
            _deletedBar.IsEnabled = historyList.SelectedItems != null && historyList.SelectedItems.Count > 0;
        }

        private void selectBar_Click(object sender, EventArgs e)
        {
            historyList.IsSelectionEnabled = true;
        }

        private void deletedBar_Click(object sender, EventArgs e)
        {
            LocalRecentFactory.Instance.DeleteRecord(historyList.SelectedItems);
        }

        private void historyList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateHistoryAppbar();
        }

        private void historyList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpHistoryAppbar();
        }
    }
}