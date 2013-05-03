using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Vod.Relevance
{
    [Serializable]
    public class RelevanceFilter : ExFilterBase 
    {
        public string vids { get; set; }

    }
}
