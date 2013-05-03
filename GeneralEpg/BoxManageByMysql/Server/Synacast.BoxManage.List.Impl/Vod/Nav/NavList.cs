using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Nav
{
    using Synacast.BoxManage.List.Nav;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.List.Impl.Utils;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class NavList : INav 
    {
        #region INav 成员

        public string Nav(NavFilter filter)
        {
            try
            {
                var res = RecommandCache.Instance.Items[filter.mode];
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                var xml = new XElement("navs");
                if (filters.Count <= 0)
                {
                    xml.Add(from re in res
                            select new XElement("nav",
                                new XElement("navid", re.Id),
                                new XElement("name", re.Language[filter.lang].Title),
                                new XElement("image", re.PicLink)
                            ));
                }
                else
                {
                    xml.Add(from re in res
                            let count = GetChannelCount(re, key, filter, filters)
                            where count > 0
                            select new XElement("nav",
                            new XElement("navid", re.Id),
                            new XElement("name", re.Language[filter.lang].Title),
                            new XElement("image", re.PicLink)
                            ));
                }
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoModeNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string NavEx(NavFilter filter)
        {
            try
            {
                var res = RecommandCache.Instance.Items[filter.mode];
                var xml = new XElement("navs");
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var filters = VideoNodesUtil.FormateVodFilter(filter);
                xml.Add(from re in res
                        let count = GetChannelCount(re, key, filter, filters)
                        where count > 0
                        select new XElement("nav",
                        new XElement("navid", re.Id),
                        new XElement("name", re.Language[filter.lang].Title),
                        new XElement("count", count),
                        new XElement("image", re.PicLink)
                        ));
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoModeNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string NavLists(NavFilter filter)
        {
            try
            {
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var filters = VideoNodesUtil.FormateVodFilter(filter);
                var rel = RecommandCache.Instance.Dictionary[filter.navid];
                var query = rel.Channels[key].Where(v =>
                {
                    bool result = true;
                    var video = ListCache.Instance.Dictionary[v];
                    foreach (var f in filters)
                    {
                        result = result && f(video, filter);
                    }
                    return result;
                });
                return CreatePageList(rel, query, filter, pars, false);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatformNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string NavListsEx(NavFilter filter)
        {
            try
            {
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var rel = RecommandCache.Instance.Dictionary[filter.navid];
                var filters = VideoNodesUtil.FormateVodFilter(filter);
                var query = rel.Channels[key].Where(v =>
                {
                    bool result = true;
                    var video = ListCache.Instance.Dictionary[v];
                    foreach (var f in filters)
                    {
                        result = result && f(video, filter);
                    }
                    return result;
                });
                return CreatePageList(rel, query, filter, pars, true);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatformNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        /// <summary>
        /// 得到该推荐分类下的频道数
        /// </summary>
        private int GetChannelCount(RecommandNode node, VideoNodeKey key, NavFilter filter, List<Func<VideoBase, ExFilterBase, bool>> filters)
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
                        result = result && f(ListCache.Instance.Dictionary[c], filter);
                    }
                    if (result)
                        count++;
                }
            }
            return count;
        }

        /// <summary>
        /// 分页输出
        /// </summary>
        private string CreatePageList(RecommandNode recommand, IEnumerable<int> videos, NavFilter filter, VideoPars pars, bool isEx)
        {
            var xml = new XElement("vlist");
            xml.Add(new XElement("nav_id", recommand.Id),
                new XElement("nav_name", recommand.Language[filter.lang].Title),
                new XElement("count", 0),
                new XElement("page_count", 0)
                );
            var count = videos.PageList(filter, pars, xml, isEx);
            xml.Element("count").SetValue(count);
            xml.Element("page_count").SetValue(PageUtils.PageCount(count, filter.c));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的提示错误
        /// </summary>
        /// <returns></returns>
        private string NoModeNav()
        {
            return BoxUtils.FormatErrorMsg("不存在该mode或该平台下的导航数据");
        }

        private string NoPlatformNav()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的导航影片");
        }
    }
}
