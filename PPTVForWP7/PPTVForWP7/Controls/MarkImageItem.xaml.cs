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
    public partial class MarkImageItem : UserControl
    {
        public MarkImageItem()
        {
            InitializeComponent();
        }

        public double Mark
        {
            get { return (double)GetValue(MarkProperty); }
            set
            {
                SetValue(MarkProperty, value);
            }
        }

        public static DependencyProperty MarkProperty = DependencyProperty.Register("Mark", typeof(double), typeof(MarkImageItem), new PropertyMetadata(5.0, new PropertyChangedCallback(OnMarkValueChanged)));
        private static void OnMarkValueChanged(object obj, DependencyPropertyChangedEventArgs args)
        {
            var o = obj as MarkImageItem;
            if (obj != null)
            {
                var value = (int)((double)args.NewValue) / 2;
                o.AdjustRate(value);
            }
        }

        private void Light(Image image, bool enable)
        {
            if (enable)
            {
                image.Source = new BitmapImage(new Uri("/PPTVForWP7;component/Images/star_lighted.png", UriKind.RelativeOrAbsolute));
            }
            else
            {
                image.Source = new BitmapImage(new Uri("/PPTVForWP7;component/Images/star_unlighted.png", UriKind.RelativeOrAbsolute));
            }
        }

        public void AdjustRate(int rate)
        {
            switch (rate)
            {
                case 1:
                    Light(One, true);
                    Light(Two, false);
                    Light(Three, false);
                    Light(Four, false);
                    Light(Five, false);
                    break;
                case 2:
                    Light(One, true);
                    Light(Two, true);
                    Light(Three, false);
                    Light(Four, false);
                    Light(Five, false);
                    break;
                case 3:
                    Light(One, true);
                    Light(Two, true);
                    Light(Three, true);
                    Light(Four, false);
                    Light(Five, false);
                    break;
                case 4:
                    Light(One, true);
                    Light(Two, true);
                    Light(Three, true);
                    Light(Four, true);
                    Light(Five, false);
                    break;
                case 5:
                    Light(One, true);
                    Light(Two, true);
                    Light(Three, true);
                    Light(Four, true);
                    Light(Five, true);
                    break;
                default:
                    Light(One, false);
                    Light(Two, false);
                    Light(Three, false);
                    Light(Four, false);
                    Light(Five, false);
                    break;
            }
        }
    }
}
