using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Language;

    public class ConverImageNode : LanguagePack<DoubleLanguageNode>
    {
        public int Vid { get; set; }

        public string ChannelType { get; set; }

        public string ImageUrl { get; set; }

        public DateTime OnlineTime { get; set; }

        public List<VideoNodeKey> PlatForms { get; set; }
    }
}
