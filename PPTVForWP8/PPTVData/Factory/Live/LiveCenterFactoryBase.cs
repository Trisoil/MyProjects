using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace PPTVData.Factory.Live
{
    using PPTVData.Entity.Live;
    using PPTVData.Utils;

    public abstract class LiveCenterFactoryBase : HttpFactoryBase<LiveArgs>
    {
        protected abstract string Id { get; }

        protected abstract LiveType Type { get; }

        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}api/v2/collection?id={1}&platform=3&auth={2}&start={3}&userLevel={4}",
                EpgUtils.LiveCenterHost, Id, EpgUtils.Auth, paras[0], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override LiveArgs AnalysisData(System.Xml.XmlReader reader)
        {
            var result = new LiveArgs(Type.CreateString());
            result.Channels = new List<LiveChannelInfo>();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "section")
                {
                    var liveInfo = new LiveChannelInfo(Type);
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element
                            && !reader.IsEmptyElement)
                        {
                            var nodeName = reader.Name;
                            reader.Read();
                            switch (nodeName)
                            {
                                case "channel_id": liveInfo.Id = int.Parse(reader.Value); break;
                                case "start_time": liveInfo.StartTime = DateTime.Parse(reader.Value); break;
                                case "end_time": liveInfo.EndTime = DateTime.Parse(reader.Value); break;
                                case "title": if (string.IsNullOrEmpty(liveInfo.NowPlay)) liveInfo.NowPlay = reader.Value; else if (string.IsNullOrEmpty(liveInfo.Title)) liveInfo.Title = reader.Value; break;
                                case "image": if (string.IsNullOrEmpty(liveInfo.ImageUri)) liveInfo.ImageUri = reader.Value; break;
                                default: break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement
                            && reader.Name == "section")
                        {
                            if (DateTime.Now <= liveInfo.EndTime
                                && liveInfo.Id > 0)
                                result.Channels.Add(liveInfo);
                            break;
                        }
                    }
                }
            }
            return result;
        }

        public static bool IsDirectPlay(LiveType liveType)
        {
            return (liveType == LiveType.Recommend
                || liveType == LiveType.Sports
                || liveType == LiveType.Games);
        }
    }
}
