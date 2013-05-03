using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Vod.Serach
{
    [Serializable]
    public class SimpleSearchFilter
    {
        public int c { get; set; }

        public int s { get; set; }

        public string k { get; set; }

        public string rankingspan { get; set; }

        public string auth { get; set; }

        public string lang { get; set; }

        public string platform { get; set; }

        public int ver { get; set; }
    }
}
