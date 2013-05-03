//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache
//{
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Internal;

//    /// <summary>
//    /// 用于接口输出的直播分类信息类
//    /// </summary>
//    public class LiveTypeDicCache : CacheBase<Dictionary<TagNodeKey,List<LiveTypeNode>>>
//    {
//        private static readonly string _bufferName = "LiveTypeDicCache";
//        public static readonly LiveTypeDicCache Instance = new LiveTypeDicCache();

//        private LiveTypeDicCache()
//        { }

//        /// <summary>
//        /// 自刷新行为
//        /// </summary>
//        public override void RefreshCache()
//        {
//            var dic = new Dictionary<TagNodeKey,List<LiveTypeNode>>();
//            foreach (var plat in PlatformUtils.FormatLivePlatform())
//            {
//                foreach (var customer in LiveTypeCache.Instance.Items)
//                {
//                    LoadIndex(customer.Key, customer.Value, plat, dic);
//                }
//            }
//            _cache = dic;
//        }

//        protected override string BufferName
//        {
//            get
//            {
//                return _bufferName;
//            }
//        }

//        private void LoadIndex(int coustomId, List<LiveTypeInfo> liveTypes, AuthNode plat, Dictionary<TagNodeKey, List<LiveTypeNode>> dic)
//        {
//            if (string.IsNullOrEmpty(plat.PlatformName) && coustomId != -1)   //Auth特列直播分类
//            {
//                var customer = CustomerCache.Instance[plat.PlatformName, coustomId];
//                if (customer != null && plat.Licence == customer.CustomerAuth)
//                {
//                    TagNodeKey key = new TagNodeKey(0, TagTypeArray.Items[(int)TagTypeEnum.Catalog], plat.PlatformName, plat.Licence);
//                    foreach (LiveTypeInfo livetype in liveTypes)
//                    {
//                        LoadList(liveTypes, plat, dic);
//                    }
//                }
//            }
//            else if (coustomId == -1 && !PlatFormCache.Instance.AuthItems.ContainsKey(new AuthKey() { PlatForm = plat.PlatformName, Auth = plat.Licence }))
//            {
//                LoadList(liveTypes, plat, dic);
//            }
//        }

//        /// <summary>
//        /// 加载频道信息
//        /// </summary>
//        private void LoadList(List<LiveTypeInfo> liveTypes, AuthNode plat, Dictionary<TagNodeKey,List<LiveTypeNode>> dic)
//        {
//            TagNodeKey key = new TagNodeKey(0, TagTypeArray.Items[(int)TagTypeEnum.Catalog], plat.PlatformName, plat.Licence);
//            List<LiveTypeNode> result = new List<LiveTypeNode>(liveTypes.Count);
//            foreach (var livetype in liveTypes)
//            {
//                var videos = LiveListCache.Instance.Items[new VideoNodeKey(plat.PlatformName, livetype.ID, plat.Licence)];
//                if (videos.Count <= 0)
//                    continue;
//                result.Add(new LiveTypeNode() { LiveType = livetype, Videos = videos });
//            }
//            result.Sort((k1, k2) => k1.LiveType.Rank - k2.LiveType.Rank);
//            dic[key] = result;
//        }
//    }
//}
