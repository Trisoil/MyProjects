using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal.Key
{
    using Synacast.BoxManage.Core.Entity;

    public class BKInfoComparer : IEqualityComparer<BKInfo>
    {
        #region IEqualityComparer<BKInfo> 成员

        public bool Equals(BKInfo x, BKInfo y)
        {
            return x.BKTypeId == y.BKTypeId;
        }

        public int GetHashCode(BKInfo obj)
        {
            return obj.BKTypeId.GetHashCode();
        }

        #endregion
    }
}
