using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Synacast.BoxManage.List.Impl
{
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Details;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.List.Impl.Vod.Lists;
    using Synacast.BoxManage.Core.Utils;

    /// <summary>
    /// 扩展方法
    /// </summary>
    public static class ExtensionClass
    {
        public static string FormateSubStr(this string source, int index)
        {
            if (index >= source.Length)
                return source;
            var builder = new StringBuilder(index + 8);
            builder.Append(source.Substring(0, index));
            builder.Append("...");
            return builder.ToString();
        }

        public static int PageList<T>(this IEnumerable<T> source, ExFilterBase filter, VideoPars pars, XElement root, bool isEx) where T : VideoNode
        {
            var index = (filter.s - 1) * filter.c;
            var max = index + filter.c;
            int num = 0;
            using (IEnumerator<T> enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    if (index <= num && num < max)
                    {
                        root.Add(pars.ResponseFun(enumerator.Current, filter, isEx));
                    }
                    num++;
                }
            }
            return num;
        }

        public static int PageList(this IEnumerable<int> source, ExFilterBase filter, VideoPars pars, XElement root, bool isEx)
        {
            var index = (filter.s - 1) * filter.c;
            var max = index + filter.c;
            int num = 0;
            using (var enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    if (index <= num && num < max)
                    {
                        root.Add(pars.ResponseFun(ListCache.Instance.Dictionary[enumerator.Current], filter, isEx));
                    }
                    num++;
                }
            }
            return num;
        }

        public static int PageList<T>(this IEnumerable<T> source, LiveFilter filter, VideoPars pars, XElement root, bool isEx) where T : LiveVideoNode
        {
            var index = (filter.s - 1) * filter.c;
            var max = index + filter.c;
            int num = 0;
            using (IEnumerator<T> enumerator = source.GetEnumerator())
            {
                while (enumerator.MoveNext())
                {
                    if (index <= num && num < max)
                    {
                        root.Add(VideoResponseUtils.CustomLiveListRes(enumerator.Current, filter, filter.type, isEx));
                    }
                    num++;
                }
            }
            return num;
        }

        public static void PageDetailVer1(this List<ViceNode> vices, DetailFilter filter, XElement xml, XElement parent)
        {
            var all = vices.Count;
            if (filter.c != 0 && filter.s != 0) // 分页
            {
                parent.Add(new XElement("countInPage", filter.c),
                        new XElement("page", filter.s)
                    );
                var index = 0; var max = 0;
                index = (filter.s - 1) * filter.c;
                max = index + filter.c;
                if (filter.order == "r")
                {
                    index = all - 1 - index;
                    max = all - 1 - max;
                    if (max < -1)
                        max = -1;
                    for (var i = index; i > max; i--)
                    {
                        xml.Add(new XElement("video",
                            new XAttribute("id", vices[i].ChannelId),
                            new XAttribute("title", vices[i].Language[filter.lang].Title),
                            BoxUtils.GetXmlCData(vices[i].PlayLink)
                            ));
                    }
                }
                else
                {
                    if (max > all)
                        max = all;
                    for (var i = index; i < max; i++)
                    {
                        xml.Add(new XElement("video",
                            new XAttribute("id", vices[i].ChannelId),
                            new XAttribute("title", vices[i].Language[filter.lang].Title),
                            BoxUtils.GetXmlCData(vices[i].PlayLink)
                            ));
                    }
                }
            }
            else
            {
                if (filter.order == "r")
                {
                    for (var i = all - 1; i >= 0; i--)
                    {
                        xml.Add(new XElement("video",
                            new XAttribute("id", vices[i].ChannelId),
                            new XAttribute("title", vices[i].Language[filter.lang].Title),
                            BoxUtils.GetXmlCData(vices[i].PlayLink)
                            ));
                    }
                }
                else
                {
                    for (var i = 0; i < all; i++)
                    {
                        xml.Add(new XElement("video",
                            new XAttribute("id", vices[i].ChannelId),
                            new XAttribute("title", vices[i].Language[filter.lang].Title),
                            BoxUtils.GetXmlCData(vices[i].PlayLink)
                            ));
                    }
                }
            }
        }

        public static void PageDetailVer2(this List<VideoNode> videos, DetailFilter filter, XElement xml, XElement parent, int all)
        {
            if (filter.c != 0 && filter.s != 0)
            {
                parent.Add(new XElement("countInPage", filter.c),
                        new XElement("page", filter.s)
                    );
                var index = 0; var max = 0;
                index = (filter.s - 1) * filter.c;
                max = index + filter.c;
                if (filter.order == "r")
                {
                    index = all - 1 - index;
                    max = all - 1 - max;
                    if (max < -1)
                        max = -1;
                    for (var i = index; i > max; i--)
                    {
                        var p = new XElement("playlink2",
                            new XAttribute("id", videos[0].ViceChannels[i].ChannelId),
                            new XAttribute("title", videos[0].ViceChannels[i].Language[filter.lang].Title),
                            new XAttribute("duration", videos[0].ViceChannels[i].Duration)
                            //new XAttribute("start_point", 60.22),
                            //new XAttribute("end_point", 289.22)
                        );
                        foreach (var v in videos)
                        {
                            p.Add(new XElement("source",
                                new XAttribute("mark", v.Language[filter.lang].Mark),
                                new XAttribute("bitrate", v.Main.BitRate),
                                new XAttribute("filelength", v.ViceChannels[i].Duration * v.Main.BitRate * (1024 / 8)),
                                new XAttribute("resolution", VideoUtils.FormatStrArray(10, v.Main.VideoWidth, "|", v.Main.VideoHeight)),
                                BoxUtils.GetXmlCData(v.ViceChannels[i].PlayLink)
                                ));
                        }
                        xml.Add(p);
                    }
                }
                else
                {
                    if (max > all)
                        max = all;
                    for (var i = index; i < max; i++)
                    {
                        var p = new XElement("playlink2",
                            new XAttribute("id", videos[0].ViceChannels[i].ChannelId),
                            new XAttribute("title", videos[0].ViceChannels[i].Language[filter.lang].Title),
                            new XAttribute("duration", videos[0].ViceChannels[i].Duration)
                            //new XAttribute("start_point", 60.22),
                            //new XAttribute("end_point", 289.22)
                        );
                        foreach (var v in videos)
                        {
                            p.Add(new XElement("source",
                                new XAttribute("mark", v.Language[filter.lang].Mark),
                                new XAttribute("bitrate", v.Main.BitRate),
                                new XAttribute("filelength", v.ViceChannels[i].Duration * v.Main.BitRate * (1024 / 8)),
                                new XAttribute("resolution", VideoUtils.FormatStrArray(10, v.Main.VideoWidth, "|", v.Main.VideoHeight)),
                                BoxUtils.GetXmlCData(v.ViceChannels[i].PlayLink)
                                ));
                        }
                        xml.Add(p);
                    }
                }
            }
            else
            {
                if (filter.order == "r")
                {
                    for (var i = all - 1; i >= 0; i--)
                    {
                        var p = new XElement("playlink2",
                            new XAttribute("id", videos[0].ViceChannels[i].ChannelId),
                            new XAttribute("title", videos[0].ViceChannels[i].Language[filter.lang].Title),
                            new XAttribute("duration", videos[0].ViceChannels[i].Duration)
                            //new XAttribute("start_point", 60.22),
                            //new XAttribute("end_point", 289.22)
                        );
                        foreach (var v in videos)
                        {
                            p.Add(new XElement("source",
                                new XAttribute("mark", v.Language[filter.lang].Mark),
                                new XAttribute("bitrate", v.Main.BitRate),
                                new XAttribute("filelength", v.ViceChannels[i].Duration * v.Main.BitRate * (1024 / 8)),
                                new XAttribute("resolution", VideoUtils.FormatStrArray(10, v.Main.VideoWidth, "|", v.Main.VideoHeight)),
                                BoxUtils.GetXmlCData(v.ViceChannels[i].PlayLink)
                                ));
                        }
                        xml.Add(p);
                    }
                }
                else
                {
                    for (var i = 0; i < all; i++)
                    {
                        var p = new XElement("playlink2",
                            new XAttribute("id", videos[0].ViceChannels[i].ChannelId),
                             new XAttribute("title", videos[0].ViceChannels[i].Language[filter.lang].Title),
                             new XAttribute("duration", videos[0].ViceChannels[i].Duration)
                             //new XAttribute("start_point", 60.22),
                             //new XAttribute("end_point", 289.22)
                         );
                        foreach (var v in videos)
                        {
                            p.Add(new XElement("source",
                                new XAttribute("mark", v.Language[filter.lang].Mark),
                                new XAttribute("bitrate", v.Main.BitRate),
                                new XAttribute("filelength", v.ViceChannels[i].Duration * v.Main.BitRate * (1024 / 8)),
                                new XAttribute("resolution", VideoUtils.FormatStrArray(10, v.Main.VideoWidth, "|", v.Main.VideoHeight)),
                                BoxUtils.GetXmlCData(v.ViceChannels[i].PlayLink)
                                ));
                        }
                        xml.Add(p);
                    }
                }
            }
        }
    }
}
