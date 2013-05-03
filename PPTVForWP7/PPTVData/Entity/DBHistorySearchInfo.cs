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
    public class DBHistorySearchInfo
    {
        public DBHistorySearchInfo() { }
        public DBHistorySearchInfo(string key, DateTime date)
        {
            Key = key;
            Date = date;
        }

        [Column(IsPrimaryKey = true, IsDbGenerated = true)]
        public int ID { get; set; }
        [Column]
        public string Key { get; set; }
        [Column]
        public DateTime Date { get; set; }
    }
}
