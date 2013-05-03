using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using log4net;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.NhibernateActiveRecord.Query;

    internal class LiveChannelCache : SubCacheBase<IList<ChannelInfo>>
    {
        static readonly ILog _logger = BoxUtils.GetLogger("boxmanage.livelistcache.log");
        public static readonly LiveChannelCache Instance = new LiveChannelCache();

        private LiveChannelCache()
        { }

        public override IList<ChannelInfo> RefreshCache()
        {
            var lastcache = Items;
            string hql = AppSettingCache.Instance["LiveChannelCache"];
            var nowcache = ChannelInfo.Execute(new HqlReadQuery<ChannelInfo>(hql)) as IList<ChannelInfo>;
            if (nowcache == null)
                return lastcache;
            if (lastcache == null)
                return nowcache;
            var hidden = lastcache.Count - nowcache.Count;
            _logger.Info(string.Format("上次频道个数为{0}，当前频道个数为{1}", lastcache.Count, nowcache.Count));
            if (hidden > AppSettingCache.LiveHiddenMaxCount)
            {
                _logger.Info(string.Format("频道个数差异为{0}，本次刷新放弃！", hidden));
                return lastcache;
            }
            return nowcache;
        }
    }
}
