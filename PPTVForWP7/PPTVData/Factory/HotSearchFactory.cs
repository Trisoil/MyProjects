using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

using PPTVData.Entity;
using System.Collections.Generic;
using System.Windows.Threading;
using System.Xml;
using System.IO;

namespace PPTVData.Factory
{
    public class HotSearchFactory : HttpFactoryBase<List<HotSearchInfo>>
    {
        protected override string CreateUri(params object[] paras)
        {
            int c = 20;
            int s = 1;
            string rankingspan = "week";
            if (paras!=null)
            {
                if (paras.Length >= 1)
                    rankingspan = (string)paras[0];
                if (paras.Length >= 2)
                    c = (int)paras[1];
                if (paras.Length >= 3)
                    s = (int)paras[2];
            }
            return string.Format(EpgUtils.HotSearchUri, c, s, rankingspan);
        }

        protected override List<HotSearchInfo> AnalysisData(XmlReader reader)
        {
            List<HotSearchInfo> hotSearchInfos = new List<HotSearchInfo>();
            int count = 0;
            int page_count = 0;
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    string node_name = reader.Name;

                    switch (node_name)
                    {
                        case "count": try
                            {
                                reader.Read();
                                count = Convert.ToInt32(reader.Value);
                            }
                            catch (Exception)
                            {
                                count = 0;
                            }; break;
                        case "page_count": try
                            {
                                reader.Read();
                                page_count = Convert.ToInt32(reader.Value);
                            }
                            catch (Exception)
                            {
                                page_count = 0;
                            }; break;
                        case "keyword":
                            if (reader.NodeType == XmlNodeType.Element)
                            {
                                string type = reader.GetAttribute("type");
                                string count1 = reader.GetAttribute("count");
                                reader.Read();
                                string key = reader.Value;
                                hotSearchInfos.Add(new HotSearchInfo(type, count1, key));
                            }
                            ; break;
                        default:
                            break;
                    }
                }
                else if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "keywords")
                {
                    break;
                }
            }
            return hotSearchInfos;
        }
    }
}
