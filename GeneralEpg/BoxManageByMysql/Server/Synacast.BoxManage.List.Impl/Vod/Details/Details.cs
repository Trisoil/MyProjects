using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Details
{
    using Synacast.BoxManage.List.Details;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class Details : IDetails 
    {
        #region IDetails 成员

        public string DetailsList(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResList(video, filter, false);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string DetailsListEx(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResList(video, filter, true);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string DetailsWithoutSet(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResWithoutset(video, filter, false);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string DetailsWithoutSetEx(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResWithoutset(video, filter, true);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string DetailsOnlySet(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResOnlyset(video, filter, false);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string DetailsOnlySetEx(DetailFilter filter)
        {
            try
            {
                var video = ListCache.Instance.Dictionary[filter.vid];
                return DetailRespnseUtils.ResOnlyset(video, filter, true);
            }
            catch (KeyNotFoundException)
            {
                return NoVideos();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        private string NoVideos()
        {
            return BoxUtils.FormatErrorMsg("不存在该影片");
        }

        #region Deleted

        //private string CreateList(VideoNode video, DetailFilter filter)
        //{
        //    var xml = new XElement("v");
        //    if (video != null)
        //    {
        //        xml.Add(
        //            new XElement("vid", video.Main.ChannelID),
        //            new XElement("title", video.Language[filter.lang].ChannelName),
        //            new XElement("subtitle", video.Language[filter.lang].SubTitle),
        //            new XElement("type", video.Type),
        //            new XElement("catalog", video.Language[filter.lang].CatalogTags),
        //            new XElement("director", video.Language[filter.lang].Directors),
        //            new XElement("act", video.Language[filter.lang].Actors),
        //            new XElement("year", video.YearTags),
        //            new XElement("area", video.Language[filter.lang].AreaTags),
        //            new XElement("imgurl", video.ResImage),
        //            new XElement("state", video.State),
        //            new XElement("note", ""),
        //            new XElement("mark", video.BKInfo.Score),
        //            new XElement("hot", video.Hot),
        //            new XElement("pv", video.PV),
        //            new XElement("bitrate", video.Main.BitRate),
        //            new XElement("resolution", video.ResResolution),
        //            new XElement("flag", video.ResFlags),
        //            new XElement("duration", video.Main.TimeLength / 60),
        //            new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction)),
        //            new XElement("video_list_count", video.StateCount)
        //            );
        //        WriteViceChannel(video, xml, filter);
        //        return xml.ToString(SaveOptions.DisableFormatting);
        //    }
        //    return NoVideos();
        //}

        //private string CreateWithout(VideoNode video, DetailFilter filter)
        //{
        //    var xml = new XElement("v");
        //    if (video != null)
        //    {
        //        xml.Add(
        //            new XElement("vid", video.Main.ChannelID),
        //            new XElement("title", video.Language[filter.lang].ChannelName),
        //            new XElement("type", video.Type),
        //            new XElement("catalog", video.Language[filter.lang].CatalogTags),
        //            new XElement("director", video.Language[filter.lang].Directors),
        //            new XElement("act", video.Language[filter.lang].Actors),
        //            new XElement("year", video.YearTags),
        //            new XElement("area", video.Language[filter.lang].AreaTags),
        //            new XElement("imgurl", video.ResImage),
        //            new XElement("state", video.State),
        //            new XElement("note", ""),
        //            new XElement("mark", video.BKInfo.Score),
        //            new XElement("hot", video.Hot),
        //            new XElement("pv", video.PV),
        //            new XElement("bitrate", video.Main.BitRate),
        //            new XElement("resolution", video.ResResolution),
        //            new XElement("flag", video.ResFlags),
        //            new XElement("duration", video.Main.TimeLength / 60),
        //            new XElement("content", BoxUtils.GetXmlCData(video.Language[filter.lang].Introduction)),
        //            new XElement("video_list_count", video.StateCount)
        //            );
        //        return xml.ToString(SaveOptions.DisableFormatting);
        //    }
        //    return NoVideos();
        //}

        //private string CreateOnlySet(VideoNode video, DetailFilter filter)
        //{
        //    var xml = new XElement("v");
        //    if (video != null)
        //    {
        //        xml.Add(
        //            new XElement("vid", video.Main.ChannelID),
        //            new XElement("title", video.Language[filter.lang].ChannelName),
        //            new XElement("type", video.Type),
        //            new XElement("imgurl", video.ResImage),
        //            new XElement("state", video.State),
        //            new XElement("mark", video.BKInfo.Score),
        //            new XElement("video_list_count", video.StateCount)
        //            );
        //        WriteViceChannel(video, xml, filter);
        //        return xml.ToString(SaveOptions.DisableFormatting);
        //    }
        //    return NoVideos();
        //}

        //private void WriteViceChannel(VideoNode video, XElement root, DetailFilter filter)
        //{
        //    if (video.ViceChannels.Count > 0)
        //    {
        //        root.Add(new XElement("video_list_page_count", PageUtils.PageCount(video.StateCount, filter.c)));
        //        if (filter.c != 0 && filter.s != 0)    //分页
        //        {
        //            root.Add(new XElement("countInPage", filter.c));
        //            root.Add(new XElement("page", filter.s));

        //            //currentVice = video.ViceChannels.Skip(filter.c * (filter.s - 1)).Take(filter.c)
        //            var index = (filter.s - 1) * filter.c;
        //            var max = index + filter.c;
        //            if (max > video.ViceChannels.Count)
        //                max = video.ViceChannels.Count;
        //            var xml = new XElement("video_list",
        //                new XAttribute("maxBytes", video.Language[filter.lang].MaxBytes),
        //                new XAttribute("maxChars", video.Language[filter.lang].MaxChars)
        //            );                    
        //            for (var i = index; i < max; i++)
        //            {
        //                xml.Add(CreateVice(video.ViceChannels[i], filter));
        //            }
        //            root.Add(xml);
        //        }
        //        else
        //        {
        //            var xml = new XElement("video_list",
        //                new XAttribute("maxBytes", video.Language[filter.lang].MaxBytes),
        //                new XAttribute("maxChars", video.Language[filter.lang].MaxChars),
        //                from vice in video.ViceChannels select CreateVice(vice,filter)
        //                );
        //            root.Add(xml);
        //        }
        //    }
        //    else
        //    {
        //        root.Add(new XElement("countInPage", 1));
        //        root.Add(new XElement("page", 1));
        //        root.Add(new XElement("video_list_page_count", 1));
        //        var xml = new XElement("video_list",
        //            new XAttribute("maxBytes", video.Language[filter.lang].MaxBytes),
        //            new XAttribute("maxChars", video.Language[filter.lang].MaxChars)
        //            );
        //        xml.Add(new XElement("video", new XAttribute("title", video.Language[filter.lang].ChannelName), BoxUtils.GetXmlCData(video.PlayLink)));
        //        root.Add(xml);
        //    }
        //}

        //private XElement CreateVice(ViceNode vice, DetailFilter filter)
        //{
        //    return new XElement("video", new XAttribute("title", vice.Language[filter.lang].Title), BoxUtils.GetXmlCData(vice.PlayLink));
        //}

        #endregion
    }
}
