using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Linq;

namespace Synacast.BoxManage.Core.Utils
{
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Internal;
    using Synacast.BoxManage.Core.Cache.Custom;

    public class ResponseUtils
    {
        /// <summary>
        /// 后端接口XML输出
        /// </summary>
        /// <param name="video"></param>
        /// <returns></returns>
        public static XElement ResponseBack(VideoNode video)
        {
            var cname = video.Main.Name.Replace("#", " ");
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var v = new XElement("v",
                new XElement("vid", video.Main.ChannelID),
                new XElement("playlink", BoxUtils.GetXmlCData(CreatePlayLink(video))),
                new XElement("title", cname),
                new XElement("type", video.Type),
                new XElement("catalog", video.Language[CustomArray.LanguageArray[0]].CatalogTags),
                new XElement("director", video.BKInfo.Directors),
                new XElement("act", video.BKInfo.Actors),
                new XElement("year", video.YearTags),
                new XElement("area", video.Language[CustomArray.LanguageArray[0]].AreaTags),
                new XElement("imgurl", video.ResImage),
                new XElement("state", video.State),
                new XElement("note", BoxUtils.GetXmlCData(video.BKInfo.Introduction)),
                new XElement("mark", video.BKInfo.Score),
                new XElement("hot", video.BKInfo.Score),
                new XElement("bitrate", video.Main.BitRate),
                new XElement("resolution", res),
                new XElement("flag", flag),
                new XElement("duration", video.Main.TimeLength / 60),
                new XElement("online-time", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss")),
                new XElement("last-update-time", video.Main.ModifyTime.ToString("yyyy-MM-dd HH:mm:ss"))
                );
            return v;
        }

        /// <summary>
        /// 保存输出到自定目录
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="xml"></param>
        public static void SaveXml(string fileName, XElement xml)
        {
            string basePath = AppSettingCache.Instance["StaticFilePath"];  //BoxUtils.GetAppSetting("StaticFiles", "path");
            if (string.IsNullOrEmpty(basePath))
                basePath = AppDomain.CurrentDomain.BaseDirectory;
            string path = string.Format("{0}/S", basePath);
            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);
            xml.Save(string.Format("{0}/{1}", path, fileName), SaveOptions.DisableFormatting);
        }

        private static string CreatePlayLink(VideoNode video)
        {
            var cname = video.Main.Name.Replace("#", " ");
            StringBuilder sb = new StringBuilder();
            if (video.ViceChannels != null && video.ViceChannels.Count > 0)
            {
                foreach (ViceNode vice in video.ViceChannels)
                {
                    var title = vice.Language[CustomArray.LanguageArray[0]].Title.Replace("#", " ");
                    if (sb.Length != 0)
                        sb.AppendFormat("#{0}({1})${2}${3}", cname, title, vice.PlayLink, "ppvod");
                    else
                        sb.AppendFormat("{0}({1})${2}${3}", cname, title, vice.PlayLink, "ppvod");
                }
            }
            else
                sb.AppendFormat("{0}${1}${2}", cname, video.PlayLink, "ppvod");
            return sb.ToString();
        }
    }
}
