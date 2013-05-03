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
using System.Windows.Media.Imaging;
namespace PPTVForWP7.Controls
{
    public partial class SearchResultItem : UserControl
    {
        public SearchResultItem()
        {
            InitializeComponent();
        }

        public int Vid
        {
            get
            {
                return (int)this.GetValue(VidProperty);
            }
            set
            {
                this.SetValue(VidProperty, value);
            }
        }

        public double Rate
        {
            get
            {
                return (int)this.GetValue(RateProperty);
            }
            set
            {
                this.SetValue(RateProperty, value);
            }
        }
        public int MovieLength
        {
            get
            {
                return (int)this.GetValue(MovieLengthProperty);
            }
            set
            {
                this.SetValue(MovieLengthProperty, value);
            }
        }

        public string MovieCover
        {
            get
            {
                return (string)this.GetValue(MovieCoverProperty);
            }
            set
            {
                this.SetValue(MovieCoverProperty, value);
            }
        }

        public string MovieTitle
        {
            get
            {
                return (string)this.GetValue(MovieTitleProperty);
            }
            set
            {
                this.SetValue(MovieTitleProperty, value);
            }
        }

        public static readonly DependencyProperty RateProperty =
        DependencyProperty.Register("Rate", typeof(double), typeof(SearchResultItem), new PropertyMetadata(-1.0, OnPropertyChanged));

        public static readonly DependencyProperty MovieLengthProperty =
        DependencyProperty.Register("MovieLength", typeof(int), typeof(SearchResultItem), new PropertyMetadata(0, OnPropertyChanged));

        public static readonly DependencyProperty VidProperty =
        DependencyProperty.Register("Vid", typeof(int), typeof(SearchResultItem), new PropertyMetadata(-1, OnPropertyChanged));

        public static readonly DependencyProperty MovieTitleProperty =
        DependencyProperty.Register("MovieTitle", typeof(string), typeof(SearchResultItem), new PropertyMetadata("", OnPropertyChanged));

        public static readonly DependencyProperty MovieCoverProperty =
        DependencyProperty.Register("MovieCover", typeof(string), typeof(SearchResultItem), new PropertyMetadata(null, OnPropertyChanged));

        static void OnPropertyChanged(
            DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            SearchResultItem item = obj as SearchResultItem;

            if (args.Property == RateProperty)
                item.MarkItem.Mark = (double)args.NewValue;

            if (args.Property == MovieLengthProperty)
            {
                string length = String.Format("时长：{0} 分钟", (int)args.NewValue);
                item.Length.Text = length;
            }

            if (args.Property == MovieTitleProperty)
            {
                item.Title.Text = (string)args.NewValue;
            }

            if (args.Property == MovieCoverProperty)
            {
                Utils.Utils.GetImage(item.CoverImage, (string)args.NewValue);
            }

        }


        public event EventHandler<GestureEventArgs> SeachResultTap;

        private void LayoutRoot_Tap(object sender, GestureEventArgs e)
        {
            if (SeachResultTap != null)
            {
                SeachResultTap(this, e);
            }
        }
    }
}
