using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class ChannelDetailFactory : HttpFactoryBase<ChannelDetailInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}detail.api?ver=2&auth={1}&platform={2}&vid={3}&userLevel={4}",
                EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override ChannelDetailInfo AnalysisData(XmlReader reader)
        {
            var result = new ChannelDetailInfo();
            
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element 
                    && !reader.IsEmptyElement)
                {
                    string node_name = reader.Name;
                    if (node_name != "video_list2")
                        reader.Read();
                    int node_value = 0;
                    switch (node_name)
                    {
                        case "vid": result.Id = int.TryParse(reader.Value, out node_value) ? node_value : -1; break;
                        case "type": result.Type = int.TryParse(reader.Value, out node_value) ? node_value : 0; break;
                        case "title": result.Title = reader.Value; break;
                        case "catalog": result.Catalog = reader.Value; break;
                        case "director": result.Director = reader.Value; break;
                        case "act": result.Act = reader.Value; break;
                        case "year": result.Year = reader.Value; break;
                        case "area": result.Area = reader.Value; break;
                        case "imgurl": result.ImageUri = reader.Value; break;
                        case "state": result.State = int.TryParse(reader.Value, out node_value) ? node_value : 0; break;
                        case "note": result.Note = reader.Value; break;
                        case "mark": result.Mark = double.Parse(reader.Value); break;
                        case "flag":
                            string[] flags = reader.Value.Split('|');
                            foreach (string flag in flags)
                            {
                                //h表示高清，r表示推荐，n表示最新更新，b表示蓝光, d表示3d
                                switch (flag)
                                {
                                    case "h": result.ChannelFlag = result.ChannelFlag | (int)ChannelFlagType.High; break;
                                    case "r": result.ChannelFlag = result.ChannelFlag | (int)ChannelFlagType.Recomamnd; break;
                                    case "n": result.ChannelFlag = result.ChannelFlag | (int)ChannelFlagType.New; break;
                                    case "b": result.ChannelFlag = result.ChannelFlag | (int)ChannelFlagType.Blue; break;
                                    case "d": result.ChannelFlag = result.ChannelFlag | (int)ChannelFlagType.D3; break;
                                    default: break;
                                }
                            }
                            break;
                        case "sloturl": result.SlotUri = reader.Value;
                            if (string.IsNullOrEmpty(result.ImageUri))
                                result.ImageUri = result.SlotUri;
                            break;
                        case "duration": result.Duration = double.Parse(reader.Value); break;
                        case "content": result.Content = reader.Value; break;
                        case "vip": result.IsVip = reader.Value == "1" ? true : false; break;
                        case "video_list2": int index = 0;
                            while (reader.Read())
                            {
                                if (reader.IsStartElement() 
                                    && reader.Name == "playlink2")
                                {
                                    var programInfo = new ProgramInfo();
                                    programInfo.Index = index;
                                    programInfo.Title = reader.GetAttribute("title");
                                    int local;
                                    if (index < 5)
                                        result.IsNumber = int.TryParse(programInfo.Title, out local);
                                    programInfo.SlotUri = reader.GetAttribute("sloturl");
                                    programInfo.ChannelId = Convert.ToInt32(reader.GetAttribute("id"));
                                    while (reader.Read())
                                    {
                                        if (reader.NodeType == XmlNodeType.EndElement
                                            && reader.Name == "playlink2")
                                        {
                                            if (result.ProgramInfos == null)
                                                result.ProgramInfos = new List<ProgramInfo>(result.State);
                                            result.ProgramInfos.Add(programInfo);
                                            index++;
                                            break;
                                        }
                                    }
                                }
                            }
                            break;
                        default: break;
                    }
                }
                    
            }
            return result;
        }
    }
}
