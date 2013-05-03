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
using System.Data.Linq.Mapping;
namespace PPTVData.Entity
{
     [Table]
    public class LocalPlay
    {
         public LocalPlay()
         {
             VID = string.Empty;
             Position = 0;
         }
        public LocalPlay(string vid, long position)
        {
            VID = vid;
            Position = position;
        }

        [Column(IsPrimaryKey = true)]
        public string VID { get; set; }

        [Column]
        public long Position { get; set; }
        
    }
}
