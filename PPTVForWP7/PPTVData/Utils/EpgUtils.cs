using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace PPTVData
{
    public static class EpgUtils
    {
        #region Properties

        /// <summary>
        /// 影片详细信息EPG
        /// </summary>
        public static readonly string DetailEpgUri =
            string.Format("http://epg.api.pptv.com/detail.api?auth={0}&vid={1}&ver=2&platform={2}", EPGSetting.Auth, "{0}", EPGSetting.PlatformName);

        /// <summary>
        /// 拼音搜索EPG
        /// </summary>
        public static readonly string SearchChUri =
            string.Format("http://so.api.pptv.com/search_ch.api?auth={0}&c={1}&s={2}&k={3}&conlen={4}&screen=v&shownav=0&platform={5}", EPGSetting.Auth, "{0}", "{1}", "{2}", "{3}", EPGSetting.PlatformName);

        /// <summary>
        /// 智能搜索EPG
        /// </summary>
        public static readonly string SearchSmartUri =
            string.Format("http://so.api.pptv.com/search_smart.api?auth={0}&c={1}&s={2}&k={3}&conlen={4}&shownav=1&ver=2&platform={5}", EPGSetting.Auth, "{0}", "{1}", "{2}", "{3}", EPGSetting.PlatformName);

        /// <summary>
        /// 频道列表EPG
        /// </summary>
        public static readonly string ListUri =
            string.Format("http://epg.api.pptv.com/list.api?auth={0}&c={1}&s={2}&type={3}&conlen={4}&platform={5}", EPGSetting.Auth, "{0}", "{1}", "{2}", "{3}", EPGSetting.PlatformName);

        /// <summary>
        /// 频道列表排序EPG
        /// </summary>
        public static readonly string ListOrderUri =
            string.Format("http://epg.api.pptv.com/list.api?auth={0}&c={1}&s={2}&type={3}&conlen={4}&order={5}&platform={6}", EPGSetting.Auth, "{0}", "{1}", "{2}", "{3}", "{4}", EPGSetting.PlatformName);

        /// <summary>
        /// 推荐频道导航EPG
        /// </summary>
        public static readonly string RecommandNavUri =
            string.Format("http://epg.api.pptv.com/Recommand_nav.api?auth={0}&mode=1&platform={1}", EPGSetting.Auth, EPGSetting.PlatformName);

        /// <summary>
        /// 推荐频道信息EPG
        /// </summary>
        public static readonly string RecommandListUri =
            string.Format("http://epg.api.pptv.com/Recommend_list.api?auth={0}&c={1}&s={2}&navid={3}&mode={4}&conlen={5}&platform={6}", EPGSetting.Auth, "{0}", "{1}", "{2}", "{3}", "{4}", EPGSetting.PlatformName);

        /// <summary>
        /// 封面页频道导航EPG
        /// </summary>
        public static readonly string CoverNavUri =
            string.Format("http://cover.api.pptv.com/cover_image_recommand_nav.api?auth={0}&mode=1&platform={1}", EPGSetting.Auth, EPGSetting.PlatformName);

        /// <summary>
        /// 封面页频道信息EPG
        /// </summary>
        public static readonly string CoverListUri =
            string.Format("http://cover.api.pptv.com/cover_image_recommand_list.api?auth={0}&c={1}&s={2}&nav_id={3}&conlen=0&platform={4}", EPGSetting.Auth, "{0}", "{1}", "{2}", EPGSetting.PlatformName);

        /// <summary>
        /// 频道分类EPG
        /// </summary>
        public static readonly string TypeUri =
            string.Format("http://epg.api.pptv.com/types-ex.api?auth={0}&platform={1}", EPGSetting.Auth, EPGSetting.PlatformName);

        /// <summary>
        /// 频道TagEPG
        /// </summary>
        public static readonly string TagUri =
            string.Format("http://epg.api.pptv.com/tags-ex.api?auth={0}&type={1}&dimension={2}&platform={3}", EPGSetting.Auth, "{0}", "{1}", EPGSetting.PlatformName);

        /// <summary>
        /// 热词搜索
        /// </summary>
        public static readonly string HotSearchUri =
            string.Format("http://so.api.pptv.com/search_hot_keywords.api?&auth={0}&platform={1}&c={2}&s={3}&rankingspan={4}", EPGSetting.Auth, EPGSetting.PlatformName, "{0}", "{1}", "{2}");
        #endregion
    }
}
