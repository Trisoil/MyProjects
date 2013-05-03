using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;

namespace PPTVWP7MediaPlayer
{
    class SegmentInfo//分割信息
    {
        public uint no = 0;
        public ulong offset = 0;
        public ulong headlength = 0;
        public double duration = 0;
        public string varid = "";
        public ulong filesize = 0;
        public double timestamp = 0;

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("no=" + no);
            sb.AppendLine("offset=" + offset);
            sb.AppendLine("headlength=" + headlength);
            sb.AppendLine("duration=" + duration);
            sb.AppendLine("varid=" + varid);
            sb.AppendLine("filesize=" + filesize);
            sb.AppendLine("timestamp=" + timestamp);
            return sb.ToString();
        }
    }

    class DragInfo//**信息
    {
        public double timestamp = 0;
        public uint segno = 0;
        public ulong offset = 0;

        public override string ToString()
        {
            return "" + timestamp + ":" + segno + ":" + offset;
        }
    }

    class VideoInfo//视频信息
    {
        public ulong filesize = 0;
        public uint width = 0;
        public uint height = 0;
        public double duration = 0.0;

        public List<SegmentInfo> segs = new List<SegmentInfo>();
        public List<DragInfo> drags = new List<DragInfo>();

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("filesize=" + filesize);
            sb.AppendLine("width=" + width);
            sb.AppendLine("height=" + height);
            sb.AppendLine("duration=" + duration);
            sb.AppendLine("seg count " + segs.Count);
            sb.AppendLine("drag count " + drags.Count);
            return sb.ToString();
        }
    }
}
