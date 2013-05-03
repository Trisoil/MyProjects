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
    public partial class SearchBar : UserControl
    {
        public SearchBar()
        {
            InitializeComponent();
        }

        //public string Text
        //{
        //    //get
        //    //{
        //    //    return SearchBox.Text;
        //    //}
        //    //set
        //    //{
        //    //    SearchBox.Text = value;
        //    //}

        //}
        public delegate void InputTextHandler(object sender, string text);
        public event InputTextHandler InputText;

        public ImageSource Source
        {
            get
            {
                return SearchLogo.Source;
            }
            set
            {
                SearchLogo.Source = value;
            }
        }

        private void SearchLogo_Tap(object sender, GestureEventArgs e)
        {
            if (InputText != null && SearchBox.Text.Length > 0)
            {
                InputText(this, SearchBox.Text);
            }
        }

        private void SearchBox_TextInput(object sender, TextCompositionEventArgs e)
        {

            if (InputText != null && SearchBox.Text.Length > 0)
            {
                InputText(this, SearchBox.Text);
            }
        }

        public void SetKeyAndSearch(string key)
        {
            SearchBox.Text = key;
            if (InputText != null && SearchBox.Text.Length > 0)
            {
                InputText(this, SearchBox.Text);
            }
        }

        private void SearchBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
    }
}
