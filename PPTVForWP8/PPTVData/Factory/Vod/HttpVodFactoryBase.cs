using System;
using System.Xml;
using System.Net;
using System.Windows;
using System.Collections.Generic;

namespace PPTVData.Factory
{
    using PPTVData.Entity;

    public abstract class HttpVodFactoryBase : HttpFactoryBase<VodChannelListInfo>
    {
        protected int _contentCharNum { get { return 0; } }

        protected override VodChannelListInfo AnalysisData(System.Xml.XmlReader reader)
        {
            var result = new VodChannelListInfo();
            var count = 4;
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "count")
                {
                    reader.Read();
                    result.ChannelCount = int.Parse(reader.Value);
                }
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "page_count")
                {
                    reader.Read();
                    result.PageCount = int.Parse(reader.Value);
                }
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "countInPage")
                {
                    reader.Read();
                    count = int.Parse(reader.Value);
                }

                if (result.Channels == null)
                    result.Channels = new List<ChannelInfo>(count);

                if (reader.NodeType == XmlNodeType.Element 
                    && reader.Name == "v")
                {
                    var channelInfo = new ChannelInfo();
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element 
                            && !reader.IsEmptyElement)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            int node_value = 0;
                            switch (node_name)
                            {
                                case "vid": channelInfo.Id = int.TryParse(reader.Value, out node_value) ? node_value : -1; break;
                                case "type": channelInfo.Type = int.TryParse(reader.Value, out node_value) ? node_value : 0; break;
                                case "title": channelInfo.Title = reader.Value; break;
                                case "act": channelInfo.Act = reader.Value; break;
                                case "imgurl": channelInfo.ImageUri = reader.Value; break;
                                case "mark": channelInfo.Mark = double.Parse(reader.Value); break;
                                case "flag":
                                    string[] flags = reader.Value.Split('|');
                                    foreach (string flag in flags)
                                    {
                                        //h表示高清，r表示推荐，n表示最新更新，b表示蓝光, d表示3d
                                        switch (flag)
                                        {
                                            case "h": channelInfo.ChannelFlag = channelInfo.ChannelFlag | (int)ChannelFlagType.High; break;
                                            case "r": channelInfo.ChannelFlag = channelInfo.ChannelFlag | (int)ChannelFlagType.Recomamnd; break;
                                            case "n": channelInfo.ChannelFlag = channelInfo.ChannelFlag | (int)ChannelFlagType.New; break;
                                            case "b": channelInfo.ChannelFlag = channelInfo.ChannelFlag | (int)ChannelFlagType.Blue; break;
                                            case "d": channelInfo.ChannelFlag = channelInfo.ChannelFlag | (int)ChannelFlagType.D3; break;
                                            default: break;
                                        }
                                    }
                                    break;
                                case "duration": channelInfo.Duration = double.Parse(reader.Value); break;
                                case "content": channelInfo.Content = reader.Value; break;
                                case "sloturl": channelInfo.SlotUri = reader.Value; break;
                                default: break;
                            }
                        } 
                        if (reader.NodeType == XmlNodeType.EndElement 
                            && reader.Name == "v")
                        {
                            if (string.IsNullOrEmpty(channelInfo.ImageUri))
                            {
                                channelInfo.ImageUri = channelInfo.SlotUri;
                            }
                            result.Channels.Add(channelInfo);
                            break;
                        } 
                    } 
                }
            }   
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
