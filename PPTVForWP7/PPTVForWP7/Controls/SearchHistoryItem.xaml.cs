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

namespace PPTVForWP7.Controls
{
    public partial class SearchHistoryItem : UserControl
    {
        public SearchHistoryItem()
        {
            InitializeComponent();
            
        }

        private int _vid;       

        public int Vid
        {
            get
            {
                return _vid;
            }

            set
            {
                _vid = value;
            }
        }

        static void OnTextChanged(DependencyObject obj, DependencyPropertyChangedEventArgs args)
        {
            SearchHistoryItem item = obj as SearchHistoryItem;

            if (args.Property == TitleTextProperty)
                item.Title.Text = (string)args.NewValue;

            if (args.Property == DateTextProperty)
                item.Date.Text = (string)args.NewValue;
        }
        public string DateText
        {
            get
            {
                return (string)this.GetValue(DateTextProperty);
            }
            set
            {
                this.SetValue(DateTextProperty, value);
            }
        }
        public static readonly DependencyProperty DateTextProperty = 
            DependencyProperty.Register("DateText", typeof(string), typeof(SearchHistoryItem), new PropertyMetadata("",OnTextChanged));

        public string TitleText
        {
            get
            {
                return  (string)this.GetValue(TitleTextProperty);
            }
            set
            {
                this.SetValue(TitleTextProperty, value);
            }
        }
        public static readonly DependencyProperty TitleTextProperty = DependencyProperty.Register( "TitleText", typeof(string), typeof(SearchHistoryItem), new PropertyMetadata("", OnTextChanged));
    }
}
