using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Tags
{
    using Synacast.BoxManage.List.Tags;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class Tags : ITags 
    {
        #region ITags Members

        /// <summary>
        /// 单维度基本接口
        /// </summary>
        public string VideoTags(TagsFilter filter)
        {
            try
            {
                var tags = TagCache.Instance.Items[new TagNodeKey() { Dimension = filter.dimension, Type = filter.type }];
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                IEnumerable<string> query = null;
                if (filters.Count <= 0)
                {
                    query = from tag in tags where tag.Channels.ContainsKey(key) select tag.Language[filter.lang].Title;
                }
                else
                {
                    query = from tag in tags
                            where tag.Channels.ContainsKey(key)
                            let count = GetChannelCount(tag, key, filter, filters)
                            where count > 0
                            select tag.Language[filter.lang].Title;
                }
                var xml = new XElement("tags",
                        new XElement("info", BoxUtils.GetXmlCData(query.FormatListToStr(SplitArray.Line, 100)))
                    );
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 单维度扩展接口
        /// </summary>
        public string TagsEx(TagsFilter filter)
        {
            try
            {
                var tags = TagCache.Instance.Items[new TagNodeKey() { Dimension = filter.dimension, Type = filter.type }];
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var xml = new XElement("tags");
                var filters = VideoNodesUtil.FormateVodFilter(filter);
                xml.Add(from tag in tags
                        where tag.Channels.ContainsKey(key)
                        let count = GetChannelCount(tag, key, filter, filters)
                        where count > 0
                        select new XElement("tag",
                            new XElement("name", tag.Language[filter.lang].Title),
                            new XElement("count", count)
                    ));
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 多维度基本接口
        /// </summary>
        public string MultiTags(TagsFilter filter)
        {
            try
            {
                var dimensions = filter.dimension.Split(SplitArray.LineArray, StringSplitOptions.RemoveEmptyEntries).Distinct();
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                var xml = new XElement("tags");
                foreach (var dimension in dimensions)
                {
                    var tags = TagCache.Instance.Items[new TagNodeKey() { Dimension = dimension, Type = filter.type }];
                    var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                    IEnumerable<string> query = null;
                    if (filters.Count <= 0)
                    {
                        query = from tag in tags where tag.Channels.ContainsKey(key) select tag.Language[filter.lang].Title;
                    }
                    else
                    {
                        query = from tag in tags
                                where tag.Channels.ContainsKey(key)
                                let count = GetChannelCount(tag, key, filter, filters)
                                where count > 0
                                select tag.Language[filter.lang].Title;
                    }
                    xml.Add(new XElement("info",
                            new XAttribute("dimension", dimension),
                            BoxUtils.GetXmlCData(query.FormatListToStr(SplitArray.Line, 100))
                        ));
                }
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 多维度扩展接口
        /// </summary>
        public string MultiTagsEx(TagsFilter filter)
        {
            try
            {
                var dimensions = filter.dimension.Split(SplitArray.LineArray, StringSplitOptions.RemoveEmptyEntries).Distinct();
                var xml = new XElement("tags");
                foreach (var dimension in dimensions)
                {
                    var tags = TagCache.Instance.Items[new TagNodeKey() { Dimension = dimension, Type = filter.type }];
                    var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                    var dimensionxml = new XElement("dimension", new XAttribute("name", dimension));
                    var filters = VideoNodesUtil.FormateVodFilter(filter);
                    dimensionxml.Add(from tag in tags
                            where tag.Channels.ContainsKey(key)
                            let count = GetChannelCount(tag, key, filter, filters)
                            where count > 0
                            select new XElement("tag",
                                new XElement("name", tag.Language[filter.lang].Title),
                                new XElement("count", count)
                            ));
                    xml.Add(dimensionxml);
                }
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 分类基本接口
        /// </summary>
        public string VideoTypes(TagsFilter filter)
        {
            try
            {
                var xml = new XElement("types");
                xml.Add(from boxtype in BoxTypeCache.Instance.Items.Values
                        select new XElement("type",
                            new XElement("tid", boxtype.TypeID),
                            new XElement("name", boxtype.Language[filter.lang].Title),
                            new XElement("image", boxtype.PicLink),
                            new XElement("tag_dimesion", boxtype.ResponseDimension),
                            new XElement("treeleft_support", boxtype.TreeSupport)
                        ));
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 分类扩展接口
        /// </summary>
        public string VideoTypesEx(TagsFilter filter)
        {
            try
            {
                var types = BoxTypeCache.Instance.Items;
                var filters = VideoNodesUtil.CommonFilter(filter);
                var root = new XElement("types");
                foreach (var boxtype in types)
                {
                    var videos = ListCache.Instance.Items[new VideoNodeKey(filter.platform, boxtype.Key, filter.auth)].OrderArray;
                    int count = 0;
                    for (var i = 0; i < videos.Count; i++)
                    {
                        bool result = true;
                        var v = ListCache.Instance.Dictionary[videos[i]];
                        foreach (var f in filters)
                            result = result && f(v, filter);
                        if (result)
                        {
                            count++;
                        }
                    }
                    root.Add(new XElement("type",
                               new XElement("tid", boxtype.Value.TypeID),
                               new XElement("name", boxtype.Value.Language[filter.lang].Title),
                               new XElement("count", count),
                               new XElement("image", boxtype.Value.PicLink),
                               new XElement("tag_dimesion", boxtype.Value.ResponseDimension),
                               new XElement("treeleft_support", boxtype.Value.TreeSupport)
                            ));
                }
                return root.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatForm();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        private int GetChannelCount(TagNode node, VideoNodeKey key, TagsFilter filter, List<Func<VideoBase, ExFilterBase, bool>> filters)
        {
            int count = 0;
            var cs = node.Channels[key];
            foreach (var c in cs)
            {
                var result = true;
                var v = ListCache.Instance.Dictionary[c];
                foreach (var f in filters)
                {
                    result = result && f(v, filter);
                }
                if (result)
                    count++;
            }
            return count;
        }

        private string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的分类");
        }

    }
}
