using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity.Player
{
    public class PlayDragInfo
    {
        public PlayDragInfo()
        {
            Segments = new List<SegmentInfo>();
            Drags = new List<DragInfo>();
        }

        public ulong FileSize { get; set; }
        public uint Width { get; set; }
        public uint Height { get; set; }
        public double Duration { get; set; }

        public List<SegmentInfo> Segments { get; set; }
        public List<DragInfo> Drags { get; set; }
    }

    public class SegmentInfo
    {
        public uint No { get; set; }
        public ulong Offset { get; set; }
        public ulong HeadLength { get; set; }
        public double Duration { get; set; }
        public string Varid { get; set; }
        public ulong FileSize { get; set; }
        public double TimeStamp { get; set; }
    }

    public class DragInfo
    {
        public uint No { get; set; }
        public double TimeStamp { get; set; }
        public ulong Offset { get; set; }
    }
}
