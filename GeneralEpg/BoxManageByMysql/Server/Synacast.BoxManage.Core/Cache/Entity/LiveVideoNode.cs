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
    /// 代表一个直播频道实体
    /// </summary>
    [Serializable]
    public class LiveVideoNode : VideoBase 
    {
        /// <summary>
        /// 频道所属类别信息
        /// </summary>
        public List<int> Types { get; set; }

        /// <summary>
        /// 频道的节目预告集合
        /// </summary>
        public List<ChannelLiveItemsNode> Items { get; set; }

        public int Rank { get; set; }

        /// <summary>
        /// 频道是否在线，0表示在线，1表示下线
        /// </summary>
        public int HiddenFlag { get; set; }

        /// <summary>
        /// 语言包
        /// </summary>
        public Dictionary<string, LiveChannelLanguageNode> Language { get; set; }

        #region Override

        public override bool Equals(object obj)
        {
            if (obj == this) return true;
            LiveVideoNode video = obj as LiveVideoNode;
            if (video == null) return false;
            return video.Main.ChannelID == Main.ChannelID;
        }

        public override int GetHashCode()
        {
            return string.Format("livevideonode-{0}", Main.ChannelID).GetHashCode();
        }

        #endregion
    }
}
