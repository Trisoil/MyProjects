using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl.Utils
{
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.Core.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.List.Live.Search;

    public class VideoResponseUtils
    {
        /// <summary>
        /// 点播节目单不带节目介绍输出
        /// </summary>
        public static XElement CustomListRes(VideoNode video, ExFilterBase filter, bool isEx)
        {
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            var v = new XElement("v",
                    new XElement("vid", video.Main.ChannelID),
                    new XElement("playlink", BoxUtils.GetXmlCData(video.PlayLink)),
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
                    new XElement("total_state",video.BKInfo.Episode),
                    new XElement("note", ""),
                    new XElement("mark", video.BKInfo.Score),
                    new XElement("hot", ImplUtils.ConvertHot(video.Hot)),
                    new XElement("pv", video.PV),
                    new XElement("bitrate", video.Main.BitRate),
                    new XElement("resolution", res),
                    new XElement("flag", flag),
                    new XElement("duration", video.Main.TimeLength / 60),
                    new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss"))
                    );
            return v;
        }

        /// <summary>
        /// 点播节目单带节目介绍输出
        /// </summary>
        public static XElement ConLenListRes(VideoNode video, ExFilterBase filter, bool isEx)
        {
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            var v = new XElement("v",
                    new XElement("vid", video.Main.ChannelID),
                    new XElement("playlink", BoxUtils.GetXmlCData(video.PlayLink)),
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
                    new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction.FormateSubStr(filter.conlen))),
                    new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss"))
                    );
            return v;
        }

        /// <summary>
        /// 二代点播节目单不带节目介绍输出
        /// </summary>
        public static XElement CustomListResVer2(VideoNode video, ExFilterBase filter, bool isEx)
        {
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            var v = new XElement("v",
                        new XElement("vid", video.Main.ChannelID)
                    );
            var first = ListCache.Instance.Dictionary[video.Groups[0]];
            var p = new XElement("playlink2",
                        new XAttribute("duration", first.ViceChannels != null && first.ViceChannels.Count > 0 ? first.ViceChannels[0].Duration : first.Main.TimeLength)
                        //new XAttribute("start_point", 60.22),
                        //new XAttribute("end_point", 289.22)
                    );
            
            var maxbit = 0;
            var maxres = string.Empty;
            IEnumerable<int> groups = video.Groups;
            if (isEx && video.Groups.Count > 1)
            {
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                groups = video.Groups.Where(vod =>
                {
                    var result = true;
                    var gv = ListCache.Instance.Dictionary[vod];
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
                    new XAttribute("filelength", gv.ViceChannels != null && gv.ViceChannels.Count > 0 ? gv.ViceChannels[0].Duration * gv.Main.BitRate * (1024 / 8) : gv.Main.TimeLength * gv.Main.BitRate * (1024 / 8)),
                    new XAttribute("resolution", res),
                    BoxUtils.GetXmlCData(gv.PlayLink)
                ));
            }
            v.Add(
                p,
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
             new XElement("virtual", video.Virtuals == null ? 0 : 1),
             new XElement("bitrate", maxbit),
             new XElement("resolution", maxres),
             new XElement("flag", flag),
             new XElement("duration", video.Main.TimeLength / 60),
             new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss"))
             );
            return v;
        }

        /// <summary>
        /// 二代点播节目单带节目介绍输出
        /// </summary>
        public static XElement ConLenListResVer2(VideoNode video, ExFilterBase filter, bool isEx)
        {
            var flag = video.Flags.FormatListToStr(SplitArray.Line);
            var v = new XElement("v",
                        new XElement("vid", video.Main.ChannelID)
                    );
            var first = ListCache.Instance.Dictionary[video.Groups[0]];
            var p = new XElement("playlink2",
                        new XAttribute("duration", first.ViceChannels != null && first.ViceChannels.Count > 0 ? first.ViceChannels[0].Duration : first.Main.TimeLength)
                //new XAttribute("start_point", 60.22),
                //new XAttribute("end_point", 289.22)
                    );

            var maxbit = 0;
            var maxres = string.Empty;
            IEnumerable<int> groups = video.Groups;
            if (isEx && video.Groups.Count > 1)
            {
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                groups = video.Groups.Where(vod =>
                {
                    var result = true;
                    var gv = ListCache.Instance.Dictionary[vod];
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
                    new XAttribute("filelength", gv.ViceChannels != null && gv.ViceChannels.Count > 0 ? gv.ViceChannels[0].Duration * gv.Main.BitRate * (1024 / 8) : gv.Main.TimeLength * gv.Main.BitRate * (1024 / 8)),
                    new XAttribute("resolution", res),
                    BoxUtils.GetXmlCData(gv.PlayLink)
                ));
            }
            v.Add(
                p,
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
             new XElement("virtual", video.Virtuals == null ? 0 : 1),
             new XElement("bitrate", maxbit),
             new XElement("resolution", maxres ),
             new XElement("flag", flag),
             new XElement("duration", video.Main.TimeLength / 60),
             new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction.FormateSubStr(filter.conlen))),
             new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss"))
             );
            return v;
        }

        /// <summary>
        /// 直播节目单带节目介绍输出
        /// </summary>
        public static XElement CustomLiveListRes(LiveVideoNode video, ExFilterBase filter, int type, bool isEx)
        {
            var res = VideoUtils.FormatStrArray(10, video.Main.VideoWidth, "|", video.Main.VideoHeight);
            var flags = video.Flags.FormatListToStr(SplitArray.Line);
            XElement playlink = null;
            if (filter.ver < 2)
                playlink = new XElement("playlink", BoxUtils.GetXmlCData(video.Main.PlayLink));
            else
                playlink = CreateLivePlayLinkVer2(video.Main.ChannelID, filter, isEx);
            var v = new XElement("v",
                    new XElement("vid", video.Main.ChannelID),
                    playlink,
                    new XElement("title", video.Language[filter.lang].ChannelName),
                    new XElement("type", type),
                    new XElement("imgurl", video.ResImage),
                    new XElement("sloturl", ImplUtils.SlogImageJoin(video.SlotImage)),
                    new XElement("note", ""),
                    new XElement("mark", video.BKInfo.Score),
                    new XElement("hot", ImplUtils.ConvertHot(video.Hot)),
                    new XElement("pv", video.PV),
                    new XElement("bitrate", video.Main.BitRate),
                    new XElement("resolution", res),
                    new XElement("flag", flags),
                    new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction)),
                    new XElement("onlinetime", video.Main.CreateTime.ToString("yyyy-MM-dd HH:mm:ss"))
                    );
            if (filter.nowplay)
            {
                var flag = false;
                for (var i = 0; i < video.Items.Count; i++)
                {
                    if (video.Items[i].StartTime > DateTime.Now)
                    {
                        if (i != 0)
                        {
                            v.Add(new XElement("nowplay",
                                new XAttribute("begin_time", video.Items[i - 1].StartTime.TimeOfDay.ToString()),
                                video.Items[i - 1].Language[filter.lang].Title)
                                );
                        }
                        v.Add(new XElement("willplay",
                                new XAttribute("begin_time", video.Items[i].StartTime.TimeOfDay.ToString()),
                                video.Items[i].Language[filter.lang].Title)
                                );
                        flag = true;
                        break;
                    }
                }
                if (!flag && video.Items.Count > 0 && video.Items[video.Items.Count - 1].StartTime.Date.Equals(DateTime.Now.Date))
                {

                    v.Add(new XElement("nowplay",
                                new XAttribute("begin_time", video.Items[video.Items.Count - 1].StartTime.TimeOfDay.ToString()),
                                video.Items[video.Items.Count - 1].Language[filter.lang].Title)
                                );
                }
            }
            return v;
        }

        /// <summary>
        /// 直播节目预告输出
        /// </summary>
        public static XElement LiveListParade(ChannelLiveItemsNode parade, LiveFilter filter)
        {
            return new XElement("parade",
                new XAttribute("begin_time", parade.StartTime.TimeOfDay.ToString()),
                parade.Language[filter.lang].Title
                );
        }

        /// <summary>
        /// 直播搜索节目预告输出
        /// </summary>
        public static XElement LiveListSearchParade(ChannelLiveItemsNode parade, LiveSearchFilter filter)
        {
            return new XElement("parade",
                new XAttribute("date", parade.StartTime.Date.ToString("yyyy-MM-dd")),
                new XAttribute("begin_time", parade.StartTime.TimeOfDay.ToString()),
                parade.Language[filter.lang].Title
                );
        }

        /// <summary>
        /// 创建二代点播播放串，合集只写出首集
        /// </summary>
        public static XElement CreateVodPlayLinkVer2(int vid, ExFilterBase filter, bool isEx)
        {
            var video = ListCache.Instance.Dictionary[vid];
            var p = new XElement("playlink2",
                            new XAttribute("duration", video.ViceChannels != null && video.ViceChannels.Count > 0 ? video.ViceChannels[0].Duration : video.Main.TimeLength)
                            //new XAttribute("start_point", 60.22),
                            //new XAttribute("end_point", 289.22)
                        );
            IEnumerable<int> groups = video.Groups;
            if (isEx && video.Groups.Count > 1)
            {
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                groups = video.Groups.Where(v =>
                {
                    var result = true;
                    var gv = ListCache.Instance.Dictionary[v];
                    foreach (var f in filters)     //过滤不符合的子频道
                    {
                        result = result && f(gv, filter);
                    }
                    return result;
                });
            }
            foreach (var g in groups)
            {
                var gv = ListCache.Instance.Dictionary[g];
                var res = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                p.Add(new XElement("source",
                    new XAttribute("mark", gv.Language[filter.lang].Mark),
                    new XAttribute("bitrate", gv.Main.BitRate),
                    new XAttribute("filelength", gv.ViceChannels != null && gv.ViceChannels.Count > 0 ? gv.ViceChannels[0].Duration * gv.Main.BitRate * (1024 / 8) : gv.Main.TimeLength * gv.Main.BitRate * (1024 / 8)),
                    new XAttribute("resolution", res),
                    BoxUtils.GetXmlCData(gv.PlayLink)
                ));
            }
            return p;
        }

        /// <summary>
        /// 创建二代直播播放串
        /// </summary>
        public static XElement CreateLivePlayLinkVer2(int vid, ExFilterBase filter, bool isEx)
        {
            var video = LiveListCache.Instance.Dictionary[vid];
            var p = new XElement("playlink2");
            IEnumerable<int> groups = video.Groups;
            if (isEx && video.Groups.Count > 1)
            {
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                groups = video.Groups.Where(v =>
                {
                    var result = true;
                    var gv = LiveListCache.Instance.Dictionary[v];
                    foreach (var f in filters)     //过滤不符合的子频道
                    {
                        result = result && f(gv, filter);
                    }
                    return result;
                });
            }
            foreach (var g in groups)
            {
                var gv = LiveListCache.Instance.Dictionary[g];
                var res = VideoUtils.FormatStrArray(10, gv.Main.VideoWidth, "|", gv.Main.VideoHeight);
                p.Add(new XElement("source",
                    new XAttribute("mark", gv.Language[filter.lang].Mark),
                    new XAttribute("bitrate", gv.Main.BitRate),
                    new XAttribute("resolution", res),
                    BoxUtils.GetXmlCData(gv.Main.PlayLink)
                ));
            }
            return p;
        }
    }
}
