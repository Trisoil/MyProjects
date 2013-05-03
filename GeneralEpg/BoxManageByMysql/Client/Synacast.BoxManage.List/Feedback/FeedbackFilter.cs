using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Feedback
{
    [Serializable]
    public class FeedbackFilter
    {
        public string auth { get; set; }

        public int vid { get; set; }

        public double mark { get; set; }
    }
}
