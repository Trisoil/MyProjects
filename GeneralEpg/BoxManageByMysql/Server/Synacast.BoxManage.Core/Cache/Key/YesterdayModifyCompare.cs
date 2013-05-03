using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Key
{
    using Synacast.BoxManage.Core.Cache.Entity;

    public class YesterdayModifyCompare : IEqualityComparer<YesterdayVideoNode>
    {
        #region IEqualityComparer<YesterdayVideoNode> 成员

        public bool Equals(YesterdayVideoNode x, YesterdayVideoNode y)
        {
            return x.ChannelId == y.ChannelId && x.ModifyTime != y.ModifyTime;
        }

        public int GetHashCode(YesterdayVideoNode obj)
        {
            return obj.ChannelId.GetHashCode();
        }

        #endregion
    }
}
