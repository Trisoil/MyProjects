using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class ChannelDetailInfo
    {
        public int Id { get; set; }

        public string Title { get; set; }

        public int Type { get; set; }

        public string Catalog { get; set; }

        public string Director { get; set; }

        public string Act { get; set; }

        public string Year { get; set; }

        public string Area { get; set; }

        public string ImageUri { get; set; }

        public string SlotUri { get; set; }

        public int State { get; set; }

        public int TotalState { get; set; }

        public double Mark { get; set; }

        public double Duration { get; set; }

        public string Note { get; set; }

        public string Content { get; set; }

        public int ChannelFlag { get; set; }

        public bool IsNumber { get; set; }

        public bool IsVip { get; set; }

        public List<ProgramInfo> ProgramInfos { get; set; }

    }
}
