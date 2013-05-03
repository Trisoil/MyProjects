using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Live
{
    [Serializable]
    public class LiveFilter : ExFilterBase
    {
        public int vid { get; set; }

        public DateTime date { get; set; }

        public int type { get; set; }

        public string flag { get; set; }

        public string k { get; set; }

        public string rankingspan { get; set; }

        public string order { get; set; }

        public string beginletter { get; set; }
    }
}
