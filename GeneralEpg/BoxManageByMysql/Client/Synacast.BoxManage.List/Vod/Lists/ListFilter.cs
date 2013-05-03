using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Lists
{
    [Serializable]
    public class ListFilter : ExFilterBase
    {
        public int type { get; set; }

        public string tag { get; set; }

        public string flag { get; set; }

        public string k { get; set; }

        public string rankingspan { get; set; }

        public string ntags { get; set; }

        public int treeleftid { get; set; }

        public string order { get; set; }

        public string beginletter { get; set; }

        public int shownav { get; set; }
        
    }
}
