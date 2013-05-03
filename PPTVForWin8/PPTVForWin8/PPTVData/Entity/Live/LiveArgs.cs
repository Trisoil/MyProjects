using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity.Live
{
    public class LiveArgs
    {
        public LiveArgs()
        { }

        public LiveArgs(string name)
        {
            GroupName = name;
        }

        public int PageCount { get; set; }

        public List<LiveChannelInfo> Channels { get; set; }

        public string GroupName { get; set; }
    }
}
