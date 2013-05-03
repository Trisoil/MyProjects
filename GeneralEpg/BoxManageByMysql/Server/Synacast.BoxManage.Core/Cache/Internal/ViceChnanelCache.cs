using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.NhibernateActiveRecord.Query;

    public class ViceChnanelCache : SubCacheBase<IList<ViceChannelInfo>>
    {
        public static readonly ViceChnanelCache Instance = new ViceChnanelCache();

        private ViceChnanelCache()
        { }

        public override IList<ViceChannelInfo> RefreshCache()
        {
            string hql = AppSettingCache.Instance["ViceChannelCache"];
            return ViceChannelInfo.Execute(new HqlReadQuery<ViceChannelInfo>(hql)) as IList<ViceChannelInfo>;
        }
    }
}
