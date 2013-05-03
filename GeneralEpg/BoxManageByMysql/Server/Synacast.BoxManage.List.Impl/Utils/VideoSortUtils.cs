using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Impl.Utils
{
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;

    public class VideoSortUtils<T> where T : VideoBase
    {
        /// <summary>
        /// 频道排序，节目获取接口使用
        /// </summary>
        public static IEnumerable<T> SortVideos(IEnumerable<T> query, ListFilter filter, VideoPars pars)
        {
            if (!string.IsNullOrEmpty(filter.order))
            {
                if (filter.order == "t")   //热度、排行榜
                    query = HotSort(query, filter);
                else if (filter.order == "n")  //更新时间
                    query = query.OrderByDescending(video => video.Main.ModifyTime);
                else            //标准
                    query = StandardSort(query, filter, pars);
            }
            else            //标准
            {
                query = StandardSort(query, filter, pars);
            }
            return query;
        }

        /// <summary>
        /// 直播频道排序，节目获取接口使用
        /// </summary>
        public static IEnumerable<LiveVideoNode> SortLiveVideos(IEnumerable<LiveVideoNode> query, LiveFilter filter, VideoPars pars)
        {
            if (!string.IsNullOrEmpty(filter.order))
            {
                if (filter.order == "t")   //热度、排行榜
                    query = HotLiveSort(query, filter);
                else if (filter.order == "n")  //更新时间
                    query = query.OrderByDescending(video => video.Main.ModifyTime);
                else if (filter.order == "a")
                    query = query.OrderBy(video => video.ChannelName);
                else if (filter.order == "g")
                    query = query.OrderByDescending(video => video.BKInfo.Score);
                else            //标准
                    query = StandardLiveSort(query, filter, pars);
            }
            else            //标准
            {
                query = StandardLiveSort(query, filter, pars);
            }
            return query;
        }

        /// <summary>
        /// 直播标准排序
        /// </summary>
        public static IEnumerable<LiveVideoNode> StandardLiveSort(IEnumerable<LiveVideoNode> query, LiveFilter filter, VideoPars pars)
        {
            if (string.IsNullOrEmpty(pars.SortTag))
            {
                //AuthKey key = new AuthKey() { Auth = filter.auth, PlatForm = filter.platform };
                //if (PlatFormCache.Instance.AuthItems.ContainsKey(key))
                //{
                //    return query.OrderBy(v => { if (v.MainRank.ContainsKey(key))return v.MainRank[key]; return 0; });
                //}
                //key.Auth = null;
                //return query.OrderBy(v => { if (v.MainRank.ContainsKey(key))return v.MainRank[key]; return 0; });
                //return query.OrderByDescending(video => video.Hot);
                return query.OrderBy(v => v.Rank);
            }
            return query.OrderBy(video => video.TagsRank[pars.SortTag]);
        }

        /// <summary>
        /// 直播热度、排行榜排序
        /// </summary>
        public static IEnumerable<LiveVideoNode> HotLiveSort(IEnumerable<LiveVideoNode> query, ExFilterBase filter)
        {
            //if (!string.IsNullOrEmpty(filter.beginletter))
            //    return query.Where(v => v.SpellName.StartsWith(filter.beginletter)).OrderByDescending(v => v.Hot);
            return query.OrderByDescending(video => video.Hot);
        }

        /// <summary>
        /// 标准排序
        /// </summary>
        public static IEnumerable<T> StandardSort(IEnumerable<T> query, ExFilterBase filter, VideoPars pars)
        {
            if (string.IsNullOrEmpty(pars.SortTag))
                //return query.OrderByDescending(video => video.Main.CreateTime);
                return query.OrderByDescending(video => video.Main.Rank).ThenByDescending(video => video.Main.CreateTime);
            return query.OrderBy(video => video.TagsRank[pars.SortTag]);
        }

        /// <summary>
        /// 热度、排行榜排序
        /// </summary>
        public static IEnumerable<T> HotSort(IEnumerable<T> query, ExFilterBase filter)
        {
            //if (!string.IsNullOrEmpty(filter.beginletter))
            //    return query.Where(v => v.SpellName.StartsWith(filter.beginletter)).OrderByDescending(v => v.Hot);
            return query.OrderByDescending(video => video.Hot);
        }

        public static IEnumerable<int> Sort(IEnumerable<int> query, ListFilter filter)
        {
            switch (filter.order)
            { 
                case "s":
                    return query;
                case "t":
                    return query.OrderByDescending(v => ListCache.Instance.Dictionary[v].Hot);
                case "n":
                    return query.OrderByDescending(v => ListCache.Instance.Dictionary[v].Main.ModifyTime);
                case "a":
                    return query.OrderBy(v => ListCache.Instance.Dictionary[v].ChannelName);
                case "g":
                    return query.OrderByDescending(v => ListCache.Instance.Dictionary[v].BKInfo.Score);
                default :
                    return query;
            }
        }
    }
}
