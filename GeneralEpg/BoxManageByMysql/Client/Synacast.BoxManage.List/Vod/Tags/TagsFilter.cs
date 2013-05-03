using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Tags
{
    [Serializable]
    public class TagsFilter : ExFilterBase
    {
        public int type { get; set; }

        public string dimension { get; set; }
    }
}
