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

    using PPTV.WPRT.CommonLibrary.ViewModel.Channel;

    public partial class ChannelTypePage : PhoneApplicationPage
    {
        ApplicationBarIconButton _selectBar;
        ApplicationBarIconButton _pinBar;
        ApplicationBarIconButton _unPinBar;
        Dictionary<object, PivotCallbacks> _callbacks;

        public ChannelTypePage()
        {
            InitializeComponent();

            selectedList.ItemsSource = ChannelTypeFactory.Instance.SelectedViewModel;
            moreList.ItemsSource = ChannelTypeFactory.Instance.AllViewModel[1];

            _callbacks = new Dictionary<object, PivotCallbacks>(2);
            _callbacks[selectedPivotItem] = new PivotCallbacks
            {
                OnActivated = SelectedPivotItemActive,
                OnBackKeyPress = SelectedPivotItemBackPress
            };
            _callbacks[morePivotItem] = new PivotCallbacks
            {
                OnActivated = MorePivotItemActive,
                OnBackKeyPress = MorePivotItemBackPress
            };

            _selectBar = new ApplicationBarIconButton();
            _selectBar.IconUri = new Uri("/Images/AppBar/select.png", UriKind.Relative);
            _selectBar.Text = "选择";
            _selectBar.Click += selectBar_Click;

            _pinBar = new ApplicationBarIconButton();
            _pinBar.IconUri = new Uri("/Images/AppBar/pin.png", UriKind.Relative);
            _pinBar.Text = "固定到首页";
            _pinBar.Click += pinBar_Click;

            _unPinBar = new ApplicationBarIconButton();
            _unPinBar.IconUri = new Uri("/Images/AppBar/unpin.png", UriKind.Relative);
            _unPinBar.Text = "取消固定";
            _unPinBar.Click += unPinBar_Click;
        }

        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(typePivot.SelectedItem, out callbacks) && (callbacks.OnBackKeyPress != null))
            {
                callbacks.OnBackKeyPress(e);
            }
        }

        #region AppBar

        void SelectedPivotItemActive()
        {
            if (selectedList.IsSelectionEnabled)
            {
                selectedList.EnforceIsSelectionEnabled = false;
            }
            else
            {
                SetUpSelectedAppBar();
            }
        }

        void SetUpSelectedAppBar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (selectedList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_unPinBar);
                UpdateSelectedAppBar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateSelectedAppBar()
        {
            _unPinBar.IsEnabled = selectedList.SelectedItems != null && selectedList.SelectedItems.Count > 0;
        }

        void SelectedPivotItemBackPress(CancelEventArgs e)
        {
            if (selectedList.IsSelectionEnabled)
            {
                selectedList.EnforceIsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        void MorePivotItemActive()
        {
            if (moreList.IsSelectionEnabled)
            {
                moreList.EnforceIsSelectionEnabled = false;
            }
            else
            {
                SetUpMoreAppBar();
            }
        }

        void SetUpMoreAppBar()
        {
            Utils.CommonUtils.ClearApplicationBar(this);
            if (moreList.IsSelectionEnabled)
            {
                ApplicationBar.Buttons.Add(_pinBar);
                UpdateMoreAppBar();
            }
            else
            {
                ApplicationBar.Buttons.Add(_selectBar);
            }
        }

        void UpdateMoreAppBar()
        {
            _pinBar.IsEnabled = moreList.SelectedItems != null && moreList.SelectedItems.Count > 0;
        }

        void MorePivotItemBackPress(CancelEventArgs e)
        {
            if (moreList.IsSelectionEnabled)
            {
                moreList.EnforceIsSelectionEnabled = false;
                e.Cancel = true;
            }
        }

        #endregion

        private void Pivot_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PivotCallbacks callbacks;
            if (_callbacks.TryGetValue(typePivot.SelectedItem, out callbacks) && (callbacks.OnActivated != null))
            {
                callbacks.OnActivated();
            }
        }

        private void selectBar_Click(object sender, EventArgs e)
        {
            if (typePivot.SelectedIndex == 0)
                selectedList.EnforceIsSelectionEnabled = true;
            else
                moreList.EnforceIsSelectionEnabled = true;
        }

        private void pinBar_Click(object sender, EventArgs e)
        {
            ChannelTypeFactory.Instance.AddSelectedTypes(moreList.SelectedItems);
        }

        private void unPinBar_Click(object sender, EventArgs e)
        {
            ChannelTypeFactory.Instance.RemoveSelectedTypes(selectedList.SelectedItems);
        }

        private void selectedList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateSelectedAppBar();
        }

        private void selectedList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpSelectedAppBar();
        }

        private void moreList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UpdateMoreAppBar();
        }

        private void moreList_IsSelectionEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetUpMoreAppBar();
        }
    }
}