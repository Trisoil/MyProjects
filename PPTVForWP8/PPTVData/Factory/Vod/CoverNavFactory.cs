using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Threading.Tasks;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class CoverNavFactory : HttpFactoryBase<List<RecommandNav>>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}cover_image_recommand_nav.api?auth={1}&mode=1&platform={2}", EpgUtils.CoverHost, EpgUtils.Auth, EpgUtils.PlatformName);
        }

        protected override List<RecommandNav> AnalysisData(XmlReader reader)
        {
            List<RecommandNav> RecommendNavs = new List<RecommandNav>();
            RecommandNav RecommendNav;//=new RecommendNav();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "nav")
                {
                    int navid = 0;
                    string name = "";
                    string image = "";
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element && !reader.IsEmptyElement)
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
                            RecommendNav = new RecommandNav(navid, name, image);
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
