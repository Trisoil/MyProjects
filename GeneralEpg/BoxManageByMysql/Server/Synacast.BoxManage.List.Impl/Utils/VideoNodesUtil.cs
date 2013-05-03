using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Utils
{
    #region Namespaces

    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Tags;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.List.Live.Search;

    #endregion

    public class VideoNodesUtil
    {

        #region FilterFuns

        /// <summary>
        /// 点播基本接口条件过滤方法集
        /// </summary>
        public static List<Func<VideoNode, VideoPars, ListFilter, bool>> FormateVideoFilter(ListFilter filter, VideoPars pars)
        {
            var filters = new List<Func<VideoNode, VideoPars, ListFilter, bool>>(4);
            if (!string.IsNullOrEmpty(filter.beginletter))
                filters.Add(FilterFunUtils.BeginLetter);
            if (pars.Flags != null)
                filters.Add(FilterFunUtils.Flags);
            if (pars.Tags != null)
                filters.Add(FilterFunUtils.Tags);
            if (filter.ver <= 1)
            {
                filters.Add(FilterFunUtils.Ver1);
            }
            else
            {
                filters.Add(FilterFunUtils.Ver2);
                if (filter.screen != "v")
                {
                    filters.Add(FilterFunUtils.Virtual);
                }
            }
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.Forbidden);  
            }
            return filters;
        }

        /// <summary>
        /// 点播扩展接口条件过滤方法集
        /// </summary>
        public static List<Func<VideoNode, VideoPars, ListFilter, bool>> FormateVideoExFilter(ListFilter filter, VideoPars pars)
        {
            var filters = new List<Func<VideoNode, VideoPars, ListFilter, bool>>(10);
            if (!string.IsNullOrEmpty(filter.beginletter))
                filters.Add(FilterFunUtils.BeginLetter);
            if (pars.Flags != null)
                filters.Add(FilterFunUtils.Flags);
            if (pars.Tags != null)
                filters.Add(FilterFunUtils.Tags);
            if (filter.ver <= 1)
            {
                filters.Add(FilterFunUtils.Ver1);
            }
            else
            {
                filters.Add(FilterFunUtils.Ver2);
                if (filter.screen != "v")
                {
                    filters.Add(FilterFunUtils.Virtual);
                }
            }
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.Forbidden);
            }
            if (filter.forbidvip == 1)
                filters.Add(FilterFunUtils.ForbidVip);
            if (filter.bitratemin != 0)
                filters.Add(FilterFunUtils.BitMin);
            if (filter.bitratemax != 0)
                filters.Add(FilterFunUtils.BitMax);
            if (filter.wmin != 0)
                filters.Add(FilterFunUtils.WMin);
            if (filter.wmax != 0)
                filters.Add(FilterFunUtils.WMax);
            if (filter.hmin != 0)
                filters.Add(FilterFunUtils.HMin);
            if (filter.hmax != 0)
                filters.Add(FilterFunUtils.HMax);
            return filters;
        }
        
        /// <summary>
        /// 直播基本接口条件过滤方法集
        /// </summary>
        public static List<Func<LiveVideoNode, VideoPars, LiveFilter, bool>> FormateLiveVideoFilter(LiveFilter filter, VideoPars pars)
        {
            var filters = new List<Func<LiveVideoNode, VideoPars, LiveFilter, bool>>(4);
            filters.Add(FilterFunUtils.LiveHiddenFlag);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.LiveForbidden);
            }
            if (!string.IsNullOrEmpty(filter.beginletter))
                filters.Add(FilterFunUtils.LiveBeginLetter);
            if (pars.Flags != null)
                filters.Add(FilterFunUtils.LiveFlags);
            if (filter.ver <= 1)
                filters.Add(FilterFunUtils.LiveVer1);
            else
                filters.Add(FilterFunUtils.LiveVer2);
            return filters;
        }

        /// <summary>
        /// 直播扩展接口条件过滤方法集
        /// </summary>
        public static List<Func<LiveVideoNode, VideoPars, LiveFilter, bool>> FormateLiveVideoExFilter(LiveFilter filter, VideoPars pars)
        {
            var filters = new List<Func<LiveVideoNode, VideoPars, LiveFilter, bool>>(11);
            filters.Add(FilterFunUtils.LiveHiddenFlag);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.LiveForbidden);
            }
            if (!string.IsNullOrEmpty(filter.beginletter))
                filters.Add(FilterFunUtils.LiveBeginLetter);
            if (pars.Flags != null)
                filters.Add(FilterFunUtils.LiveFlags);
            if (filter.ver <= 1)
                filters.Add(FilterFunUtils.LiveVer1);
            else
                filters.Add(FilterFunUtils.LiveVer2);
            if (filter.forbidvip == 1)
                filters.Add(FilterFunUtils.LiveForbidVip);
            if (filter.bitratemin != 0)
                filters.Add(FilterFunUtils.LiveBitMin);
            if (filter.bitratemax != 0)
                filters.Add(FilterFunUtils.LiveBitMax);
            if (filter.wmin != 0)
                filters.Add(FilterFunUtils.LiveWMin);
            if (filter.wmax != 0)
                filters.Add(FilterFunUtils.LiveWMax);
            if (filter.hmin != 0)
                filters.Add(FilterFunUtils.LiveHMin);
            if (filter.hmax != 0)
                filters.Add(FilterFunUtils.LiveHMax);
            return filters;
        }

        /// <summary>
        /// 不基于版本的通用扩展接口过滤方法集
        /// </summary>
        public static List<Func<VideoBase, ExFilterBase, bool>> FormateTagFilter(ExFilterBase filter)
        {
            var filters = new List<Func<VideoBase, ExFilterBase, bool>>(7);
            FormatForbiddenName(filter);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.CommonForbidden);
            }
            if (filter.forbidvip == 1)
                filters.Add(FilterFunUtils.ComForbidVip);
            if (filter.bitratemax != 0)
                filters.Add(FilterFunUtils.ComBitMax);
            if (filter.bitratemin != 0)
                filters.Add(FilterFunUtils.ComBitMin);
            if (filter.hmax != 0)
                filters.Add(FilterFunUtils.ComHMax);
            if (filter.hmin != 0)
                filters.Add(FilterFunUtils.ComHMin);
            if (filter.wmax != 0)
                filters.Add(FilterFunUtils.ComWMax);
            if (filter.wmin != 0)
                filters.Add(FilterFunUtils.ComWMin);
            return filters;
        }

        /// <summary>
        /// 基于版本的点播通用基本过滤方法集
        /// </summary>
        public static List<Func<VideoBase, ExFilterBase, bool>> FormateVodFilter(ExFilterBase filter) 
        {
            var filters = new List<Func<VideoBase, ExFilterBase, bool>>(8);
            FormatForbiddenName(filter);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.CommonForbidden);
            }
            if (filter.ver <= 1)
            {
                filters.Add(FilterFunUtils.ComVer1);
            }
            else
            {
                filters.Add(FilterFunUtils.ComVer2);
                if (filter.screen != "v")
                {
                    filters.Add(FilterFunUtils.ComVirtual);
                }
            }
            if (filter.forbidvip == 1)
                filters.Add(FilterFunUtils.ComForbidVip);
            if (filter.bitratemax != 0)
                filters.Add(FilterFunUtils.ComBitMax);
            if (filter.bitratemin != 0)
                filters.Add(FilterFunUtils.ComBitMin);
            if (filter.hmax != 0)
                filters.Add(FilterFunUtils.ComHMax);
            if (filter.hmin != 0)
                filters.Add(FilterFunUtils.ComHMin);
            if (filter.wmax != 0)
                filters.Add(FilterFunUtils.ComWMax);
            if (filter.wmin != 0)
                filters.Add(FilterFunUtils.ComWMin);
            return filters;
        }

        public static List<Func<VideoBase, ExFilterBase, bool>> CommonFilter(ExFilterBase filter)
        {
            var filters = new List<Func<VideoBase, ExFilterBase, bool>>(4);
            FormatForbiddenName(filter);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.CommonForbidden);
            }
            if (filter.ver <= 1)
            {
                filters.Add(FilterFunUtils.ComVer1);
            }
            else
            {
                filters.Add(FilterFunUtils.ComVer2);
                if (filter.screen != "v")
                {
                    filters.Add(FilterFunUtils.ComVirtual);
                }
            }
            if (filter.forbidvip == 1)
                filters.Add(FilterFunUtils.ComForbidVip);
            filters.Add(FilterFunUtils.Common);
            return filters;
        }

        public static List<Func<VideoBase, ExFilterBase, bool>> CommonCustomFilter(ExFilterBase filter)
        {
            var filters = new List<Func<VideoBase, ExFilterBase, bool>>(1);
            FormatForbiddenName(filter);
            if (filter.forbidden)
            {
                filters.Add(FilterFunUtils.CommonForbidden);
            }
            return filters;
        }

        private static void FormatForbiddenName(ExFilterBase filter)
        {
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        filter.commonfbcode = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
        }

        #endregion

        #region Deleted

        /// <summary>
        /// 根据比特率等各参数返回所有符合条件的节目
        /// </summary>
        /// <param name="source">源节目集合</param>
        /// <param name="filter">比特率等过滤条件</param>
        /// <returns>符合条件的节目集合</returns>
        //public static List<VideoNode> VideosExFilter(IEnumerable<VideoNode> source, ExFilterBase filter)
        //{
        //    var vsource = source;
        //    if (filter.bitratemin != 0 || filter.bitratemax != 0)
        //    {
        //        vsource = vsource.OrderBy(v => v.Main.BitRate).SkipWhile(v => v.Main.BitRate < filter.bitratemin);
        //        if (filter.bitratemax != 0)
        //            vsource = vsource.TakeWhile(v => v.Main.BitRate <= filter.bitratemax);
        //    }
        //    if (filter.hmin != 0 || filter.hmax != 0)
        //    {
        //        vsource = vsource.OrderBy(v => v.Main.VideoHeight).SkipWhile(v => v.Main.VideoHeight < filter.hmin);
        //        if (filter.hmax != 0)
        //            vsource = vsource.TakeWhile(v => v.Main.VideoHeight <= filter.hmax);
        //    }
        //    if (filter.wmin != 0 || filter.wmax != 0)
        //    {
        //        vsource = vsource.OrderBy(v => v.Main.VideoWidth).SkipWhile(v => v.Main.VideoWidth < filter.wmin);
        //        if (filter.wmax != 0)
        //            vsource = vsource.TakeWhile(v => v.Main.VideoWidth <= filter.wmax);
        //    }
        //    return vsource.ToList();
        //}

        #endregion

    }
}
