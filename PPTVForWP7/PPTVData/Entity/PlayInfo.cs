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
using System.Collections.Generic;
using System.Globalization;

namespace PPTVData.Entity
{
    public class PlayInfo
    {
        public class DTInfo
        {
            // 服务器IP 
            public string Sh { get; set; }
            // 服务器时间 HTTP Time Format
            public string St { get; set; }
            // 服务器带宽时间情况
            public int Bwt { get; set; }
            public DTInfo(string sh, string st, int bwt)
            {
                Sh = sh;
                St = st;
                Bwt = bwt;
            }
            public DTInfo()
            {
            }
        }
        public class Item
        {
            public Item(int bitrate, int fileType, string rid, DTInfo dtInfo)
            {
                Bitrate = bitrate;
                FileType = fileType;
                Rid = rid;
                DtInfo = dtInfo;
            }
            public Item()
            {
                Bitrate = 0;
                FileType = -1;
                Rid = "";
            }
            // 码流率
            public int Bitrate { get; set; }
            // File Type
            // 0 高清 1 蓝光 5 流畅 6 比流畅更低码率
            public int FileType { get; set; }
            // Resource ID 可以认为是文件名称
            public string Rid { get; set; }
            // 文件所在的服务器信息
            public DTInfo DtInfo { get; set; }
            public string url
            {
                get
                {
                    if (string.IsNullOrEmpty(this.Rid) || DtInfo == null
                        || string.IsNullOrEmpty(DtInfo.Sh) || string.IsNullOrEmpty(DtInfo.St))
                    {
                        return "";
                    }
                    try
                    {
                        CultureInfo ci = new CultureInfo("en-US");
                        DateTime server_base_time = DateTime.ParseExact(DtInfo.St, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
                        string mediaUrl =  String.Format("http://{0}/{1}?type=wp7&w=1&key={2}" ,DtInfo.Sh, this.Rid, KeyGenerator.GetKey(server_base_time));
                        return mediaUrl;
                    }
                    catch
                    {
                        return string.Empty;
                    }
                    
                }
            }
        }
        public PlayInfo()
        {
            items = new List<Item>();
        }

        public void Add(Item item)
        {
            items.Add(item);
        }
        public void Add(int bitrate, int ft, string rid, DTInfo dtinfo)
        {
            items.Add(new Item(bitrate, ft, rid, dtinfo));
        }
        private int PlayInfoItemGreater(Item lsh, Item rsh)
        {
            return rsh.Bitrate - lsh.Bitrate;
        }
        public void Sort()
        {
            items.Sort(PlayInfoItemGreater);
        }
        public PlayInfo.Item GetItemByFileType(int fileType)
        {
            foreach (var item in items)
            {
                if (item.FileType == fileType)
                {
                    return item;
                }
            }
            return null;
        }
        protected List<Item> items { get; set; }
    }
}
