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
    public class DBMyDownloadInfo
    {
        public DBMyDownloadInfo()
        {

        }
        public DBMyDownloadInfo(string vid, string titleImgUrl, string title, string fileUrl, string localPathName, long progress, long fileLen, int status,int rate)
        {
            VID = vid;
            TitleImgUrl = titleImgUrl;
            Title = title;
            FileUrl = fileUrl;
            LocalPathName = localPathName;
            Progress = progress;
            FileLen = fileLen;
            Status = status;
            Rate = rate;
          
        }
        [Column(IsPrimaryKey = true)]
        public string VID { get; set; }

        [Column]
        public string TitleImgUrl { get; set; }
        [Column]
        public string Title { get; set; }
        [Column]
        public string FileUrl { get; set; }
        [Column]
        public string LocalPathName { get; set; }
        [Column]
        public long Progress { get; set; }
        [Column]
        public long FileLen { get; set; }
        [Column]
        public int Status { get; set; }
        [Column]
        public int Rate { get; set; }
        [Column]
        public int ChannelVid { get; set; }
       
    }
}
