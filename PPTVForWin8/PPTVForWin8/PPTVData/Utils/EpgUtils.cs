using System;
using System.Net;
using System.Text;

namespace PPTVData.Utils
{
    using PPTVData.Factory;

    public static class EpgUtils
    {
        /// <summary>
        /// 客户端版本号
        /// </summary>
        public static string ClientVersion = "1.2.1";

        /// <summary>
        /// 验证码
        /// </summary>
        public static string Auth = "3dd5a901dcf9a61d127bab56fbcf7089";

        /// <summary>
        /// 平台名称
        /// </summary>
        public static string PlatformName = "win8";

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

        /// <summary>
        /// 生成Play的Type
        /// </summary>
        /// <param name="isDownload"></param>
        /// <returns></returns>
        public static string CreatePlayType(bool isDownload)
        {
            var bulid = new StringBuilder(20);

            bulid.Append("winrt");
            if (isDownload)
                bulid.Append(".download");
            if (PersonalFactory.Instance.Logined
                && PersonalFactory.Instance.DataInfos[0].UserStateInfo.VIP == 1)
                bulid.Append(".vip");

            return bulid.ToString();
        }

    }
}
