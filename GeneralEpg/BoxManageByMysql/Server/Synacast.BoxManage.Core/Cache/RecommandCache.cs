using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.NhibernateActiveRecord.Query;

    public class RecommandCache  : CacheBase<Dictionary<int,List<RecommandNode>>>
    {
        private Dictionary<int, RecommandNode> _all;

        public static readonly RecommandCache Instance = new RecommandCache();

        private RecommandCache()
        { }

        public Dictionary<int, RecommandNode> Dictionary
        {
            get {
                return _all;
            }
        }

        public override void RefreshCache()
        {            
            string hql = "from RecommandInfo r";
            var res = RecommandInfo.Execute(new HqlReadQuery<RecommandInfo>(hql)) as IList<RecommandInfo>;
            var gquery = from re in res group re by re.Mode;
            var dic = new Dictionary<int, List<RecommandNode>>(gquery.Count());
            var all = new Dictionary<int, RecommandNode>(res.Count);
            foreach (var g in gquery)
            {
                dic[g.Key] = new List<RecommandNode>(g.Count());
                var relquery = from re in g
                               orderby re.Rank
                               join rel in ChannelRecomRelCache.Instance.Items
                               on re.ID equals rel.RecommandID
                               into rels
                               select new
                               {
                                   Node = new RecommandNode()
                                   {
                                       Id = re.ID,
                                       PicLink = re.PicLink,
                                       Language = SimpleLanguageHandler.ProcessLang(re.Name, re.NameEng),
                                       Channels = new Dictionary<VideoNodeKey, List<int>>(PlatformUtils.FormatVodPlatform().Count)
                                   },
                                   Rels = rels.OrderBy(r => r.Rank)
                               };
                foreach (var rel in relquery)
                {
                    var result = false;
                    var rcount = rel.Rels.Count();
                    foreach (var r in rel.Rels)
                    {
                        VideoNode v;
                        if (ListCache.Instance.Dictionary.TryGetValue(r.ChannelID, out v))
                        {
                            foreach (var plat in v.PlatForms)
                            {
                                var vk = VideoNodeKeyArray.Instance.FindKey(plat.PlatformName, 0, plat.Licence); 
                                if (!rel.Node.Channels.ContainsKey(vk))
                                    rel.Node.Channels.Add(vk, new List<int>(rcount));
                                rel.Node.Channels[vk].Add(v.Main.ChannelID);
                                result = true;
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
