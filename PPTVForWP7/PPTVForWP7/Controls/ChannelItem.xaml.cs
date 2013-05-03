using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVForWP7
{
	public partial class ChannelItem : UserControl
	{
		public ChannelItem()
		{
			// Required to initialize variables
			InitializeComponent();
            _typeid = -1;
		}

        private int _typeid;
        public ImageSource Source
        {
            get
            {
                return ChannelIcon.Source;
            }
            set
            {
                ChannelIcon.Source = value;
            }
        }
        public string Text
        {
            get
            {
                return ChannelText.Text;
            }
            set
            {
                ChannelText.Text = value;
            }
        }
        public int TypeId
        {
            get
            {
                return _typeid;
            }
            set
            {
                _typeid = value;
            }
        }
        public event RoutedEventHandler ChannelClick;
        private void ChannelButton_Click(object sender, RoutedEventArgs e)
        {
            if (ChannelClick != null)
            {
                ChannelClick(this, e);
            }            
        }
	}
}