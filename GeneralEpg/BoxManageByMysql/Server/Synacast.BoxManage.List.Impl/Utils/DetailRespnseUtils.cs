using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Utils
{
    using Synacast.BoxManage.List.Details;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;

    public static class DetailRespnseUtils
    {
        #region 完整接口

        public static string ResList(VideoNode video, DetailFilter filter, bool isEx)
        {
            var xml = new XElement("v");
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            xml.Add(
                new XElement("vid", video.Main.ChannelID),
                new XElement("title", video.Language[filter.lang].ChannelName),
                new XElement("subtitle", video.Language[filter.lang].SubTitle),
                new XElement("type", video.Type),
                new XElement("catalog", video.Language[filter.lang].CatalogTags),
                new XElement("director", video.Language[filter.lang].Directors),
                new XElement("act", video.Language[filter.lang].Actors),
                new XElement("year", video.YearTags),
                new XElement("area", video.Language[filter.lang].AreaTags),
                new XElement("imgurl", video.ResImage),
                new XElement("sloturl", ImplUtils.SlogImageJoin(video.SlotImage)),
                new XElement("state", video.State),
                new XElement("total_state", video.BKInfo.Episode),
                new XElement("note", ""),
                new XElement("mark", video.BKInfo.Score),
                new XElement("hot", ImplUtils.ConvertHot(video.Hot)),
                new XElement("pv", video.PV),
                new XElement("bitrate", video.Main.BitRate),
                new XElement("resolution", res),
                new XElement("flag", flag),
                new XElement("duration", video.Main.TimeLength / 60),
                new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction)),
                new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss")),
                new XElement("video_list_count", video.StateCount)
            );
            if (filter.ver <= 1)
            {
                CreateLinkVer1(video, filter, xml);
            }
            else
            {
                CreateLinkVer2(video, filter, xml, isEx);
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 一代播放串
        /// </summary>
        private static void CreateLinkVer1(VideoNode video,DetailFilter filter, XElement xml)
        {
            var link = new XElement("video_list",
                    new XAttribute("maxBytes", video.Language[filter.lang].MaxBytes),
                    new XAttribute("maxChars", video.Language[filter.lang].MaxChars),
                    new XAttribute("isNumber", video.IsNumber)
                );
            if (video.ViceChannels == null)
                throw new ApplicationException("该节目为二代节目，请将参数ver设置为2");
            if (video.ViceChannels.Count > 0)
            {
                video.ViceChannels.PageDetailVer1(filter, link, xml);
                xml.Add(new XElement("video_list_page_count", PageUtils.PageCount(video.StateCount, filter.c)));
            }
            else
            {
                xml.Add(new XElement("countInPage", 1),
                    new XElement("page", 1),
                    new XElement("video_list_page_count", 1)
                    );
                link.Add(new XElement("video",
                    new XAttribute("id", video.Main.ChannelID),
                    new XAttribute("title", video.Language[filter.lang].ChannelName),
                    BoxUtils.GetXmlCData(video.PlayLink)
                    ));
            }
            xml.Add(link);
        }

        private static void CreateLinkVer2(VideoNode video, DetailFilter filter, XElement xml, bool isEx)
        {
            var vir = video.Virtuals == null ? 0 : 1;
            xml.Add(new XElement("virtual", vir));
            var first = video;
            if (vir == 1) //虚拟频道，取第一条子频道
                first = ListCache.Instance.Dictionary[video.Groups[0]];
            if (first.StateCount <= 1)    //不带分集的正常频道
            {
                //var first = ListCache.Instance.Dictionary[video.Groups[0]];   
                var link = new XElement("video_list2",
                    new XAttribute("maxBytes", first.Language[filter.lang].MaxBytes),
                    new XAttribute("maxChars", first.Language[filter.lang].MaxChars),
                    new XAttribute("isNumber", first.IsNumber)
                );
                xml.Add(new XElement("countInPage", 1),
                    new XElement("page", 1),
                    new XElement("video_list_page_count", 1)
                    );
                var p = new XElement("playlink2",
                    new XAttribute("id",first.Main.ChannelID),
                        new XAttribute("title", first.ChannelName),
                        new XAttribute("duration", first.Main.TimeLength)
                        //new XAttribute("start_point", 60.22),
                        //new XAttribute("end_point", 289.22)
                    );
                var maxbit = 0; var maxres = string.Empty;
                IEnumerable<int> groups = first.Groups;
                if (isEx && first.Groups.Count > 1)
                {
                    var filters = VideoNodesUtil.FormateTagFilter(filter);
                    groups = first.Groups.Where(v =>
                    {
                        var result = true;
                        VideoNode gv = ListCache.Instance.Dictionary[v];
                        foreach (var f in filters)     //过滤不符合的子频道
                        {
                            result = result && f(gv, filter);
                        }
                        return result;
                    });
                }
                foreach (var g in groups)
                {
                    VideoNode gv = ListCache.Instance.Dictionary[g];
                    var res = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                    if (maxbit < gv.Main.BitRate)
                    {
                        maxbit = gv.Main.BitRate;
                        maxres = res;
                    }
                    p.Add(new XElement("source",
                        new XAttribute("mark", gv.Language[filter.lang].Mark),
                        new XAttribute("bitrate", gv.Main.BitRate),
                        new XAttribute("filelength", gv.Main.TimeLength * gv.Main.BitRate * (1024 / 8)),
                        new XAttribute("resolution", res),
                        BoxUtils.GetXmlCData(gv.PlayLink))
                    );
                }
                link.Add(p);
                xml.Add(link);
                if (xml.Element("bitrate") != null)
                {
                    xml.Element("bitrate").SetValue(maxbit);
                    xml.Element("resolution").SetValue(maxres);
                }
            }
            else
            {
                var maxbit = 0;
                var maxres = string.Empty;
                List<VideoNode> vices = null;
                if (isEx && first.Groups.Count > 1)   //扩展接口，过滤分组子频道
                {
                    var filters = VideoNodesUtil.FormateTagFilter(filter);
                    vices = first.Groups.Where(v =>
                    {
                        var result = true;
                        VideoNode gv = ListCache.Instance.Dictionary[v];
                        var res = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                        if (maxbit < gv.Main.BitRate)   //最大码率赋值给主频道
                        {
                            maxbit = gv.Main.BitRate;
                            maxres = res;
                        }
                        foreach (var f in filters)     //过滤不符合的子频道
                        {
                            result = result && f(gv, filter);
                        }
                        return result;
                    }).Select(v => ListCache.Instance.Dictionary[v]).ToPerfectList();
                }
                else     //非扩展接口
                {
                    vices = first.Groups.Where(v =>
                    {
                        VideoNode gv = ListCache.Instance.Dictionary[v];
                        var res = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                        if (maxbit < gv.Main.BitRate)
                        {
                            maxbit = gv.Main.BitRate;
                            maxres = res;
                        }
                        return true;
                    }).Select(v => ListCache.Instance.Dictionary[v]).ToPerfectList();
                }
                if (vices.Count < 1)   //分组里没有子集
                {
                    xml.Add(new XElement("video_list_page_count", 0));
                    xml.Element("video_list_count").SetValue(0);
                    xml.Element("bitrate").SetValue(0);
                    xml.Element("resolution").SetValue(0);
                    return;
                }
                if (xml.Element("bitrate") != null)
                {
                    xml.Element("bitrate").SetValue(maxbit);
                    xml.Element("resolution").SetValue(maxres);
                }
                var link = new XElement("video_list2",
                    new XAttribute("maxBytes", vices[0].Language[filter.lang].MaxBytes),
                    new XAttribute("maxChars", vices[0].Language[filter.lang].MaxChars),
                    new XAttribute("isNumber", vices[0].IsNumber)
                );
                xml.Add(new XElement("video_list_page_count", PageUtils.PageCount(video.StateCount, filter.c)));
                vices.PageDetailVer2(filter, link, xml, first.StateCount);
                xml.Add(link);
            }
            if (vir == 1)
            {
                IEnumerable<int> virtuals = video.Virtuals;
                if (isEx)
                {
                    var filters = VideoNodesUtil.FormateTagFilter(filter);
                    virtuals.Where(v =>
                    {
                        var result = true;
                        VideoNode gv = ListCache.Instance.Dictionary[v];
                        foreach (var f in filters)     //过滤不符合的子频道
                        {
                            result = result && f(gv, filter);
                        }
                        return result;
                    });
                }
                var virs = new XElement("virtual_list");
                foreach (var virid in virtuals)
                {
                    VideoNode virnode = ListCache.Instance.Dictionary[virid];
                    virs.Add(new XElement("virtual",
                        new XAttribute("vid", virid),
                        new XAttribute("title", virnode.ChannelName),
                        new XAttribute("imgurl", virnode.ResImage)
                    ));
                }
                xml.Add(virs);
            }
        }

        #endregion

        #region 不带分集信息

        public static string ResWithoutset(VideoNode video, DetailFilter filter, bool isEx)
        {
            var xml = new XElement("v");
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            xml.Add(
                new XElement("vid", video.Main.ChannelID),
                new XElement("title", video.Language[filter.lang].ChannelName),
                new XElement("subtitle", video.Language[filter.lang].SubTitle),
                new XElement("type", video.Type),
                new XElement("catalog", video.Language[filter.lang].CatalogTags),
                new XElement("director", video.Language[filter.lang].Directors),
                new XElement("act", video.Language[filter.lang].Actors),
                new XElement("year", video.YearTags),
                new XElement("area", video.Language[filter.lang].AreaTags),
                new XElement("imgurl", video.ResImage),
                new XElement("sloturl", ImplUtils.SlogImageJoin(video.SlotImage)),
                new XElement("state", video.State),
                new XElement("total_state", video.BKInfo.Episode),
                new XElement("note", ""),
                new XElement("mark", video.BKInfo.Score),
                new XElement("hot", video.Hot),
                new XElement("pv", video.PV),
                new XElement("bitrate", video.Main.BitRate),
                new XElement("resolution", res),
                new XElement("flag", flag),
                new XElement("duration", video.Main.TimeLength / 60),
                new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction)),
                new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss")),
                new XElement("video_list_count", video.StateCount)
            );
            if (filter.ver >= 2)
            {
                var vir = video.Virtuals == null ? 0 : 1;
                xml.Add(new XElement("virtual", vir));
                if (vir == 1)
                {
                    IEnumerable<int> virtuals = video.Virtuals;
                    if (isEx)
                    {
                        var filters = VideoNodesUtil.FormateTagFilter(filter);
                        virtuals.Where(v =>
                        {
                            var result = true;
                            VideoNode gv = ListCache.Instance.Dictionary[v];
                            foreach (var f in filters)     //过滤不符合的子频道
                            {
                                result = result && f(gv, filter);
                            }
                            return result;
                        });
                    }
                    var virs = new XElement("virtual_list");
                    foreach (var virid in virtuals)
                    {
                        VideoNode virnode = ListCache.Instance.Dictionary[virid];
                        virs.Add(new XElement("virtual",
                            new XAttribute("vid", virid),
                            new XAttribute("title", virnode.ChannelName),
                            new XAttribute("imgurl", virnode.ResImage)
                        ));
                    }
                    xml.Add(virs);
                }

                var maxbit = 0;
                var maxres = string.Empty;
                IEnumerable<int> vices = null;
                if (isEx)   //扩展接口，过滤分组子频道
                {
                    var filters = VideoNodesUtil.FormateTagFilter(filter);
                    vices = video.Groups.Where(v =>
                    {
                        var result = true;
                        VideoNode gv = ListCache.Instance.Dictionary[v];
                        if (maxbit < gv.Main.BitRate)   //最大码率赋值给主频道
                        {
                            maxbit = gv.Main.BitRate;
                            maxres = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                        }
                        foreach (var f in filters)     //过滤不符合的子频道
                        {
                            result = result && f(gv, filter);
                        }
                        return result;
                    });
                    var count = vices.Count();
                    if (count < 1)   //分组里没有子集
                    {
                        xml.Element("video_list_count").SetValue(0);
                        xml.Element("bitrate").SetValue(0);
                        xml.Element("resolution").SetValue(0);
                    }
                    else
                    {
                        xml.Element("bitrate").SetValue(maxbit);
                        xml.Element("resolution").SetValue(maxres);
                    }
                }
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        #endregion

        #region 只带分集

        public static string ResOnlyset(VideoNode video, DetailFilter filter, bool isEx)
        {
            var xml = new XElement("v");
            xml.Add(
                new XElement("vid", video.Main.ChannelID),
                new XElement("title", video.Language[filter.lang].ChannelName),
                new XElement("subtitle", video.Language[filter.lang].SubTitle),
                new XElement("type", video.Type),
                new XElement("imgurl", video.ResImage),
                new XElement("sloturl", ImplUtils.SlogImageJoin(video.SlotImage)),
                new XElement("state", video.State),
                new XElement("total_state", video.BKInfo.Episode),
                new XElement("mark", video.BKInfo.Score),
                new XElement("video_list_count", video.StateCount)
            );
            if (filter.ver <= 1)
            {
                CreateLinkVer1(video, filter, xml);
            }
            else
            {
                CreateLinkVer2(video, filter, xml, isEx);
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        #endregion
    }
}
