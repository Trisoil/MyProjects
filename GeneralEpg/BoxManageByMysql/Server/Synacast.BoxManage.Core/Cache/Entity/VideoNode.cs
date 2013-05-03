using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Language;

    /// <summary>
    /// 代表一个点播频道实体
    /// </summary>
    [Serializable]
    public class VideoNode : VideoBase
    {
        /// <summary>
        /// 频道所属类别信息
        /// </summary>
        public int Type { get; set; }

        /// <summary>
        /// 播放串
        /// </summary>
        public string PlayLink { get; set; }

        /// <summary>
        /// 语言包
        /// </summary>
        public Dictionary<string, ChannelLanguageNode> Language { get; set; }

        /// <summary>
        /// 频道年份Tag
        /// </summary>
        public string YearTags { get; set; }

        /// <summary>
        /// 合集信息
        /// </summary>
        public List<ViceNode> ViceChannels { get; set; }

        /// <summary>
        /// 虚拟频道ID
        /// </summary>
        public List<int> Virtuals { get; set; }

        /// <summary>
        /// 合集的最大集数
        /// </summary>
        public int State { get; set; }

        /// <summary>
        /// 合集的个数，不同于集数，如电视剧可能少一集等情况（Detail接口使用）
        /// </summary>
        public int StateCount { get; set; }

        /// <summary>
        /// 分集名称是否只包含数字，1只包含
        /// </summary>
        public int IsNumber { get; set; }


        #region Override

        public override bool Equals(object obj)
        {
            VideoNode node = obj as VideoNode;
            if (obj == null) return false;
            return Main.ChannelID == node.Main.ChannelID;
        }

        public override int GetHashCode()
        {
            return Main.ChannelID.GetHashCode();
        }

        #endregion
    }
}
