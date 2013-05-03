using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Live.Search
{
    [Serializable]
    public class LiveSearchFilter : ExFilterBase 
    {
        public string k { get; set; }

        public string flag { get; set; }

        public long lastday { get; set; }
    }
}
