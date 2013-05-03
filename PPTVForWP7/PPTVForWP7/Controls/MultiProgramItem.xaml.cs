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
using PPTVData.Entity;
namespace PPTVForWP7.Controls
{
    public partial class MultiProgramItem : UserControl
    {
        public MultiProgramItem()
        {
            InitializeComponent();
        }


        public delegate void ItemTapEventHandler(object sender, ProgramInfo programInfo);
        public event ItemTapEventHandler ItemTap;

        public static DependencyProperty ProgramInfosProperty =
            DependencyProperty.Register(
               "ProgramInfos", typeof(List<ProgramInfo>), typeof(MultiProgramItem),
                new PropertyMetadata(new List<ProgramInfo>(), new PropertyChangedCallback(OnPropertyChanged)));



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

     
        private static void OnPropertyChanged(object obj, DependencyPropertyChangedEventArgs args)
        {            
            (obj as MultiProgramItem).InitElement();
        }

        private static void OnDownIndexChanged(object obj, DependencyPropertyChangedEventArgs e)
        {

        }
        
        private void EnableDownSign(int Index)
        {
            foreach (Grid grid in LayoutRoot.Children)
            {
                Button button = grid.Children[0] as Button;
                ProgramInfo programInfo = button.Tag as ProgramInfo;
                if (programInfo.Index == Index)
                    grid.Children[1].Visibility = Visibility.Visible;
            }
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
                    Grid grid = (LayoutRoot.Children[i] as Grid);

                    if (grid != null)
                    {
                        Button button = (grid.Children[0] as Button);
                        button.Content = "";
                        button.Visibility = Visibility.Collapsed;
                    }
                }
                else
                {

                    Grid grid = (LayoutRoot.Children[i] as Grid);
                    if (grid != null)
                    {
                        Button button = (grid.Children[0] as Button);
                        button.Content = ProgramInfos[i].Title;
                        button.Tag = ProgramInfos[i];
                        button.Visibility = Visibility.Visible;
                        text += ProgramInfos[i].Title + " ";
                        if (ProgramInfos[i].IsDown== System.Windows.Visibility.Visible)
                        {
                            button.Foreground = App.Current.Resources["Orange"]as SolidColorBrush;
                        }
                        else
                        {
                            button.Foreground = new SolidColorBrush(Colors.White);
                        }
                      //      grid.Children[1].Visibility = ProgramInfos[i].IsDown;
                        
                    }
                }
            }
            
            
        }      

        private void Button_Tap(object sender, GestureEventArgs e)
        {            
            Button button = sender as Button;           
            if (ItemTap != null)
                ItemTap(sender, button.Tag as ProgramInfo);
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            InitElement();
        }
    }
}
