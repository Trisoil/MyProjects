using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Vod.Serach
{
    [Serializable]
    public class TagSearchFilter : ExFilterBase 
    {
        public string k { get; set; }

        public string dimension { get; set; }
    }
}
