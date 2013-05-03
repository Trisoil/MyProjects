using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Key
{
    using Synacast.BoxManage.Core.Cache.Entity;

    public class VidEquailityCompare : IEqualityComparer<VideoNode>
    {
        #region IEqualityComparer<VideoNode> 成员

        public bool Equals(VideoNode x, VideoNode y)
        {
            return x.Main.ChannelID == y.Main.ChannelID;
        }

        public int GetHashCode(VideoNode obj)
        {
            return obj.Main.ChannelID.GetHashCode();
        }

        #endregion
    }
}
