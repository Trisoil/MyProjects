using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelRecomRelCache : SubCacheBase<IList<ChannelRecomRelInfo>>
    {
        public static readonly ChannelRecomRelCache Instance = new ChannelRecomRelCache();

        private ChannelRecomRelCache()
        { }

        public override IList<ChannelRecomRelInfo> RefreshCache()
        {
            string hql = "from ChannelRecomRelInfo c";
            return ChannelRecomRelInfo.Execute(new HqlReadQuery<ChannelRecomRelInfo>(hql)) as IList<ChannelRecomRelInfo>;
        }
    }
}
