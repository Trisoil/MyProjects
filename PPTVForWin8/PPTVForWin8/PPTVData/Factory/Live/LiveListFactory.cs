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

    public class LiveListFactory : HttpFactoryBase<LiveArgs>
    {
        private int _count = 4;
        private LiveType _liveType;

        protected override string CreateUri(params object[] paras)
        {
            _liveType = (LiveType)paras[0];
            _count = Convert.ToInt32(paras[1]);

            return string.Format("{0}live-list.api?auth={1}&platform={2}&type={3}&c={4}&s={5}&vt=4&nowplay=1&userLevel={6}",
                EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], paras[1], paras[2], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override LiveArgs AnalysisData(System.Xml.XmlReader reader)
        {
            var result = new LiveArgs(_liveType.CreateString());
            result.Channels = new List<LiveChannelInfo>(_count);
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "page_count")
                {
                    reader.Read();
                    result.PageCount = int.Parse(reader.Value);
                }
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "v")
                {
                    var liveInfo = new LiveChannelInfo(_liveType);
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element
                            && !reader.IsEmptyElement)
                        {
                            var nodeName = reader.Name;
                            if (nodeName != "nowplay"
                                && nodeName != "willplay")
                                reader.Read();
                            switch (nodeName)
                            {
                                case "vid": liveInfo.Id = int.Parse(reader.Value); break;
                                case "title": liveInfo.Title = reader.Value; break;
                                case "imgurl": liveInfo.ImageUri = reader.Value; break;
                                case "nowplay": var ntime = reader["begin_time"]; reader.Read(); liveInfo.NowPlay = string.Format("{0} {1}", ntime, reader.Value); break;
                                case "willplay": var wtime = reader["begin_time"]; reader.Read(); liveInfo.WillPlay = string.Format("{0} {1}", wtime, reader.Value); break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement
                            && reader.Name == "v")
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
