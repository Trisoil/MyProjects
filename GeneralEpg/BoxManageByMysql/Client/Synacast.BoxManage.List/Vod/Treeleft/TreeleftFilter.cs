using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Vod.Treeleft
{
    [Serializable]
    public class TreeleftFilter : ExFilterBase
    {
        public int type { get; set; }

        public int treeleftid { get; set; }

        public int rank { get; set; }
    }
}
