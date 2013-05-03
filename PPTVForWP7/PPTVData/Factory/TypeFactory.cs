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
    public class TypeFactory : HttpFactoryBase<TypeInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return EpgUtils.TypeUri;
        }

        protected override TypeInfo AnalysisData(XmlReader reader)
        {
            TypeInfo typeInfo = null;
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element && reader.Name == "type")
                {
                    int tid = 0;
                    string name = string.Empty;
                    int count = 0;
                    while (reader.Read())
                    {
                        if (reader.NodeType == XmlNodeType.Element)
                        {
                            string node_name = reader.Name;
                            reader.Read();
                            switch (node_name)
                            {
                                case "tid": tid = int.Parse(reader.Value); break;
                                case "name": name = reader.Value; break;
                                case "count": count = int.Parse(reader.Value); break;
                                default: break;
                            }
                        }   // end if
                        if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "type")
                        {
                            typeInfo = new TypeInfo(tid, name, count);
                            break;
                        }   // end if
                    }   //end while
                }   // end if
            }   // end while
            return typeInfo;
        }
    }
}
