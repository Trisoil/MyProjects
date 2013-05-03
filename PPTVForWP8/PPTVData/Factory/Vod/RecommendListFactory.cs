using System;
using System.Net;
using System.Windows;

namespace PPTVData.Factory
{
    using PPTVData.Utils;

    public class RecommendListFactory : HttpVodFactoryBase
    {

        protected override string CreateUri(params object[] paras)
        {
            return string.Format("{0}recommend_list.api?auth={1}&platform={2}&c={3}&s={4}&navid={5}&mode=1&conlen={6}&userLevel={7}",
                EpgUtils.VodHost, EpgUtils.Auth, EpgUtils.PlatformName, paras[0], paras[1], paras[2], _contentCharNum, PPTVData.Factory.WAY.WAYGetFactory.GetUserLevel);
        }
    }
}
