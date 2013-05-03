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
    public class RecommendNavFactory : HttpFactoryBase<List<RecommendNav>>
    {
        protected override string CreateUri(params object[] paras)
        {
            return EpgUtils.RecommandNavUri;
        }

        protected override List<RecommendNav> AnalysisData(XmlReader reader)
        {
            List<RecommendNav> RecommendNavs = new List<RecommendNav>();
            RecommendNav RecommendNav;//=new RecommendNav();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "nav")
                {
                    int navid = 0;
                    string name = "";
                    string image = "";
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "navid": navid = int.Parse(reader.Value); break;
                                case "name": name = reader.Value; break;
                                default:
                                    break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "nav")
                        {
                            RecommendNav = new RecommendNav(navid, name, image, true);
                            RecommendNavs.Add(RecommendNav);
                            break;
                        }
                    }
                }
            }

            return RecommendNavs;
        }
    }
}
