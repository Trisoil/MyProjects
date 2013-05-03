using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WinRT.CommonLibrary.DataModel
{
    public class ChannelRecommendListItem
    {
        public int ChannelId { get; set; }

        public int ChannelFlag { get; set; }

        public double Mark { get; set; }

        public double Duration { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }

        public string Act { get; set; }
    }
}
