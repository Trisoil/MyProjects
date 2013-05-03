using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Entity
{
    using Synacast.BoxManage.Core.Entity;

    /// <summary>
    /// 频道实体基类
    /// </summary>
    [Serializable]
    public class VideoBase
    {
        /// <summary>
        /// 频道信息
        /// </summary>
        public ChannelInfo Main { get; set; }

        /// <summary>
        /// BK信息
        /// </summary>
        public BKInfo BKInfo { get; set; }

        /// <summary>
        /// 该频道所属的平台集合
        /// </summary>
        public List<AuthNode> PlatForms { get; set; }

        /// <summary>
        /// 频道名第一个汉字拼音首字母
        /// </summary>
        public List<string> SpellName { get; set; }

        /// <summary>
        /// 频道标记 高清、推荐、最近更新、3D
        /// </summary>
        public List<string> Flags { get; set; }

        /// <summary>
        /// 频道热度
        /// </summary>
        public int Hot { get { return Main.Hot + Main.CorrectHot; } }

        /// <summary>
        /// 频道观看次数
        /// </summary>
        public int PV { get; set; }

        /// <summary>
        /// 频道Tag对应的权限
        /// </summary>
        public Dictionary<string, int> TagsRank { get; set; }

        /// <summary>
        /// 分组频道ID
        /// </summary>
        public List<int> Groups { get; set; }

        /// <summary>
        /// 该频道是否被分配进组里
        /// </summary>
        public bool IsGroup { get; set; }

        /// <summary>
        /// 输出的频道名字
        /// </summary>
        public string ChannelName { get; set; }

        /// <summary>
        /// 输出的频道图片
        /// </summary>
        public string ResImage { get; set; }

        //public string ResResolution { get; set; }

        //public string ResFlags { get; set; }

        public string[] SlotImage { get; set; }

        public string[] ForbiddenAreas { get; set; }
    }
}
