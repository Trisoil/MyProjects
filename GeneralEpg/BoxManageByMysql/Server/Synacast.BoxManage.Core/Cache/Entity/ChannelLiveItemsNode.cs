using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Language;
    
    [Serializable]
    public class ChannelLiveItemsNode : LanguagePack<SimpleLanguageNode>
    {
        public int ID { get; set; }

        public int ChannelID { get; set; }

        public DateTime StartTime { get; set; }
    }
}
