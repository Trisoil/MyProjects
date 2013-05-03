using System;
using System.Text;
using System.Collections.Generic;
using System.Xml;
using System.IO;

using Windows.UI.Xaml;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class CoverListFactory : HttpFactoryBase<List<CoverInfo>>
    {
        protected override bool IsBackThreadCallBack
        {
            get
            {
                return false;
            }
        }

        protected override int TimerOut
        {
            get
            {
                return 15;
            }
        }

        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}cover_image_recommand_list.api?conlen=0&auth={1}&platform={2}&c=6&s=1&nav_id={3}&userLevel={4}",
                EpgUtils.CoverHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override List<CoverInfo> AnalysisData(XmlReader reader)
        {
            var coverInfos = new List<CoverInfo>();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element 
                    && reader.Name == "c")
                {
                    var coverInfo = new CoverInfo();
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element
                            && !reader.IsEmptyElement)
                        {
                            var node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "title": coverInfo.Title = reader.Value; break;
                                case "note": coverInfo.Note = reader.Value; break;
                                case "vid": coverInfo.Vid = Convert.ToInt32(reader.Value); break;
                                case "cover_imgurl": coverInfo.ImageUri = reader.Value; break;
                                default:
                                    break;
                            }
                        }

                        if (reader.NodeType == XmlNodeType.EndElement
                            && reader.Name == "c")
                        {
                            coverInfos.Add(coverInfo);
                            break;
                        }
                    }
                }
            }
            return coverInfos;
        }
    }
}
