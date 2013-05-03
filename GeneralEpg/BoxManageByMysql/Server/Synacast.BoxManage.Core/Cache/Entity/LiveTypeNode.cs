using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Language;

    /// <summary>
    /// 直播分类信息描述
    /// </summary>
    [Serializable]
    public class LiveTypeNode : LanguagePack<SimpleLanguageNode>
    {
        public int ID { get; set; }

        public string PicLink { get; set; }

        public Dictionary<VideoNodeKey, List<int>> Channels { get; set; }

    }
}
