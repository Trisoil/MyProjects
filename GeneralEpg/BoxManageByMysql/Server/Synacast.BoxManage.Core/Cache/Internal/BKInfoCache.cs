using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal.Key;
    using Synacast.NhibernateActiveRecord.Query;

    internal class BKInfoCache : SubCacheBase<IList<BKInfo>>
    {
        private Dictionary<string, BKInfo> _dic;
        public static readonly BKInfoCache Instance = new BKInfoCache();

        private BKInfoCache()
        { }

        public override IList<BKInfo> RefreshCache()
        {
            string sql = AppSettingCache.Instance["BKInfoCache"];
            var list = BKInfo.Execute(new SqlReadQuery<BKInfo>(sql)) as IList<BKInfo>;
            _dic = list.ToPerfectDictionary(bk => bk.BKTypeId);
            list.Clear();
            return null;
        }

        public BKInfo this[string index]
        {
            get 
            {
                BKInfo info = null;
                _dic.TryGetValue(index, out info);
                return info;
            } 
        }

    }
}
