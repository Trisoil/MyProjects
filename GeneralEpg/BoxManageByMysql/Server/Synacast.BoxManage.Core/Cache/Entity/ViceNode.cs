using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Cache.Language;

    /// <summary>
    /// 分集信息
    /// </summary>
    [Serializable]
    public class ViceNode : LanguagePack<SimpleLanguageNode> 
    {
        /// <summary>
        /// 频道ID
        /// </summary>
        public int ChannelId { get; set; }

        /// <summary>
        /// 时长
        /// </summary>
        public int Duration { get; set; }

        /// <summary>
        /// 分集播放串
        /// </summary>
        public string PlayLink { get; set; }

    }
}
