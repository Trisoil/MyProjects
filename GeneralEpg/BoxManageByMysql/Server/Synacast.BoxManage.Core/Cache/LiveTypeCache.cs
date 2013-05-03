using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.NhibernateActiveRecord.Query;

    /// <summary>
    /// 用于建立索引的直播分类
    /// </summary>
    public class LiveTypeCache : CacheBase<List<LiveTypeInfo>>
    {
        public static readonly LiveTypeCache Instance = new LiveTypeCache();

        private LiveTypeCache()
        { }

        public override void RefreshCache()
        {
            string hql = "from LiveTypeInfo l order by l.Rank";
            _cache = LiveTypeInfo.Execute(new HqlReadQuery<LiveTypeInfo>(hql)) as List<LiveTypeInfo>; 
        }

        protected override string BufferName
        {
            get
            {
                return "LiveTypeCache";
            }
        }
    }
}
