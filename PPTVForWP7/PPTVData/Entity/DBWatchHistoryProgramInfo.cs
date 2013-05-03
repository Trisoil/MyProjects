//using System;
//using System.Net;
//using System.Windows;
//using System.Windows.Controls;
//using System.Windows.Documents;
//using System.Windows.Ink;
//using System.Windows.Input;
//using System.Windows.Media;
//using System.Windows.Media.Animation;
//using System.Windows.Shapes;
//using System.Data.Linq.Mapping;

//namespace PPTVData.Entity
//{
//    [Table]
//    public class DBWatchHistoryProgramInfo
//    {
//        public DBWatchHistoryProgramInfo() { }
//        public DBWatchHistoryProgramInfo(int vid, int programIndex, double position, string programTitle)
//        {
//            VID = vid;
//            ProgramIndex = programIndex;
//            Position = position;
//            ProgramTitle = programTitle;
//        }
//        [Column(IsPrimaryKey = true, IsDbGenerated = true)]
//        public int ID { get; set; }
//        [Column]
//        public int VID { get; set; }
//        [Column]
//        public int ProgramIndex { get; set; }
//        [Column]
//        public double Position { get; set; }
//        [Column]
//        public string ProgramTitle { get; set; }
//    }
//}
