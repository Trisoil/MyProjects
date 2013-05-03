using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Lists
{
    #region NameSpaces

    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.List.Impl.Vod.Lists;
    using Synacast.BoxManage.Core.Cache.Custom;

    #endregion

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class List : IList
    {
        #region IList 成员

        /// <summary>
        /// 获取视频数据基本接口
        /// </summary>
        public string VideoList(ListFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormatVideoPars(filter);
                var vkey = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var filters = VideoNodesUtil.FormateVideoFilter(filter, pars);
                IEnumerable<int> channels;
                if (filter.treeleftid != 0)
                {
                    var node = TreeNodeCache.Instance.Items[filter.treeleftid];
                    if (node.Channels.ContainsKey(vkey))
                        channels = VideoSortUtils<VideoNode>.Sort(node.Channels[vkey], filter);
                    else
                        channels = new List<int>();
                }
                else if (!string.IsNullOrEmpty(pars.SortTag))
                {
                    try
                    {
                        var tag = pars.SortTag.Substring(0, pars.SortTag.LastIndexOf(":"));
                        var dimension = pars.SortTag.Substring(pars.SortTag.LastIndexOf(":") + 1);
                        var key = new TagNodeKey() { Dimension = dimension, Type = filter.type };
                        if (TagCache.Instance.Items.ContainsKey(key))
                        {
                            var node = TagCache.Instance.Items[key].FirstOrDefault(v => v.Language[CustomArray.LanguageArray[0]].Title == tag);
                            if (node != null && node.Channels.ContainsKey(vkey))
                                channels = node.Channels[vkey];
                            else
                                channels = new List<int>();
                        }
                        else
                            channels = new List<int>();
                    }
                    catch
                    {
                        return BoxUtils.FormatErrorMsg("ntags分析错误");
                    }
                }
                else
                {
                    channels = ListCache.Instance.Items[vkey].OrderArray;
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
                return CreatePageList(VideoSortUtils<VideoNode>.Sort(query, filter), filter, pars, false);
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
        /// 获取视频数据扩展接口
        /// </summary>
        public string VideoListEx(ListFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormatVideoPars(filter);
                var vkey = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var filters = VideoNodesUtil.FormateVideoExFilter(filter, pars);
                IEnumerable<int> channels;
                if (filter.treeleftid != 0)
                {
                    var node = TreeNodeCache.Instance.Items[filter.treeleftid];
                    if (node.Channels.ContainsKey(vkey))
                        channels = VideoSortUtils<VideoNode>.Sort(node.Channels[vkey], filter);
                    else
                        channels = new List<int>();
                }
                else if (!string.IsNullOrEmpty(pars.SortTag))
                {
                    try
                    {
                        string tag = pars.SortTag.Substring(0, pars.SortTag.LastIndexOf(":"));
                        string dimension = pars.SortTag.Substring(pars.SortTag.LastIndexOf(":") + 1);
                        var key = new TagNodeKey() { Dimension = dimension, Type = filter.type };
                        if (TagCache.Instance.Items.ContainsKey(key))
                        {
                            var node = TagCache.Instance.Items[key].FirstOrDefault(v => v.Language[CustomArray.LanguageArray[0]].Title == tag);
                            if (node != null && node.Channels.ContainsKey(vkey))
                                channels = node.Channels[vkey];
                            else
                                channels = new List<int>();
                        }
                        else
                            channels = new List<int>();
                    }
                    catch
                    {
                        return BoxUtils.FormatErrorMsg("ntags分析错误");
                    }
                }
                else
                {
                    channels = ListCache.Instance.Items[vkey].OrderArray;
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
                return CreatePageList(VideoSortUtils<VideoNode>.Sort(query, filter), filter, pars, true);
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

        #region Methods

        /// <summary>
        /// 分页输出
        /// </summary>
        private static string CreatePageList(IEnumerable<int> list, ListFilter filter, VideoPars pars, bool isEx)
        {
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

        #endregion
    }
}
