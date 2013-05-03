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
    public class DBWatchHistoryChanneInfo
    {
        [Column(IsPrimaryKey = true)]
        public int VID { get; set; }

        [Column]
        public string Title { get; set; }

        /// <summary>
        /// -1表示 单集电影,其它为有连续剧的电影
        /// </summary>
        [Column]
        public int ProgramIndex { get; set; }

        /// <summary>
        /// 子集的名称
        /// </summary>
        [Column]
        public string ProgrameTitle { get; set; }

        [Column]
        public double Position { get; set; }

        [Column]
        public DateTime Time { get; set; }

        [Column]
        public bool IsOver { get; set; }
    }
}
