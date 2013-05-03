//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Xml.Serialization;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;

//    /// <summary>
//    /// 代表一个频道实体
//    /// </summary>
//    [Serializable]
//    public class VideoNode
//    {
//        /// <summary>
//        /// 频道信息
//        /// </summary>
//        public ChannelInfo Main { get; set; }

//        /// <summary>
//        /// 合集时的分集
//        /// </summary>
//        public List<ViceNode> ViceChannels { get; set; }

//        /// <summary>
//        /// 合集的集数
//        /// </summary>
//        public int State { get; set; }

//        /// <summary>
//        /// 合集的个数，不同于集数，如电视剧可能少一集等情况
//        /// </summary>
//        public int StateCount { get; set; }

//        /// <summary>
//        /// 频道对应的BoxType
//        /// </summary>
//        public int Type { get; set; }

//        /// <summary>
//        /// BK信息
//        /// </summary>
//        public BKInfo BKInfo { get; set; }

//        /// <summary>
//        /// 播放串
//        /// </summary>
//        public string PlayLink { get; set; }

//        /// <summary>
//        /// 频道名中文拼音首字母
//        /// </summary>
//        public string SpellName { get; set; }

//        /// <summary>
//        /// 频道标记 高清、推荐、最近更新、3D
//        /// </summary>
//        public List<string> Flags { get; set; }

//        /// <summary>
//        /// 频道所属BOX类型分类
//        /// </summary>
//        public List<string> CatalogBoxTags { get; set; }

//        /// <summary>
//        /// 频道所属BOX地区分类
//        /// </summary>
//        public List<string> AreaBoxTags { get; set; }

//        /// <summary>
//        /// 频道所属BOX年份分类
//        /// </summary>
//        public List<string> YearBoxTags { get; set; }

//        /// <summary>
//        /// 频道所属BOX演员分类
//        /// </summary>
//        public List<string> ActBkTags { get; set; }

//        /// <summary>
//        /// 频道所属BOX导演分类
//        /// </summary>
//        public List<string> DirectorBKTags { get; set; }

//        /// <summary>
//        /// 频道总时长（单位：分钟）
//        /// </summary>
//        public int Duration { get; set; }

//        #region Responses

//        public string ChannelName { get; set; }

//        public string CatalogTags { get; set; }

//        public string AreaTags { get; set; }

//        public string YearTags { get; set; }

//        public string ResResolution { get; set; }

//        public string ResFlags { get; set; }

//        public string ResImage { get; set; }

//        #endregion

//        public override bool Equals(object obj)
//        {
//            VideoNode node = obj as VideoNode;
//            if (obj == null) return false;
//            return Main.ChannelID == node.Main.ChannelID;
//        }

//        public override int GetHashCode()
//        {
//            return Main.ChannelID.GetHashCode();
//        }
//    }
//}
