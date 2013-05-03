using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Nav
{
    [Serializable]
    public class NavFilter : ExFilterBase 
    {
        public int navid { get; set; }

        public int mode { get; set; }

    }
}
