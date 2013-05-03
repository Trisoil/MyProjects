using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Cover
{
    [Serializable]
    public class CoverFilter : ExFilterBase 
    {
        public int img_width { get; set; }

        public int img_height { get; set; }

        public string mode { get; set; }

        public int navid { get; set; }

        public int navmode { get; set; }
    }
}
