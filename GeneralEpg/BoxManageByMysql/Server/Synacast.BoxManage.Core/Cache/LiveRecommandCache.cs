using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.Core.Cache
{
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Language;
    using Synacast.NhibernateActiveRecord.Query;

    /// <summary>
    /// 直播推荐缓存
    /// </summary>
    public class LiveRecommandCache : CacheBase<Dictionary<RecommandKey,List<LiveRecommandNode>>>
    {
        private static readonly string _buffername = "LiveRecommandCache";
        public static readonly LiveRecommandCache Instance = new LiveRecommandCache();

        private LiveRecommandCache()
        { }

        /// <summary>
        /// 自刷新行为
        /// </summary>
        public override void RefreshCache()
        {
            string hql = "from LiveRecommandInfo l order by l.Rank";
            var recommands = LiveRecommandInfo.Execute(new HqlReadQuery<LiveRecommandInfo>(hql)) as IList<LiveRecommandInfo>;
            var modes = recommands.Select(r => r.Mode).Distinct().ToPerfectList();
            var dic = InitIndex(modes);

            foreach (var plat in PlatformUtils.FormatLivePlatform())
            {
                var query = from r in recommands join recrel in ChannelLiveRecomRelCache.Instance.Items on r.ID equals recrel.LiveRecommandID into recrels let x = FormatRecommands(dic, r, plat, recrels) select x;
                query.Count();
            }

            _cache = dic;
        }

        protected override string BufferName
        {
            get
            {
                return _buffername;
            }
        }

        /// <summary>
        /// 初始化平台索引
        /// </summary>
        private Dictionary<RecommandKey, List<LiveRecommandNode>> InitIndex(List<int> modes)
        {
            var dic = new Dictionary<RecommandKey, List<LiveRecommandNode>>();
            foreach (var f in PlatformUtils.FormatLivePlatform())
            {
                foreach (var mode in modes)
                {
                    dic.Add(new RecommandKey(mode, f.PlatformName, f.Licence), new List<LiveRecommandNode>());
                }
            }
            return dic;
        }

        /// <summary>
        /// 根据平台加载导航实体
        /// </summary>
        /// <returns></returns>
        private object FormatRecommands(Dictionary<RecommandKey, List<LiveRecommandNode>> dic, LiveRecommandInfo info, AuthNode plat, IEnumerable<ChannelLiveRecomRelInfo> recrels)
        {
            var videos = new List<LiveVideoNode>(recrels.Count());
            var orderrecrels = recrels.OrderBy(r => r.Rank);
            foreach (var recrel in orderrecrels)
            {
                LiveVideoNode video;
                if (LiveListCache.Instance.Dictionary.TryGetValue(recrel.ChannelID, out video))
                {
                    if (video.PlatForms.Contains(plat))
                        videos.Add(video);
                }
            }
            if (videos.Count <= 0)
                return null;
            var key = new RecommandKey(info.Mode, plat.PlatformName, plat.Licence);
            var node = new LiveRecommandNode()
            {
                Language = SimpleLanguageHandler.ProcessLang(info.Name, string.Empty),
                ID = info.ID,
                PicLink = info.PicLink,
                Lists = videos
            };
            dic[key].Add(node);
            dic[new RecommandKey(plat.PlatformName, plat.Licence, info.ID)] = new List<LiveRecommandNode>() { node };
            return null;
        }
    }
}
