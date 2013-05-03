using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.BoxManage.Core.Utils;

    public class LiveTypeDicCache : CacheBase<List<LiveTypeNode>>
    {
        private static readonly string _bufferName = "LiveTypeDicCache";
        public static readonly LiveTypeDicCache Instance = new LiveTypeDicCache();

        private LiveTypeDicCache()
        { }

        protected override string BufferName
        {
            get
            {
                return _bufferName;
            }
        }

        public override void RefreshCache()
        {
            var livetypes = LiveTypeCache.Instance.Items;
            var list = new List<LiveTypeNode>(livetypes.Count);
            var types = from livetype in livetypes
                        join rel in ChannelLiveRelCache.Instance.Items
                        on livetype.ID equals rel.LiveTypeID
                        into rels
                        select new
                        {
                            Type = livetype,
                            Rels = rels.OrderBy(r => r.Rank)
                        };
            foreach (var type in types)
            {
                var count = type.Rels.Count();
                if (count <= 0)
                    continue;
                var node = new LiveTypeNode()
                {
                    Language = SimpleLanguageHandler.ProcessLang(type.Type.TypeName, type.Type.TypeNameEng),
                    PicLink = type.Type.PicLink,
                    ID = type.Type.ID,
                    Channels = new Dictionary<VideoNodeKey, List<int>>(PlatformUtils.FormatVodPlatform().Count)
                };
                foreach (var rel in type.Rels)
                {
                    LiveVideoNode v;
                    if (LiveListCache.Instance.Dictionary.TryGetValue(rel.ChannelID, out v))
                    {
                        foreach (var plat in v.PlatForms)
                        {
                            var vk = new VideoNodeKey(plat.PlatformName, 0, plat.Licence);
                            if (!node.Channels.ContainsKey(vk))
                                node.Channels.Add(vk, new List<int>(count));
                            node.Channels[vk].Add(v.Main.ChannelID);
                        }
                    }
                }
                list.Add(node);
            }
            _cache = list;
        }
    }
}
