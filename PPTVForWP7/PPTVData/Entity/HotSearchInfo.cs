using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVData.Entity
{
    public class HotSearchInfo
    {
        public HotSearchInfo() { }
        public HotSearchInfo(string type, string count, string key)
        {
            Type = type;
            try
            {
                Count = Convert.ToInt32(count);
            }
            catch (Exception)
            {
                Count = 0;
            }
            Key = key;
        }
        public string Type
        {
            get;
            private set;
        }
        public int Count
        {
            get;
            private set;
        }
        public string Key
        {
            get;
            set;
        }
    }
}
