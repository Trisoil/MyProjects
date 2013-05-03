using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;
    using Synacast.BoxManage.Core.Cache.Internal;

    public class ForbiddenAreaCache : CacheBase<Dictionary<string, ForbiddenAreaInfo>>
    {
        public List<string> ForbiddenNames;
        public static readonly ForbiddenAreaCache Instance = new ForbiddenAreaCache();

        private ForbiddenAreaCache()
        { }

        public override void RefreshCache()
        {
            var fbs = AppSettingCache.Instance["SupportFbArea"];
            var hql = "from ForbiddenAreaInfo f";
            if (fbs != "0")
            {
                hql = hql + fbs;
            }
            var list = ForbiddenAreaInfo.Execute(new HqlReadQuery<ForbiddenAreaInfo>(hql)) as IList<ForbiddenAreaInfo>;
            ForbiddenNames = list.Select(f => f.ForbiddenAreaName).ToPerfectList();
            _cache = list.ToPerfectDictionary(f => f.ForbiddenAreaCityCode);
            list.Clear();
        }
    }
}
