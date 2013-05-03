using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Globalization;

namespace PPTVData.Factory.Player
{
    using PPTVData.Entity.Player;

    public class PlayJumpFactory : HttpFactoryBase<PlayJumpInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return string.Format("http://jump.g1d.net/{0}", paras[0]);
        }

        protected override PlayJumpInfo AnalysisData(XmlReader reader)
        {
            var info = new PlayJumpInfo();

            if (reader.ReadToFollowing("server_host"))
            {
                info.ServerHost = reader.ReadElementContentAsString();
            }

            if (reader.ReadToFollowing("server_time"))
            {
                var serverTime = reader.ReadElementContentAsString();
                var ci = new CultureInfo("en-US");
                info.ServerTime = DateTime.ParseExact(serverTime, "ddd MMM dd HH:mm:ss yyyy UTC", ci);
            }

            info.LocalTime = DateTime.Now;
            return info;
        }
    }
}
