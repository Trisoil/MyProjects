using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.BoxManage.List.Impl.Utils
{
    using Synacast.BoxManage.List.Lists;
    using Synacast.BoxManage.List.Impl;
    using Synacast.BoxManage.Core;
    using Synacast.BoxManage.Core.Utils;
    using Synacast.BoxManage.Core.Cache;
    using Synacast.BoxManage.Core.Cache.Custom;
    using Synacast.BoxManage.List.Live;
    using Synacast.BoxManage.List.Live.Search;

    public static class VideoParsUtils
    {
        /// <summary>
        /// 接口请求flag排序
        /// </summary>
        public static string OrderAscFlags(string flags)
        {
            var fs = flags.FormatStrToArray(SplitArray.LineArray);
            return string.Join("|", fs.OrderBy(f => f).ToArray());
        }

        /// <summary>
        /// 点播节目请求参数处理
        /// </summary>
        public static VideoPars FormatVideoPars(ListFilter filter)
        {
            var vp = new VideoPars();
            if (filter.ver <= 1)
            {
                if (!string.IsNullOrEmpty(filter.tag))
                {
                    var tags = filter.tag.FormatStrToArrayEmpty(SplitArray.LineArray);
                    var dimensiontags = new List<string>(tags.Length);
                    for (int i = 0; i < tags.Length; i++)
                    {
                        var childetags = tags[i].FormatStrToArray(SplitArray.DHArray);
                        if (i == 0 && childetags.Length > 0)
                        {
                            dimensiontags.AddEnumerator(childetags.Select(v => VideoUtils.FormatStrArray(v.Length + 8, v, ":", CustomArray.CatalogDimension)));
                        }
                        else if (i == 1 && childetags.Length > 0)
                        {
                            dimensiontags.AddEnumerator(childetags.Select(v => VideoUtils.FormatStrArray(v.Length + 8, v, ":", CustomArray.AreaDimension)));
                        }
                        else if (i == 2 && childetags.Length > 0)
                        {
                            dimensiontags.AddEnumerator(childetags.Select(v => VideoUtils.FormatStrArray(v.Length + 8, v, ":", CustomArray.YearDimension)));
                        }
                        else if (i == 3 && childetags.Length > 0)
                        {
                            dimensiontags.AddEnumerator(childetags.Select(v => VideoUtils.FormatStrArray(v.Length + 8, v, ":", CustomArray.ActDimension)));
                        }
                        else if (i == 4 && childetags.Length > 0)
                        {
                            dimensiontags.AddEnumerator(childetags.Select(v => VideoUtils.FormatStrArray(v.Length + 12, v, ":", CustomArray.DirectorDimension)));
                        }
                    }
                    if (dimensiontags.Count > 0 && filter.order == "s")
                    {
                        vp.SortTag = dimensiontags[0];
                        dimensiontags.RemoveAt(0);
                    }
                    vp.Tags = dimensiontags;
                }
            }
            else
            {
                if (!string.IsNullOrEmpty(filter.ntags))
                {
                    var ntags = filter.ntags.FormatStrToArray(SplitArray.LineArray);
                    if (ntags.Length > 0 && filter.order == "s")
                    {
                        vp.SortTag = ntags[0];
                    }
                    vp.Tags = ntags;
                }
            }
            if (!string.IsNullOrEmpty(filter.flag))
            {
                vp.Flags = filter.flag.FormatStrToArray(SplitArray.LineArray);
            }
            if (filter.conlen == 0)
            {
                if (filter.ver <= 1)
                    vp.ResponseFun = VideoResponseUtils.CustomListRes;
                else
                    vp.ResponseFun = VideoResponseUtils.CustomListResVer2;
            }
            else
            {
                if (filter.ver <= 1)
                    vp.ResponseFun = VideoResponseUtils.ConLenListRes;
                else
                    vp.ResponseFun = VideoResponseUtils.ConLenListResVer2;
            }
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        vp.ForbiddenName = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            return vp;
        }

        /// <summary>
        /// 直播节目请求参数处理
        /// </summary>
        public static VideoPars FormatLiveVideoPars(LiveFilter filter)
        {
            VideoPars vp = new VideoPars();
            if (filter.type != 0)
            {
                vp.SortTag = filter.type.ToString();
            }
            if (!string.IsNullOrEmpty(filter.flag))
            {
                vp.Flags = filter.flag.FormatStrToArray(SplitArray.LineArray);
            }
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        vp.ForbiddenName = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            return vp;
        }

        /// <summary>
        /// 搜索请求参数处理
        /// </summary>
        public static VideoPars FormatSearchVideoPars(ListFilter filter)
        {
            VideoPars vp = new VideoPars();
            if (!string.IsNullOrEmpty(filter.flag))
            {
                vp.Flags = filter.flag.FormatStrToArray(SplitArray.LineArray);
            }
            if (filter.conlen == 0)
                vp.ResponseFun = VideoResponseUtils.CustomListRes;
            else
                vp.ResponseFun = VideoResponseUtils.ConLenListRes;
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        vp.ForbiddenName = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            return vp;
        }

        /// <summary>
        /// 直播搜索请求参数处理
        /// </summary>
        public static VideoPars FormatLiveSearchVideoPars(LiveSearchFilter filter)
        {
            var vp = new VideoPars();
            if (!string.IsNullOrEmpty(filter.flag))
            {
                vp.Flags = filter.flag.FormatStrToArray(SplitArray.LineArray);
            }
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        vp.ForbiddenName = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            return vp;
        }


        /// <summary>
        /// LuceneNet搜索请求参数处理
        /// </summary>
        public static VideoPars FormateSearchVideoPars(ExFilterBase filter)
        {
            var vp = new VideoPars();
            if (filter.conlen == 0)
            {
                if (filter.ver <= 1)
                    vp.ResponseFun = VideoResponseUtils.CustomListRes;
                else
                    vp.ResponseFun = VideoResponseUtils.CustomListResVer2;
            }
            else
            {
                if (filter.ver <= 1)
                    vp.ResponseFun = VideoResponseUtils.ConLenListRes;
                else
                    vp.ResponseFun = VideoResponseUtils.ConLenListResVer2;
            }
            return vp;
            //var vp = new VideoPars();
            //if (filter.conlen == 0)
            //    vp.ResponseFun = VideoResponseUtils.CustomListRes;
            //else
            //    vp.ResponseFun = VideoResponseUtils.ConLenListRes;
            //return vp;
        }

        public static VideoPars FormatListByTags(ListFilter filter)
        {
            var vp = new VideoPars();
            var ntags = filter.ntags.FormatStrToArray(SplitArray.LineArray);
            if (ntags.Length > 0 )
            {
                vp.SortTag = ntags[0];
            }
            vp.Tags = ntags.Skip(1);
            if (!string.IsNullOrEmpty(filter.flag))
            {
                vp.Flags = filter.flag.FormatStrToArray(SplitArray.LineArray);
            }
            if (filter.conlen == 0)
            {
                vp.ResponseFun = VideoResponseUtils.CustomListResVer2;
            }
            else
            {
                vp.ResponseFun = VideoResponseUtils.ConLenListResVer2;
            }
            if (filter.forbidden)
            {
                filter.forbidden = VideoNodeKeyArray.Instance.Forbidden[new VideoNodeKey(filter.platform, 0, filter.auth)];
                if (filter.forbidden)
                {
                    if (ForbiddenAreaCache.Instance.Items.ContainsKey(filter.fbcode))
                    {
                        vp.ForbiddenName = ForbiddenAreaCache.Instance.Items[filter.fbcode].ForbiddenAreaName;
                    }
                    else
                    {
                        filter.forbidden = false;
                    }
                }
            }
            return vp;
        }
    }
}
