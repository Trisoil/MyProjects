using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Language;

    [Serializable]
    public class CoverNavNode : LanguagePack<SimpleLanguageNode>
    {
        /// <summary>
        /// 推荐ID
        /// </summary>
        public int Id { get; set; }

        /// <summary>
        /// 推荐点播所属频道ID
        /// </summary>
        public Dictionary<VideoNodeKey, List<int>> Images { get; set; }

        /// <summary>
        /// 推荐图片
        /// </summary>
        public string PicLink { get; set; }
    }

    
}
