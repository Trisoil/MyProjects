using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class ChannelSearchFactory : HttpFactoryBase<ChannelSearchInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}search_smart.api?shownav=1&ver=2&conlen=0&auth={1}&platform={2}&c={3}&s={4}&k={5}&type={6}&userLevel={7}",
                EpgUtils.SearchHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], paras[1], paras[2], paras[3], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        /// <summary>
        /// 分析集合大小
        /// </summary>
        /// <param name="reader"></param>
        /// <returns></returns>
        protected ChannelSearchOutlineInfo AnalysisCount(System.Xml.XmlReader reader)
        {
            ChannelSearchOutlineInfo channelSearchOutLineInfo = new ChannelSearchOutlineInfo();
            if (reader.ReadToFollowing("count"))
            {
                channelSearchOutLineInfo.Count = reader.ReadElementContentAsInt();
                if (reader.IsStartElement("page_count"))
                {
                    channelSearchOutLineInfo.PageCount = reader.ReadElementContentAsInt();
                    if (reader.IsStartElement("countInPage"))
                    {
                        channelSearchOutLineInfo.CountInPage = reader.ReadElementContentAsInt();
                        if (reader.IsStartElement("page"))
                        {
                            channelSearchOutLineInfo.CurrentPage = reader.ReadElementContentAsInt();
                            ///*
                            // * 总个数是否小于每页的个数,小于的话第一页返回总个数，其余页返回0
                            // * 不小于判断是哪页，其余页返回每页个数，最后一页算一下还剩下多少页
                            // */
                            //return count > countInPage ?
                            //    (pageIndex > pageCount ? 0 : (pageIndex == pageCount) ? count - (pageIndex - 1) * countInPage : countInPage) :
                            //    (pageIndex > 1 ? 0 : count);
                        }
                    }
                }
            }
            return channelSearchOutLineInfo;
        }

        protected override ChannelSearchInfo AnalysisData(XmlReader reader)
        {
            ChannelSearchOutlineInfo outLineInfo = AnalysisCount(reader);
            var channel = new ChannelSearchInfo()
            {
                Navs = new List<ChannelSearchNavInfo>(8),
                Lists = new List<ChannelSearchListInfo>(),
                OutlineInfo = outLineInfo
            };

            if (reader.IsStartElement("nav"))
            {
                reader.Read();
                while (reader.IsStartElement("type"))
                {
                    var nav = new ChannelSearchNavInfo();
                    nav.Id = int.Parse(reader.GetAttribute("id"));
                    nav.Name = reader.GetAttribute("name");
                    nav.Count = int.Parse(reader.GetAttribute("count"));
                    channel.Navs.Add(nav);
                    reader.ReadToNextSibling("type");
                }
            }
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "v")
                {
                    var list = new ChannelSearchListInfo();
                    if (reader.ReadToFollowing("vid"))
                        list.Vid = reader.ReadElementContentAsInt();
                    if (reader.ReadToFollowing("title"))
                        list.Text = reader.ReadElementContentAsString();
                    if (reader.ReadToFollowing("catalog"))
                        list.CatalogTags = reader.ReadElementContentAsString();
                    if (reader.ReadToFollowing("imgurl"))
                        list.ImgSource = reader.ReadElementContentAsString();
                    if (string.IsNullOrEmpty(list.ImgSource)
                        && reader.ReadToFollowing("sloturl"))
                        list.ImgSource = reader.ReadElementContentAsString();
                    if (reader.ReadToFollowing("duration"))
                        list.Duration = reader.ReadElementContentAsDouble();
                        
                    channel.Lists.Add(list);
                }
            }
            return channel;
        }
    }
}
