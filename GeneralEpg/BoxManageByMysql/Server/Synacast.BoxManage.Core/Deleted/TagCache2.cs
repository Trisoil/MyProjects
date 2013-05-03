//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//namespace Synacast.BoxManage.Core.Cache
//{
//    #region Namespaces

//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Internal;
//    using Synacast.BoxManage.Core.Cache.Key;
//    using Synacast.BoxManage.Core.Cache.Custom;
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Cache.Language;

//    #endregion

//    public class TagCache : CacheBase<Dictionary<TagNodeKey, List<TagNode>>>
//    {
//        #region Constructor

//        private TagCache()
//        { }

//        #endregion

//        #region Propeties

//        public static readonly TagCache Instance = new TagCache();

//        #endregion

//        /// <summary>
//        /// 自刷新行为
//        /// </summary>
//        public override void RefreshCache()
//        {
//            var boxtags = BoxTagCache.Instance.Dictionary.Values;
//            var dic = IndexInit();
//            foreach (var plat in PlatformUtils.FormatVodPlatform())
//            {
//                var query = from tag in boxtags join tagrel in ChannelTagRelCache.Instance.Items on tag.ID equals tagrel.TagID into tagrels let x = FormateBoxTag(tag, dic, plat, tagrels) select tag;
//                query.Count();
//            }
//            foreach (KeyValuePair<TagNodeKey, List<TagNode>> v in dic)
//            {
//                v.Value.Sort((t1, t2) => t1.TagRank - t2.TagRank);
//            }
//            foreach (var plat in PlatformUtils.FormatVodPlatform())
//            {
//                FormateBKTags(plat, dic, CustomArray.ActDimension);
//                FormateBKTags(plat, dic, CustomArray.DirectorDimension);
//            }
//            _cache = dic;
//        }

//        /// <summary>
//        /// 初始化索引
//        /// </summary>
//        /// <returns></returns>
//        private Dictionary<TagNodeKey, List<TagNode>> IndexInit()
//        {
//            var dic = new Dictionary<TagNodeKey, List<TagNode>>(PlatformUtils.FormatVodPlatform().Count + TagTypeCache.Instance.Dictionary.Count);
//            foreach (var plat in PlatformUtils.FormatVodPlatform())
//            {
//                foreach (var boxtype in BoxTypeCache.Instance.Items.Values)
//                {
//                    foreach (var dimension in boxtype.ResponseDimension.FormatStrToArray(SplitArray.LineArray))
//                    {
//                        if (!string.IsNullOrEmpty(dimension))
//                        {
//                            var key = new TagNodeKey(boxtype.TypeID, dimension, plat.PlatformName, plat.Licence);
//                            dic.Add(key, new List<TagNode>());
//                        }
//                    }
//                }
//            }
//            return dic;
//        }


//        /// <summary>
//        /// 将BKTag信息添加到索引
//        /// </summary>
//        private void FormateBKTags(AuthNode plat, Dictionary<TagNodeKey, List<TagNode>> sourcedic, string dimension)
//        {
//            foreach (var boxtype in BoxTypeCache.Instance.Items.Values)
//            {
//                if (!boxtype.ResponseDimension.Contains(dimension))
//                    continue;
//                var videos = ListCache.Instance.Items[new VideoNodeKey(plat.PlatformName, boxtype.TypeID, plat.Licence)];
//                if (videos.Count <= 0)
//                    continue;
//                var tagdic = new Dictionary<string, int>(100);
//                foreach (var video in videos)
//                {
//                    string[] tags = null;
//                    if (dimension == CustomArray.ActDimension)
//                        tags = video.BKInfo.Actors.FormatStrToArray(SplitArray.DHArray);
//                    else
//                        tags = video.BKInfo.Directors.FormatStrToArray(SplitArray.DHArray);
//                    foreach (var tag in tags)
//                    {
//                        if (tagdic.ContainsKey(tag))
//                            tagdic[tag]++;
//                        else
//                            tagdic[tag] = 1;
//                    }
//                }
//                var query = from bktag in tagdic select new TagNode() { Language = SimpleLanguageHandler.ProcessLang(bktag.Key.Substring(0, bktag.Key.IndexOf(":")), string.Empty), Dimension = dimension, BoxType = boxtype.TypeID, ListNumber = bktag.Value };
//                var key = new TagNodeKey(boxtype.TypeID, dimension, plat.PlatformName, plat.Licence);
//                sourcedic[key].AddRange(query.ToPerfectList());
//            }
//        }

//        /// <summary>
//        /// 将BoxTag信息添加到索引
//        /// </summary>
//        private object FormateBoxTag(BoxTagInfo tag, Dictionary<TagNodeKey, List<TagNode>> dic, AuthNode plat, IEnumerable<ChannelTagRelInfo> tagRels)
//        {
//            int count = 0;
//            foreach (var tagrel in tagRels)
//            {
//                if (ListCache.Instance.Dictionary.ContainsKey(tagrel.ChannelID))
//                {
//                    var video = ListCache.Instance.Dictionary[tagrel.ChannelID];
//                    if (video.PlatForms != null && video.PlatForms.Contains(plat))
//                        count++;
//                }
//            }
//            if (count <= 0)
//                return null;
//            var dimension = TagTypeCache.Instance[tag.TagType];
//            if (dimension != null)
//            {
//                var key = new TagNodeKey(tag.BoxTypeID, dimension.TagTypeLimitName, plat.PlatformName, plat.Licence);
//                if (dic.ContainsKey(key))
//                    dic[key].Add(new TagNode() { Language = SimpleLanguageHandler.ProcessLang(tag.TagName, tag.TagNameEng), BoxType = tag.BoxTypeID, Dimension = dimension.TagTypeLimitName, ListNumber = count, TagRank = tag.TagRank });
//            }
//            return null;
//        }

//    }
//}
