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
    using PPTVData.Entity;

    public partial class ChannelTagControl : UserControl
    {
        public Action<int> TagChanged { get; set; }
        public TagInfo[] CurrentSelectedTags { get; private set; }

        public ChannelTagControl()
        {
            InitializeComponent();

            catalogList.SelectionChanged += catalogList_SelectionChanged;
            areaList.SelectionChanged += areaList_SelectionChanged;
            yearList.SelectionChanged += yearList_SelectionChanged;

            selectedControl.SetSource(this);
            CurrentSelectedTags = new TagInfo[3];
        }

        public void Clear()
        {
            CurrentSelectedTags[0] = CurrentSelectedTags[1] = CurrentSelectedTags[2] = null;

            catalogList.ScrollIntoView(null);
            areaList.ScrollIntoView(null);
            yearList.ScrollIntoView(null);
        }

        public void SetSources(List<TagInfo> sources, TagDimension dimension)
        {
            switch (dimension)
            {
                case TagDimension.Catalog:
                    catalogList.ItemsSource = sources;
                    catalogList.SelectedIndex = 0;
                    break;
                case TagDimension.Area:
                    areaList.ItemsSource = sources;
                    areaList.SelectedIndex = 0;
                    break;
                case TagDimension.Year:
                    yearList.ItemsSource = sources;
                    yearList.SelectedIndex = 0;
                    break;
            }
        }

        void catalogList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = catalogList.SelectedItem as TagInfo;
                if (tag != null)
                {
                    CurrentSelectedTags[0] = tag;
                    TagSelected(0);
                }
            }
        }

        void areaList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = areaList.SelectedItem as TagInfo;
                if (tag != null)
                {
                    CurrentSelectedTags[1] = tag;
                    TagSelected(1);
                }
            }
        }

        void yearList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count > 0)
            {
                var tag = yearList.SelectedItem as TagInfo;
                if (tag != null)
                {
                    CurrentSelectedTags[2] = tag;
                    TagSelected(2);
                }
            }
        }

        void catalogSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            catalogList.SelectedIndex = 0;
        }

        void areaSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            areaList.SelectedIndex = 0;
        }

        void yearSelectedBtn_Click(object sender, RoutedEventArgs e)
        {
            yearList.SelectedIndex = 0;
        }

        void TagSelected(int index)
        {
            var showTip = catalogList.SelectedIndex != 0
                || areaList.SelectedIndex != 0
                || yearList.SelectedIndex != 0;
            selectedControl.TagSelected(index);

            if (TagChanged != null)
                TagChanged(index);
        }
    }
}
