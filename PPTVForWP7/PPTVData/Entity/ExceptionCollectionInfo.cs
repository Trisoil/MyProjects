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
    public class ExceptionCollectionInfo
    {
        public ExceptionCollectionInfo(string time, string msg)
        {
            Time = time;
            ExceptionContent = msg;
        }
        public ExceptionCollectionInfo() { }
        public string Time { get; set; }
        public string ExceptionContent { get; set; }
    }

}
