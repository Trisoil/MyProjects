using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Live.Search
{
    using Synacast.BoxManage.List.Live.Search;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.LuceneNetSearcher.Searcher;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.LuceneNet.Live;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class LiveSearcher : ILiveSearch 
    {
        #region ILiveSearch 成员

        public string LiveSearch(LiveSearchFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.LiveSearch(filter, LiveEpgNode.NameFields, Searcher.SegmentKeyWord(filter.k), false), filter, false);
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

        public string LiveSearchEx(LiveSearchFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.LiveSearchEx(filter, LiveEpgNode.NameFields, Searcher.SegmentKeyWord(filter.k), false), filter, true);
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

        public string LiveParadeSearch(LiveSearchFilter filter)
        {
            try
            {
                return CreateParadeSmarkPageList(LuceneNetUtil.LiveSearch(filter, LiveEpgNode.ParadeFields, Searcher.SegmentKeyWord(filter.k), true), filter, false);
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

        public string LiveParadeSearchEx(LiveSearchFilter filter)
        {
            try
            {
                return CreateParadeSmarkPageList(LuceneNetUtil.LiveSearchEx(filter, LiveEpgNode.ParadeFields, Searcher.SegmentKeyWord(filter.k), true), filter, true);
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

        public string LiveSearchCh(LiveSearchFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.LiveSearch(filter, LiveEpgNode.CnNameField, Searcher.SegmentChWord(filter.k), false), filter, false);
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

        public string LiveSearchChEx(LiveSearchFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.LiveSearchEx(filter, LiveEpgNode.CnNameField, Searcher.SegmentChWord(filter.k), false), filter, true);
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

        public string LiveParadeSearchCh(LiveSearchFilter filter)
        {
            try
            {
                return CreateParadeSmarkPageList(LuceneNetUtil.LiveSearch(filter, LiveEpgNode.ParadeCnFields, Searcher.SegmentChWord(filter.k), true), filter, false);
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

        public string LiveParadeSearchChEx(LiveSearchFilter filter)
        {
            try
            {
                return CreateParadeSmarkPageList(LuceneNetUtil.LiveSearchEx(filter, LiveEpgNode.ParadeCnFields, Searcher.SegmentChWord(filter.k), true), filter, true);
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
        /// LuceneNet分页输出
        /// </summary>
        private string CreateSmarkPageList(LuceneResultNode result, LiveSearchFilter filter, bool isEx)
        {
            var key = new VideoNodeKey(filter.platform, 0, filter.auth);
            var xml = new XElement("vlist");
            xml.Add(new XElement("count", result.AllCount));
            xml.Add(new XElement("page_count", PageUtils.PageCount(result.AllCount, filter.c)));
            xml.Add(new XElement("countInPage", filter.c));
            xml.Add(new XElement("page", filter.s));
            xml.Add(from v in result.Result select VideoResponseUtils.CustomLiveListRes(LiveListCache.Instance.Dictionary[int.Parse(v)], filter, 0, isEx));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// LuceneNet节目预告分页输出
        /// </summary>
        private string CreateParadeSmarkPageList(LuceneResultNode result, LiveSearchFilter filter, bool isEx)
        {
            var vkey = new VideoNodeKey(filter.platform, 0, filter.auth);
            var parades = from parade in result.Result select ChannelLiveItemsCache.Instance.Items[int.Parse(parade)];
            var query = from parade in parades group parade by parade.ChannelID;
            var list = query.ToList();
            //var groupquery = query.Skip(filter.c * (filter.s - 1)).Take(filter.c);
            //var count = groupquery.Count();
            var index = (filter.s - 1) * filter.c;
            var max = index + filter.c;
            if (max > list.Count)
                max = list.Count;
            var count = max - index;
            var xml = new XElement("vlist");
            xml.Add(new XElement("count", count));
            xml.Add(new XElement("page_count", PageUtils.PageCount(count, filter.c)));
            xml.Add(new XElement("countInPage", filter.c));
            xml.Add(new XElement("page", filter.s));
            for (var i = index; i < max; i++)
            {
                var group = list[i];
                var v = VideoResponseUtils.CustomLiveListRes(LiveListCache.Instance.Dictionary[group.Key], filter, 0, isEx);
                var paraderoot = new XElement("parade_list");
                foreach (var key in group)
                {
                    paraderoot.Add(VideoResponseUtils.LiveListSearchParade(key, filter));
                }
                v.Add(paraderoot);
                xml.Add(v);
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的错误提示
        /// </summary>
        private string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的直播频道");
        }
    }
}
