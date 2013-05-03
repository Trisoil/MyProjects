using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Vod.Search
{
    using Synacast.BoxManage.List.Vod.Serach;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.LuceneNetSearcher.Searcher;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.LuceneNet;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class VodTagSerach : ITagSearch 
    {
        #region ITagSearch Members

        public string TagSearch(TagSearchFilter filter)
        {
            try
            {
                var xml = new XElement("taglist");
                var count = LuceneNetUtil.TagSearch(filter, EpgIndexNode.NameField, Searcher.SegmentKeyWord(filter.k), xml);
                xml.AddFirst(new XElement("count", count), new XElement("page_count", PageUtils.PageCount(count, filter.c)));
                return xml.ToString(SaveOptions.DisableFormatting);
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

        public string TagSearchCH(TagSearchFilter filter)
        {
            try
            {
                var xml = new XElement("taglist");
                var count = LuceneNetUtil.TagSearch(filter, EpgIndexNode.CnNameField, Searcher.SegmentChWord(filter.k), xml);
                xml.AddFirst(new XElement("count", count), new XElement("page_count", PageUtils.PageCount(count, filter.c)));
                return xml.ToString(SaveOptions.DisableFormatting);
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

        public string TagSearchSmart(TagSearchFilter filter)
        {
            try
            {
                var xml = new XElement("taglist");
                var count = LuceneNetUtil.TagSearch(filter, EpgIndexNode.NameField, Searcher.SegmentKeyWord(filter.k), xml);
                xml.AddFirst(new XElement("count", count), new XElement("page_count", PageUtils.PageCount(count, filter.c)));
                return xml.ToString(SaveOptions.DisableFormatting);
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
        /// 索引不存在的错误提示
        /// </summary>
        private static string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的影片");
        }
    }
}
