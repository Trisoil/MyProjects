using System;
using System.Net;

namespace PPTVData.Utils
{
    public static class EpgUtils
    {
        /// <summary>
        /// 客户端版本号
        /// </summary>
        public static string ClientVersion = "1.0";

        /// <summary>
        /// 验证码
        /// </summary>
        public static string Auth = "3dd5a901dcf9a61d127bab56fbcf7089";

        /// <summary>
        /// 平台名称
        /// </summary>
        public static string PlatformName = "wp7";

        /// <summary>
        /// 点播Host
        /// </summary>
        public static readonly string VodHost = "http://epg.api.pptv.com/";

        /// <summary>
        /// Cover Host
        /// </summary>
        public static readonly string CoverHost = "http://cover.api.pptv.com/";

        /// <summary>
        /// 搜索Host
        /// </summary>
        public static readonly string SearchHost = "http://so.api.pptv.com/";

        /// <summary>
        /// 云同步Host
        /// </summary>
        public static readonly string SyncHost = "http://sync.pptv.com/v2/";

        /// <summary>
        /// DAC Host
        /// </summary>
        public static readonly string DACUri = "http://ios.synacast.com/1.html?";

        /// <summary>
        /// WAY Host
        /// </summary>
        public static readonly string WAYHost = "http://way.pptv.com/";

        /// <summary>
        /// 直播中心Host
        /// </summary>
        public static readonly string LiveCenterHost = "http://livecenter.pptv.com/";

        /// <summary>
        /// CMS Host
        /// </summary>
        public static readonly string CMSHost = "http://app.aplus.pptv.com/";

    }
}
