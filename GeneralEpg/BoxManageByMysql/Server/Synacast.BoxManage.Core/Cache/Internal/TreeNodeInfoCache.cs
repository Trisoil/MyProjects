using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class TreeNodeInfoCache : SubCacheBase<IList<TreeNodeInfo>>
    {
        public static readonly TreeNodeInfoCache Instance = new TreeNodeInfoCache();

        private TreeNodeInfoCache()
        { }

        public override IList<TreeNodeInfo> RefreshCache()
        {
            string hql = "from TreeNodeInfo t";
            return TreeNodeInfo.Execute(new HqlReadQuery<TreeNodeInfo>(hql)) as IList<TreeNodeInfo>;
        }
    }
}
