using System;
using System.Net;
using System.Windows;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;

namespace PPTVData.Factory
{
    using PPTVData.Utils;
    using PPTVData.Entity;

    public class ChannelListFactory : HttpVodFactoryBase
    {
        protected override string CreateUri(params object[] paras)
        {
            var bulider = new StringBuilder(100);

            bulider.AppendFormat("{0}list.api?auth={1}&platform={2}&c={3}&s={4}&type={5}&conlen={6}",
                EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[3], paras[2], paras[0], _contentCharNum);
            bulider.AppendFormat("&tag={0}&order={1}",GenerateTagString(paras[1]), paras[4]);

            if (paras[5] != null && !string.IsNullOrEmpty(paras[5].ToString()))
                bulider.AppendFormat("&vt={0}", paras[5]);

            if (paras[6] != null && !string.IsNullOrEmpty(paras[6].ToString()))
                bulider.AppendFormat("&ft={0}", paras[6]);

            bulider.AppendFormat("&userLevel={0}", PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);

            return bulider.ToString();
        }

        private string GenerateTagString(object tags)
        {
            var tagInfos = tags as TagInfo[];
            if (tagInfos == null)
                return "||";

            var catalogTag = string.Empty;
            var areaTag = string.Empty;
            var yearTag = string.Empty;

            if (tagInfos.Length >= 1
                 && tagInfos[0] != null && tagInfos[0].Name != "全部")
                catalogTag = tagInfos[0].Name;

            if (tagInfos.Length >= 2
                && tagInfos[1] != null && tagInfos[1].Name != "全部")
                areaTag = tagInfos[1].Name;

            if (tagInfos.Length >= 3
                && tagInfos[2] != null && tagInfos[2].Name != "全部")
                yearTag = tagInfos[2].Name;

            return string.Format("{0}|{1}|{2}", catalogTag, areaTag, yearTag);
        }
    }
}
