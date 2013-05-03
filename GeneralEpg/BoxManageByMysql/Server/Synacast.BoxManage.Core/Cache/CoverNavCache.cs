using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.NhibernateActiveRecord.Query;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Utils;

    public class CoverNavCache : CacheBase<Dictionary<int,List<CoverNavNode>>>
    {
        private Dictionary<int, CoverNavNode> _all;
        public static readonly CoverNavCache Instance = new CoverNavCache();

        private CoverNavCache()
        { }

        public Dictionary<int, CoverNavNode> Dictionary
        {
            get
            {
                return _all;
            }
        }

        public override void RefreshCache()
        {
            string hql = "from CoverNavInfo c ";
            var res = CoverNavInfo.Execute(new HqlReadQuery<CoverNavInfo>(hql)) as IList<CoverNavInfo>;
            res.Add(new CoverNavInfo() { ID = 0, Mode = -1 });
            var gquery = from re in res group re by re.Mode;
            var dic = new Dictionary<int, List<CoverNavNode>>(gquery.Count());
            var all = new Dictionary<int, CoverNavNode>(res.Count);
            foreach (var g in gquery)
            {
                dic[g.Key] = new List<CoverNavNode>(g.Count());
                var relquery = from re in g
                               orderby re.Rank
                               join rel in CoverImageCache.Instance.Items.Values
                               on re.ID equals rel.NavId 
                               into rels
                               select new
                               {
                                   Node = new CoverNavNode()
                                   {
                                       Id = re.ID,
                                       PicLink = re.PicLink,
                                       Language = SimpleLanguageHandler.ProcessLang(re.Name, re.NameEng),
                                       Images = new Dictionary<VideoNodeKey, List<int>>(PlatformUtils.FormatVodPlatform().Count)
                                   },
                                   Rels = rels//.OrderBy(r => r.Rank)
                               };
                foreach (var rel in relquery)
                {
                    var result = false;
                    var rcount = rel.Rels.Count();
                    foreach (var r in rel.Rels)
                    {
                        VideoNode vod;
                        LiveVideoNode live;
                        if (r.ChannelType == 3)
                        {
                            if (ListCache.Instance.Dictionary.TryGetValue(r.ChannelId, out vod))
                            {
                                foreach (var plat in vod.PlatForms)
                                {
                                    var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, 0, plat.Licence);
                                    if (!rel.Node.Images.ContainsKey(vk))
                                        rel.Node.Images.Add(vk, new List<int>(rcount));
                                    rel.Node.Images[vk].Add(r.ImageId);
                                    result = true;
                                }
                            }
                        }
                        else if (r.ChannelType == 0)
                        {
                            if (LiveListCache.Instance.Dictionary.TryGetValue(r.ChannelId, out live))
                            {
                                foreach (var plat in live.PlatForms)
                                {
                                    var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, 0, plat.Licence);
                                    if (!rel.Node.Images.ContainsKey(vk))
                                        rel.Node.Images.Add(vk, new List<int>(rcount));
                                    rel.Node.Images[vk].Add(r.ImageId);
                                    result = true;
                                }
                            }
                        }
                    }
                    if (result)
                    {
                        dic[g.Key].Add(rel.Node);
                        all[rel.Node.Id] = rel.Node;
                    }
                }
            }
            _cache = dic;
            _all = all;
        }
    }
}
