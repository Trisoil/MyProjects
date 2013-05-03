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
    /// 输出树节点模型
    /// </summary>
    [Serializable]
    public class TreeNode : LanguagePack<DoubleLanguageNode>
    {
        /// <summary>
        /// 树节点深度，根节点为0，以此类推
        /// </summary>
        public int Depth { get; set; }

        /// <summary>
        /// 树节点主信息
        /// </summary>
        public TreeNodeInfo Node { get; set; }

        /// <summary>
        /// 直接孩子节点ID
        /// </summary>
        public List<int> Childrens { get; set; }

        /// <summary>
        /// 所有孩子节点总数
        /// </summary>
        public int ALLChildrens { get; set; }

        /// <summary>
        /// 直属频道ID
        /// </summary>
        public Dictionary<VideoNodeKey, List<int>> Channels { get; set; }

        /// <summary>
        /// 所有孩子节点的频道ID，但不包括直属频道
        /// </summary>
        public Dictionary<VideoNodeKey, List<int>> AllChannels { get; set; }
    }
}
