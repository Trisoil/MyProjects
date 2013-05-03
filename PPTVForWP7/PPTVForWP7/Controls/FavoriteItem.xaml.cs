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
    public partial class FavoriteItem : UserControl
    {
        public FavoriteItem()
        {
            InitializeComponent();
            Vid = -1;
            Id = -1;
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
        public int Id
        {
            get
            {
                return (int)this.GetValue(IdProperty);
            }
            set
            {
                this.SetValue(IdProperty, value);
            }
        }

        public double Rate
        {

            get
            {
                return (double)this.GetValue(RateProperty);
            }
            set
            {
                this.SetValue(RateProperty, value);
            }
        }

        public string ActorText
        {
            get
            {
                return (string)this.GetValue(ActorTextProperty);
            }
            set
            {
                this.SetValue(ActorTextProperty, value);
            }
        }

        public string TitleText
        {
            get
            {
                return (string)this.GetValue(TitleTextProperty);
            }
            set
            {
                this.SetValue(TitleTextProperty, value);
            }
        }
        public ImageSource MovieCover
        {
            get
            {
                return (ImageSource)this.GetValue(MovieCoverProperty);
            }
            set
            {
                this.SetValue(MovieCoverProperty, value);
            }
        }

        public string ImageUrl
        {
            get
            {
                return (string)this.GetValue(ImageUrlProperty);
            }
            set
            {
                this.SetValue(ImageUrlProperty, value);
            }
        }

        public event RoutedEventHandler FavoriteDetailClick;
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (FavoriteDetailClick != null)
            {
                FavoriteDetailClick(this, e);
            }

        }
        public event RoutedEventHandler FavoriteDeleteClick;
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (FavoriteDeleteClick != null)
            {
                FavoriteDeleteClick(this, e);
            }

        }

        public event EventHandler<GestureEventArgs> FavoriteTap;

        private void FavoriteImage_Tap(object sender, GestureEventArgs e)
        {
            if (FavoriteTap != null)
            {
                FavoriteTap(this, e);
            }
        }

        private void ScrollViewer_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            e.Handled = true;
        }

        private void ScrollViewer_ManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        {
            e.Handled = true;
        }
        public static readonly DependencyProperty ActorTextProperty =
        DependencyProperty.Register("ActorText", typeof(string), typeof(FavoriteItem), new PropertyMetadata("", OnPropertyChanged));

        public static readonly DependencyProperty ImageUrlProperty =
       DependencyProperty.Register("ImageUrl", typeof(string), typeof(FavoriteItem), new PropertyMetadata("", OnPropertyChanged));


        public static readonly DependencyProperty TitleTextProperty =
        DependencyProperty.Register("TitleText", typeof(string), typeof(FavoriteItem), new PropertyMetadata("", OnPropertyChanged));

        public static readonly DependencyProperty RateProperty =
        DependencyProperty.Register("Rate", typeof(double), typeof(FavoriteItem), new PropertyMetadata(0.0, OnPropertyChanged));

        public static readonly DependencyProperty VidProperty =
        DependencyProperty.Register("Vid", typeof(int), typeof(FavoriteItem), new PropertyMetadata(-1, OnPropertyChanged));

        public static readonly DependencyProperty IdProperty =
        DependencyProperty.Register("Id", typeof(int), typeof(FavoriteItem), new PropertyMetadata(-1, OnPropertyChanged));

        public static readonly DependencyProperty MovieCoverProperty =
        DependencyProperty.Register("MovieCover", typeof(ImageSource), typeof(FavoriteItem), new PropertyMetadata(null, OnPropertyChanged));

        static void OnPropertyChanged(DependencyObject obj,
                           DependencyPropertyChangedEventArgs args)
        {
            FavoriteItem item = obj as FavoriteItem;

            if (args.Property == RateProperty)
                item.xMarkItem.Mark = (double)args.NewValue;

            if (args.Property == TitleTextProperty)
            {
                item.Title.Text = (string)args.NewValue;
            }
            if (args.Property == ImageUrlProperty)
            {
                Utils.Utils.GetImage(item.FavoriteImage, (string)args.NewValue);
            }
            if (args.Property == MovieCoverProperty)
            {
                item.FavoriteImage.Source = (ImageSource)args.NewValue;
            }

            if (args.Property == ActorTextProperty)
            {
                item.Actor.Text = (string)args.NewValue;
            }

        }
    }
}
