using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Vod.Relevance
{
    using PanGu;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.List.Vod.Relevance;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.LuceneNetSearcher.Searcher;
    using Synacast.BoxManage.Core.Utils;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class VodRelevance : IRelevance 
    {

        #region IRelevance Members

        public string Relevance(RelevanceFilter filter)
        {
            try
            {
                var vids = filter.vids.FormatStrToArray(SplitArray.LineArray);
                int vid = 0;
                if (int.TryParse(vids[0], out vid))
                {
                    var video = ListCache.Instance.Dictionary[vid];
                    return CreateSmarkPageList(LuceneNetUtil.RelevanceSearch(filter, SegmentKeyWord(video), filter.c + 1), filter, VideoParsUtils.FormateSearchVideoPars(filter), false);
                }
                return NoPlatForm();
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

        public string RelevanceEx(RelevanceFilter filter)
        {
            try
            {
                var vids = filter.vids.FormatStrToArray(SplitArray.LineArray);
                int vid = 0;
                if (int.TryParse(vids[0], out vid))
                {
                    var video = ListCache.Instance.Dictionary[vid];
                    return CreateSmarkPageList(LuceneNetUtil.RelevanceSearchEx(filter, SegmentKeyWord(video), filter.c + 1), filter, VideoParsUtils.FormateSearchVideoPars(filter), true);
                }
                return NoPlatForm();
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

        private static string SegmentKeyWord(VideoNode video)
        {
            var builder = new StringBuilder(50);
            var segment = new Segment();
            var words = segment.DoSegment(video.ChannelName);
            foreach (var word in words)
            {
                if (word == null)
                    continue;
                builder.AppendFormat("{0}^{1}.0 ", word.Word, (int)Math.Pow(3, word.Rank));
            }
            //if (!string.IsNullOrEmpty(video.Language[CustomArray.LanguageArray[0]].CatalogTags))
            //{
            //    var catalogs = video.Language[CustomArray.LanguageArray[0]].CatalogTags.FormatStrToArray(SplitArray.DHArray);
            //    foreach (var catalog in catalogs)
            //    {
            //        builder.AppendFormat("{0}^{1}.0 ", catalog, 3);
            //    }
            //}

            //if(!string.IsNullOrEmpty(video.BKInfo.Actors))
            //{
            //    var acs = video.BKInfo.Actors.FormatStrToArray(SplitArray.DHArray);
            //    foreach (var ac in acs)
            //    {
            //        builder.AppendFormat("{0}^{1}.0 ", ac, 2);
            //    }
            //}

            //if(!string.IsNullOrEmpty(video.Language[CustomArray.LanguageArray[0]].AreaTags))
            //{
            //    var areas = video.Language[CustomArray.LanguageArray[0]].AreaTags.FormatStrToArray(SplitArray.DHArray);
            //    foreach (var area in areas)
            //    {
            //        builder.AppendFormat("{0}^{1}.0 ", area, 1);
            //    }
            //}
            return builder.ToString();
        }


        /// <summary>
        /// LuceneNet分页输出
        /// </summary>
        private static string CreateSmarkPageList(LuceneResultNode result, RelevanceFilter filter, VideoPars pars, bool isEx)
        {
            var xml = new XElement("vlist");
            var count = result.AllCount - 1;
            if (count < 0)
                count = 0;
            xml.Add(new XElement("count", count));
            //xml.Add(new XElement("page_count", PageUtils.PageCount(result.AllCount, filter.c)));
            //xml.Add(new XElement("countInPage", filter.c));
            //xml.Add(new XElement("page", filter.s));
            for (var i = 1; i < result.AllCount; i++)
            {
                xml.Add(pars.ResponseFun(ListCache.Instance.Dictionary[int.Parse(result.Result[i])], filter, isEx));
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的错误提示
        /// </summary>
        private static string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("无相关推荐影片");
        }

    }
}
