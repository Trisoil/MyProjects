using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Details
{
    [Serializable]
    public class DetailFilter : ExFilterBase 
    {
        public int vid { get; set; }

        public string order { get; set; }

    }
}
