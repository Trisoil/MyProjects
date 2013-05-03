//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Xml.Linq;
//using System.IO;
//using System.ServiceModel;

//namespace Synacast.BoxManage.List.Impl.Tags
//{
//    using Synacast.BoxManage.Core;
//    using Synacast.BoxManage.List.Tags;
//    using Synacast.BoxManage.List.Impl.Utils;
//    using Synacast.BoxManage.Core.Utils;
//    using Synacast.BoxManage.Core.Cache;
//    using Synacast.BoxManage.Core.Entity;
//    using Synacast.BoxManage.Core.Cache.Entity;
//    using Synacast.BoxManage.Core.Cache.Custom;

//    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
//    public class Tags : ITags
//    {
//        #region Fields

//        static string[] splitWord = new string[] { "|" };

//        #endregion

//        #region ITags 成员

//        public string VideoTags(TagsFilter filter)
//        {
//            try
//            {
//                StringBuilder builder = new StringBuilder(500);
//                FormatTags(filter, builder);
//                var xml = new XElement("tags",
//                    new XElement("info", BoxUtils.GetXmlCData(builder.ToString()))
//                    );
//                return xml.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        public string TagsEx(TagsFilter filter)
//        {
//            try
//            {
//                var root = new XElement("tags");
//                IEnumerable<XElement> query = null;
//                if (!filter.ExFilter)
//                {
//                    var tags = TagQuery(filter).ToDictionary(tag => string.Format("{0}:{1}", tag.Language[filter.lang].Title, filter.dimension));
//                    TagVideos(filter, tags, filter.dimension);
//                    query = from tag in tags where tag.Value.DynamicNum > 0 select new XElement("tag", new XElement("name", tag.Key), new XElement("count", tag.Value.DynamicNum));
//                }
//                else
//                {
//                    var tags = TagQuery(filter);
//                    query = from tag in tags select new XElement("tag", new XElement("name", tag.Language[filter.lang].Title), new XElement("count", tag.ListNumber));
//                }
//                root.Add(query);
//                return root.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        public string MultiTags(TagsFilter filter)
//        {
//            try
//            {
//                var dimensions = filter.dimension.Split(splitWord, StringSplitOptions.RemoveEmptyEntries).Distinct();
//                var xml = new XElement("tags");
//                foreach (string dimension in dimensions)
//                {
//                    StringBuilder sb = new StringBuilder(1000);
//                    FormatListName(TagNoJudgeQuery(filter.type, dimension, filter.platform, filter.auth), sb, filter);
//                    xml.Add(new XElement("info", new XAttribute("dimension", dimension), BoxUtils.GetXmlCData(sb.ToString())));
//                }
//                return xml.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        public string MultiTagsEx(TagsFilter filter)
//        {
//            try
//            {
//                var dimensions = filter.dimension.Split(splitWord, StringSplitOptions.RemoveEmptyEntries).Distinct();
//                var xml = new XElement("tags");
//                foreach (string dimension in dimensions)
//                {
//                    var dimensionxml = new XElement("dimension", new XAttribute("name", dimension));
//                    IEnumerable<XElement> x = null;
//                    if (!filter.ExFilter)
//                    {
//                        var tags = TagQuery(filter, dimension).ToDictionary(tag => string.Format("{0}:{1}", tag.Language[filter.lang].Title, filter.dimension));
//                        TagVideos(filter, tags, dimension);
//                        x = from tag in tags where tag.Value.DynamicNum > 0 select new XElement("tag", new XElement("name", tag.Key), new XElement("count", tag.Value.DynamicNum));
//                    }
//                    else
//                    {
//                        var tags = TagQuery(filter, dimension);
//                        x = from tag in tags select new XElement("tag", new XElement("name", tag.Language[filter.lang].Title), new XElement("count", tag.ListNumber));
//                    }
//                    //var taglist = x.ToList();
//                    dimensionxml.Add(x);
//                    xml.Add(dimensionxml);
//                }
//                return xml.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        public string VideoTypes(TagsFilter filter)
//        {
//            try
//            {
//                var root = new XElement("types");
//                foreach (var vtype in BoxTypeCache.Instance.Items.Values )
//                {
//                    var tagxml = new XElement("type");
//                    tagxml.Add(
//                        new XElement("tid", vtype.TypeID),
//                        new XElement("name", vtype.Language[filter.lang].Title),
//                        new XElement("image", vtype.PicLink),
//                        new XElement("tag_dimesion", vtype.ResponseDimension),
//                        new XElement("treeleft_support", vtype.TreeSupport)
//                        );
//                    root.Add(tagxml);
//                }
//                return root.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        public string VideoTypesEx(TagsFilter filter)
//        {
//            try
//            {
//                var root = new XElement("types");
//                var counts = new Dictionary<int, int>(BoxTypeCache.Instance.Items.Count);
//                var filters = VideoNodesUtil.FormateTagFilter<VideoNode>(filter);
//                ListCache.Instance.Items[new VideoNodeKey(filter.platform, filter.type, filter.auth)].Where(video => JudgeVideoEx(filters, video, counts)).Count();
//                var xmlquery = from type in BoxTypeCache.Instance.Items.Values 
//                               select new XElement("type",
//                                   new XElement("tid", type.TypeID),
//                                   new XElement("name", type.Language[filter.lang].Title),
//                                   new XElement("count", counts[type.TypeID]),
//                                   new XElement("image", type.PicLink),
//                                   new XElement("tag_dimesion", type.ResponseDimension),
//                                   new XElement("treeleft_support", type.TreeSupport)
//                                   );
//                root.Add(xmlquery);
//                return root.ToString(SaveOptions.DisableFormatting);
//            }
//            catch (KeyNotFoundException)
//            {
//                return NoPlatForm();
//            }
//            catch (Exception ex)
//            {
//                return BoxUtils.FormatErrorMsg(ex);
//            }
//        }

