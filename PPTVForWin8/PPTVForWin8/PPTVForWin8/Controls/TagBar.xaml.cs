using System;
using System.Collections.Generic;
using PPTVData.Entity;
using PPTVData.Factory;
using PPTVForWin8.Utils;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace PPTVForWin8.Controls
{
    using PPTVData.Utils;

    public sealed partial class TagBar : UserControl
    {
        private class BindData
        {
            public string Name { get; set; }
            public string Value { get; set; }
        }

        private static List<BindData> _timeSoreSources = new List<BindData>() { new BindData() { Name = "最近更新", Value = "n" }, new BindData() { Name = "最高人气", Value = string.Empty }, new BindData() { Name = "最受好评", Value = "g" } };
        private static List<BindData> _programSoreSources = new List<BindData>() { new BindData() { Name = "节目", Value = string.Empty }, new BindData() { Name = "合集", Value = "22" }, new BindData() { Name = "视频", Value = "3" }, new BindData() { Name = "榜单", Value = "23" } };
        private static List<BindData> _bitrateSoreSources = new List<BindData>() { new BindData() { Name = "全部", Value = string.Empty }, new BindData() { Name = "超清", Value = "2"  }, new BindData() { Name = "蓝光", Value = "3" } };

        public Action TagChanged { get; set; }
        public TagInfo[] currentSelectedTagInfos { get; set; }

        public string Order { get; set; }
        public string VT { get; set; }
        public string FT { get; set; }

        public TagBar()
        {
            this.InitializeComponent();

            currentSelectedTagInfos = new TagInfo[3];

            catalogListView.SelectionChanged += listBoxcatalog_SelectionChanged;
            areaListView.SelectionChanged += listBoxAre_SelectionChanged;
            yearListView.SelectionChanged += listBoxYear_SelectionChanged;

            timeSortListView.ItemsSource = _timeSoreSources;
            programSortListView.ItemsSource = _programSoreSources;
            bitrateSortListView.ItemsSource = _bitrateSoreSources;
        }

        public void Clear()
        {
            currentSelectedTagInfos[0] = currentSelectedTagInfos[1] = currentSelectedTagInfos[2] = null;
            Order = VT = FT = string.Empty;

            timeSortListView.SelectedIndex = 1;
            programSortListView.SelectedIndex = 0;
            bitrateSortListView.SelectedIndex = 0;

            catalogScrollViewer.ScrollToHorizontalOffset(0);
            areaScrollViewer.ScrollToHorizontalOffset(0);
            yearScrollViewer.ScrollToHorizontalOffset(0);
        }

        public void SetSources(List<TagInfo> sources, TagDimension dimension)
        {
            switch (dimension)
            {
                case TagDimension.Catalog:
                    catalogListView.ItemsSource = sources;
                    catalogListView.SelectedIndex = 0;
                    break;
                case TagDimension.Area:
                    areaListView.ItemsSource = sources;
                    areaListView.SelectedIndex = 0;
                    break;
                case TagDimension.Year:
                    yearListView.ItemsSource = sources;
                    yearListView.SelectedIndex = 0;
                    break;
            }
        }

        private void listBoxcatalog_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = catalogListView.SelectedItem as TagInfo;
                if (tag != null)
                {
                    currentSelectedTagInfos[0] = tag;
                    OnTagChanged();
                }
            }
        }

        private void listBoxAre_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = areaListView.SelectedItem as TagInfo;
                if (tag != null)
                {
                    currentSelectedTagInfos[1] = tag;
                    OnTagChanged();
                }
            }
        }

        private void listBoxYear_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = yearListView.SelectedItem as TagInfo;
                if (tag != null)
                {
                    currentSelectedTagInfos[2] = tag;
                    OnTagChanged();
                }
            }
        }

        private void timeSortListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var order = timeSortListView.SelectedItem as BindData;
                if (order != null)
                {
                    Order = order.Value;
                    OnTagChanged();
                }
            }
        }

        private void programSortListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var vt = programSortListView.SelectedItem as BindData;
                if (vt != null)
                {
                    VT = vt.Value;
                    OnTagChanged();
                }
            }
        }

        private void bitrateSortListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var ft = bitrateSortListView.SelectedItem as BindData;
                if (ft != null)
                {
                    FT = ft.Value;
                    OnTagChanged();
                }
            }
        }

        private void OnTagChanged()
        {
            if (TagChanged != null)
                TagChanged();
        }

        #region ScrollViewer

        private void filterLeft_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            var scrollViewer = getFilter_ScrollViewer(sender);
            if (scrollViewer != null)
            {
                if (scrollViewer.HorizontalOffset > 0)
                    scrollViewer.ScrollToHorizontalOffset(scrollViewer.HorizontalOffset - 300);
            }
        }

        private void filterRight_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
        {
            var scrollViewer = getFilter_ScrollViewer(sender);
            if (scrollViewer != null)
            {
                if (scrollViewer.HorizontalOffset < scrollViewer.ScrollableWidth)
                    scrollViewer.ScrollToHorizontalOffset(scrollViewer.HorizontalOffset + 300);
            }
        }

        private ScrollViewer getFilter_ScrollViewer(object sender)
        {
            ScrollViewer scrollViewer = null;
            var dataItem = sender as Image;
            if (dataItem != null)
            {
                switch (dataItem.Tag.ToString())
                {
                    case "catalog":
                        scrollViewer = catalogScrollViewer; break;
                    case "area":
                        scrollViewer = areaScrollViewer; break;
                    case "year":
                        scrollViewer = yearScrollViewer; break;
                }
            }
            return scrollViewer;
        }

        #endregion
    }
}
