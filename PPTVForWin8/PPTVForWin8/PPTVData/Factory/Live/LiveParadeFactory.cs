using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace PPTVData.Factory.Live
{
    using PPTVData.Utils;
    using PPTVData.Entity.Live;

    public class LiveParadeFactory : HttpFactoryBase<List<LiveParadeInfo>>
    {
        int _index;

        protected override string CreateUri(params object[] paras)
        {
            if (DateTime.Now.Date.Equals(DateTime.Parse(paras[1].ToString())))
                _index = 0;
            else
                _index = 2;

            return string.Format("{0}live-parade.api?auth={1}&platform={2}&vid={3}&date={4}&userLevel={5}"
                , EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], paras[1], PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }

        protected override List<LiveParadeInfo> AnalysisData(System.Xml.XmlReader reader)
        {
            var list = new List<LiveParadeInfo>();

            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element
                    && reader.Name == "parade")
                {
                    var info = new LiveParadeInfo();
                    info.BeginTime = reader["begin_time"];
                    reader.Read();
                    info.Name = reader.Value;
                    info.Index = _index;
                    _index++;
                    list.Add(info);
                }
            }

            return list;
        }
    }
}
