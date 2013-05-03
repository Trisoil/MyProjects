using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Entity.Live
{
    public class LiveChannelInfo
    {
        public LiveChannelInfo(LiveType liveType)
        {
            LiveType = liveType;
        }

        public int Id { get; set; }

        public string Title { get; set; }

        public string NowPlay { get; set; }

        public string WillPlay { get; set; }

        public DateTime StartTime { get; set; }

        public DateTime EndTime { get; set; }

        public LiveType LiveType { get; set; }

        public string ImageUri { get; set; }
    }
}
