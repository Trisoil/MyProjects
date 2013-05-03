using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelLiveRecomRelCache : SubCacheBase<IList<ChannelLiveRecomRelInfo>>
    {
        public static readonly ChannelLiveRecomRelCache Instance = new ChannelLiveRecomRelCache();

        private ChannelLiveRecomRelCache()
        { }

        public override IList<ChannelLiveRecomRelInfo> RefreshCache()
        {
            string hql = "from ChannelLiveRecomRelInfo c";
            return ChannelLiveRecomRelInfo.Execute(new HqlReadQuery<ChannelLiveRecomRelInfo>(hql)) as IList<ChannelLiveRecomRelInfo>;
        }
    }
}
