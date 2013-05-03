using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Update
{
    [Serializable]
    public class UpdateFilter
    {
        public int c { get; set; }

        public int s { get; set; }

        public DateTime lasttime { get; set; }
    }
}