//        #endregion

//        #region Methods

//        private IEnumerable<TagNode2> TagQuery(TagsFilter filter)
//        {
//            TagNodeKey2 key = new TagNodeKey2(filter.type, filter.dimension, filter.platform, filter.auth);
//            if (filter.ExFilter)
//                return TagCache.Instance.Items[key];
//            return SetTagNumber(TagCache.Instance.Items[key]);
//        }

//        private IEnumerable<TagNode2> TagQuery(TagsFilter filter, string dimension)
//        {
//            TagNodeKey2 key = new TagNodeKey2(filter.type, dimension, filter.platform, filter.auth);
//            if (filter.ExFilter)
//                return TagCache.Instance.Items[key];
//            return SetTagNumber(TagCache.Instance.Items[key]);
//        }

//        private IEnumerable<TagNode2> TagNoJudgeQuery(int tagType, string dimension, string platform, string auth)
//        {
//            TagNodeKey2 key = new TagNodeKey2(tagType, dimension, platform, auth);
//            return TagCache.Instance.Items[key];
//        }

//        private IEnumerable<TagNode2> TagNoJudgeQuery(TagsFilter filter)
//        {
//            TagNodeKey2 key = new TagNodeKey2(filter.type, filter.dimension, filter.platform, filter.auth);
//            return TagCache.Instance.Items[key];
//        }

//        private void FormatTags(TagsFilter filter, StringBuilder builder)
//        {
//            FormatListName(TagNoJudgeQuery(filter), builder, filter);
//        }

//        private void FormatListName(IEnumerable<TagNode2> query, StringBuilder builder,TagsFilter filter)
//        {
//            builder.Append(query.Select(t => t.Language[filter.lang].Title).FormatListToStr("|"));
//        }

//        private List<TagNode2> SetTagNumber(List<TagNode2> tags)
//        {
//            foreach (TagNode2 tag in tags)
//                tag.DynamicNum = 0;
//            return tags;
//        }

//        private void TagVideos(TagsFilter filter, Dictionary<string, TagNode2> tags, string dimension)
//        {
//            var videos = ListCache.Instance.Items[new VideoNodeKey(filter.platform, filter.type, filter.auth)];
//            var filters = VideoNodesUtil.FormateTagFilter<VideoNode>(filter);
//            if (videos != null)
//            {
//                foreach (var video in videos)
//                {
//                    JudgeVideoEx(filters, filter, video, tags, video.TagsRank.Keys);
//                }
//            }
//        }

//        /// <summary>
//        /// BoxTypes使用，计算各类型视频个数，如电影1000个，电视剧500个
//        /// </summary>
//        private bool JudgeVideoEx(List<Func<VideoNode, bool>> filters, VideoNode video, Dictionary<int, int> types)
//        {
//            bool result = true;
//            foreach (Func<VideoNode, bool> f in filters)
//                result = result && f(video);
//            if (result)
//            {
//                if (types.ContainsKey(video.Type))
//                    types[video.Type]++;
//                else
//                    types[video.Type] = 1;
//            }
//            return result;
//        }

//        /// <summary>
//        /// BoxTag使用，计算各类型下各分类的视频个数，例如电影中动作片200个，剧情片300个，电视剧中动作片100个等
//        /// </summary>
//        private bool JudgeVideoEx(List<Func<VideoNode, bool>> filters, TagsFilter filter, VideoNode video, Dictionary<string, TagNode2> tags, Dictionary<string, int>.KeyCollection videoTags)
//        {
//            bool result = true;
//            foreach (var f in filters)
//                result = result && f(video);
//            if (result)
//            {
//                foreach (string vt in videoTags)
//                {
//                    TagNode2 ti;
//                    if (tags.TryGetValue(vt, out ti))
//                        ti.DynamicNum++;
//                }
//            }
//            return result;
//        }

//        private string NoPlatForm()
//        {
//            return BoxUtils.FormatErrorMsg("不存在该平台下的分类");
//        }

//        #endregion

//    }
//}
