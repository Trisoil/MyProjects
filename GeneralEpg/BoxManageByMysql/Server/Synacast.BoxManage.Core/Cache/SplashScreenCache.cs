using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    public class SplashScreenCache : CacheBase<List<SplashScreenInfo>>
    {
        public static readonly SplashScreenCache Instance = new SplashScreenCache();

        private SplashScreenCache()
        { }

        public override void RefreshCache()
        {
            var hql = "from SplashScreenInfo s";
            _cache = SplashScreenInfo.Execute(new HqlReadQuery<SplashScreenInfo>(hql)) as List<SplashScreenInfo>;
        }
    }
}
