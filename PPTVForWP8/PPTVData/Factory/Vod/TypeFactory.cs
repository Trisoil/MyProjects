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

    public class TypeFactory : HttpFactoryBase<List<TypeInfo>>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}types.api?auth={1}&platform={2}", EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName);
        }

        protected override List<TypeInfo> AnalysisData(XmlReader reader)
        {
            var types = new List<TypeInfo>(16);
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element 
                    && reader.Name == "type")
                {
                    var type = new TypeInfo();
                    int tid = 0;
                    reader.ReadToFollowing("tid");
                    reader.Read();
                    type.TypeId = int.TryParse(reader.Value, out tid) ? tid : -1;
                    reader.ReadToFollowing("name");
                    reader.Read();
                    type.Name = reader.Value;
                    reader.ReadToFollowing("image");
                    reader.Read();
                    type.Image = reader.Value;
                    types.Add(type);
                }  
            }
            return types;
        }
    }
}
