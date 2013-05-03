using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace PPTVForWP8.Controls
{
    public partial class ChannelTagSelectedControl : UserControl
    {
        ChannelTagControl _tagControl;

        public ChannelTagSelectedControl()
        {
            InitializeComponent();
        }

        public void SetSource(ChannelTagControl tagConrtrol)
        {
            _tagControl = tagConrtrol;
        }

        public void TagSelected(int index)
        {
            if (index < 0) return;
            StackPanel panel = catalogSelected;
            Button btn = catalogSelectedBtn;

            if (index == 1)
            {
                panel = areaSelected;
                btn = areaSelectedBtn;
            }
            else if (index == 2)
            {
                panel = yearSelected;
                btn = yearSelectedBtn;
            }

            var name = _tagControl.CurrentSelectedTags[index].Name;
            if (name == "全部")
            {
                panel.Visibility = Visibility.Collapsed;
            }
            else
            {
                panel.Visibility = Visibility.Visible;
                btn.Content = name;
            }
        }

        void catalogSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            _tagControl.catalogList.SelectedIndex = 0;
        }

        void areaSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            _tagControl.areaList.SelectedIndex = 0;
        }

        void yearSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            _tagControl.yearList.SelectedIndex = 0;
        }
    }
}
