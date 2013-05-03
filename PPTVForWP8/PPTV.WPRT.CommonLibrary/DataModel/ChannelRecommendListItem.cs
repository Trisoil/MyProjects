using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.DataModel
{
    public class ChannelRecommendListItem
    {
        public int Index { get; set; }

        public string GruopName { get; set; }

        public int GroupCount { get; set; }

        public int ChannelId { get; set; }

        public int ChannelFlag { get; set; }

        public double Mark { get; set; }

        public double Duration { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }

        //public string Act { get; set; }
    }
}
