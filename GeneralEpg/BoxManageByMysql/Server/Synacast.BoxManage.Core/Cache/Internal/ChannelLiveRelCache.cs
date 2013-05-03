using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelLiveRelCache : SubCacheBase<IList<ChannelLiveRelInfo>>
    {
        public static readonly ChannelLiveRelCache Instance = new ChannelLiveRelCache();

        private ChannelLiveRelCache()
        { }

        public override IList<ChannelLiveRelInfo> RefreshCache()
        {
            string hql = "from ChannelLiveRelInfo c";
            return ChannelLiveRelInfo.Execute(new HqlReadQuery<ChannelLiveRelInfo>(hql)) as IList<ChannelLiveRelInfo>;
        }
    }
}
