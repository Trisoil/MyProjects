using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache.Internal
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.NhibernateActiveRecord.Query;

    internal class ChannelCache : SubCacheBase<IList<ChannelInfo>>
    {
        public static readonly ChannelCache Instance = new ChannelCache();

        private ChannelCache()
        { }

        public override IList<ChannelInfo> RefreshCache()
        {
            //string hql = string.Format("from ChannelInfo c where c.IsOnline=1 and c.ChannelType=3 and c.IsPublish=1 and ((c.ParentChannelID=0 and c.BKType!=-1) or (c.ParentChannelID!=0))");
            string hql = AppSettingCache.Instance["VodChannelCache"];
            return ChannelInfo.Execute(new HqlReadQuery<ChannelInfo>(hql)) as IList<ChannelInfo>;
            //return new List<ChannelInfo>(list.OfType<ChannelInfo>());
        }
    }
}
