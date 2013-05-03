using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class TreeNodeChannelRelCache : SubCacheBase<IList<TreeNodeChannelRelInfo>>
    {
        public static readonly TreeNodeChannelRelCache Instance = new TreeNodeChannelRelCache();

        private TreeNodeChannelRelCache()
        { }

        public override IList<TreeNodeChannelRelInfo> RefreshCache()
        {
            string hql = "from TreeNodeChannelRelInfo t";
            return TreeNodeChannelRelInfo.Execute(new HqlReadQuery<TreeNodeChannelRelInfo>(hql)) as IList<TreeNodeChannelRelInfo>;
        }
    }
}
