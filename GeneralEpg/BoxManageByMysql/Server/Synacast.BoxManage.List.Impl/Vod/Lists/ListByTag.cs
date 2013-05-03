using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Vod.Lists
{
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Vod.Lists;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class ListByTag : IListByTags 
    {
        #region IListByTags Members

        public string ListByTags(ListFilter filter)
        {
            try
            {
                var pindex = VideoNodeKeyArray.Instance.Items[new VideoNodeKey(filter.platform, 0, filter.auth)];
                var pars = VideoParsUtils.FormatListByTags(filter);
                var filters = VideoNodesUtil.FormateVideoFilter(filter, pars);
                var tag = pars.SortTag.Substring(0, pars.SortTag.LastIndexOf(":"));
                var dimension = pars.SortTag.Substring(pars.SortTag.LastIndexOf(":") + 1);
                IEnumerable<int> channels = new List<int>();
                foreach (var type in BoxTypeCache.Instance.Items.Values)
                {
                    var key = new TagNodeKey() { Dimension = dimension, Type = type.TypeID };
                    if (TagCache.Instance.Items.ContainsKey(key))
                    {
                        var vkey = new VideoNodeKey(filter.platform, type.TypeID, filter.auth);
                        var node = TagCache.Instance.Items[key].FirstOrDefault(v => v.Language[CustomArray.LanguageArray[0]].Title == tag);
                        if (node != null && node.Channels.ContainsKey(vkey))
                            channels = channels.Union(node.Channels[vkey]);
                    }
                }
                var query = channels.Where(v =>
                    {
                        bool result = true;
                        var video = ListCache.Instance.Dictionary[v];
                        foreach (var f in filters)
                        {
                            result = result && f(video, pars, filter);
                        }
                        return result;
                    }
                );
                return CreatePageList(query, filter, pars, false);
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

        public string ListByTagsEx(ListFilter filter)
        {
            try
            {
                var pindex = VideoNodeKeyArray.Instance.Items[new VideoNodeKey(filter.platform, 0, filter.auth)];
                var pars = VideoParsUtils.FormatListByTags(filter);
                var filters = VideoNodesUtil.FormateVideoExFilter(filter, pars);
                var tag = pars.SortTag.Substring(0, pars.SortTag.LastIndexOf(":"));
                var dimension = pars.SortTag.Substring(pars.SortTag.LastIndexOf(":") + 1);
                IEnumerable<int> channels = new List<int>();
                foreach (var type in BoxTypeCache.Instance.Items.Values)
                {
                    var key = new TagNodeKey() { Dimension = dimension, Type = type.TypeID };
                    if (TagCache.Instance.Items.ContainsKey(key))
                    {
                        var vkey = new VideoNodeKey(filter.platform, type.TypeID, filter.auth);
                        var node = TagCache.Instance.Items[key].FirstOrDefault(v => v.Language[CustomArray.LanguageArray[0]].Title == tag);
                        if (node != null && node.Channels.ContainsKey(vkey))
                            channels = channels.Union(node.Channels[vkey]);
                    }
                }
                var query = channels.Where(v =>
                {
                    bool result = true;
                    var video = ListCache.Instance.Dictionary[v];
                    foreach (var f in filters)
                    {
                        result = result && f(video, pars, filter);
                    }
                    return result;
                });
                return CreatePageList(query, filter, pars, true);
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

        /// <summary>
        /// 分页输出
        /// </summary>
        private static string CreatePageList(IEnumerable<int> list, ListFilter filter, VideoPars pars, bool isEx)
        {
            if (filter.order != "s")
                list = VideoSortUtils<VideoNode>.Sort(list, filter);
            var xml = new XElement("vlist");
            xml.Add(new XElement("countInPage", filter.c), new XElement("page", filter.s));
            var count = list.PageList(filter, pars, xml, isEx);
            xml.AddFirst(new XElement("count", count), new XElement("page_count", PageUtils.PageCount(count, filter.c)));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的错误提示
        /// </summary>
        private static string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的影片");
        }
    }
}
