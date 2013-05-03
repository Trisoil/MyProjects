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
    public class CoverInfo
    {
        public CoverInfo() { }
        public CoverInfo(string title, string note, int vid, string vType, string mark, string bitrate, int fileLength, string resolution, string playLink, string coverImgUrl, string onlineTime)
        {
            Title = title; Note = note; Vid = vid; Vtype = vType; Mark = mark; Bitrate = bitrate; Filelength = fileLength; Resolution = resolution; PlayLink = playLink; CoverImgUrl = coverImgUrl; OnlineTime = onlineTime;
        }
        public string Title { get; set; }
        public string Note { get; set; }
        public int Vid { get; set; }
        public string Vtype { get; set; }
        public string Mark { get; set; }
        public string Bitrate { get; set; }
        public int Filelength { get; set; }
        public string Resolution { get; set; }
        public string PlayLink { get; set; }
        public string CoverImgUrl { get; set; }
        public string OnlineTime { get; set; }
    }
}
