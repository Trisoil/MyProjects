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
    public class DBFavourInfo
    {
        public DBFavourInfo() { }
        public DBFavourInfo(ChannelDetailInfo channelDetailInfo)
        {
            VID = channelDetailInfo.VID;
            Title = channelDetailInfo.Title;
            Mark = channelDetailInfo.Mark;
            ImageUrl = channelDetailInfo.ImgUrl;
            ActorTag = channelDetailInfo.ActTags;
            Content = channelDetailInfo.Content;
        }

        [Column(IsPrimaryKey = true, IsDbGenerated = true)]
        public int ID { get; set; }
        [Column]
        public int VID { get; set; }
        [Column]
        public string Title { get; set; }
        [Column]
        public double Mark { get; set; }
        [Column]
        public string ImageUrl { get; set; }
        [Column]
        public string ActorTag { get; set; }
        [Column]
        public string Content { get; set; }
    }
}
