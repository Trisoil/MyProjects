using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Live.Nav
{
    using Synacast.BoxManage.List.Live.Nav;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class LiveNav : ILiveNav
    {
        #region ILiveNav 成员

        public string LiveRecommandNav(LiveNavFilter filter)
        {
            try
            {
                var recommands = LiveRecommandCache.Instance.Items[new RecommandKey(filter.mode, filter.platform, filter.auth)];
                var root = new XElement("navs");
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                if (filters.Count <= 0)
                {
                    root.Add(from recommand in recommands select CreateNav(recommand, filter));
                }
                else
                {
                    root.Add(from recommand in recommands let vs = FilterList(recommand, filters, filter) where vs.Count > 0 select CreateNav(recommand, filter));
                }
                return root.ToString(SaveOptions.DisableFormatting);
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

        public string LiveRecommandNavEx(LiveNavFilter filter)
        {
            try
            {
                var recommands = LiveRecommandCache.Instance.Items[new RecommandKey(filter.mode, filter.platform, filter.auth)];
                var root = new XElement("navs");
                IEnumerable<XElement> query = null;
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                query = from recommand in recommands let vs = FilterList(recommand, filters, filter) where vs.Count > 0 select CreateNavEx(recommand, vs.Count, filter);
                root.Add(query);
                return root.ToString(SaveOptions.DisableFormatting);
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

        public string LiveRecommandList(LiveNavFilter filter)
        {
            try
            {
                var recommands = LiveRecommandCache.Instance.Items[new RecommandKey(filter.platform, filter.auth, filter.navid)];
                if (recommands.Count <= 0)
                    return NoModeNav();
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                if (filters.Count <= 0)
                {
                    return CreatePageList(recommands[0], recommands[0].Lists, filter, pars, false);
                }
                else
                {
                    var videos = FilterList(recommands[0], filters, filter);
                    return CreatePageList(recommands[0], videos, filter, pars, false);
                }
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

        public string LiveRecommandListEx(LiveNavFilter filter)
        {
            try
            {
                var recommands = LiveRecommandCache.Instance.Items[new RecommandKey(filter.platform, filter.auth, filter.navid)];
                if (recommands.Count <= 0)
                    return NoModeNav();
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                var videos = FilterList(recommands[0], filters, filter);
                return CreatePageList(recommands[0], videos, filter, pars, true);
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
        /// 生成基本导航分类
        /// </summary>
        private XElement CreateNav(LiveRecommandNode recommand, LiveNavFilter filter)
        {
            return new XElement("nav",
                new XElement("navid", recommand.ID),
                new XElement("name", recommand.Language[filter.lang].Title ),
                new XElement("image", recommand.PicLink)
                );
        }

        /// <summary>
        /// 生成扩展导航分类
        /// </summary>
        private XElement CreateNavEx(LiveRecommandNode recommand, int count, LiveNavFilter filter)
        {
            return new XElement("nav",
                new XElement("navid", recommand.ID),
                new XElement("name", recommand.Language[filter.lang].Title),
                new XElement("count", count),
                new XElement("image", recommand.PicLink)
                );
        }

        /// <summary>
        /// 过滤不符合条件的频道
        /// </summary>
        private List<LiveVideoNode> FilterList(LiveRecommandNode recommand, List<Func<VideoBase, ExFilterBase, bool>> filters, LiveNavFilter filter)
        {
            var videos = new List<LiveVideoNode>();
            foreach (var video in recommand.Lists)
            {
                bool result = true;
                foreach (var f in filters)
                    result = result && f(video, filter);
                if (result)
                    videos.Add(video);
            }
            return videos;
        }

        /// <summary>
        /// 分页输出
        /// </summary>
        private string CreatePageList(LiveRecommandNode recommand, List<LiveVideoNode> videos, LiveNavFilter filter, VideoPars pars, bool isEx)
        {
            var vkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var xml = new XElement("vlist");
            xml.Add(new XElement("nav_id", recommand.ID));
            xml.Add(new XElement("nav_name", recommand.Language[filter.lang].Title));
            xml.Add(new XElement("count", videos.Count));
            xml.Add(new XElement("page_count", PageUtils.PageCount(videos.Count, filter.c)));
            if (filter.c != 0 && filter.s != 0)
            {
                //currentVideos = videos.Skip(filter.c * (filter.s - 1)).Take(filter.c).ToList();
                xml.Add(new XElement("countInPage", filter.c));
                xml.Add(new XElement("page", filter.s));

                var index = (filter.s - 1) * filter.c;
                var max = index + filter.c;
                if (max > videos.Count)
                    max = videos.Count;
                for (var i = index; i < max; i++)
                {
                    xml.Add(VideoResponseUtils.CustomLiveListRes(videos[i], filter, 0, isEx));
                }
            }
            else
            {
                xml.Add(from v in videos select VideoResponseUtils.CustomLiveListRes(v, filter, 0, isEx));
            }
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
