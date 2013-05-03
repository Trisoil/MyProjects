using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    [Serializable]
    public class YesterdayVideoNode
    {
        public int ChannelId { get; set; }

        public DateTime ModifyTime { get; set; }
    }
}
