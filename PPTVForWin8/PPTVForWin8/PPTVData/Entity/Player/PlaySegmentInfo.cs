using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity
{
    public class PlaySegmentInfo
    {
        public int No { get; set; }

        public ulong FileSize { get; set; }

        public ulong HeadLength { get; set; }

        public double Duration { get; set; }

        public double TotalDuration { get; set; }
    }
}
