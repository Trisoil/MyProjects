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

    public class LiveCMSListFactory : HttpFactoryBase<LiveArgs>
    {

        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}mtbu_tvlist/{1}/?auth={2}&platform={3}&userLevel={4}",
                EpgUtils.CMSHost, paras[0], EpgUtils.Auth, EpgUtils.PlatformName, PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override LiveArgs AnalysisData(System.Xml.XmlReader reader)
        {
            var result = new LiveArgs(LiveType.Recommend.CreateString());
            result.Channels = new List<LiveChannelInfo>(10);
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "live")
                {
                    var liveInfo = new LiveChannelInfo(LiveType.Recommend);
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element
                            && !reader.IsEmptyElement)
                        {
                            var nodeName = reader.Name;
                            reader.Read();
                            switch (nodeName)
                            {
                                case "id": liveInfo.Id = int.Parse(reader.Value); break;
                                case "title": liveInfo.Title = reader.Value; break;
                                case "startTime": liveInfo.StartTime = DateTime.Parse(reader.Value); break;
                                case "endTime": liveInfo.EndTime = DateTime.Parse(reader.Value); break;
                                default: break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement
                            && reader.Name == "live")
                        {
                            result.Channels.Add(liveInfo);
                            break;
                        }
                    }
                }
            }
            return result;
        }
    }
}
