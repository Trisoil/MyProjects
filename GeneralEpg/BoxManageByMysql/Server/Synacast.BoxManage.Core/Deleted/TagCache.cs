//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Deleted
//{
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Internal;
//    using Synacast.BoxManage.Core.Cache.Key;
//    using Synacast.BoxManage.Core.Cache.Custom;

//    public class TagCache : CacheBase<Dictionary<TagNodeKey,List<TagNode>>>
//    {
//        #region Fields

//        private static string[] _split = new string[] { " " };

//        #endregion

//        #region Constructor

//        private TagCache()
//        { }

//        #endregion

//        #region Propeties

//        public static readonly TagCache Instance = new TagCache();

//        #endregion

//        public override void RefreshCache()
//        {
//            var boxtypes = BoxTypeCache.Instance.Items;
//            var boxtags = BoxTagCache.Instance.Items;
//            var bktags = BKTagCache.Instance.Items;
//            var platforms = PlatFormCache.Instance.Items;
//            Dictionary<TagNodeKey, List<TagNode>> dic = new Dictionary<TagNodeKey, List<TagNode>>();
//            foreach (BoxTypeInfo boxtype in boxtypes)
//            {
//                foreach (var plat in platforms)
//                {
//                    var videos = ListCache.Instance.Items[new VideoNodeKey(plat.Name, boxtype.TypeID, plat.Licence)];
//                    //分类，地区，年份
//                    FormatBoxTags(boxtags, boxtype, plat, (int)TagTypeEnum.Catalog, "catalog", videos, (video, tag) => video.CatalogBoxTags.Contains(tag), dic);
//                    FormatBoxTags(boxtags, boxtype, plat, (int)TagTypeEnum.Area, "area", videos, (video, tag) => video.AreaBoxTags.Contains(tag), dic);
//                    FormatBoxTags(boxtags, boxtype, plat, (int)TagTypeEnum.Year, "year", videos, (video, tag) => video.YearBoxTags.Contains(tag), dic);

//                    //演员，导演
//                    FormatBKTags(bktags, boxtype, plat, (int)TagTypeEnum.Act, "act", videos, (video, tag) => video.ActBkTags.Contains(tag), dic);
//                    FormatBKTags(bktags, boxtype, plat, (int)TagTypeEnum.Director, "director", videos, (video, tag) => video.DirectorBKTags.Contains(tag), dic);
//                }
//            }
//            _cache = dic;
//        }

//        private void FormatBoxTags(List<BoxTagInfo> boxtags, BoxTypeInfo boxtype, AuthNode plat, int tagstype, string dimension, List<VideoNode> videos, Func<VideoNode, string, bool> fun, Dictionary<TagNodeKey, List<TagNode>> dic)
//        {
//            var query = from boxtag in boxtags where boxtag.TagType == tagstype && boxtype.BKTypes.Split(_split, StringSplitOptions.RemoveEmptyEntries).Contains(boxtag.BKType.ToString()) select new TagNode() { Name = boxtag.TagName, Dimension = dimension, BoxType = boxtype.TypeID, ListNumber = videos.Count(video => fun(video, boxtag.TagName)), TagRank = boxtag.TagRank };
//            TagNodeKey key = new TagNodeKey(boxtype.TypeID, dimension, plat.Name, plat.Licence);
//            dic.Add(key, query.Distinct(new TagNameEquality()).Where(tag => tag.ListNumber > 0).OrderByDescending(tag => tag.TagRank).ToList());  //去除节目数量为0的分类
//        }

//        private void FormatBKTags(List<BKTagInfo> bktags, BoxTypeInfo boxtype, AuthNode plat, int tagstype, string dimension, List<VideoNode> videos, Func<VideoNode, string, bool> fun, Dictionary<TagNodeKey, List<TagNode>> dic)
//        {
//            var query = from bktag in bktags where bktag.TagType == tagstype && boxtype.BKTypes.Split(_split, StringSplitOptions.RemoveEmptyEntries).Contains(bktag.BKType.ToString()) select new TagNode() { Name = bktag.TagName, Dimension = dimension, BoxType = boxtype.TypeID, ListNumber = videos.Count(video => fun(video, bktag.TagName)), TagRank = bktag.TagRank };
//            TagNodeKey key = new TagNodeKey(boxtype.TypeID, dimension, plat.Name, plat.Licence);
//            dic.Add(key, query.Distinct(new TagNameEquality()).Where(tag => tag.ListNumber > 0).OrderByDescending(tag => tag.TagRank).ToList());
//        }

//    }
//}
