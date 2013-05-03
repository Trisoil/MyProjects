using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.NhibernateActiveRecord.Query;

    public class PlatFormInfoCache : CacheBase<List<PlatFormInfo>>
    {
        public static readonly PlatFormInfoCache Instance = new PlatFormInfoCache();

        private PlatFormInfoCache()
        { }

        public override void RefreshCache()
        {
            var fbs = AppSettingCache.Instance["ForbiddenPlatformHql"];
            string hql = "from PlatFormInfo p where p.CurrentSupport=1";
            if (fbs != "0")
            {
                hql = hql + fbs;
            }
            _cache = PlatFormInfo.ExecuteQuery(new HqlReadQuery<PlatFormInfo>(hql)) as List<PlatFormInfo>;
        }
    }
}
