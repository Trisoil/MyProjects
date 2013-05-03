using System;
using System.Diagnostics;
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
using PPTVData.Entity;
namespace PPTVForWP7.Controls
{
    public partial class SmallProgramItem : UserControl
    {
        static int _no = 0;
        public static int _count = 0;
        public SmallProgramItem()
        {

            InitializeComponent();
            Loaded += new RoutedEventHandler(SmallProgramItem_Loaded);
        }

        public delegate void ItemTapEventHandler(object sender, ProgramInfo programInfo);
        public event ItemTapEventHandler ItemTap;

        public static DependencyProperty ProgramInfosProperty =
            DependencyProperty.Register(
               "ProgramInfos", typeof(List<ProgramInfo>), typeof(SmallProgramItem),
                new PropertyMetadata(new List<ProgramInfo>(), new PropertyChangedCallback(OnProgramInfosChanged)));

        public List<ProgramInfo> ProgramInfos
        {
            get
            {
                return (List<ProgramInfo>)GetValue(ProgramInfosProperty);
            }
            set
            {
                SetValue(ProgramInfosProperty, value);
            }
        }

        private static void OnProgramInfosChanged(object obj, DependencyPropertyChangedEventArgs args)
        {
            Debug.WriteLine("OnProgramInfosChanged " + (obj as SmallProgramItem).Name + " " + (args.NewValue as List<ProgramInfo>)[0].Title);
            (obj as SmallProgramItem).InitElement();
        }

        void InitElement()
        {

            if (ProgramInfos == null)
                return;

            string text = " ";
            for (int i = 0; i < 5; i++)
            {
                if (i > ProgramInfos.Count - 1)
                {
                    Button button = (LayoutRoot.Children[i] as Button);
                    button.Content = "";
                    button.Visibility = Visibility.Collapsed;
                }
                else
                {
                    Debug.WriteLine(_no);
                    Button button = (LayoutRoot.Children[i] as Button);
                    button.Content = ProgramInfos[i].Title;
                    button.Tag = ProgramInfos[i];
                    button.Visibility = Visibility.Visible;
                    text += ProgramInfos[i].Title + " ";
                }

            }

            Debug.WriteLine(this.Name + " InitElement()" + text);
        }

        void SmallProgramItem_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void Button_Tap(object sender, GestureEventArgs e)
        {
            Button button = sender as Button;
            if (ItemTap != null)
                ItemTap(sender, button.Tag as ProgramInfo);
        }
    }
}
