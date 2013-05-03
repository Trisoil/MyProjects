using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelLiveCustomerRelCache : SubCacheBase<IList<ChannelLiveCustomerRelInfo>>
    {
        public static readonly ChannelLiveCustomerRelCache Instance = new ChannelLiveCustomerRelCache();

        public override IList<ChannelLiveCustomerRelInfo> RefreshCache()
        {
            var hql = "from ChannelLiveCustomerRelInfo c";
            return ChannelLiveCustomerRelInfo.Execute(new HqlReadQuery<ChannelLiveCustomerRelInfo>(hql)) as IList<ChannelLiveCustomerRelInfo>;
        }
    }
}
