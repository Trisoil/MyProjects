using System;
using System.Net;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Linq.Expressions;

namespace PPTVData.Entity
{
    public class PlayInfo
    {
        /// <summary>
        /// 服务器IP
        /// </summary>
        public string Sh { get; set; }

        /// <summary>
        /// 服务器时间 HTTP Time Format
        /// </summary>
        public DateTime St { get; set; }

        /// <summary>
        /// 服务器带宽时间情况
        /// </summary>
        public int Bwt { get; set; }

        /// <summary>
        /// 码流率
        /// </summary>
        public int Bitrate { get; set; }

        /// <summary>
        /// Resource ID 可以认为是文件名称
        /// </summary>
        public string Rid { get; set; }

        /// <summary>
        /// File Type
        /// </summary>
        public int ft { get; set; }

        /// <summary>
        /// 分段信息
        /// </summary>
        public List<PlaySegmentInfo> Segments { get; set; }

        /// <summary>
        /// 视频总时长
        /// </summary>
        public double TotalDuration { get; set; }

        public string Key { get; set; }

        public DateTime Expire { get; set; }
    }

    public enum PlayFileType
    {
        Smooth = 0,
        HD = 1,
        SuperHD = 2
    }
}
