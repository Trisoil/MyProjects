using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using System.ServiceModel;

namespace Synacast.BoxManage.List.Impl.Cover
{
    using Synacast.BoxManage.List.Cover;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.Core.Cache.Entity;
    using Synacast.BoxManage.List.Impl.Utils;
    using Synacast.BoxManage.Core.Utils;

    [ServiceBehavior(ConcurrencyMode = ConcurrencyMode.Multiple, InstanceContextMode = InstanceContextMode.PerCall)]
    public class ConverImpl : ICover 
    {
        #region ICover Members

        public string ImageNav(CoverFilter filter)
        {
            try
            {
                var res = CoverNavCache.Instance.Items[filter.navmode];
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                var xml = new XElement("navs");
                if (filters.Count <= 0)
                {
                    xml.Add(from re in res
                            select new XElement("nav",
                                new XElement("navid", re.Id),
                                new XElement("name", re.Language[filter.lang].Title),
                                new XElement("image", re.PicLink)
                            ));
                }
                else
                {
                    xml.Add(from re in res
                            let count = GetChannelCount(re, key, filter, filters)
                            where count > 0
                            select new XElement("nav",
                            new XElement("navid", re.Id),
                            new XElement("name", re.Language[filter.lang].Title),
                            new XElement("image", re.PicLink)
                            ));
                }
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoModeNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string ImageNavEx(CoverFilter filter)
        {
            try
            {
                var res = CoverNavCache.Instance.Items[filter.navmode];
                var xml = new XElement("navs");
                var key = new VideoNodeKey(filter.platform, 0, filter.auth);
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                xml.Add(from re in res
                        let count = GetChannelCount(re, key, filter, filters)
                        where count > 0
                        select new XElement("nav",
                        new XElement("navid", re.Id),
                        new XElement("name", re.Language[filter.lang].Title),
                        new XElement("count", count),
                        new XElement("image", re.PicLink)
                        ));
                return xml.ToString(SaveOptions.DisableFormatting);
            }
            catch (KeyNotFoundException)
            {
                return NoModeNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string ImageRecommand(CoverFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var filters = VideoNodesUtil.CommonCustomFilter(filter);
                var rel = CoverNavCache.Instance.Dictionary[filter.navid];
                return CreatePageList(rel, filter, pars, filters, false);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatformNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        public string ImageRecommandEx(CoverFilter filter)
        {
            try
            {
                var pars = VideoParsUtils.FormateSearchVideoPars(filter);
                var filters = VideoNodesUtil.FormateTagFilter(filter);
                var rel = CoverNavCache.Instance.Dictionary[filter.navid];
                return CreatePageList(rel, filter, pars, filters, true);
            }
            catch (KeyNotFoundException)
            {
                return NoPlatformNav();
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        //#region ICover Members

        //public string ImageRecommand(CoverFilter filter)
        //{
        //    try
        //    {
        //        var images = ConverImageCache.Instance.Items;
        //        int all = 0;
        //        var xml = new XElement("cover_list");
        //        var vkey = new VideoNodeKey(filter.platform, 0, filter.auth);
        //        var filters = VideoNodesUtil.CommonCustomFilter(filter);
        //        for (var i = 0; i < images.Count && all < filter.c; i++)
        //        {
        //            var image = images[i];
        //            var result = image.PlatForms.Contains(vkey);
        //            if (!result) continue;
        //            if (image.ChannelType == "vod")
        //            {
        //                //result = ListCache.Instance.Dictionary.ContainsKey(image.Vid);
        //                VideoNode video;
        //                if (ListCache.Instance.Dictionary.TryGetValue(image.Vid, out video))
        //                {
        //                    foreach (var f in filters)
        //                    {
        //                        result = result && f(video, filter);
        //                    }
        //                }
        //                else
        //                    continue;
        //            }
        //            else
        //            {
        //                //result = LiveListCache.Instance.Dictionary.ContainsKey(image.Vid);
        //                LiveVideoNode video;
        //                if (LiveListCache.Instance.Dictionary.TryGetValue(image.Vid, out video))
        //                {
        //                    foreach (var f in filters)
        //                    {
        //                        result = result && f(video, filter);
        //                    }
        //                }
        //                else
        //                    continue;
        //            }
        //            if (result)
        //            {
        //                all++;
        //                XElement playlink;
        //                if (image.ChannelType == "vod")
        //                    playlink = VideoResponseUtils.CreateVodPlayLinkVer2(image.Vid, filter, false);
        //                else
        //                    playlink = VideoResponseUtils.CreateLivePlayLinkVer2(image.Vid, filter, false);
        //                var subttitle = image.Language[filter.lang].SubTitle;
        //                if (filter.conlen != 0)
        //                    subttitle = subttitle.FormateSubStr(filter.conlen);
        //                xml.Add(new XElement("c",
        //                        new XElement("title", image.Language[filter.lang].Title),
        //                        new XElement("note", subttitle),
        //                        new XElement("vid", image.Vid),
        //                        new XElement("vtype", image.ChannelType),
        //                        playlink,
        //                        new XElement("cover_imgurl", image.ImageUrl),
        //                        new XElement("onlinetime", image.OnlineTime.ToString("yyyy-MM-dd HH:mm:ss"))
        //                    ));
        //            }
        //        }
        //        return xml.ToString(SaveOptions.DisableFormatting);
        //    }
        //    catch (Exception ex)
        //    {
        //        return BoxUtils.FormatErrorMsg(ex);
        //    }
        //}

        //public string ImageRecommandEx(CoverFilter filter)
        //{
        //    try
        //    {
        //        var images = ConverImageCache.Instance.Items;
        //        int all = 0;
        //        var xml = new XElement("cover_list");
        //        var vkey = new VideoNodeKey(filter.platform, 0, filter.auth);
        //        var filters = VideoNodesUtil.FormateTagFilter(filter);
        //        for (var i = 0; i < images.Count && all < filter.c; i++)
        //        {
        //            var image = images[i];
        //            var result = image.PlatForms.Contains(vkey);
        //            if (!result) continue;
        //            if (image.ChannelType == "vod")
        //            {
        //                VideoNode video;
        //                if (ListCache.Instance.Dictionary.TryGetValue(image.Vid, out video))
        //                {
        //                    foreach (var f in filters)
        //                    {
        //                        result = result && f(video, filter);
        //                    }
        //                }
        //                else
        //                    continue;
        //            }
        //            else
        //            {
        //                LiveVideoNode video;
        //                if (LiveListCache.Instance.Dictionary.TryGetValue(image.Vid, out video))
        //                {
        //                    foreach (var f in filters)
        //                    {
        //                        result = result && f(video, filter);
        //                    }
        //                }
        //                else
        //                    continue;
        //            }
        //            if (result)
        //            {
        //                all++;
        //                XElement playlink;
        //                if (image.ChannelType == "vod")
        //                    playlink = VideoResponseUtils.CreateVodPlayLinkVer2(image.Vid, filter, true);
        //                else
        //                    playlink = VideoResponseUtils.CreateLivePlayLinkVer2(image.Vid, filter, true);
        //                var subttitle = image.Language[filter.lang].SubTitle;
        //                if (filter.conlen != 0)
        //                    subttitle = subttitle.FormateSubStr(filter.conlen);
        //                xml.Add(new XElement("c",
        //                        new XElement("title", image.Language[filter.lang].Title),
        //                        new XElement("note", subttitle),
        //                        new XElement("vid", image.Vid),
        //                        new XElement("vtype", image.ChannelType),
        //                        playlink,
        //                        new XElement("cover_imgurl", image.ImageUrl),
        //                        new XElement("onlinetime", image.OnlineTime.ToString("yyyy-MM-dd HH:mm:ss"))
        //                    ));
        //            }
        //        }
        //        return xml.ToString(SaveOptions.DisableFormatting);
        //    }
        //    catch (Exception ex)
        //    {
        //        return BoxUtils.FormatErrorMsg(ex);
        //    }
        //}

        //#endregion

        #region ICover Members


        public string SplashScreeen(CoverFilter filter)
        {
            try
            {
                var modes = filter.mode.FormatStrToInt(SplitArray.LineArray);
                var spls = SplashScreenCache.Instance.Items;
                if (spls.Count <= 0 || modes.Count <= 0)
                {
                    var xml = new XElement("splashscreen");
                    return xml.ToString(SaveOptions.DisableFormatting);
                }
                else
                {
                    var xml = new XElement("slist");
                    foreach (var mode in modes)
                    {
                        var xmlquery = from spl in spls
                                        where spl.Mode == mode
                                        select new XElement("splashscreen",
                                            new XElement("mode", mode),
                                            new XElement("etag", spl.ETag),
                                            new XElement("photo", spl.Photo),
                                            new XElement("expires", spl.Expires.ToString("yyyy-MM-dd HH:mm:ss"))
                                            );
                        xml.Add(xmlquery);
                    }
                    return xml.ToString(SaveOptions.DisableFormatting);
                }
            }
            catch (Exception ex)
            {
                return BoxUtils.FormatErrorMsg(ex);
            }
        }

        #endregion

        /// <summary>
        /// 得到该推荐分类下的频道数
        /// </summary>
        private int GetChannelCount(CoverNavNode node, VideoNodeKey key, CoverFilter filter, List<Func<VideoBase, ExFilterBase, bool>> filters)
        {
            int count = 0;
            if (node.Images.ContainsKey(key))
            {
                var images = node.Images[key];
                foreach (var image in images)
                {
                    var c = CoverImageCache.Instance.Items[image];
                    var result = true;
                    VideoBase v = null;
                    if (c.ChannelType == 3)
                        v = ListCache.Instance.Dictionary[c.ChannelId];
                    else
                        v = LiveListCache.Instance.Dictionary[c.ChannelId];
                    foreach (var f in filters)
                    {
                        result = result && f(v, filter);
                    }
                    if (result)
                        count++;
                }
            }
            return count;
        }

        /// <summary>
        /// 分页输出
        /// </summary>
        private string CreatePageList(CoverNavNode node, CoverFilter filter, VideoPars pars, List<Func<VideoBase, ExFilterBase, bool>> filters, bool isEx)
        {
            var key = new VideoNodeKey(filter.platform, 0, filter.auth);
            var xml = new XElement("cover_list");
            int all = 0;
            var images = node.Images[key];
            for (var i = 0; i < images.Count && all < filter.c; i++)
            {
                var image = CoverImageCache.Instance.Items[images[i]];
                bool result = true;
                VideoBase video = null;
                if (image.ChannelType == 3)
                    video = ListCache.Instance.Dictionary[image.ChannelId];
                else
                    video = LiveListCache.Instance.Dictionary[image.ChannelId];
                foreach (var f in filters)
                {
                    result = result && f(video, filter);
                }
                if (!result)
                    continue;
                all++;

                XElement playlink;
                if (image.ChannelType == 3)
                    playlink = VideoResponseUtils.CreateVodPlayLinkVer2(image.ChannelId, filter, isEx);
                else
                    playlink = VideoResponseUtils.CreateLivePlayLinkVer2(image.ChannelId, filter, isEx);
                var subttitle = image.Language[filter.lang].SubTitle;
                if (filter.conlen != 0)
                    subttitle = subttitle.FormateSubStr(filter.conlen);
                xml.Add(new XElement("c",
                        new XElement("title", image.Language[filter.lang].Title),
                        new XElement("note", subttitle),
                        new XElement("vid", image.ChannelId),
                        new XElement("vtype", image.ChannelType == 3 ? "vod" : "live"),
                        playlink,
                        new XElement("cover_imgurl", image.ImageUrl),
                        new XElement("onlinetime", image.OnlineTime.ToString("yyyy-MM-dd HH:mm:ss"))
                    ));
            }
            return xml.ToString(SaveOptions.DisableFormatting);
        }

        /// <summary>
        /// 索引不存在的提示错误
        /// </summary>
        /// <returns></returns>
        private string NoModeNav()
        {
            return BoxUtils.FormatErrorMsg("不存在该mode或该平台下的导航数据");
        }

        private string NoPlatformNav()
        {
            return BoxUtils.FormatErrorMsg("不存在该平台下的导航影片");
        }
    }
}
