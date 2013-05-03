using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;

using System.Xml;
using System.IO;
using PPTVData.Entity;
using PPTVData;
namespace PPTVData.Factory
{
    public class ChannelDetailFactory : HttpFactoryBase<ChannelDetailInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format(EpgUtils.DetailEpgUri, paras[0]);
        }

        protected override ChannelDetailInfo AnalysisData(XmlReader reader)
        {
            ChannelDetailInfo channelDetailInfo = null;
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "v")
                {
                    int vid = 0;
                    string title = "";
                    string programTitle = "";
                    int id = 0;
                    string slotUrl = "";
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
                    bool isNumber = false;
                    int index = 0;
                    List<ProgramInfo> programInfos = new List<ProgramInfo>();
                    while (reader.Read())
                    {
                        if (reader.Name == "video_list2")
                        {
                            //string isNumberStr = reader.GetAttribute("isNumber");
                            //if (isNumberStr == "0")
                            //    isNumber = false;
                            //else
                            //    isNumber = true;
                            while (reader.Read())
                            {
                                if (reader.IsStartElement() && reader.Name == "playlink2")
                                {
                                    programTitle = reader.GetAttribute("title");
                                    int local;
                                    if (index < 5)
                                        isNumber = int.TryParse(programTitle, out local);
                                    slotUrl = reader.GetAttribute("sloturl");
                                    id = Convert.ToInt32(reader.GetAttribute("id"));
                                    reader.Read();
                                    if (reader.IsStartElement() && reader.Name == "source")
                                    {
                                        reader.Read();
                                        playLink = reader.Value;
                                        reader.Read();
                                    }
                                    if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "source")
                                    {
                                        programInfos.Add(new ProgramInfo(id, programTitle, playLink, slotUrl, index, System.Windows.Visibility.Collapsed));
                                        index++;
                                    }
                                }
                                if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "video_list2")
                                    break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            int node_value = 0;
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
                                case "state": state = int.TryParse(reader.Value, out node_value) ? node_value : 1; break;
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
                            channelDetailInfo = new ChannelDetailInfo(vid, title, playLink, typeID, imgUrl, catalogTags, directorTags, actTags, yearTags, areaTags, state, note, mark, bitrate, resolutionWidth, resolutionHeight, isHD, isRecommend, isNew, is3D, duration, content, slotUrl, programInfos, System.Windows.Visibility.Collapsed);
                            if (string.IsNullOrEmpty(channelDetailInfo.ImgUrl))
                                channelDetailInfo.ImgUrl = channelDetailInfo.SlotUrl;
                            channelDetailInfo.IsNumber = isNumber;
                            break;
                        }   // end if
                    }   //end while
                }   // end if
            }   // end while
            return channelDetailInfo;
        }
    }
}
