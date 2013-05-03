using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class BoxTypeInfoCache : SubCacheBase<IList<BoxTypeInfo>>
    {
        public static readonly BoxTypeInfoCache Instance = new BoxTypeInfoCache();

        public override IList<BoxTypeInfo> RefreshCache()
        {
            string hql = "from BoxTypeInfo b";
            return BoxTypeInfo.Execute(new HqlReadQuery<BoxTypeInfo>(hql)) as IList<BoxTypeInfo>;
        }
    }
}
