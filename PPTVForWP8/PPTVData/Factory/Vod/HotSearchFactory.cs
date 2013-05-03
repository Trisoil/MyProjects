using System;
using System.Collections.Generic;
using System.Xml;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Vod
{
    using PPTVData.Entity.Vod;
    using PPTVData.Utils;

    public class HotSearchFactory : HttpFactoryBase<List<HotSearchInfo>>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}search_hot_keywords.api?&auth={1}&platform={2}&c=20&s=1&rankingspan=week",
                EpgUtils.SearchHost, EpgUtils.Auth, EpgUtils.PlatformName);
        }

        protected override List<HotSearchInfo> AnalysisData(System.Xml.XmlReader reader)
        {
            var list = new List<HotSearchInfo>(20);
            while (reader.Read())
            {
                while (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "keyword")
                {
                    int nodeValue = 0;
                    list.Add(new HotSearchInfo()
                     {
                         Type = int.TryParse(reader.GetAttribute("type"), out nodeValue) ? nodeValue : 0,
                         Count = int.TryParse(reader.GetAttribute("count"), out nodeValue) ? nodeValue : 0,
                         Name = reader.ReadElementContentAsString()
                     });
                }
            }
            return list;
        }
    }
}
