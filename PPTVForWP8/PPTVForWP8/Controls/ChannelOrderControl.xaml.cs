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
    public partial class ChannelOrderControl : UserControl
    {
        private class BindData
        {
            public string Name { get; set; }
            public string Value { get; set; }
        }

        private static List<BindData> _timeSoreSources = new List<BindData>() { new BindData() { Name = "最近更新", Value = "n" }, new BindData() { Name = "最高人气", Value = string.Empty }, new BindData() { Name = "最受好评", Value = "g" } };

        public string Order { get; set; }
        public Action<int> OrderChanged { get; set; }

        public ChannelOrderControl()
        {
            InitializeComponent();

            sortList.ItemsSource = _timeSoreSources;
        }

        public void Clear()
        {
            Order = string.Empty;
            sortList.SelectedIndex = 1;
        }

        private void sortList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var order = sortList.SelectedItem as BindData;
                if (order != null)
                {
                    Order = order.Value;
                    if (OrderChanged != null)
                        OrderChanged(-1);
                }
            }
        }
    }
}
