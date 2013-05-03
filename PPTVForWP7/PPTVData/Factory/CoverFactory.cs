using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Text;
using System.Collections.Generic;
using PPTVData.Entity;
using System.Xml;
using System.IO;
using PPTVData;
namespace PPTVData.Factory
{
    public class CoverFactory : HttpFactoryBase<List<CoverInfo>>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format(EpgUtils.CoverListUri, '6', '1', paras[0]);
        }

        protected override List<CoverInfo> AnalysisData(XmlReader reader)
        {
            List<CoverInfo> coverInfos = new List<CoverInfo>();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "c")
                {
                    string title = "";
                    string note = "";
                    string vid = "";
                    string vType = "";
                    string mark = "";
                    string bitrate = "";
                    string fileLength = "0";
                    string resolution = "";
                    string playLink = "";
                    string coverImgUrl = "";
                    string onlinetime = "";
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "title": title = reader.Value; break;
                                case "note": note = reader.Value; break;
                                case "vid": vid = reader.Value; break;
                                case "vtype": vType = reader.Value; break;
                                case "playlink2":
                                    if (reader.NodeType == XmlNodeType.Element)
                                    {
                                        string node_name2 = reader.Name;
                                        mark = reader.GetAttribute("mark");
                                        bitrate = reader.GetAttribute("bitrate");
                                        fileLength = reader.GetAttribute("filelength");
                                        resolution = reader.GetAttribute("resolution");
                                        reader.Read();
                                        if (node_name2 == "source")
                                        {
                                            playLink = reader.Value;
                                        }
                                    }
                                    if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "source")
                                    {
                                        break;
                                    }
                                    ; break;
                                case "cover_imgurl": coverImgUrl = reader.Value; break;
                                case "onlinetime": onlinetime = reader.Value; break;
                                default:
                                    break;
                            }

                        }
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "c")
                        {
                            coverInfos.Add(new CoverInfo(title, note, Convert.ToInt32(vid), vType, mark, bitrate, Convert.ToInt32(fileLength), resolution, playLink, coverImgUrl, onlinetime));
                            break;
                        }
                    }
                }
            }
            return coverInfos;
        }
    }
}
