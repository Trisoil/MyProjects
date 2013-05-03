using System;
using System.Xml;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public abstract class HttpVodFactoryBase : HttpFactoryBase<VodChannelListInfo>
    {
        protected int _contentCharNum { get { return 0; } }

        protected string GenerateTagString(List<TagInfo> tagInfos)
        {
            if (tagInfos == null)
                return "||";
            string catalogTag = string.Empty;
            string areaTag = string.Empty;
            string yearTag = string.Empty;

            if (tagInfos[0] == null)
                catalogTag = string.Empty;
            else if (tagInfos[0].Dimension == "catalog" && tagInfos[0].TagName != "全部")
                catalogTag = tagInfos[0].TagName;

            if (tagInfos[1] == null)
                areaTag = "";
            else if (tagInfos[1].Dimension == "area" && tagInfos[1].TagName != "全部")
                areaTag = tagInfos[1].TagName;

            if (tagInfos[2] == null)
                yearTag = "";
            else if (tagInfos[2].Dimension == "year" && tagInfos[2].TagName != "全部")
                yearTag = tagInfos[2].TagName;
            return string.Format("{0}|{1}|{2}", catalogTag, areaTag, yearTag);
        }

        /// <summary>
        /// 分析集合大小
        /// </summary>
        /// <param name="reader"></param>
        /// <returns></returns>
        protected virtual int AnalysisCount(System.Xml.XmlReader reader)
        {
            if (reader.ReadToFollowing("count"))
            {
                var count = reader.ReadElementContentAsInt();
                if (reader.IsStartElement("page_count"))
                {
                    var pageCount = reader.ReadElementContentAsInt();
                    if (reader.IsStartElement("countInPage"))
                    {
                        var countInPage = reader.ReadElementContentAsInt();
                        if (reader.IsStartElement("page"))
                        {
                            var pageIndex = reader.ReadElementContentAsInt();
                            /*
                             * 总个数是否小于每页的个数,小于的话第一页返回总个数，其余页返回0
                             * 不小于判断是哪页，其余页返回每页个数，最后一页算一下还剩下多少页
                             */
                            return count > countInPage ?
                                (pageIndex > pageCount ? 0 : (pageIndex == pageCount) ? count - (pageIndex - 1) * countInPage : countInPage) :
                                (pageIndex > 1 ? 0 : count);
                        }
                    }
                }
            }
            return 20;
        }

        protected override VodChannelListInfo AnalysisData(System.Xml.XmlReader reader)
        {
            var result = new VodChannelListInfo();
            result.Channels = new List<ChannelInfo>();
            while (reader.Read())
            {
                if (reader.IsStartElement("count"))
                {
                    reader.Read();
                    result.ChannelCount = int.Parse(reader.Value);
                }
                if (reader.IsStartElement("page_count"))
                {
                    reader.Read();
                    result.PageCount = int.Parse(reader.Value);
                }

                if (reader.NodeType == XmlNodeType.Element && reader.Name == "v")
                {
                    //LogManager.Ins.Log("v begin");
                    int vid = 0;
                    string title = "";
                    string playLink = "";
                    int typeID = 0;
                    string imgUrl = "";
                    string catalogTags = "";
                    string directorTags = "";
                    string actTags = "";
                    string yearTags = "";
                    string areaTags = "";
                    int state = 0;
                    string note = "";
                    double mark = 0;
                    int bitrate = 0;
                    int resolutionWidth = 0;
                    int resolutionHeight = 0;
                    bool isHD = false;
                    bool isRecommend = false;
                    bool isNew = false;
                    bool is3D = false;
                    double duration = 0;
                    string resolution = "";
                    string content = "";
                    string slotUrl = "";
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "vid": vid = int.Parse(reader.Value); break;
                                case "type": typeID = int.Parse(reader.Value); break;
                                case "playlink": playLink = reader.Value; break;
                                case "title": title = reader.Value; break;
                                case "catalog": catalogTags = reader.Value; break;
                                case "director": directorTags = reader.Value; break;
                                case "act": actTags = reader.Value; break;
                                case "year": yearTags = reader.Value; break;
                                case "area": areaTags = reader.Value; break;
                                case "imgurl": imgUrl = reader.Value; break;
                                case "state":
                                    try
                                    {
                                        state = int.Parse(reader.Value);
                                    }
                                    catch (Exception)
                                    {
                                        state = 1;
                                    }

                                    break;
                                case "note": note = reader.Value; break;
                                case "mark": mark = double.Parse(reader.Value); break;
                                case "bitrate": bitrate = int.Parse(reader.Value); break;
                                case "resolution": resolution = reader.Value;
                                    string[] widthHeight = resolution.Split('|');
                                    if (widthHeight.Length > 0)
                                    {
                                        resolutionWidth = int.Parse(widthHeight[0]);
                                        resolutionHeight = int.Parse(widthHeight[1]);
                                    }
                                    break;
                                case "flag":
                                    string[] flags = reader.Value.Split('|');
                                    foreach (string flag in flags)
                                    {
                                        //h表示高清，r表示推荐，n表示最新更新，b表示蓝光, d表示3d
                                        switch (flag)
                                        {
                                            case "h": isHD = true; break;
                                            case "r": isRecommend = true; break;
                                            case "n": isNew = true; break;
                                            case "d": is3D = true; break;
                                            default: break;
                                        }
                                    }
                                    break;
                                case "duration": duration = double.Parse(reader.Value); break;
                                case "content": content = reader.Value; break;
                                case "sloturl": slotUrl = reader.Value; break;
                                default: break;
                            }
                        }   // end if
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "v")
                        {
                            ChannelInfo channelInfo = new ChannelInfo(vid, title, playLink, typeID, imgUrl, catalogTags, directorTags, actTags, yearTags, areaTags, state, note, mark, bitrate, resolutionWidth, resolutionHeight, isHD, isRecommend, isNew, is3D, duration, content, slotUrl);
                            if (string.IsNullOrEmpty(channelInfo.ImgUrl))
                                channelInfo.ImgUrl = channelInfo.SlotUrl;
                            result.Channels.Add(channelInfo);
                            break;
                        }   // end if
                    }   //end while
                }   // end if
            }   // end while
            return result;
        }

        protected abstract override string CreateUri(params object[] paras);
    }

    /// <summary>
    /// 点播频道下载完成回调
    /// </summary>
    public class VodChannelListInfo
    {
        public List<ChannelInfo> Channels;

        public int ChannelCount;

        public int PageCount;
    }
}
