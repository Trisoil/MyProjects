using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Live
{
    #region Namespaces

    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    #endregion

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class LiveList : ILive 
    {
        #region ILive 成员

        /// <summary>
        /// 直播分类信息基本接口
        /// </summary>
        public string LiveTypes(LiveFilter filter)
        {
            try
            {
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                var root = new XElement("types");
                if (filters.Count <= 0)
                {
                    root.Add(
                        from livetype in LiveTypeDicCache.Instance.Items
                        where livetype.Channels.ContainsKey(key)
                        select new XElement("type",
                            new XElement("tid", livetype.ID),
                            new XElement("name", livetype.Language[filter.lang].Title),
                            new XElement("image", livetype.PicLink)
                    ));
                }
                else
                {
                    root.Add(from livetype in LiveTypeDicCache.Instance.Items
                             where livetype.Channels.ContainsKey(key)
                             let count = GetChannelCount(livetype, key, filter, filters)
                             where count > 0
                             select new XElement("type",
                                new XElement("tid", livetype.ID),
                                new XElement("name", livetype.Language[filter.lang].Title),
                                new XElement("image", livetype.PicLink)
                        ));
                }
                return root.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatLive();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 直播分类信息扩展接口
        /// </summary>
        public string LiveTypesEx(LiveFilter filter)
        {
            try
            {
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var root = new XElement("types");
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                root.Add(from livetype in LiveTypeDicCache.Instance.Items
                         where livetype.Channels.ContainsKey(key)
                         let count = GetChannelCount(livetype, key, filter, filters)
                         where count > 0
                         select new XElement("type",
                            new XElement("tid", livetype.ID),
                            new XElement("name", livetype.Language[filter.lang].Title),
                            new XElement("count", count),
                            new XElement("image", livetype.PicLink)
                    ));
                return root.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatLive();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 获取节目信息基本接口
        /// </summary>
        public string LiveLists(LiveFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormatLiveVideoPars(filter);
                var filters = VideoNodesUtil.FormateLiveVideoFilter(filter, pars);
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var query = LiveListCache.Instance.Items[key].Where(v =>
                {
                    bool result = true;
                    foreach (var f in filters)
                    {
                        result = result && f(v, pars, filter);
                    }
                    return result;
                });
                return CreatePageList(VideoSortUtils<LiveVideoNode>.SortLiveVideos(query, filter, pars), filter, pars, key, false);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatLive();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 获取节目信息扩展接口
        /// </summary>
        public string LiveListsEx(LiveFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormatLiveVideoPars(filter);
                var filters = VideoNodesUtil.FormateLiveVideoExFilter(filter, pars);
                var key = new VideoNodeKey(filter.platform, filter.type, filter.auth);
                var query = LiveListCache.Instance.Items[key].Where(v =>
                {
                    bool result = true;
                    foreach (var f in filters)
                    {
                        result = result && f(v, pars, filter);
                    }
                    return result;
                });
                return CreatePageList(VideoSortUtils<LiveVideoNode>.SortLiveVideos(query, filter, pars), filter, pars, key, true);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatLive();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        /// <summary>
        /// 获取节目预告列表
        /// </summary>
        public string LiveParade(LiveFilter filter)
        {
            try
            {
                var video = LiveListCache.Instance.Dictionary[filter.vid];
                var parades = video.Items.Where(p => p.StartTime.Date.Equals(filter.date.Date)).OrderBy(p => p.StartTime);
                return CreateParades(video, parades, filter);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatLive();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        private int GetChannelCount(LiveTypeNode node, VideoNodeKey key, LiveFilter filter, List<Func<VideoBase, ExFilterBase, bool>> filters)
        {
            int count = 0;
            if (node.Channels.ContainsKey(key))
            {
                var cs = node.Channels[key];
                foreach (var c in cs)
                {
                    var result = true;
                    foreach (var f in filters)
                    {
                        result = result && f(LiveListCache.Instance.Dictionary[c], filter);
                    }
                    if (result)
                        count++;
                }
            }
            return count;
        }

        /// <summary>
        /// 分页输出频道列表
        /// </summary>
        private string CreatePageList(IEnumerable<LiveVideoNode> list, LiveFilter filter, VideoPars pars, VideoNodeKey key, bool isEx)
        {
            var xml = new XElement("vlist");
            xml.Add(new XElement("countInPage", filter.c), new XElement("page", filter.s));
            var count = list.PageList(filter, pars, xml, isEx);
            xml.AddFirst(new XElement("count", count), new XElement("page_count", PageUtils.PageCount(count, filter.c)));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 输出频道预告列表
        /// </summary>
        private string CreateParades(LiveVideoNode video, IEnumerable<ChannelLiveItemsNode> parades, LiveFilter filter)
        {
            var root = new XElement("v",
                new XElement("vid", video.Main.ChannelID),
                new XElement("title", video.Language[filter.lang].ChannelName),
                new XElement("parade-date", filter.date.ToString("yyyy-MM-dd"))
                );
            var paradelist = new XElement("parade_list");
            paradelist.Add(from parade in parades select VideoResponseUtils.LiveListParade(parade, filter));
            root.Add(paradelist);
            return root.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 找不到Key的情况
        /// </summary>
        /// <returns></returns>
        private string NoPlatLive()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的直播信息");
        }
    }
}
