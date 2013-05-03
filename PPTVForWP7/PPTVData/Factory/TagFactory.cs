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
    public class TagFactoryInfo
    {
        public List<TagInfo> Tags { get; set; }

        public int TypeId { get; set; }

        public string Dimension { get; set; }
    }

    public class TagFactory : HttpFactoryBase<TagFactoryInfo>
    {
        int _typeID = 0;
        string _dimension = string.Empty;

        protected override string CreateUri(params object[] paras)
        {
            _typeID = (int)paras[0];
            _dimension = (string)paras[1];
            return string.Format(EpgUtils.TagUri, _typeID, _dimension);
        }

        protected override TagFactoryInfo AnalysisData(XmlReader reader)
        {
            var result = new TagFactoryInfo();
            result.Tags = new List<TagInfo>();
            result.TypeId = _typeID;
            result.Dimension = _dimension;
            result.Tags.Add(new TagInfo(_typeID, _dimension, "全部", -1));
            while (reader.Read())
            {
                TagInfo tagInfo = null;
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "tag")
                {
                    string name = "";
                    int count = 0;
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "name": name = reader.Value; break;
                                case "count": count = int.Parse(reader.Value); break;
                                default: break;
                            }
                        }   // end if
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "tag")
                        {
                            tagInfo = new TagInfo(_typeID, _dimension, name, count);
                            result.Tags.Add(tagInfo);
                            break;
                        }   // end if
                    }   //end while
                }   // end if
            }   // end while
            return result;
        }
    }
}
