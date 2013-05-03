using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Language;

    /// <summary>
    /// 直播导航
    /// </summary>
    [Serializable]
    public class LiveRecommandNode : LanguagePack<SimpleLanguageNode>
    {
        public int ID { get; set; }

        public string PicLink { get; set; }

        /// <summary>
        /// 导航所属频道ID集合
        /// </summary>
        public List<LiveVideoNode> Lists { get; set; }

    }
}
