using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Live.Nav
{
    [Serializable]
    public class LiveNavFilter : ExFilterBase
    {
        public int navid { get; set; }

        public int mode { get; set; }
    }
}
