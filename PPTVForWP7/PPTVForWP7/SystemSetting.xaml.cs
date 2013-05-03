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
using Microsoft.Phone.Controls;

namespace PPTVForWP7
{
    public partial class SystemSetting : PhoneApplicationPage
    {
        public SystemSetting()
        {
            InitializeComponent();
            tapSlideToggle1.IsChecked = Utils.Utils.GetEnableDefinition();
        }

        private void tapSlideToggle1_Unchecked(object sender, RoutedEventArgs e)
        {
            Utils.Utils.SetEnableDefinition(false);
        }

        private void tapSlideToggle1_Checked(object sender, RoutedEventArgs e)
        {
            Utils.Utils.SetEnableDefinition(true);
        }
    }
}