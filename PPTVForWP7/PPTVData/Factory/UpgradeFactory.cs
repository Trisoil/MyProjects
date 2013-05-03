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
    public class UpgradeFactory : HttpFactoryBase<UpgradeInfo>
    {
        protected override string CreateUri(params object[] paras)
        {
            return "http://up.pplive.com/multiterminal/WP7_Config.xml";
        }

        protected override UpgradeInfo AnalysisData(XmlReader reader)
        {
            bool allow3g = false;
            string url = "";
            string latestversioninfo = "";
            string latestversion = "";
            string newversion = "";
            string newversioninfo = "";

            reader.ReadToFollowing("allow3g");
            reader.Read();
            allow3g = bool.Parse(reader.Value);

            reader.ReadToFollowing("url");
            reader.Read();
            url = reader.Value;

            reader.ReadToFollowing("latest_version");
            latestversion = reader.GetAttribute("version");
            reader.Read();
            latestversioninfo = reader.Value;


            reader.ReadToFollowing("new_version");
            newversion = reader.GetAttribute("version");
            reader.Read();
            newversioninfo = reader.Value;

            return new UpgradeInfo(allow3g, url, latestversion, newversion, latestversioninfo, newversioninfo);
        }

        protected override int TimeOut
        {
            get
            {
                return 3000;
            }
        }
    }
}
