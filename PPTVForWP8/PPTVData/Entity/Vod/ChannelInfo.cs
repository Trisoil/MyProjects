using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class ChannelInfo
    {
        public int Id { get; set; }

        public int Type { get; set; }

        public int ChannelFlag { get; set; }

        public double Mark { get; set; }

        public double Duration { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }

        public string SlotUri { get; set; }

        public string Content { get; set; }

        public string Act { get; set; }
    }

    public enum ChannelFlagType
    { 
        High = 1,
        Recomamnd = 2,
        New = 4,
        Blue = 8,
        D3 = 16
    }
}
