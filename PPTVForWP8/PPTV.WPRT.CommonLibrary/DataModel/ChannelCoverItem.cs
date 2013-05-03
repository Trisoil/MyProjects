using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTV.WPRT.CommonLibrary.DataModel
{
    public class ChannelCoverItem
    {
        public int ChannelId { get; set; }

        public string Title { get; set; }

        public string ImageUri { get; set; }

        public string Note { get; set; }
    }
}
