using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class TagFactoryInfo
    {
        public List<TagInfo> Tags { get; set; }

        public int TypeId { get; set; }

        public TagDimension Dimension { get; set; }
    }

    public class TagFactory : HttpFactoryBase<TagFactoryInfo>
    {
        int _typeID = 0;
        TagDimension _dimension;

        protected override string CreateUri(params object[] paras)
        {
            _typeID = (int)paras[0];
            _dimension = (TagDimension)paras[1];

            return string.Format("{0}tags-ex.api?auth={1}&platform={2}&type={3}&dimension={4}",
                EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, _typeID, _dimension.CreateString());
        }

        protected override TagFactoryInfo AnalysisData(XmlReader reader)
        {
            var result = new TagFactoryInfo();
            result.Tags = new List<TagInfo>();
            result.TypeId = _typeID;
            result.Dimension = _dimension;
            result.Tags.Add(new TagInfo() { Count = 0, Name = "全部" });

            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "tag")
                {
                    string name = string.Empty;
                    int count = 0;
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element && !reader.IsEmptyElement)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "name": name = reader.Value; break;
                                case "count": count = int.Parse(reader.Value); break;
                                default: break;
                            }
                        }
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "tag")
                        {
                            var tagInfo = new TagInfo() { Count = count, Name = name };
                            result.Tags.Add(tagInfo);
                            break;
                        } 
                    }  
                } 
            }   
            return result;
        }
    }
}
