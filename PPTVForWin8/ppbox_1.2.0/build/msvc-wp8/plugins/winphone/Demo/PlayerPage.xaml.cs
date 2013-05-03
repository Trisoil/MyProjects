using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace Demo
{
    public partial class PlayerPage : PhoneApplicationPage
    {



        public PlayerPage()
        {
            InitializeComponent();

            timer.Interval = new TimeSpan(0, 0, 1);
            timer.Tick += new EventHandler(timer_Tick);
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);

            string url = "";
            if (NavigationContext.QueryString.TryGetValue("url", out url))
            {
                if (url.IndexOf('?') > 0)
                {
                    url += '&';
                }
                else
                {
                    url += '?';
                }
                url += "cdn.live.noshift=true&m3u8.noshift=true";
                video.SetSource(new PpboxSource.PpboxSource(url));
            }
            else
            {
                video.SetSource(new PpboxSource.PpboxSource("pplive2://host/e9301e073cf94732a380b765c8b9573d-5-400"));
            }
            timer.Start();
        }

        private System.Windows.Threading.DispatcherTimer timer =
            new System.Windows.Threading.DispatcherTimer();

        private static String duration_string(double v)
        {
            TimeSpan time = new TimeSpan(0, 0, (int)v);

            return time.ToString();
        }

        private void video_Opened(object sender, RoutedEventArgs e)
        {
            slider.Maximum = (double)(video.NaturalDuration.TimeSpan.TotalSeconds);
            text2.Text = duration_string(slider.Maximum);
        }

        private void playButton_Click(object sender, RoutedEventArgs e)
        {
            video.Play();
            timer.Start();
        }

        private void pauseButton_Click(object sender, RoutedEventArgs e)
        {
            video.Pause();
            timer.Stop();
        }

        private void stopButton_Click(object sender, RoutedEventArgs e)
        {
            video.Stop();
            timer.Stop();
        }

        private void backButton_Click(object sender, RoutedEventArgs e)
        {
            NavigationService.GoBack();
        }

        private void slider_Changed(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            TimeSpan time = new TimeSpan(0, 0, (int)slider.Value);
            TimeSpan time2 = video.Position;
            if (time2.TotalSeconds + 5 < time.TotalSeconds || time2.TotalSeconds > time.TotalSeconds + 5)
            {
                video.Position = time;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            TimeSpan time = video.Position;
            slider.Value = (double)(time.TotalSeconds);
            text1.Text = duration_string(slider.Value);
        }
    }
}