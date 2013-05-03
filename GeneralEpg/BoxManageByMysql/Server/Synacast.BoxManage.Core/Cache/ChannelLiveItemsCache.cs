using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.NhibernateActiveRecord.Query;

    public class ChannelLiveItemsCache : CacheBase<Dictionary<int, ChannelLiveItemsNode>>
    {
        public static readonly ChannelLiveItemsCache Instance = new ChannelLiveItemsCache();

        private ChannelLiveItemsCache()
        { }

        protected override string BufferName
        {
            get
            {
                return "ChannelLiveItemsCache";
            }
        }

        public override void RefreshCache()
        {
            string hql = "from ChannelLiveItemsInfo c";
            var items = ChannelLiveItemsInfo.Execute(new HqlReadQuery<ChannelLiveItemsInfo>(hql)) as IList<ChannelLiveItemsInfo>;
            var dic = new Dictionary<int, ChannelLiveItemsNode>(items.Count);
            foreach (var item in items)
            {
                var node = new ChannelLiveItemsNode()
                {
                    Language = SimpleLanguageHandler.ProcessLang(item.Title, string.Empty),
                    ID = item.ID,
                    ChannelID = item.ChannelID,
                    StartTime = item.StartTime
                };
                dic[item.ID] = node;
            }
            _cache = dic;
        }
    }
}
