using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelTagRelCache : SubCacheBase<IList<ChannelTagRelInfo>>
    {
        public static readonly ChannelTagRelCache Instance = new ChannelTagRelCache();

        private ChannelTagRelCache()
        { }

        public override IList<ChannelTagRelInfo> RefreshCache()
        {
            var hql = "from ChannelTagRelInfo c";
            return ChannelTagRelInfo.Execute(new HqlReadQuery<ChannelTagRelInfo>(hql)) as IList<ChannelTagRelInfo>;
        }
    }
}
