using System;
using System.ComponentModel;
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
    using PPTVForWP8.Common;

    using PPTV.WPRT.CommonLibrary.DataModel.Download;
    using PPTV.WPRT.CommonLibrary.ViewModel.Download;

    public partial class DownloadPage : PhoneApplicationPage
    {
        ApplicationBarIconButton _selectBar;
        ApplicationBarIconButton _deletedBar;

        Dictionary<object, PivotCallbacks> _callbacks;

        public DownloadPage()
        {
            InitializeComponent();

            downloadingList.ItemsSource = DownloadViewModel.Instance.DownloadingItems;
            downloadedList.ItemsSource = DownloadViewModel.Instance.DownloadedItems;

            _callbacks = new Dictionary<object, PivotCallbacks>(2);
            _callbacks[downloadingPivotItem] = new PivotCallbacks
            {
                OnActivated= DownloadingActive,
                OnBackKeyPress = DownloadingBackPress
            };
            _callbacks[downloadedPivotItem] = new PivotCallbacks
            {
                OnActivated = DownloadedActive,
                OnBackKeyPress = DownloadedBackPress
            };

            _selectBar = new ApplicationBarIconButton();
            _selectBar.IconUri = new Uri("/Images/AppBar/select.png", UriKind.Relative);
            _selectBar.Text = "选择";
            _selectBar.Click += selectBar_Click;

            _deletedBar = new ApplicationBarIconButton();
            _deletedBar.IconUri = new Uri("/Images/AppBar/delete.png", UriKind.Relative);
            _deletedBar.Text = "删除";
            _deletedBar.Click += deletedBar_Click;
        }

        protected override void OnBackKeyPress(CancelEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(downLoadPivot.SelectedItem, out callbacks) && (callbacks.OnBackKeyPress != null))
            {
                callbacks.OnBackKeyPress(e);
            }
        }

        #region AppBar

        void DownloadingActive()
        {
            if (downloadingList.IsSelectionEnabled)
            {
                downloadingList.IsSelectionEnabled = true;
            }
            else
            {
                SetUpDownloadingAppbar();
            }
        }

        void SetUpDownloadingAppbar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (downloadingList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_deletedBar);
                UpdateDownloadingAppbar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateDownloadingAppbar()
        {
            _deletedBar.IsEnabled = downloadingList.SelectedItems != null && downloadingList.SelectedItems.Count > 0;
        }

        void DownloadingBackPress(CancelEventArgs e)
        {
            if (downloadingList.IsSelectionEnabled)
            {
                downloadingList.IsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        void DownloadedActive()
        {
            if (downloadedList.IsSelectionEnabled)
            {
                downloadedList.IsSelectionEnabled = true;
            }
            else
            {
                SetUpDownloadedAppbar();
            }
        }

        void SetUpDownloadedAppbar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (downloadedList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_deletedBar);
                UpdateDownloadedAppbar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateDownloadedAppbar()
        {
            _deletedBar.IsEnabled = downloadedList.SelectedItems != null && downloadedList.SelectedItems.Count > 0;
        }

        void DownloadedBackPress(CancelEventArgs e)
        {
            if (downloadedList.IsSelectionEnabled)
            {
                downloadedList.IsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        #endregion

        private void downLoadPivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(downLoadPivot.SelectedItem, out callbacks) && (callbacks.OnActivated != null))
            {
                callbacks.OnActivated();
            }
        }

        private void selectBar_Click(object sender, EventArgs e)
        {
            if (downLoadPivot.SelectedIndex == 0)
                downloadingList.IsSelectionEnabled = true;
            else
                downloadedList.IsSelectionEnabled = true;
        }

        private void deletedBar_Click(object sender, EventArgs e)
        {
            if (downLoadPivot.SelectedIndex == 0)
            {
                DownloadViewModel.Instance.DeletedDownloadings(downloadingList.SelectedItems);
            }
            else
            {
                DownloadViewModel.Instance.DeletedDownloadeds(downloadedList.SelectedItems);
            }
        }

        private void downloadingList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateDownloadingAppbar();
        }

        private void downloadingList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpDownloadingAppbar();
        }

        private void downloadedList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateDownloadedAppbar();
        }

        private void downloadedList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpDownloadedAppbar();
        }
    }
}