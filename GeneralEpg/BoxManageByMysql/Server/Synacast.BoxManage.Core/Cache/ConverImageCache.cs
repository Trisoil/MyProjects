using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.NhibernateActiveRecord.Query;

    public class ConverImageCache : CacheBase<List<ConverImageNode>>
    {
        public static readonly ConverImageCache Instance = new ConverImageCache();

        private ConverImageCache()
        { }

        public override void RefreshCache()
        {
            string hql = "from ConverImageInfo c where c.IsPublish=1 order by c.Rank";
            var images = ConverImageInfo.Execute(new HqlReadQuery<ConverImageInfo>(hql)) as IList<ConverImageInfo>;
            var list = new List<ConverImageNode>(images.Count);
            foreach (var image in images)
            {
                var result = false;
                if (image.ChannelType == 3)
                {
                    result = ListCache.Instance.Dictionary.ContainsKey(image.ChannelID);
                }
                else if (image.ChannelType == 0)
                {
                    result = LiveListCache.Instance.Dictionary.ContainsKey(image.ChannelID);
                }
                if (result)
                {
                    var node = new ConverImageNode()
                    {
                        Vid = image.ChannelID,
                        ChannelType = image.ChannelType == 3 ? "vod" : "live",
                        Language = SimpleLanguageHandler.DoubleProcessLang(image.Title, image.Note),
                        ImageUrl = image.ImageUrl,
                        OnlineTime = image.OnlineTime,
                        PlatForms = PlatformUtils.ConvertPlatForms(image.ChannelID, image.ChannelType)
                    };
                    list.Add(node);
                }
            }
            _cache = list;
        }
    }
}
