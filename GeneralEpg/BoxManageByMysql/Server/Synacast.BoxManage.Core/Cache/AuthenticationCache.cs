using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class AuthenticationCache : CacheBase<List<AuthenticationInfo>>
    {
        public static readonly AuthenticationCache Instance = new AuthenticationCache();

        private AuthenticationCache()
        { }

        public override void RefreshCache()
        {
            string hql = "from AuthenticationInfo a where a.CurrentSupport=1";
            _cache = AuthenticationInfo.ExecuteQuery(new HqlReadQuery<AuthenticationInfo>(hql)) as List<AuthenticationInfo>;
        }
    }
}
