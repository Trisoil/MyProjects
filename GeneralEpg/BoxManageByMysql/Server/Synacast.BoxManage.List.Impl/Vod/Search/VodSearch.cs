using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ServiceModel;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Vod.Search
{
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.List.Vod.Serach;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.LuceneNet;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.LuceneNetSearcher.Stat;
    using Synacast.LuceneNetSearcher.Stat.Instrument;
    using Synacast.LuceneNetSearcher.Searcher;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class VodSearch : ISearch
    {
        #region ISearch Members

        /// <summary>
        /// 频道名中文基本搜索
        /// </summary>
        public string VideoSearch(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.Search(filter, EpgIndexNode.NameField, EpgIndexNode.NameOccurs, Searcher.SegmentKeyWord(filter.k), true), filter, VideoParsUtils.FormateSearchVideoPars(filter), false);
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
        /// 频道名中文扩展搜索
        /// </summary>
        public string VideoSearchEx(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.SearchEx(filter, EpgIndexNode.NameField, EpgIndexNode.NameOccurs, Searcher.SegmentKeyWord(filter.k), true), filter, VideoParsUtils.FormateSearchVideoPars(filter), true);
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
        /// 频道名拼音基本搜索
        /// </summary>
        public string VideoSearchCH(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.Search(filter, EpgIndexNode.CnNameField, EpgIndexNode.NameOccurs, Searcher.SegmentChWord(filter.k), false), filter, VideoParsUtils.FormateSearchVideoPars(filter),false);
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
        /// 频道名拼音扩展搜索
        /// </summary>
        public string VideoSearchCHEx(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.SearchEx(filter, EpgIndexNode.CnNameField, EpgIndexNode.NameOccurs, Searcher.SegmentChWord(filter.k), false), filter, VideoParsUtils.FormateSearchVideoPars(filter), true);
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
        /// LuceneNet基本搜索
        /// </summary>
        public string VideoSearchSmart(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.Search(filter, EpgIndexNode.Fields, EpgIndexNode.Occurs, Searcher.SegmentKeyWord(filter.k), true), filter, VideoParsUtils.FormateSearchVideoPars(filter), false);
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
        /// LuceneNet扩展搜索
        /// </summary>
        public string VideoSearchSmartEx(ListFilter filter)
        {
            try
            {
                return CreateSmarkPageList(LuceneNetUtil.SearchEx(filter, EpgIndexNode.Fields, EpgIndexNode.Occurs, Searcher.SegmentKeyWord(filter.k), true), filter, VideoParsUtils.FormateSearchVideoPars(filter), true);
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
        /// 搜索热词
        /// </summary>
        public string HotSearchWord(SimpleSearchFilter filter)
        {
            try
            {
                var pindex = VideoNodeKeyArray.Instance.Items[new VideoNodeKey(filter.platform, 0, filter.auth)];
                var words = LuceneDictionary.Instance.Dictionary[pindex].Items;
                IEnumerable<XElement> page = null;
                if (filter.lang == CustomArray.LanguageArray[1])
                {
                    page = words.Skip(filter.c * (filter.s - 1)).Take(filter.c).Select<InstrumentNode, XElement>
                    (v =>
                    {
                        return new XElement("keyword", LanguageUtils.FormatTraditionChinese(v.Key));
                    });
                }
                else
                {
                    page = words.Skip(filter.c * (filter.s - 1)).Take(filter.c).Select<InstrumentNode, XElement>
                    (v =>
                    {
                        return new XElement("keyword", v.Key);
                    });
                }
                var xml = new XElement("keywords",
                    new XElement("count", words.Count),
                    new XElement("page_count", PageUtils.PageCount(words.Count, filter.c)),
                    page
                    );
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return BoxUtils.FormatErrorMsg("不存在该平台下的热词信息");
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
        private static string CreateSmarkPageList(LuceneResultNode result, ListFilter filter, VideoPars pars, bool isEx)
        {
            var xml = new XElement("vlist");
            xml.Add(new XElement("count", result.AllCount));
            xml.Add(new XElement("page_count", PageUtils.PageCount(result.AllCount, filter.c)));
            xml.Add(new XElement("countInPage", filter.c));
            xml.Add(new XElement("page", filter.s));
            if (filter.shownav == 1)
            {
                xml.Add(new XElement("nav",
                    from type in result.Types
                    select
                        new XElement("type",
                        new XAttribute("id", type.Key),
                        new XAttribute("name", BoxTypeCache.Instance.Items[type.Key].Language[filter.lang].Title),
                        new XAttribute("count", type.Value)
                    )));
            }
            xml.Add(from v in result.Result select pars.ResponseFun(ListCache.Instance.Dictionary[int.Parse(v)], filter, isEx));
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的错误提示
        /// </summary>
        private static string NoPlatForm()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的影片");
        }

    }
}
