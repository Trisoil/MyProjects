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

    public class CoverImageCache : CacheBase<Dictionary<int,CoverImageNode>>
    {
        public static readonly CoverImageCache Instance = new CoverImageCache();

        private CoverImageCache()
        { }

        public override void RefreshCache()
        {
            string hql = "from ConverImageInfo c where c.IsPublish=1 order by c.Rank";
            var images = ConverImageInfo.Execute(new HqlReadQuery<ConverImageInfo>(hql)) as List<ConverImageInfo>;
            var dic = new Dictionary<int, CoverImageNode>(images.Count);
            foreach (var image in images)
            {
                var node = new CoverImageNode()
                {
                    ImageId = image.ImageID,
                    ChannelId = image.ChannelID,
                    ChannelType = image.ChannelType,
                    NavId = image.NavID,
                    Language = SimpleLanguageHandler.DoubleProcessLang(image.Title, image.Note),
                    ImageUrl = image.ImageUrl,
                    OnlineTime = image.OnlineTime
                };
                dic.Add(image.ImageID, node);
            }
            _cache = dic;
        }
    }
}
