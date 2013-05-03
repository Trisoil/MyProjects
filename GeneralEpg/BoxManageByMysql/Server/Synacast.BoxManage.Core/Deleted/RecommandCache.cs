//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Internal;
//    using Synacast.NhibernateActiveRecord.Query;

//    /// <summary>
//    /// 点播推荐缓存
//    /// </summary>
//    public class RecommandCache : CacheBase<Dictionary<RecommandKey, List<RecommandNode>>>
//    {
//        public static readonly RecommandCache Instance = new RecommandCache();

//        private RecommandCache()
//        { }

//        /// <summary>
//        /// 自刷新行为
//        /// </summary>
//        public override void RefreshCache()
//        {
//            string hql = "from RecommandInfo r";
//            var recommands = RecommandInfo.Execute(new HqlReadQuery<RecommandInfo>(hql)) as IList<RecommandInfo>;
//            var modes = recommands.Select(r => r.Mode).Distinct().ToList();
//            var dic = InitIndex(modes);

//            foreach (var plat in PlatformUtils.FormatVodPlatform())
//            {
//                var query = from r in recommands join recrel in ChannelRecomRelCache.Instance.Items on r.ID equals recrel.RecommandID into recrels let x = FormatRecommands(dic, r, plat, recrels) select x;
//                query.ToList();
//            }
//            foreach (KeyValuePair<RecommandKey, List<RecommandNode>> v in dic)
//            {
//                v.Value.Sort((t1, t2) => t1.Recommand.Rank - t2.Recommand.Rank);
//            }

//            _cache = dic;
//        }

//        /// <summary>
//        /// 初始化平台索引
//        /// </summary>
//        private Dictionary<RecommandKey, List<RecommandNode>> InitIndex(List<int> modes)
//        {
//            var dic = new Dictionary<RecommandKey, List<RecommandNode>>();
//            foreach (var f in PlatformUtils.FormatVodPlatform())
//            {
//                foreach (var mode in modes)
//                {
//                    dic.Add(new RecommandKey(mode, f.PlatformName, f.Licence), new List<RecommandNode>());
//                }
//            }
//            return dic;
//        }

//        /// <summary>
//        /// 根据平台加载导航实体
//        /// </summary>
//        /// <returns></returns>
//        private object FormatRecommands(Dictionary<RecommandKey, List<RecommandNode>> dic, RecommandInfo info, AuthNode plat, IEnumerable<ChannelRecomRelInfo> recrels)
//        {
//            var videos = new List<VideoNode>();
//            var orderrecrels = recrels.OrderBy(r => r.Rank);
//            foreach (var recrel in orderrecrels)
//            {
//                try
//                {
//                    VideoNode video = ListCache.Instance.Dictionary[recrel.ChannelID];
//                    if (video.PlatForms.Contains(plat))
//                        videos.Add(video);
//                }
//                catch
//                { }
//            }
//            if (videos.Count <= 0)
//                return null;
//            RecommandKey key = new RecommandKey(info.Mode, plat.PlatformName, plat.Licence);
//            RecommandNode node = new RecommandNode() { Recommand = info, Lists = videos };
//            dic[key].Add(node);
//            dic[new RecommandKey(plat.PlatformName, plat.Licence, info.ID)] = new List<RecommandNode>() { node };
//            return null;
//        }
//    }
//}
