using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Language;

    [Serializable]
    public class CoverImageNode : LanguagePack<DoubleLanguageNode>
    {
        public int ImageId { get; set; }

        public int ChannelId { get; set; }

        public int ChannelType { get; set; }

        public int NavId { get; set; }

        public string ImageUrl { get; set; }

        public DateTime OnlineTime { get; set; }
    }
}
