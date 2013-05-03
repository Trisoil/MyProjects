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
    public partial class RecommendTitle : UserControl
    {
        public RecommendTitle()
        {
            InitializeComponent();
        }

        public ImageSource Source
        {
            get
            {
                return RecommendTitleLogo.Source;
            }
            set
            {
                RecommendTitleLogo.Source = value;
            }
        }

        public string Text
        {
            get
            {
                return RecommendTitleText.Text;
            }

            set
            {
                RecommendTitleText.Text = value;
            }
        }
    }
}
