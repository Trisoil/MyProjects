using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Utils
{
    #region Namespaces

    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Vod.Serach;
    using Synacast.BoxManage.List.Vod.Relevance;
    using Synacast.BoxManage.List.Live.Search;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.LuceneNet;
    using Synacast.BoxManage.Core.LuceneNet.Tag;
    using Synacast.BoxManage.Core.LuceneNet.Live;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.LuceneNetSearcher.Searcher;

    #endregion

    public class LuceneNetUtil
    {
        /// <summary>
        /// 点播基本搜索
        /// </summary>
        public static LuceneResultNode Search(ListFilter filter, string[] fields, Lucene.Net.Search.BooleanClause.Occur[] occurs, string query, bool isStat)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.PlatformIndex, Text = pindx });
            if (!string.IsNullOrEmpty(filter.flag))
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Flags, Text = VideoParsUtils.OrderAscFlags(filter.flag) });
            }
            if (filter.ver <= 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.TableType, Text = "0" });
            }
            else
            {
                if (filter.screen == "v")
                {
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.IsGroup, Text = "0" });
                }
                else
                {
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Screen, Text = "0" });
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.IsGroup, Text = "0" });
                }
            }
            //if (filter.type != 0)
            //{
            //    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Type, Text = filter.type.ToString() });
            //}
            var node = new SearchNode();
            node.Key = filter.k;
            node.Pindex = pindx;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = fields;
            node.Occurs = occurs;
            //node.SortFields = EpgIndexNode.SortFields;
            node.ContainFilters = containfilters;
            node.LongRnageFilters = new List<LongRangeNode>();
            node.Start = filter.s;
            node.Limit = filter.c;
            node.IndexValue = EpgIndexNode.IndexValue;
            node.IsStat = isStat;
            node.VodType = filter.type;
            node.ShowNav = filter.shownav;
            node.TotalTypes = BoxTypeCache.Instance.Items.Count;
            return Searcher.Search(EpgLuceneNet.Instance.IndexDic, node);
        }

        /// <summary>
        /// 点播扩展搜索
        /// </summary>
        public static LuceneResultNode SearchEx(ListFilter filter, string[] fields, Lucene.Net.Search.BooleanClause.Occur[] occurs, string query, bool isStat)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var rangenodes = new List<NumberRangeNode> ();
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.PlatformIndex, Text = pindx });
            if (filter.bitratemin != 0 || filter.bitratemax != 0)
            {
                if (filter.bitratemax == 0) filter.bitratemax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.BitrateRange, MinValue = filter.bitratemin, MaxValue = filter.bitratemax });
            }
            if (filter.hmin != 0 || filter.hmax != 0)
            {
                if (filter.hmax == 0) filter.hmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.HightRange, MinValue = filter.hmin, MaxValue = filter.hmax });
            }
            if (filter.wmin != 0 || filter.wmax != 0)
            {
                if (filter.wmax == 0) filter.wmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.WidthRange, MinValue = filter.wmin, MaxValue = filter.wmax });
            }
            if (!string.IsNullOrEmpty(filter.flag))
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Flags, Text = VideoParsUtils.OrderAscFlags(filter.flag) });
            }
            if (filter.forbidvip == 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.VipFlag, Text = "0" });
            }
            if (filter.ver <= 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.TableType, Text = "0" });
            }
            else
            {
                if (filter.screen == "v")
                {
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.IsGroup, Text = "0" });
                }
                else
                {
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Screen, Text = "0" });
                    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.IsGroup, Text = "0" });
                }
            }
            //if (filter.type != 0)
            //{
            //    containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Type, Text = filter.type.ToString() });
            //}
            var node = new SearchNode();
            node.Key = filter.k;
            node.Pindex = pindx;
            node.Start = filter.s;
            node.Limit = filter.c;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = fields;
            node.Occurs = occurs;
            //node.SortFields = EpgIndexNode.SortFields;
            node.NumberRangeFiters = rangenodes;
            node.ContainFilters = containfilters;
            node.LongRnageFilters = new List<LongRangeNode>();
            node.IndexValue = EpgIndexNode.IndexValue;
            node.IsStat = isStat;
            node.VodType = filter.type;
            node.ShowNav = filter.shownav;
            node.TotalTypes = BoxTypeCache.Instance.Items.Count;
            return Searcher.SearchEx(EpgLuceneNet.Instance.IndexDic, node);
        }

        /// <summary>
        /// Tag搜索
        /// </summary>
        public static int TagSearch(TagSearchFilter filter, string[] fields, string query, XElement root)
        {
            var containfilters = new List<ContainFilterNode>();
            var pindx = VideoNodeKeyArray.Instance.Items[new VideoNodeKey(filter.platform, 0, filter.auth)].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Flags, Text = filter.dimension });
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.PlatformIndex, Text = pindx });
            //var fbname = string.Empty;
            //if (filter.forbidden)
            //{
            //    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
            //    {
            //        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
            //    }
            //    else
            //    {
            //        filter.forbidden = false;
            //    }
            //}
            //if (filter.forbidden)
            //{
            //    containfilters.Add(new ContainFilterNode() { FieldName = string.Format("{0}{1}", EpgIndexNode.ForbiddenCode, pindx), Text = fbname.ToLower() });
            //}
            var node = new SearchNode();
            if (filter.lang == CustomArray.LanguageArray[1])
                node.IsNoPaging = true;
            node.Key = filter.k;
            node.Pindex = pindx;
            node.Queries = query;
            node.Fields = fields;
            node.Occurs = EpgIndexNode.NameOccurs;
            node.Start = filter.s;
            node.Limit = filter.c;
            node.ContainFilters = containfilters;
            node.IndexValue = EpgIndexNode.IndexValue;
            return Searcher.DistinctSearch(TagLuceneNet.Instance.IndexDic, node, root);
        }

        /// <summary>
        /// 点播关联推荐
        /// </summary>
        public static LuceneResultNode RelevanceSearch(RelevanceFilter filter, string query, int maxHit)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.PlatformIndex, Text = pindx });
            if (filter.ver > 1 && filter.screen != "v")
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Screen, Text = "0" });
            }
            var node = new SearchNode();
            node.Key = filter.vids;
            node.Pindex = pindx;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = EpgIndexNode.RelevanceFields;
            node.Occurs = EpgIndexNode.RelevanceOccurs;
            node.ContainFilters = containfilters;
            node.Start = 1;
            node.Limit = maxHit;
            node.IndexValue = EpgIndexNode.IndexValue;
            return Searcher.SearchMaxHit(EpgLuceneNet.Instance.IndexDic, node, maxHit);
        }

        /// <summary>
        /// 点播扩展关联推荐
        /// </summary>
        public static LuceneResultNode RelevanceSearchEx(RelevanceFilter filter, string query, int maxHit)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var rangenodes = new List<NumberRangeNode>();
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.PlatformIndex, Text = pindx });
            if (filter.bitratemin != 0 || filter.bitratemax != 0)
            {
                if (filter.bitratemax == 0) filter.bitratemax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.BitrateRange, MinValue = filter.bitratemin, MaxValue = filter.bitratemax });
            }
            if (filter.hmin != 0 || filter.hmax != 0)
            {
                if (filter.hmax == 0) filter.hmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.HightRange, MinValue = filter.hmin, MaxValue = filter.hmax });
            }
            if (filter.wmin != 0 || filter.wmax != 0)
            {
                if (filter.wmax == 0) filter.wmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = EpgIndexNode.WidthRange, MinValue = filter.wmin, MaxValue = filter.wmax });
            }
            if (filter.forbidvip == 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.VipFlag, Text = "0" });
            }
            if (filter.ver > 1 && filter.screen != "v")
            {
                containfilters.Add(new ContainFilterNode() { FieldName = EpgIndexNode.Screen, Text = "0" });
            }
            var node = new SearchNode();
            node.Key = filter.vids;
            node.Pindex = pindx;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = EpgIndexNode.RelevanceFields;
            node.Occurs = EpgIndexNode.RelevanceOccurs;
            //node.SortFields = EpgIndexNode.SortFields;
            node.ContainFilters = containfilters;
            node.NumberRangeFiters = rangenodes;
            node.Start = 1;
            node.Limit = maxHit;
            node.IndexValue = EpgIndexNode.IndexValue;
            return Searcher.SearchMaxHitEx(EpgLuceneNet.Instance.IndexDic, node, maxHit);
        }

        /// <summary>
        /// 直播基本搜索
        /// </summary>
        public static LuceneResultNode LiveSearch(LiveSearchFilter filter, string[] searchFields, string query, bool isNoPaging)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.PlatformIndex, Text = pindx });
            if (!string.IsNullOrEmpty(filter.flag))
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.Flags, Text = VideoParsUtils.OrderAscFlags(filter.flag) });
            }
            if (filter.ver <= 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.TableType, Text = "0" });
            }
            else
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.IsGroup, Text = "0" });
            }
            List<LongRangeNode> longs = new List<LongRangeNode>();
            if (filter.lastday != 0)
            {
                longs.Add(new LongRangeNode() { FieldName = LiveEpgNode.ParadeBeginTime, MinValue = 0, MaxValue = filter.lastday });
            }
            SearchNode node = new SearchNode();
            node.Key = filter.k;
            node.Pindex = pindx;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = searchFields;
            node.Occurs = LiveEpgNode.Occurs;
            node.LongRnageFilters = longs;
            node.ContainFilters = containfilters;
            node.Start = filter.s;
            node.Limit = filter.c;
            node.IsNoPaging = isNoPaging;
            node.IndexValue = LiveEpgNode.IndexValue;
            return Searcher.Search(LiveEpgLuceneNet.Instance.IndexDic, node);
        }

        /// <summary>
        /// 直播扩展搜索
        /// </summary>
        public static LuceneResultNode LiveSearchEx(LiveSearchFilter filter, string[] searchFields, string query, bool isNoPaging)
        {
            var fkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var containfilters = new List<ContainFilterNode>();
            var fbname = string.Empty;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[fkey];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        fbname = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            if (filter.forbidden)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.ForbiddenCode, Text = fbname.ToLower() });
            }
            var pindx = VideoNodeKeyArray.Instance.Items[fkey].ToString();
            containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.PlatformIndex, Text = pindx });
            if (!string.IsNullOrEmpty(filter.flag))
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.Flags, Text = VideoParsUtils.OrderAscFlags(filter.flag) });
            }
            if (filter.ver <= 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.TableType, Text = "0" });
            }
            else
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.IsGroup, Text = "0" });
            }
            if (filter.forbidvip == 1)
            {
                containfilters.Add(new ContainFilterNode() { FieldName = LiveEpgNode.VipFlag, Text = "0" });
            }
            List<LongRangeNode> longs = new List<LongRangeNode>();
            if (filter.lastday != 0)
            {
                longs.Add(new LongRangeNode() { FieldName = LiveEpgNode.ParadeBeginTime, MinValue = 0, MaxValue = filter.lastday });
            }
            List<NumberRangeNode> rangenodes = new List<NumberRangeNode>();
            if (filter.bitratemin != 0 || filter.bitratemax != 0)
            {
                if (filter.bitratemax == 0) filter.bitratemax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = LiveEpgNode.BitrateRange, MinValue = filter.bitratemin, MaxValue = filter.bitratemax });
            }
            if (filter.hmin != 0 || filter.hmax != 0)
            {
                if (filter.hmax == 0) filter.hmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = LiveEpgNode.HightRange, MinValue = filter.hmin, MaxValue = filter.hmax });
            }
            if (filter.wmin != 0 || filter.wmax != 0)
            {
                if (filter.wmax == 0) filter.wmax = int.MaxValue;
                rangenodes.Add(new NumberRangeNode() { FieldName = LiveEpgNode.WidthRange, MinValue = filter.wmin, MaxValue = filter.wmax });
            }
            SearchNode node = new SearchNode();
            node.Key = filter.k;
            node.Pindex = pindx;
            node.Queries = query;//Searcher.SegmentKeyWord(filter.k);
            node.Fields = searchFields;
            node.Occurs = LiveEpgNode.Occurs;
            node.LongRnageFilters = longs;
            node.ContainFilters = containfilters;
            node.NumberRangeFiters = rangenodes;
            node.Start = filter.s;
            node.Limit = filter.c;
            node.IsNoPaging = isNoPaging;
            node.IndexValue = LiveEpgNode.IndexValue;
            return Searcher.SearchEx(LiveEpgLuceneNet.Instance.IndexDic, node);
        }
    }
}
